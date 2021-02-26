/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__jp__libamtrs__alarm__AlarmReceiver__hpp
#define	__libamtrs__android__java_classes__jp__libamtrs__alarm__AlarmReceiver__hpp
#include <amtrs/java/android/content/BroadcastReceiver.hpp>
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::app {
struct	Activity;
}


namespace jp::libamtrs::alarm {

AMTRS_JAVA_DEFINE_CLASS(AlarmReceiver, android::content::BroadcastReceiver)
{
	using	Activity		= android::app::Activity;
	using	Class			= java::lang::Class;
	using	String			= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("jp/libamtrs/alarm/AlarmReceiver");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(addAlarm
			, void(Activity, Class receiver, jlong time, jint id, String title, String message, String channelId, String channelTitle, jint wakeup)
		)

		AMTRS_JAVA_DEFINE_METHOD(cancelAll
			, void(Activity, Class receiver)
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
