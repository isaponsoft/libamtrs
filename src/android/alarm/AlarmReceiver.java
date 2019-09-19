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
import android.content.BroadcastReceiver;
import android.support.v4.app.NotificationCompat.Builder;

import java.util.Date;
import java.io.FileInputStream;
import java.io.DataInputStream;


import	android.content.pm.ApplicationInfo;
import	android.content.pm.PackageManager;
import	android.os.PowerManager;

public class	AlarmReceiver extends android.content.BroadcastReceiver 
{
	public static final String	ALARM_FIELD_TITLE			= "AlarmReceiver:TITLE";
	public static final String	ALARM_FIELD_MESSAGE			= "AlarmReceiver:MESSAGE";
	public static final String	ALARM_FIELD_ACTIVITY		= "AlarmReceiver:ACTIVITY";

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
	public static void addAlarm(Activity _activity, Class _receiverClass, long _time, int _id, String _title, String _message)
	{
	//	android.util.Log.d("lamtrs", "AlarmReceiver.java : add alarm " + ((_time - System.currentTimeMillis())/1000));

		Intent			i		= new Intent(_activity, _receiverClass);
		i.putExtra(ALARM_FIELD_TITLE,   	_title);
		i.putExtra(ALARM_FIELD_MESSAGE, 	_message);
		i.putExtra(ALARM_FIELD_ACTIVITY,	_activity.getClass().getCanonicalName());

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


	// ========================================================================
	//! 通知を表示します。
	// ------------------------------------------------------------------------
	//!	_context, _intent
	//!		onReceiveに渡されたもの
	//!
	//!	_iconId
	//!		通知に表示するアイコンのID。 -1 を指定した場合はアプリアイコン。
	// ------------------------------------------------------------------------
	protected static void notificationAlarm(Context _context, Intent _intent, int _iconId)
	{
		try
		{
			Builder			builder		= new Builder(_context);

			// パラメータを取得
			String			title			= _intent.getStringExtra(ALARM_FIELD_TITLE); 
			String			message			= _intent.getStringExtra(ALARM_FIELD_MESSAGE); 
			builder
				.setContentTitle(title)
				.setContentText(message);


			// アプリアイコンを取得
			int			iconId		= _iconId;
			if (iconId == -1)
			{
				iconId	= _context.getPackageManager().getApplicationInfo(_context.getPackageName(), 0).icon;
			}
			builder.setSmallIcon(iconId);


			// アクティビティの起動用クラスオブジェクト
			String			classname		= _intent.getStringExtra(ALARM_FIELD_ACTIVITY); 
			Class			activity		= _context.getClassLoader().loadClass(classname);
			Intent			intent_wake		= new Intent(_context, activity) ;
			PendingIntent	pendingIntent	= PendingIntent.getActivity(_context, 0, intent_wake, 0);
			builder.setContentIntent(pendingIntent);

			// ローカル通知を表示
			((NotificationManager)_context.getSystemService(Context.NOTIFICATION_SERVICE)).notify(1, builder.build());
		}
		catch (Exception e)
		{}
	}


	public static void alart(Context _context, Class _activity, Intent _intent)
	{
		String			title			= _intent.getStringExtra(ALARM_FIELD_TITLE); 
		String			message			= _intent.getStringExtra(ALARM_FIELD_MESSAGE); 
		jp.libamtrs.AmtrsAlartActivity.alart(_context, _activity, title, message);
	}


	public Class onHomeAlart()
	{
		return	null;
	}

	public static void cancelAll(Activity _activity, Class _receiverClass)
	{
		//android.util.Log.d("lamtrs", "AlarmReceiver : cancelAll");

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
		android.util.Log.d("lamtrs", "AlarmReceiver.java : onReceive");
		notificationAlarm(_context, _intent, -1);

		Class	alartActivity	= onHomeAlart();
		if (alartActivity != null)
		{
			alart(_context, alartActivity, _intent);
		}
	}
}
