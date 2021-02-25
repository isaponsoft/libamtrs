/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__jc__java__io__File__hpp
#define	__libamtrs__java__jc__java__io__File__hpp
#include <amtrs/java/java/lang.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::io {

// https://developer.android.com/reference/java/io/File.html
AMTRS_JAVA_DEFINE_CLASS(File , java::lang::Object)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("java/io/File");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(getAbsolutePath
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getPath
			, String()
		)

	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
