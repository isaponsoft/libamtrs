/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__power_android__hpp
#define	__libamtrs__system__power_android__hpp
#include "android-api-android_app.hpp"
#include "../java/android/view/Window.hpp"
#include "../java/android/view/WindowManager.hpp"
AMTRS_NAMESPACE_BEGIN

class	power_android : public power
{
public:
	//! 画面のパワーを常にONするように指定します。
	virtual void keep_screen_power(bool _enable) override
	{
		auto	activity	= os::android::get_activity();
		jint	flg			= java::classes::android::view::WindowManager::LayoutParams::find().get_FLAG_KEEP_SCREEN_ON();
		if (_enable)
		{
			activity.getWindow().addFlags(flg);
		}
		else
		{
			activity.getWindow().clearFlags(flg);
		}
	}
};

inline ref<power> power::create()
{
	ref<power_android>	retval	= new power_android();
	return	retval;
}

AMTRS_NAMESPACE_END
#endif
