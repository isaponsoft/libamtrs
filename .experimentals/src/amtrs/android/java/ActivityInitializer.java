/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
package jp.libamtrs;
import	android.content.Context;
import	android.os.Bundle;
import	android.os.Handler;
import	android.os.Looper;
import	android.view.KeyEvent;
import	android.view.MotionEvent;
import	android.view.View;
import	android.view.ViewGroup;
import	android.widget.FrameLayout;


public class	ActivityInitializer
{
	public static	Context	activity;

	public native static long initialize(AmtrsActivity _activity);
}
