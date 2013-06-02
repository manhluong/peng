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
import android.provider.Settings;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

import com.getpebble.android.kit.*;
import com.getpebble.android.kit.util.*;

public class PengService extends Service
	{
	public static final UUID PENG_APP_UUID = UUID.fromString("895F9096-6D88-48B5-AFB7-EA0C92947E01");
	public static final String SCREEN_ON_KEY = "com.luongbui.peng.android.SCREENVALUE";
	
	private static final int CMD_PENG = 0x00;
	private static final int CMD_PENG_START = 0x00;
	private static final int CMD_PENG_STOP = 0x01;
	
	private static final int CMD_VOLUME = 0x02;
	
	private static final int CMD_SCREEN = 0x03;
	private static final int CMD_SCREEN_ON = 0x04;
	private static final int CMD_SCREEN_OFF = 0x05;
	
	private static final int VOLUME_MAX = 100;
	private static final int VOLUME_MIN = 0;
	
	private final ServiceBinder _binder = new ServiceBinder();
	
	private boolean _turnScreenOn;
	
	private int _oldVolume;
	
	private int _oldRingerMode;
	
	private PebbleKit.PebbleDataReceiver dataReceiver;
	
	private Ringtone _ringtone;
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId)
		{
		
		if(_ringtone==null)
			_ringtone = RingtoneManager.getRingtone(this, Settings.System.DEFAULT_RINGTONE_URI);
		
		//Pebble default starts turning the screen on.
		_turnScreenOn = true;
		//Everytime, check that the Service is registered to only one.
		if (dataReceiver != null)
			{
            unregisterReceiver(dataReceiver);
            dataReceiver = null;
			}
		dataReceiver = new PebbleKit.PebbleDataReceiver(PENG_APP_UUID)
			{
            @Override
            public void receiveData(final Context context, final int transactionId, final PebbleDictionary data)
            	{
            	if(data.contains(CMD_PENG))
            		{
            		final int cmd = data.getUnsignedInteger(CMD_PENG).intValue();
            		PebbleKit.sendAckToPebble(context, transactionId);
            		Intent intent;
            		AudioManager audioManager;
            		switch (cmd)
                		{
                		case CMD_PENG_START:
                			//Two consecutive CMQ_PENG_START are unlikely, because we switch status in Pebble immediately.
                			
                			//Expected with Peng!
                			final int screenVal = data.getUnsignedInteger(CMD_SCREEN).intValue();
                			final int volumeVal = data.getUnsignedInteger(CMD_VOLUME).intValue();
                			
                			//Ringtone.
                			audioManager = (AudioManager)context.getSystemService(Context.AUDIO_SERVICE);
                			int volIndex = VOLUME_MIN;
                			if(volumeVal == VOLUME_MAX)
                				volIndex = audioManager.getStreamMaxVolume(AudioManager.STREAM_RING);
                			else
                				volIndex = ((volumeVal * audioManager.getStreamMaxVolume(AudioManager.STREAM_RING)) / 100);
                			_oldVolume = audioManager.getStreamVolume(AudioManager.STREAM_RING);
                			_oldRingerMode = audioManager.getRingerMode();
                			audioManager.setStreamVolume(AudioManager.STREAM_RING,
                										 volIndex,
                										 AudioManager.RINGER_MODE_NORMAL);
                			
                			//Screen.
                			_turnScreenOn = true;
                			if(screenVal == CMD_SCREEN_OFF)
                				_turnScreenOn = false;
                			intent = new Intent(context, MainActivity.class);
                			intent.putExtra(SCREEN_ON_KEY, _turnScreenOn);
                			intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
                			intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                			startActivity(intent);
                			
                			_ringtone.play();
                			
                			break;
                		case CMD_PENG_STOP:
                			//In stopRingtone() we send a message to the Pebble, so it changes status.
                			//This way, the user can send multiple Stops if some messages are dropped.
                			stopRingtone(context);
                			
                			//Everytime we dismiss the Activity, so onCreate() will be called next time.
                			//In this way we've got the screen turned on in the Activity lifecycle.
                			//Also, it is more elegant.
                			intent = new Intent("CMD_PENG_STOP_INTENT");
                			LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
                			
                			break;
                		}
            		}
            	else if(data.contains(CMD_SCREEN))
        			{
            		final int cmd = data.getUnsignedInteger(CMD_SCREEN).intValue();
            		PebbleKit.sendAckToPebble(context, transactionId);
            		//Useless because we will resend it with the Peng! Needed only for UI updates.
            		switch (cmd)
            			{
            			case CMD_SCREEN_ON:
            				_turnScreenOn = true;
            				break;
            			case CMD_SCREEN_OFF:
            				_turnScreenOn = false;
            				break;
            			}
        			}
            	}
			};
		PebbleKit.registerReceivedDataHandler(this, dataReceiver);
		//Log.d("PengService", "Service started.");
		//Let the OS restart it.
		return Service.START_STICKY;
		}
	
	@Override
	public void onDestroy()
		{
		if (dataReceiver != null)
			{
            unregisterReceiver(dataReceiver);
            dataReceiver = null;
			}
		if(_ringtone!=null)
			{
			_ringtone.stop();
			_ringtone = null;
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
	
	public void stopRingtone(Context context)
		{
		if(_ringtone!=null)
			{
			AudioManager audioManager = (AudioManager)context.getSystemService(Context.AUDIO_SERVICE);
			audioManager.setRingerMode(_oldRingerMode);
			audioManager.setStreamVolume(AudioManager.STREAM_RING,
											_oldVolume,
					 						AudioManager.RINGER_MODE_NORMAL);
			
			_ringtone.stop();
			//Signal Pebble that we stopped.
			PebbleDictionary dict = new PebbleDictionary();
			dict.addInt8(CMD_PENG, (byte)CMD_PENG_STOP);
			PebbleKit.sendDataToPebble(this,
										PENG_APP_UUID,
										dict);
			}
		}

	}
