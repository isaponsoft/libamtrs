/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__androidx__core__app__ActivityCompat__hpp
#define	__libamtrs__android__java_classes__androidx__core__app__ActivityCompat__hpp
#include "../content/ContextCompat.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace java::lang {
struct	String;
}

namespace android::app {
struct	Activity;
}

namespace android::content {
struct	Context;
struct	Intent;
}

namespace android::net {
struct	Uri;
}

namespace android::os {
struct	Bundle;
}

namespace androidx::core::app {


AMTRS_JAVA_DEFINE_CLASS(ActivityCompat, androidx::core::content::ContextCompat)
{
	using	Activity	= android::app::Activity;
	using	Bundle		= android::os::Bundle;
	using	Context		= android::content::Context;
	using	Intent		= android::content::Intent;
	using	String		= java::lang::String;
	using	Uri			= android::net::Uri;

	AMTRS_JAVA_CLASS_SIGNATURE("androidx/core/app/ActivityCompat");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(finishAffinity
				, void(Activity activity)
			)

		AMTRS_JAVA_DEFINE_METHOD(	finishAfterTransition
				, void(Activity activity)
			)

		AMTRS_JAVA_DEFINE_METHOD(getReferrer
				, Uri(Activity activity)
			)

		AMTRS_JAVA_DEFINE_METHOD(requestPermissions
				, void(Activity activity, String permissions[], int requestCode)
			)

		AMTRS_JAVA_DEFINE_METHOD(shouldShowRequestPermissionRationale
				, jboolean(Activity activity, String permission)
			)

		AMTRS_JAVA_DEFINE_METHOD(startActivityForResult
				, void(Activity activity, Intent intent, jint requestCode, Bundle options)
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
