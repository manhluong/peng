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

import com.getpebble.android.kit.PebbleKit;

import android.media.RingtoneManager;
import android.os.*;
import android.provider.Settings;
import android.app.*;
import android.content.*;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.LocalBroadcastManager;
import android.util.*;
import android.view.*;
import android.view.WindowManager.LayoutParams;

public class MainActivity extends FragmentActivity
	{
	/**
	 * The Service handles Pebble communcation.
	 */
	private PengService _service;
	
	/**
	 * Retrieve the Service.
	 */
	private ServiceConnection _connection = new ServiceConnection()
		{

	    public void onServiceConnected(ComponentName className, IBinder binder)
	    	{
	    	_service = ((PengService.ServiceBinder) binder).getService();
	    	//Log.d("ServiceConnection", "Connected.");
	    	}

	    public void onServiceDisconnected(ComponentName className)
	    	{
	    	_service = null;
	    	//Log.d("ServiceConnection", "Disconnected.");
	    	}
	    };
	   
	/**
	 * Used by the Service to end the Activity.
	 */
	private BroadcastReceiver _messageReceiver = new BroadcastReceiver()
		{
	    @Override
	    public void onReceive(Context context, Intent intent)
	    	{
	        finish();
	    	}
	    };

	@Override
	protected void onCreate(Bundle savedInstanceState)
		{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		LocalBroadcastManager.getInstance(this).registerReceiver(_messageReceiver,
			      new IntentFilter("CMD_PENG_STOP_INTENT"));
		//Start our Service.
		startPengService();
		//Request Peng in Pebble.
		PebbleKit.startAppOnPebble(this, PengService.PENG_APP_UUID);
		//Check the screen.
		//In PengService we use FLAG_ACTIVITY_NEW_TASK.
		if(getIntent()!=null)
			{
			if(getIntent().getBooleanExtra(PengService.SCREEN_ON_KEY, false))
				turnScreenOn();
			}
		}
	
	@Override
	protected void onDestroy()
		{
		//Unbind from the Service. Memory leaked without this.
		//Service still running if not stopped.
		unbindService(_connection);
		LocalBroadcastManager.getInstance(this).unregisterReceiver(_messageReceiver);
		super.onDestroy();
		}
	
	protected void startPengService()
		{
		Intent serviceIntent = new Intent(this, PengService.class);
		//Start the service.
		startService(serviceIntent);
		//Bind to it.
		bindService(serviceIntent,
					_connection,
					Context.BIND_AUTO_CREATE);
		}
	
	protected void stopPengService()
		{
		//Stop the Ringtone, eventually.
		if(_service!=null)
			_service.stopRingtone(this);
		Intent serviceIntent = new Intent(this, PengService.class);
		stopService(serviceIntent);
		finish();
		}
	
	protected void turnScreenOn()
		{
		getWindow().addFlags(LayoutParams.FLAG_DISMISS_KEYGUARD);
		getWindow().addFlags(LayoutParams.FLAG_SHOW_WHEN_LOCKED);
		getWindow().addFlags(LayoutParams.FLAG_KEEP_SCREEN_ON);
		getWindow().addFlags(LayoutParams.FLAG_TURN_SCREEN_ON);
		}
	
	@Override
	/**
	 * Disable screen orientation changes.
	 * @param newConfig
	 */
	public void onConfigurationChanged(Configuration newConfig)
		{
	    super.onConfigurationChanged(newConfig);
	    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		}
	
	public void onStopRingtoneBtn(View source)
		{
		if(_service!=null)
			_service.stopRingtone(this);
		finish();
		}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu)
		{
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
		}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
		{
	    switch (item.getItemId())
	    	{
	    	case R.id.menu_about:
	    		showAboutDialog();
	    		return true;
	    	case R.id.menu_stop_service:
	    		stopPengService();
	    		return true;
	    	default:
	    		return super.onOptionsItemSelected(item);
	    	}
		}
	
	private void showAboutDialog()
		{
        android.support.v4.app.FragmentManager fm = getSupportFragmentManager();
        AboutDialog aboutDlg = new AboutDialog();
        aboutDlg.show(fm, "fragment_about");
		}

	}
