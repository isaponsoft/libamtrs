package jp.libamtrs.window;
import android.content.Context;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.view.inputmethod.InputMethodManager;
import android.view.KeyEvent;
import android.view.ViewGroup;
import android.graphics.Color;
import android.view.Gravity;
import android.text.InputType;
import android.text.Editable;
import android.text.TextWatcher;
import java.lang.CharSequence;

public class	DialogTextInput extends EditText implements TextWatcher
{
	private	long	mNativeHandle;


	public DialogTextInput(Context _context, long _nativeHandle)
	{
		super(_context);
		mNativeHandle	= _nativeHandle;
		addTextChangedListener(this);
		setBackgroundColor(Color.WHITE);
		setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_NORMAL);

		setGravity(Gravity.TOP);
		imeShow(true);
	}


	public void imeShow(boolean _visible)
	{
		if (_visible)
		{
			post(new Runnable()
			{
				@Override
				public void run()
				{
					InputMethodManager	imm	= ((InputMethodManager)getContext().getSystemService(Context.INPUT_METHOD_SERVICE));
					requestFocus();
					imm.showSoftInput(DialogTextInput.this, InputMethodManager.SHOW_IMPLICIT);
				}
			});
		}
		else
		{
			post(new Runnable()
			{
				@Override
				public void run()
				{
					InputMethodManager	imm	= ((InputMethodManager)getContext().getSystemService(Context.INPUT_METHOD_SERVICE));
					imm.showSoftInput(DialogTextInput.this, 0);
					imm.hideSoftInputFromWindow(DialogTextInput.this.getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
				}
			});
		}
	}


	public void close()
	{
		imeShow(false);
		post(new Runnable() {
			@Override
			public void run() {
				((ViewGroup)getParent()).removeView(DialogTextInput.this);
			}
		});
	}


	@Override
	public boolean onKeyDown(int _keyCode, KeyEvent _event)
	{
		onKeyDown(mNativeHandle, _keyCode, _event);
		return	super.onKeyDown(_keyCode, _event);
	}


	@Override
	public void beforeTextChanged(CharSequence s, int start, int count, int after)
	{
	}

 
	@Override
	public void onTextChanged(CharSequence s, int start, int before, int count)
	{
	}


	@Override
	public void afterTextChanged(Editable s)
	{
		onAfterTextChanged(mNativeHandle, s.toString());
	}

	native private boolean onKeyDown(long _nativeHandle, int _keyCode, KeyEvent _event); 
	native private void onAfterTextChanged(long _nativeHandle, String _txt); 
}
