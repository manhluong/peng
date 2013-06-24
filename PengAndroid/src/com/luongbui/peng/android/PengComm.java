package com.luongbui.peng.android;

import java.util.UUID;

import android.content.Context;
import android.content.Intent;
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
		else if(data.contains(PengService.CMD_FLASHLIGHT))
			{
			PebbleKit.sendAckToPebble(context, transactionId);
			final int cmd = data.getUnsignedInteger(PengService.CMD_FLASHLIGHT).intValue();
			if(cmd==PengService.CMD_FLASHLIGHT_CHECK)
				sendComm(PengService.CMD_FLASHLIGHT_CHECK, (byte)_ctx.getFlashlightStatus());
			else
				//It also sends a CMD_FLASHLIGHT_CHECK as response.
				_ctx.turnFlashlight(cmd);
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
