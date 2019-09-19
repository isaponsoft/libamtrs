package jp.libamtrs;
import	android.os.Bundle;
import	android.content.Intent;
import	android.content.Context;
import	android.support.v4.app.FragmentActivity;
import	android.app.PendingIntent;


public class AmtrsAlartActivity extends android.support.v4.app.FragmentActivity
{
	public static void alart(Context _context, Class _alartClass, String _title, String _message)
	{
		try
		{
			Intent			intent			= new Intent(_context, _alartClass);
			intent.putExtra("title",	_title);
			intent.putExtra("message",	_message);
			PendingIntent	pendingIntent	= PendingIntent.getActivity(_context, 0, intent, 0);
			pendingIntent.send();
		}
		catch (Exception e)
		{}
	}


	@Override
	protected void onCreate(Bundle _savedInstanceState)
	{
		super.onCreate(_savedInstanceState);
		Intent	intent			= getIntent();
		String	title			= intent.getStringExtra("title");
		String	message			= intent.getStringExtra("message");
		AlertDialogFragment.create(title, message).show(getSupportFragmentManager(), "amtrs");
		//finish();
	}

	public static class AlertDialogFragment extends android.support.v4.app.DialogFragment
	{
		public static AlertDialogFragment create(String _title, String _message)
		{
			AlertDialogFragment	dialog	= new AlertDialogFragment();
			Bundle				bundle	= new Bundle();
			bundle.putString("title",		_title);
			bundle.putString("message",		_message);
			dialog.setArguments(bundle);
			return	dialog;
		}

		@Override
		public android.app.Dialog onCreateDialog(Bundle savedInstanceState)
		{
			android.app.AlertDialog.Builder		builder	= new android.app.AlertDialog.Builder(getActivity());
			builder.setTitle(getArguments().getString("title"));
			builder.setMessage(getArguments().getString("message"));
			builder.setPositiveButton("OK", null);
			android.app.Dialog	dialog	= builder.create();
			dialog.setCanceledOnTouchOutside(true);
			return	dialog;
		}

		@Override
		public void onStop()
		{
			super.onStop();
			getActivity().finish();
			getActivity().moveTaskToBack(false);
		}
	}

}
