/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__database__Cursor__hpp
#define	__libamtrs__android__java_classes__android__database__Cursor__hpp
#include "../../java/lang/String.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::os {
struct	Bundle;
}
namespace android::database {

struct	CharArrayBuffer;

AMTRS_JAVA_DEFINE_CLASS(Cursor, java::lang::Object)
{
	using	Bundle	= android::os::Bundle;
	using	String	= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("android/database/Cursor");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint,		FIELD_TYPE_BLOB)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FIELD_TYPE_FLOAT)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FIELD_TYPE_INTEGER)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FIELD_TYPE_NULL)
		AMTRS_JAVA_DEFINE_FIELD(jint,		FIELD_TYPE_STRING)
	};

	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(close
				, void()
			)

		AMTRS_JAVA_DEFINE_METHOD(copyStringToBuffer
				, void(jint columnIndex, CharArrayBuffer buffer)
			)

		AMTRS_JAVA_DEFINE_METHOD(deactivate
				, void()
			)
/*
		AMTRS_JAVA_DEFINE_METHOD(getBlob
				, jbyte[](jint columnIndex)
			)
*/
		AMTRS_JAVA_DEFINE_METHOD(getColumnCount
				, jint()
			)

		AMTRS_JAVA_DEFINE_METHOD(getColumnIndex
				, jint(String columnName)
			)

		AMTRS_JAVA_DEFINE_METHOD(getColumnIndexOrThrow
				, jint(String columnName)
			)

		AMTRS_JAVA_DEFINE_METHOD(getColumnName
				, String(jint columnIndex)
			)
/*
		AMTRS_JAVA_DEFINE_METHOD(getColumnNames
				, String[]()
			)
*/
		AMTRS_JAVA_DEFINE_METHOD(getCount
				, jint()
			)

		AMTRS_JAVA_DEFINE_METHOD(getDouble
				, jdouble(jint columnIndex)
			)

		AMTRS_JAVA_DEFINE_METHOD(getExtras
				, Bundle()
			)

		AMTRS_JAVA_DEFINE_METHOD(getFloat
				, jfloat(jint columnIndex)
			)

		AMTRS_JAVA_DEFINE_METHOD(getInt
				, jint(jint columnIndex)
			)

		AMTRS_JAVA_DEFINE_METHOD(getLong
				, jlong(jint columnIndex)
			)

		AMTRS_JAVA_DEFINE_METHOD(getPosition
				, jint()
			)

		AMTRS_JAVA_DEFINE_METHOD(getShort
				, jshort(jint columnIndex)
			)

		AMTRS_JAVA_DEFINE_METHOD(getString
				, String(jint columnIndex)
			)

		AMTRS_JAVA_DEFINE_METHOD(getType
				, jint(jint columnIndex)
			)

		AMTRS_JAVA_DEFINE_METHOD(getWantsAllOnMoveCalls
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(isAfterLast
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(isBeforeFirst
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(isClosed
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(isFirst
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(isLast
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(isNull
				, jboolean(jint columnIndex)
			)

		AMTRS_JAVA_DEFINE_METHOD(move
				, jboolean(jint offset)
			)

		AMTRS_JAVA_DEFINE_METHOD(moveToFirst
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(moveToLast
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(moveToNext
				, jboolean()
			)

		AMTRS_JAVA_DEFINE_METHOD(moveToPosition
				, jboolean(jint position)
			)

		AMTRS_JAVA_DEFINE_METHOD(moveToPrevious
				, jboolean()
			)
	};

};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
