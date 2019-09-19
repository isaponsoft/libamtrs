package jp.libamtrs.window;
import android.app.Dialog;
import android.app.TimePickerDialog;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;
import android.widget.TimePicker;
import java.util.Calendar;
import android.content.DialogInterface;
 

public class	DialogTimePicker
				extends		DialogFragment
				implements	TimePickerDialog.OnTimeSetListener
{
	private	long				mNativeHandle;
	private	int					mHour		= -1;
	private	int					mMinute		= -1;
	private	TimePickerDialog	mTimePicker;



	public static DialogTimePicker create(long _nativeHandle)
	{
		DialogTimePicker	dialog	= new DialogTimePicker();
		Bundle				bundle	= new Bundle();
		bundle.putLong("native",	_nativeHandle);
		bundle.putInt("hour",		-1);
		bundle.putInt("min",		-1);
		dialog.setArguments(bundle);
		return	dialog;
	}


	public DialogTimePicker create(long _nativeHandle, int _hour, int _minute)
	{
		DialogTimePicker	dialog	= new DialogTimePicker();
		Bundle				bundle	= new Bundle();
		bundle.putLong("native",	_nativeHandle);
		bundle.putInt("hour",		_hour);
		bundle.putInt("min",		_minute);
		dialog.setArguments(bundle);
		return	dialog;
	}



	void updateTime(int _hourOfDay, int _minuteOfHour)
	{
		if (mTimePicker != null)
		{
			mTimePicker.updateTime(_hourOfDay, _minuteOfHour);
		}
		mHour			= _hourOfDay;
		mMinute			= _minuteOfHour;
	}

	@Override
	public Dialog onCreateDialog(Bundle savedInstanceState)
	{
		mNativeHandle	= getArguments().getLong("native");
	//	mHour			= getArguments().getInt("hour");
	//	mMinute			= getArguments().getInt("min");
		if (mHour == -1 || mMinute == -1)
		{
			Calendar			calendar	= Calendar.getInstance();
			int					hour		= calendar.get(Calendar.HOUR_OF_DAY);
			int					minute		= calendar.get(Calendar.MINUTE);
			if (mHour == -1)
			{
				mHour	= hour;
			}
			if (mMinute == -1)
			{
				mMinute	= minute;
			}
		}
		mTimePicker		= new TimePickerDialog(jp.libamtrs.ActivityInitializer.activity, this, mHour, mMinute, true);
		return	mTimePicker;
	}



	@Override
	public void onTimeSet(TimePicker view, int _hour, int _minute)
	{
		onTimeSet(mNativeHandle, _hour, _minute);
	}


	private native void onTimeSet(long _native, int _hour, int _minute);
}
