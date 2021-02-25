/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__graphics__Typeface__hpp
#define	__libamtrs__android__java_classes__android__graphics__Typeface__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::content::res {
struct	AssetManager;
}
namespace java::io {
struct	File;
}
namespace android::graphics {


AMTRS_JAVA_DEFINE_CLASS(Typeface, java::lang::Object)
{
	using	AssetManager		= android::content::res::AssetManager;
	using	File				= java::io ::File;
	using	String				= java::lang::String;
	using	Object				= java::lang::Object;


	AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Typeface");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(create,
			Typeface(Typeface _family, jint _weight, jboolean _italic),
			Typeface(Typeface _family, jint _style),
			Typeface(String _familyName, jint _style)
		)

		AMTRS_JAVA_DEFINE_METHOD(createFromAsset,
			Typeface(AssetManager _mgr, String _path)
		)

		AMTRS_JAVA_DEFINE_METHOD(createFromFile,
			Typeface(File _file),
			Typeface(String _path)
		)

		AMTRS_JAVA_DEFINE_METHOD(defaultFromStyle,
			Typeface(jint _style)
		)

		AMTRS_JAVA_DEFINE_FIELD(jint, BOLD)
		AMTRS_JAVA_DEFINE_FIELD(jint, BOLD_ITALIC)
		AMTRS_JAVA_DEFINE_FIELD(jint, ITALIC)
		AMTRS_JAVA_DEFINE_FIELD(jint, NORMAL)

		AMTRS_JAVA_DEFINE_FIELD(Typeface, DEFAULT)
		AMTRS_JAVA_DEFINE_FIELD(Typeface, DEFAULT_BOLD)
		AMTRS_JAVA_DEFINE_FIELD(Typeface, MONOSPACE)
		AMTRS_JAVA_DEFINE_FIELD(Typeface, SANS_SERIF)
		AMTRS_JAVA_DEFINE_FIELD(Typeface, SERIF)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getStyle,
			jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getWeight,
			jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(hashCode,
			jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(isBold,
			jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isItalic,
			jboolean()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
