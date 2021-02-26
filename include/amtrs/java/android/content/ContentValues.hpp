/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__content__ContentValues__hpp
#define	__libamtrs__android__java_classes__android__content__ContentValues__hpp
#include "../../java/lang/String.hpp"
#include "../../java/lang/Byte.hpp"
#include "../../java/lang/Boolean.hpp"
#include "../../java/lang/Double.hpp"
#include "../../java/lang/Float.hpp"
#include "../../java/lang/Integer.hpp"
#include "../../java/lang/Long.hpp"
#include "../../java/lang/Short.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::lang {
struct	Boolean;
struct	Byte;
struct	Double;
struct	Float;
struct	Integer;
struct	Long;
struct	Short;
}
namespace android::content {


AMTRS_JAVA_DEFINE_CLASS(ContentValues , java::lang::Object)
{
	using	Boolean	= java::lang::Boolean;
	using	Byte	= java::lang::Byte;
	using	Double	= java::lang::Double;
	using	Float	= java::lang::Float;
	using	Integer	= java::lang::Integer;
	using	Long	= java::lang::Long;
	using	Short	= java::lang::Short;
	using	String	= java::lang::String;


	AMTRS_JAVA_CLASS_SIGNATURE("android/content/ContentValues");

	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String, TAG)

		AMTRS_JAVA_DEFINE_INIT(
			  void()
			, void(jint size)
			, void(ContentValues from)
		)

	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(
			clear,
			void()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			containsKey,
			jboolean(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			describeContents,
			jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(
			get,
			Object(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsBoolean,
			Boolean(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsByte,
			Byte(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsByteArray,
			jbyte*(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsDouble,
			Double(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsFloat,
			Float(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsInteger,
			Integer(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsLong,
			Long(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsShort,
			Short(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			getAsString,
			String(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			put
			, void(String key, Short value)
			, void(String key, Long value)
			, void(String key, Double value)
			, void(String key, Integer value)
			, void(String key, String value)
			, void(String key, Boolean value)
			, void(String key, Float value)
			, void(String key, jbyte value[])
			, void(String key, Byte value)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			putAll,
			void(ContentValues other)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			putNull,
			void(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			remove,
			void(String key)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			size,
			jint()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
