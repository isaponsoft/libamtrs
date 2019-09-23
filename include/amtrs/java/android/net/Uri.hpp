/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__net__Uri__hpp
#define	__libamtrs__android__java_classes__android__net__Uri__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::net {


AMTRS_JAVA_DEFINE_CLASS(Uri, java::lang::Object)
{
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/net/Uri");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(parse
			, Uri(String uriString)
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
