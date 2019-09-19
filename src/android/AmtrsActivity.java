package jp.libamtrs;
import	android.content.Intent;
import	android.os.Bundle;
import	android.os.Handler;
import	android.os.Looper;
import	android.view.KeyEvent;
import	android.view.MotionEvent;
import	android.view.View;
import	android.view.ViewGroup;
import	android.view.WindowManager;
import	android.widget.FrameLayout;

import	android.os.Handler;
import	android.os.Looper;
import	java.lang.Runnable;

import	java.util.HashMap;


public class AmtrsActivity extends android.support.v4.app.FragmentActivity 
{
	private	FrameLayout									mContentsFrame;
	private	long										mNativeHandle;
	private	Handler										mLoopHandler;
	private	Runnable									mUpdater;
	private	boolean										mIsForeground;
	private	HashMap<Integer, AmtrsActivityResult>		mActivityResult	= new HashMap<Integer, AmtrsActivityResult>();


	@Override
	protected void onCreate(Bundle _savedInstanceState)
	{
		super.onCreate(_savedInstanceState);
		ActivityInitializer.activity	= this;

		mNativeHandle	= ActivityInitializer.initialize(this);
		mContentsFrame	= new FrameLayout(this);
		setContentView(mContentsFrame);
		onCreate(mNativeHandle);

		mLoopHandler	= new Handler(Looper.getMainLooper());
		mUpdater		= new Runnable()
		{
			private	int	loopCount	= 0;

			@Override
			public void run()
			{
				long	thiz	= mNativeHandle;
				if (thiz != 0)
				{
					onLooper(thiz);
					mLoopHandler.post(this);
				}
			//	java.lang.Runtime.getRuntime().gc();
			}
		};
		mLoopHandler.post(mUpdater);
	}

	void openDocument(AmtrsActivityResult _interface, String _mime)
	{
		Intent	intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
		intent.setType(_mime);
		intent.addCategory(Intent.CATEGORY_OPENABLE);
		startActivityForResult(Intent.createChooser(intent, "Open a file"), addActivityResult(_interface));
	}




	// return
	//	requestCode.
	int addActivityResult(AmtrsActivityResult _interface)
	{
		int	requestCode	= _interface.hashCode() & 0xffff;
		while (mActivityResult.containsKey(new Integer(requestCode)))
		{
			requestCode = (requestCode + 1) & 0xffff;;
		}
		mActivityResult.put(new Integer(requestCode), _interface);
		return	requestCode;
	}


	@Override
	protected void onActivityResult(int _requestCode, int _resultCode, Intent _data)
	{
		super.onActivityResult(_requestCode, _resultCode, _data);
		AmtrsActivityResult	aar	= (AmtrsActivityResult)mActivityResult.get(_requestCode);
		if (aar != null)
		{
			aar.onActivityResult(_requestCode, _resultCode, _data);
			mActivityResult.remove(_requestCode);
		}
	}


	public void foreground()
	{
		if (!mIsForeground)
		{
			startActivity(new Intent(getApplicationContext(), ActivityInitializer.activity.getClass()));
		}
	}

	public void pushView(View _view)
	{
		mContentsFrame.addView(
			_view,
			new ViewGroup.LayoutParams(
				ViewGroup.LayoutParams.MATCH_PARENT,
				ViewGroup.LayoutParams.FILL_PARENT
		));
		_view.setFocusable(true);
		_view.setFocusableInTouchMode(true);
		_view.requestFocus();
	}


	public void popView()
	{
		Handler	handler	= new Handler();
		handler.post(new Runnable() {
			@Override
			public void run() {
				int	last	= mContentsFrame.getChildCount() - 1;
				if (last >= 0)
				{
					mContentsFrame.removeViewAt(last);
				}
			}
		});
	}


	@Override
	public void onStart()
	{
		super.onStart();
		onStart(mNativeHandle);
	}

	@Override
	public void onRestart()
	{
		super.onPause();
		onRestart(mNativeHandle);
	}

	@Override
	public void onResume()
	{
		super.onResume();
		onResume(mNativeHandle);
		mIsForeground	= true;
	}

	@Override
	public void onPause()
	{
		onPause(mNativeHandle);
		super.onPause();
	}

	@Override
	public void onStop()
	{
		mIsForeground	= false;
		onStop(mNativeHandle);
		super.onStop();
	}

	@Override
	public void onDestroy()
	{
		onDestroy(mNativeHandle);
		mLoopHandler.removeCallbacks(mUpdater);
		mLoopHandler	= null;
		mNativeHandle	= 0;
		super.onDestroy();
	}


	private native void onCreate(long _nativeThis);
	private native void onRestart(long _nativeThis);
	private native void onStart(long _nativeThis);
	private native void onResume(long _nativeThis);
	private native void onPause(long _nativeThis);
	private native void onStop(long _nativeThis);
	private native void onDestroy(long _nativeThis);
	private native void onLooper(long _nativeThis);
}
