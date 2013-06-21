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

import java.util.UUID;

import android.app.*;
import android.content.*;
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
	
	static final int CMD_PENG = 0x00;
	static final int CMD_PENG_START = 0x00;
	static final int CMD_PENG_STOP = 0x01;
	
	static final int CMD_VOLUME = 0x02;
	
	static final int CMD_SCREEN = 0x03;
	static final int CMD_SCREEN_ON = 0x04;
	static final int CMD_SCREEN_OFF = 0x05;
	
	static final int VOLUME_MAX = 100;
	static final int VOLUME_MIN = 0;
	
	private final ServiceBinder _binder = new ServiceBinder();
	
	private int _oldVolume;
	
	private int _oldRingerMode;
	
	private PengComm _comm;
	
	private Ringtone _ringtone;
	
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
		
		//Log.d("PengService", "Service started.");
		//Let the OS restart it.
		return Service.START_STICKY;
		}
	
	@Override
	public void onDestroy()
		{
		if (_comm != null)
			{
			stopRingtone();
			_ringtone = null;
            unregisterReceiver(_comm);
            _comm = null;
			}
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
			Integer oldValue = PreferenceManager.getDefaultSharedPreferences(this).getInt(OLD_VOLUME_KEY, 0);
			_oldVolume = oldValue.intValue();
			oldValue = PreferenceManager.getDefaultSharedPreferences(this).getInt(OLD_RINGER_KEY, 0);
			_oldRingerMode = oldValue.intValue();
			AudioManager audioManager = (AudioManager)this.getSystemService(Context.AUDIO_SERVICE);
			audioManager.setRingerMode(_oldRingerMode);
			audioManager.setStreamVolume(AudioManager.STREAM_RING,
											_oldVolume,
					 						AudioManager.RINGER_MODE_NORMAL);
			
			_ringtone.stop();
			//Signal Pebble that we stopped.
			_comm.sendComm(CMD_PENG, (byte)CMD_PENG_STOP);
			}
		}
	}
