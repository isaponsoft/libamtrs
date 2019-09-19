/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__dalvik_systm_VMDebug__hpp
#define	__libamtrs__android__java_classes__android__dalvik_systm_VMDebug__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace dalvik::system {


AMTRS_JAVA_DEFINE_CLASS(VMDebug, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("dalvik/system/VMDebug");




	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		//! 参照テーブルをダンプして logcat へ出力します。
		//! 実機では無視されます。開発用エミューレータなどを使用してください。
		AMTRS_JAVA_DEFINE_METHOD(dumpReferenceTables,
			void()
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
