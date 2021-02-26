/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
package jp.libamtrs.window;
import android.content.Context;
import android.widget.*;
import android.view.inputmethod.*;
import android.view.*;
import android.graphics.Color;
import android.view.Gravity;
import android.text.InputType;
import android.text.Editable;
import android.text.TextWatcher;
import java.lang.CharSequence;
import jp.libamtrs.*;

public class	DialogTextInput extends RelativeLayout implements View.OnClickListener, View.OnKeyListener, TextWatcher
{
	class	EditTextEx extends EditText
	{
		public EditTextEx(Context _context)
		{
			super(_context);
		}

		@Override
		public boolean onKeyPreIme(int keyCode, KeyEvent event)
		{
			if (keyCode == KeyEvent.KEYCODE_BACK)
			{
				DialogTextInput.this.cancel();
				return	true;
			}
			return	super.onKeyPreIme(keyCode, event);
		}

	}

	private	long			mNativeHandle;
	private	Context			mContext;
	private	EditTextEx		mEdit;
	private	Button			mButton;
	private	LinearLayout	mBox;
	private	boolean			mClosed	= false;

	public DialogTextInput(Context _context, long _nativeHandle)
	{
		super(_context);
		mNativeHandle	= _nativeHandle;
		mContext		= _context;
		setBackgroundColor(android.graphics.Color.argb(127, 0, 0, 0));

		setEnabled(true);
		setFocusable(true);
		setOnKeyListener(this);

		{
			RelativeLayout.LayoutParams	lp	= new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
			setLayoutParams(lp);
		}


		mBox	= new LinearLayout(_context);
		mBox.setOrientation(LinearLayout.HORIZONTAL);
		mBox.setBackgroundColor(android.graphics.Color.argb(255, 255, 255, 255));
		{
			RelativeLayout.LayoutParams	lp	= new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
			lp.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
			mBox.setLayoutParams(lp);
		}
		addView(mBox);


		{
			mEdit	= new EditTextEx(_context);
			mEdit.addTextChangedListener(this);
			mEdit.setOnKeyListener(this);
			LinearLayout.LayoutParams	lp	= new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
			lp.weight	= 1;
			mEdit.setLayoutParams(lp);
			mEdit.setImeOptions(EditorInfo.IME_ACTION_DONE|EditorInfo.IME_FLAG_NO_FULLSCREEN);
			mBox.addView(mEdit);
		}
		{
			mButton = new Button(_context);
			mButton.setText("OK");
			mButton.setBackgroundColor(android.graphics.Color.argb(0, 255, 255, 255));
			mButton.setOnClickListener(this);
			mBox.addView(mButton);
		}
	}


	@Override
	public boolean onTouchEvent(MotionEvent motionEvent)
	{
		if (androidx.core.view.MotionEventCompat.getActionMasked(motionEvent) == MotionEvent.ACTION_DOWN)
		{
			cancel();
		}
		return	true;
	}

	@Override
	public boolean onKey(View v, int keyCode, KeyEvent event)
	{
		if (keyCode == KeyEvent.KEYCODE_BACK)
		{
			cancel();
			return true;
		}
		return false;
	}

	public void setText(String text)
	{
		mEdit.setText(text);
		mEdit.setSelection(text.length());
		imeOn();
	}

	public void cancel()
	{
		close();
	}

	public void close()
	{
		if (!mClosed)
		{
			mClosed	= true;
			post(new Runnable()
			{
				@Override
				public void run()
				{
					_close();
				}
			});
		}
	}

	private void _close()
	{
		InputMethodManager	imm	= ((InputMethodManager)getContext().getSystemService(Context.INPUT_METHOD_SERVICE));
		imm.showSoftInput(mEdit, 0);
		imm.hideSoftInputFromWindow(mEdit.getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
		((ViewGroup)getParent()).removeView(this);
	}

	private void imeOn()
	{
		post(new Runnable()
		{
			@Override
			public void run()
			{
				mEdit.setEnabled(true);
				mEdit.setFocusable(true);
				mEdit.setFocusableInTouchMode(true);
				mEdit.requestFocus();
				InputMethodManager	imm	= ((InputMethodManager)getContext().getSystemService(Context.INPUT_METHOD_SERVICE));
				imm.showSoftInput(mEdit, InputMethodManager.SHOW_IMPLICIT);
			}
		});
	}

	@Override
	public void onClick(View view)
	{
		onEnter(mNativeHandle);
		close();
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
		onTextChange(mNativeHandle, s.toString());
	}

	native private void onTextChange(long _nativeHandle, String _text); 
	native private void onEnter(long _nativeHandle); 

}
