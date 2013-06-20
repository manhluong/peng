package com.luongbui.peng.android;

import java.util.UUID;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.media.AudioManager;
import android.preference.PreferenceManager;
import android.support.v4.content.LocalBroadcastManager;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.PebbleKit.PebbleDataReceiver;
import com.getpebble.android.kit.util.PebbleDictionary;

public class PengComm extends PebbleDataReceiver
	{
	
	private PengService _ctx;

	public PengComm(UUID uuid, PengService ctx)
		{
		super(uuid);
		PebbleKit.registerReceivedDataHandler(ctx, this);
		_ctx = ctx;
		}

	@Override
	public void receiveData(Context context,
							int transactionId,
							PebbleDictionary data)
		{
		if(data.contains(PengService.CMD_PENG))
			{
			PebbleKit.sendAckToPebble(context, transactionId);
			final int cmd = data.getUnsignedInteger(PengService.CMD_PENG).intValue();
			switch (cmd)
    			{
    			case PengService.CMD_PENG_START:
    			
    				//Expected with Peng!
    				final int screenVal = data.getUnsignedInteger(PengService.CMD_SCREEN).intValue();
    				final int volumeVal = data.getUnsignedInteger(PengService.CMD_VOLUME).intValue();
    				
    				//Retrieve and save old volume level.
    				AudioManager audioManager = (AudioManager)context.getSystemService(Context.AUDIO_SERVICE);
    				int oldVolume = audioManager.getStreamVolume(AudioManager.STREAM_RING);
    				int oldRingerMode = audioManager.getRingerMode();
    				SharedPreferences.Editor editor = PreferenceManager.getDefaultSharedPreferences(context).edit();
    		        editor.putInt(PengService.OLD_VOLUME_KEY, oldVolume);
    		        editor.putInt(PengService.OLD_RINGER_KEY, oldRingerMode);
    		        editor.commit();
    			
    		        //Screen.
    		        if(screenVal == PengService.CMD_SCREEN_OFF)
    		        	_ctx.turnScreen(false);
    		        else
    		        	_ctx.turnScreen(true);
    		        
    		        //Ringtone
    		        _ctx.playRingtone(volumeVal);
    		        
    		        break;
    			case PengService.CMD_PENG_STOP:
    				//In stopRingtone() we send a message to the Pebble, so it changes status.
    				//This way, the user can send multiple Stops if some messages are dropped.
    				_ctx.stopRingtone();
    			
    				//Everytime we dismiss the Activity, so onCreate() will be called next time.
    				//In this way we've got the screen turned on in the Activity lifecycle.
    				//Also, it is more elegant.
    				Intent intent = new Intent("CMD_PENG_STOP_INTENT");
    				LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    			
    				break;
    			}
			}
		}
	
	public void sendComm(int cmd, byte val)
		{
		PebbleDictionary dict = new PebbleDictionary();
		dict.addInt8(cmd, val);
		PebbleKit.sendDataToPebble(_ctx,
									PengService.PENG_APP_UUID,
									dict);
		}
	
	}
