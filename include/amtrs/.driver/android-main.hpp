/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__main__main_android__hpp
#define	__libamtrs__main__main_android__hpp
#include "android-api-android_app.hpp"
#include "android-api-amtrs_activity.hpp"



#define	AMTRS_GUI_MAIN(_mainclass)	\
	extern "C" jlong _libamtrs_gui_main(amtrs_application_class* _activity)\
	{\
		auto	app = _mainclass;\
		amtrs::os::android::amtrs_activity::set_application(app);\
		return amtrs::os::android::amtrs_activity::initialize(reinterpret_cast<_jobject*>(_activity));\
	};

#endif
