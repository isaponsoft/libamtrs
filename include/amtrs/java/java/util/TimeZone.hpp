/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__java__util_TimeZone__hpp
#define	__libamtrs__android__java_classes__java__util_TimeZone__hpp
#include "../../java/lang/Enum.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace java::util {

AMTRS_JAVA_DEFINE_CLASS(TimeZone, java::lang::Object)
{
	using	String				= java::lang::String;
	AMTRS_JAVA_CLASS_SIGNATURE("java/util/TimeZone");



	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint, LONG)
		AMTRS_JAVA_DEFINE_FIELD(jint, SHORT)


		AMTRS_JAVA_DEFINE_INIT(
			  TimeZone()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDefault
			, TimeZone()
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getDisplayName
			, String(jboolean daylightTime, jint style)
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getID
			, String()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
