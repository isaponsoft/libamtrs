/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
package	jp.libamtrs.alarm;


import android.app.Activity;
import android.os.Bundle;
import android.app.NativeActivity;
import android.widget.*;
import android.view.*;
import android.widget.LinearLayout.LayoutParams;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.inputmethod.*;
import android.content.Context;
import android.os.ResultReceiver;
import android.os.IBinder;
import android.os.Build;

import android.app.NotificationManager;
import android.app.Notification;
import android.content.Intent;
import android.app.PendingIntent;
import android.app.AlarmManager;
import android.app.NotificationChannel;
import android.app.NotificationChannelGroup;
import android.content.BroadcastReceiver;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationCompat.Builder;

import java.util.Date;
import java.io.FileInputStream;
import java.io.DataInputStream;


import	android.content.pm.ApplicationInfo;
import	android.content.pm.PackageManager;
import	android.os.PowerManager;

public class	AlarmReceiver extends android.content.BroadcastReceiver 
{
	public static final String	ALARM_FIELD_CHANNEL_ID		= "AlarmReceiver:CHANNEL_ID";
	public static final String	ALARM_FIELD_TITLE			= "AlarmReceiver:TITLE";
	public static final String	ALARM_FIELD_MESSAGE			= "AlarmReceiver:MESSAGE";
	public static final String	ALARM_FIELD_ACTIVITY		= "AlarmReceiver:ACTIVITY";
	public static final String	ALARM_FIELD_WAKEUP			= "AlarmReceiver:WAKEUP";


	// ========================================================================
	//! アラーム通知を登録します。
	// ------------------------------------------------------------------------
	//! _activity
	//!		アプリのメインアクティビティ
	//!	_receiverClass
	//!		AlarmReceiverを継承したクラス(AndroidManifest.xmlで登録してあるもの)
	//!	_time
	//!		アラームを起動する時間(ミリ秒)
	//!	_id
	//!		通知のID
	//!	_title
	//!		通知のタイトル
	//!	_message
	//!		通知のメッセージ
	// ------------------------------------------------------------------------
	public static void addAlarm(Activity _activity, Class _receiverClass, long _time, int _id, String _title, String _message, String _channelId, String _cnannelTitle, int _wakeup)
	{
		createChannel(_activity, _channelId, _cnannelTitle);

		Intent			i			= new Intent(_activity, _receiverClass);
		i.putExtra(ALARM_FIELD_CHANNEL_ID, 	_channelId);
		i.putExtra(ALARM_FIELD_TITLE,   	_title);
		i.putExtra(ALARM_FIELD_MESSAGE, 	_message);
		i.putExtra(ALARM_FIELD_ACTIVITY,	_activity.getClass().getCanonicalName());
		i.putExtra(ALARM_FIELD_WAKEUP,		(_wakeup != 0) ?  "YES" : "");
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT)
		{
			((AlarmManager)_activity.getSystemService(Context.ALARM_SERVICE))
				.set(AlarmManager.RTC_WAKEUP, _time, PendingIntent.getBroadcast(_activity, _id, i, 0));
		}
		else
		{
			((AlarmManager)_activity.getSystemService(Context.ALARM_SERVICE))
				.setExact(AlarmManager.RTC_WAKEUP, _time, PendingIntent.getBroadcast(_activity, _id, i, 0));
		}
	}

	public static void cancelAll(Activity _activity, Class _receiverClass)
	{
		AlarmManager	am				= (AlarmManager)_activity.getSystemService(Context.ALARM_SERVICE);
		Intent			intent			= new Intent(_activity, _receiverClass);
		for (int i = 0; i < 100; ++i)
		{
			PendingIntent	pendingIntent	= PendingIntent.getBroadcast(_activity, i, intent, PendingIntent.FLAG_UPDATE_CURRENT);
			am.cancel(pendingIntent);
			pendingIntent.cancel();
		}
	}

	@Override
	public void onReceive(Context _context, Intent _intent)
	{
		String			channelId		= _intent.getStringExtra(ALARM_FIELD_CHANNEL_ID);
		String			title			= _intent.getStringExtra(ALARM_FIELD_TITLE);
		String			message			= _intent.getStringExtra(ALARM_FIELD_MESSAGE);
		String			classname		= _intent.getStringExtra(ALARM_FIELD_ACTIVITY);
		String			wakeup			= _intent.getStringExtra(ALARM_FIELD_WAKEUP);
		try
		{
			if (!wakeup.equals(""))
			{
				// アプリ起動
				PowerManager			pm			= (PowerManager)_context.getSystemService(Context.POWER_SERVICE);  
				PowerManager.WakeLock	wl			= pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "Alarm");  
				wl.acquire(5 * 60);

				Intent					newIntent	= new Intent(_context, _context.getClassLoader().loadClass(classname));
				newIntent.setFlags(android.content.Intent.FLAG_ACTIVITY_NEW_TASK); 
				_context.startActivity(newIntent);
			}
			else
			{
				NotificationManager			nm		= (NotificationManager)_context.getSystemService(Context.NOTIFICATION_SERVICE);

				// 通知
				// パラメータを取得
				Builder		builder	= new Builder(_context, channelId);
				builder
					.setContentTitle(title)
					.setContentText(message);

				// アプリアイコンを取得
				int			iconId	= _context.getPackageManager().getApplicationInfo(_context.getPackageName(), 0).icon;
				builder.setSmallIcon(iconId);

				Intent			intent_wake		= new Intent(_context, _context.getClassLoader().loadClass(classname));
				PendingIntent	pendingIntent	= PendingIntent.getActivity(_context, 0, intent_wake, 0);
				builder.setContentIntent(pendingIntent);


				// ローカル通知を表示
				nm.notify(1, builder.build());
			}
		}
		catch (Exception e)
		{
		}
	}


	private static void createChannel(Context _context, String _channelId, String _channelTitle)
	{
		NotificationManager			nm		= (NotificationManager)_context.getSystemService(Context.NOTIFICATION_SERVICE);
		NotificationChannel			channel	= new NotificationChannel(_channelId, _channelTitle, NotificationManager.IMPORTANCE_DEFAULT);
		channel.enableLights(true);
		channel.setLightColor(android.graphics.Color.RED);
		channel.enableVibration(true);
		//channel.setGroup(grpId);
		//channel.setVibrationPattern(new long[]{100, 200, 300, 400, 500, 400, 300, 200, 400});
		nm.createNotificationChannel(channel);
	}
}
