package com.luongbui.peng.android;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.util.Log;

public class InstallWatchAppTask extends AsyncTask<Void, Void, String>
	{
	private static final String LOGTAG = "InstallWatchAppTask";
	
	private Context _ctx;
	
	public InstallWatchAppTask(Context ctx)
		{
		_ctx = ctx;
		}

	@Override
	protected String doInBackground(Void... params)
		{
		InputStream pbwInput = _ctx.getResources().openRawResource(R.raw.peng);
		try
			{
			File newPbw = new File(_ctx.getExternalCacheDir(), "peng.pbw");
			Log.d(LOGTAG, newPbw.getAbsolutePath());
			FileOutputStream pbwOutput = new FileOutputStream(newPbw);
			for(int b = pbwInput.read();b>=0;b = pbwInput.read())
				pbwOutput.write(b);
			pbwOutput.flush();
			pbwOutput.close();
			Intent viewPbw = new Intent();
			viewPbw.setAction(android.content.Intent.ACTION_VIEW);
			viewPbw.setDataAndType(Uri.fromFile(newPbw), "application/octet-stream");
			_ctx.startActivity(viewPbw);
			}
		catch(FileNotFoundException fnfe)
			{
			Log.e(LOGTAG, fnfe.toString());
			}
		catch(IOException io)
			{
			Log.e(LOGTAG, io.toString());
			}
		return null;
		}
	
	@Override
	protected void onPostExecute(String result)
		{
		
		}

	}
