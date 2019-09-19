package jp.libamtrs.window;
import android.app.Dialog;
import android.app.DatePickerDialog;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;
import android.widget.DatePicker;
import java.util.Calendar;
import android.content.DialogInterface;
 

public class	DialogDatePicker
				extends		DialogFragment
				implements	DatePickerDialog.OnDateSetListener
{
	private	long				mNativeHandle;
	private	int					mYear		= -1;
	private	int					mMon		= -1;
	private	int					mDay		= -1;
	private	DatePickerDialog	mDatePicker;



	public static DialogDatePicker create(long _nativeHandle)
	{
		DialogDatePicker	dialog	= new DialogDatePicker();
		Bundle				bundle	= new Bundle();
		bundle.putLong("native",	_nativeHandle);
		bundle.putInt("year",		-1);
		bundle.putInt("month",		-1);
		bundle.putInt("day",		-1);
		dialog.setArguments(bundle);
		return	dialog;
	}


	public DialogDatePicker create(long _nativeHandle, int _year, int _mon, int _day)
	{
		DialogDatePicker	dialog	= new DialogDatePicker();
		Bundle				bundle	= new Bundle();
		bundle.putLong("native",	_nativeHandle);
		bundle.putInt("year",		_year);
		bundle.putInt("month",		_mon);
		bundle.putInt("day",		_day);
		dialog.setArguments(bundle);
		return	dialog;
	}



	void updateDate(int _year, int _mon, int _day)
	{
		if (mDatePicker != null)
		{
			mDatePicker.updateDate(_year, _mon, _day);
		}
		mYear			= _year;
		mMon			= _mon;
		mDay			= _day;
	}

	@Override
	public Dialog onCreateDialog(Bundle savedInstanceState)
	{
		mNativeHandle	= getArguments().getLong("native");
	//	mYear			= getArguments().getInt("year");
	//	mMon			= getArguments().getInt("month");
	//	mDay			= getArguments().getInt("day");
		if (mYear == -1 || mMon == -1 || mDay == -1)
		{
			Calendar			calendar	= Calendar.getInstance();
			int					year		= calendar.get(Calendar.YEAR);
			int					mon			= calendar.get(Calendar.MONTH);
			int					day			= calendar.get(Calendar.DAY_OF_MONTH);
			if (mYear == -1)
			{
				mYear	= year;
			}
			if (mMon == -1)
			{
				mMon	= mon;
			}
			if (mDay == -1)
			{
				mDay	= day;
			}
		}
		mDatePicker		= new DatePickerDialog(jp.libamtrs.ActivityInitializer.activity, this, mYear, mMon, mDay);
		return	mDatePicker;
	}



	@Override
	public void onDateSet(DatePicker view, int _year, int _mon, int _day)
	{
		onDateSet(mNativeHandle, _year, _mon, _day);
	}


	private native void onDateSet(long _native, int _year, int _mon, int _day);
}
