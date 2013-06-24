/**
 * Copyright (©) 2013, Manh Luong Bui
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1) Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.\n\n2) Redistributions in binary form must
 * reproduce the above copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the distribution.
 *
 * 3) Neither the name of the Author nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

package com.luongbui.peng.android;

import java.util.List;
import java.util.UUID;

import android.app.*;
import android.content.*;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.Parameters;
import android.media.AudioManager;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.os.*;
import android.preference.PreferenceManager;
import android.provider.Settings;

public class PengService extends Service
	{
	public static final UUID PENG_APP_UUID = UUID.fromString("895F9096-6D88-48B5-AFB7-EA0C92947E01");
	public static final String SCREEN_ON_KEY = "com.luongbui.peng.android.SCREENVALUE";
	public static final String OLD_VOLUME_KEY = "com.luongbui.peng.android.OLDVOLUME";
	public static final String OLD_RINGER_KEY = "com.luongbui.peng.android.OLDRINGER";
	public static final String FLASHLIGHT_KEY = "com.luongbui.peng.android.FLASHLIGHT";
	
	static final int CMD_PENG = 0x00;
	static final int CMD_PENG_START = 0x00;
	static final int CMD_PENG_STOP = 0x01;
	
	static final int CMD_VOLUME = 0x02;
	
	static final int CMD_SCREEN = 0x03;
	static final int CMD_SCREEN_ON = 0x04;
	static final int CMD_SCREEN_OFF = 0x05;
	
	static final int CMD_FLASHLIGHT = 0x06;
	static final int CMD_FLASHLIGHT_CHECK = 0x07;
	static final int CMD_FLASHLIGHT_ON = 0x08;
	static final int CMD_FLASHLIGHT_OFF = 0x09;
	static final int CMD_FLASHLIGHT_NOT_AVAILABLE = 0x0A;
	
	static final int VOLUME_MAX = 100;
	static final int VOLUME_MIN = 0;
	
	private final ServiceBinder _binder = new ServiceBinder();
	
	private int _oldVolume;
	
	private int _oldRingerMode;
	
	private PengComm _comm;
	
	private Ringtone _ringtone;
	
	private int _flashlight;
	
	private Camera _cam;
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId)
		{
		
		if(_ringtone==null)
			_ringtone = RingtoneManager.getRingtone(this, Settings.System.DEFAULT_RINGTONE_URI);

		//Everytime, check that the Service is registered to only one receiver.
		if (_comm != null)
			{
            unregisterReceiver(_comm);
            _comm = null;
			}
		_comm = new PengComm(PENG_APP_UUID, this);
		
		_cam = null;
		//Check if there is the flashlight.
		if(!getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FLASH))
			_flashlight = CMD_FLASHLIGHT_NOT_AVAILABLE;
		else
			_flashlight = PreferenceManager.getDefaultSharedPreferences(this).getInt(FLASHLIGHT_KEY, CMD_FLASHLIGHT_OFF);
		
		//Log.d("PengService", "Service started.");
		//Let the OS restart it.
		return Service.START_STICKY;
		}
	
	@Override
	public void onDestroy()
		{
		//Turn off to release the camera pointer.
		turnFlashlight(CMD_FLASHLIGHT_OFF);
		//From the Peng watchapp point of view, is like the flashlight is not available anymore.
		_comm.sendComm(PengService.CMD_FLASHLIGHT_CHECK, (byte)CMD_FLASHLIGHT_NOT_AVAILABLE);
		//Release the ringtone.
		stopRingtone();
		_ringtone = null;
		//Release the comm handler with the Pebble.
        unregisterReceiver(_comm);
        _comm = null;
		}
	
	@Override
	public IBinder onBind(Intent intent)
		{
	    return _binder;
		}

	public class ServiceBinder extends Binder
		{
		PengService getService()
			{
			return PengService.this;
			}
		}
	
	public void playRingtone(int vol)
		{
		//If the ringtone is already playing, do nothing.
		//This could happen in case of two consecutive Peng! with no Stop! in between.
		if(_ringtone.isPlaying())
			return;
		
		saveOldVolumeAndRinger();
		AudioManager audioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
		int volIndex = PengService.VOLUME_MIN;
		if(vol == PengService.VOLUME_MAX)
			volIndex = audioManager.getStreamMaxVolume(AudioManager.STREAM_RING);
		else
			volIndex = ((vol * audioManager.getStreamMaxVolume(AudioManager.STREAM_RING)) / 100);
		audioManager.setStreamVolume(AudioManager.STREAM_RING,
										volIndex,
										AudioManager.RINGER_MODE_NORMAL);
		
		_ringtone.play();
		}
	
	public void turnScreen(boolean on)
		{
		Intent intent = new Intent(this, MainActivity.class);
		intent.putExtra(SCREEN_ON_KEY, on);
		intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(intent);
		}
	
	public void stopRingtone()
		{
		if(_ringtone!=null)
			{
			_ringtone.stop();
			loadOldVolumeAndRinger();
			//Signal Pebble that we stopped.
			_comm.sendComm(CMD_PENG, (byte)CMD_PENG_STOP);
			}
		}
	
	public int getFlashlightStatus()
		{
		return _flashlight;
		}
	
	public void turnFlashlight(int val)
		{
		if(_flashlight==CMD_FLASHLIGHT_NOT_AVAILABLE)
			return;
		_flashlight = val;
		//Lazy init of camera pointer.
		if(_cam==null)
			_cam = Camera.open();
		SharedPreferences.Editor editor = PreferenceManager.getDefaultSharedPreferences(this).edit();
        editor.putInt(FLASHLIGHT_KEY, _flashlight);
        editor.commit();
		if(_flashlight==CMD_FLASHLIGHT_ON)
			{
			Parameters camParams = _cam.getParameters();
			_cam.startPreview();
			//Check all flashlight supported modes.
			List<String> modes = camParams.getSupportedFlashModes();
			if(modes.contains(Parameters.FLASH_MODE_TORCH))
				camParams.setFlashMode(Parameters.FLASH_MODE_TORCH);
			else
				camParams.setFlashMode(Parameters.FLASH_MODE_ON);
			_cam.setParameters(camParams);
			_cam.autoFocus(new AutoFocusCallback()
				{
				@Override
				public void onAutoFocus(boolean success, Camera camera)
					{ }
				});
			}
		else if(_flashlight==CMD_FLASHLIGHT_OFF)
			{
			_cam.stopPreview();
			_cam.release();
			_cam = null;
			}
		_comm.sendComm(PengService.CMD_FLASHLIGHT_CHECK, (byte)getFlashlightStatus());
		}
	
	private void saveOldVolumeAndRinger()
		{
		//Retrieve and save old volume level and ringer mode.
		AudioManager audioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
		int oldVolume = audioManager.getStreamVolume(AudioManager.STREAM_RING);
		int oldRingerMode = audioManager.getRingerMode();
		SharedPreferences.Editor editor = PreferenceManager.getDefaultSharedPreferences(this).edit();
		editor.putInt(PengService.OLD_VOLUME_KEY, oldVolume);
		editor.putInt(PengService.OLD_RINGER_KEY, oldRingerMode);
		editor.commit();
		android.util.Log.d("PengService", "Save Old volume: " + oldVolume + " Old ringer: " + oldRingerMode);
		}
	
	private void loadOldVolumeAndRinger()
		{
		Integer oldValue = PreferenceManager.getDefaultSharedPreferences(this).getInt(OLD_VOLUME_KEY, 0);
		_oldVolume = oldValue.intValue();
		oldValue = PreferenceManager.getDefaultSharedPreferences(this).getInt(OLD_RINGER_KEY, 0);
		_oldRingerMode = oldValue.intValue();
		AudioManager audioManager = (AudioManager)this.getSystemService(Context.AUDIO_SERVICE);
		audioManager.setRingerMode(_oldRingerMode);
		audioManager.setStreamVolume(AudioManager.STREAM_RING,
										_oldVolume,
				 						AudioManager.RINGER_MODE_NORMAL);
		android.util.Log.d("PengService", "Load Old volume: " + _oldVolume + " Old ringer: " + _oldRingerMode);
		}
	}
