/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__graphics__Paint__hpp
#define	__libamtrs__android__java_classes__android__graphics__Paint__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::graphics {

struct	Rect;

AMTRS_JAVA_DEFINE_CLASS(Paint, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Paint");


	AMTRS_JAVA_DEFINE_CLASS(FontMetrics, java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Paint$FontMetrics");

		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(float, top)
			AMTRS_JAVA_DEFINE_FIELD(float, ascent)
			AMTRS_JAVA_DEFINE_FIELD(float, descent)
			AMTRS_JAVA_DEFINE_FIELD(float, bottom)
			AMTRS_JAVA_DEFINE_FIELD(float, leading)
		};
	};


	AMTRS_JAVA_DEFINE_CLASS(Style, java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Paint$Style");

		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;

			AMTRS_JAVA_DEFINE_FIELD(Style, FILL)
			AMTRS_JAVA_DEFINE_FIELD(Style, FILL_AND_STROKE)
			AMTRS_JAVA_DEFINE_FIELD(Style, STROKE)
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};
	};


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(breakText,
			jint(java::lang::String _text, jboolean _measureForwards, jfloat _maxWidth, jfloatArray _measuredWidth)
		)

		AMTRS_JAVA_DEFINE_METHOD(getTextBounds,
			void(java::lang::String _text, jint _start, jint _end, Rect _bounds)
		)

		AMTRS_JAVA_DEFINE_METHOD(getTextSize,
			jfloat()
		)

		//! public FontMetrics getFontMetrics();
		//! public float getFontMetrics(FontMetrics);
		AMTRS_JAVA_DEFINE_METHOD(getFontMetrics,
			FontMetrics(),
			float(FontMetrics)
		)

		//! public float measureText(String text);
		AMTRS_JAVA_DEFINE_METHOD(measureText,
			jfloat(java::lang::String _text)
		)

		//! public void setAntiAlias(boolean aa);
		AMTRS_JAVA_DEFINE_METHOD(setAntiAlias,
			void(jboolean)
		)

		//! public void setColor(int color);
		AMTRS_JAVA_DEFINE_METHOD(setColor,
			void(jint color)
		)

		//! public void setStrokeWidth(jfloat width);
		AMTRS_JAVA_DEFINE_METHOD(setStrokeWidth,
			void(jfloat width)
		)

		//! public void setStyle(Style style);
		AMTRS_JAVA_DEFINE_METHOD(setStyle,
			void(Style style)
		)

		//! public void setARGB(int a, int r, int g, int b);
		AMTRS_JAVA_DEFINE_METHOD(setARGB,
			void(int a, int r, int g, int b)
		)


		//! public void setTextSize(float textSize);
		AMTRS_JAVA_DEFINE_METHOD(setTextSize,
			void(float)
		)
	};


	AMTRS_JAVA_DEFINE_CLASS(Config, java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/graphics/Bitmap$Config");
	};

};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
