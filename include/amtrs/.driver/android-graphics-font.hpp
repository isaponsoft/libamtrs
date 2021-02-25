/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__bitmaps__font_android__hpp
#define	__libamtrs__bitmaps__font_android__hpp
#include "../window.hpp"
#include "../graphics.hpp"
#include "../iostream.hpp"
#include "../jni.hpp"
#include "../string.hpp"
#include "../java/android/graphics/Bitmap.hpp"
#include "../java/android/graphics/Canvas.hpp"
#include "../java/android/graphics/Rect.hpp"
#include "../java/android/graphics/Typeface.hpp"
#include "../java/android/text/TextPaint.hpp"



AMTRS_NAMESPACE_BEGIN

//! このクラスはプラットフォームの機能を利用します。
class	android_font
		: public font
{
public:
	// https://material.io/design/typography/#type-scale
	static constexpr float	textSize_body		= 14;			// 14sp.

	using	Bitmap				= amtrs::java::classes::android::graphics::Bitmap;
	using	Canvas				= amtrs::java::classes::android::graphics::Canvas;
	using	Rect				= amtrs::java::classes::android::graphics::Rect;
	using	Typeface			= amtrs::java::classes::android::graphics::Typeface;
	using	TextPaint			= amtrs::java::classes::android::text::TextPaint;
	using	FontMetrics			= amtrs::java::classes::android::graphics::Paint::FontMetrics;
	using	FontMetricsObject	= amtrs::java::classes::android::graphics::Paint::FontMetricsObject;
	using	String				= amtrs::java::classes::java::lang::String;

	template<class... Args>
	android_font(const char* _fontname, font_size_type _default_size, pixel_type _default_color)
		: font(_fontname, _default_size, _default_color)
	{
		if (_fontname)
		{
			mFontname	= _fontname;
		}
	}

	template<class... Args>
	android_font(family_type _family, font_size_type _default_size, pixel_type _default_color)
		: font(nullptr, _default_size, _default_color)
	{
		mFamily		= _family;
		mTypeface	= Typeface::find().get_MONOSPACE();
	}


	~android_font()
	{
		release();
	}


	void update()
	{
		if (mTextPaint.empty())
		{
			auto		obj		= TextPaint::init();
			obj.setAntiAlias((jboolean)true);
			mTextPaint		= obj;
		}

		if (mCurrentFontSize != mFontSize)
		{
			java::jobj<TextPaint>	obj(mTextPaint);

			obj.setTextSize((jfloat)mFontSize);
			mCurrentFontSize	= mFontSize;

			auto	metrics	= obj.getFontMetrics();
			mFontMetrics	= metrics;
			mAscent			= metrics.get_ascent();
			mDescent		= metrics.get_descent();
		}
	}


	virtual auto get_metrics() -> metrics override
	{
		metrics	retval;
		update();
		retval.ascent	= mAscent;
		retval.descent	= mDescent;
		return	retval;
	}


	virtual void on_draw(bitmap<pixel_type>& _out, vec2<int> _origin, size2<int>* _clip, std::string_view _text, const draw_info& _info) override
	{
		update();

		#if	DEBUG
		if (_out.size().width <= 0 || _out.size().height <= 0)
		{
			std::string	t(_text);
			AMTRS_DEBUG_LOG("out bitmap size is <= 0. size = %dx%d, text = %s", _out.size().width, _out.size().height, t.c_str());
		}
		#endif

		JNIEnv*		env			= java::get_jnienv();

		// 文字列
		auto		text		= java::jobj<String>(java::to_jstring(_text));


		// 描画用のBitmapを用意
		auto		conf		= Bitmap::Config::find().get_ARGB_8888();
		auto		objBitmap	= Bitmap::find()
								. createBitmap((int)_out.size().width, (int)_out.size().height, conf);

		// キャンバスを作成
		auto		canvas		= Canvas::init(objBitmap);

		// TextPaintを作成
		auto		tp			= java::jobj<TextPaint>(mTextPaint);
		if (!mTypeface.empty())
		{
			tp.setTypeface(java::jobj<Typeface>(mTypeface));
		}


		// 枠線ありの場合
		if (_info.edgeSize > 0)
		{
			tp.setStrokeWidth(_info.edgeSize);
			tp.setStyle(TextPaint::Style::find().get_STROKE());
			tp.setARGB(_info.edgeColor.a, _info.edgeColor.r, _info.edgeColor.g, _info.edgeColor.b);

			canvas.drawText(text, (jfloat)_origin.x, (jfloat)_origin.y, tp);

			tp.setStrokeWidth(0);
			tp.setStyle(TextPaint::Style::find().get_FILL());
		}

		tp.setARGB(_info.color.a, _info.color.r, _info.color.g, _info.color.b);
		canvas.drawText(text, (jfloat)_origin.x, (jfloat)_origin.y, tp);

		auto		cr		= Rect::init();
		tp.getTextBounds(text, 0, text.length(), cr);

		rect<int>	clip		= {
			(int)_origin.x + (int)cr.get_left() - _info.edgeSize/2,
			(int)_origin.y + (int)cr.get_top() - _info.edgeSize/2,
			(int)cr.get_right()  - (int)cr.get_left() + _info.edgeSize,
			(int)cr.get_bottom() - (int)cr.get_top() + _info.edgeSize
		};

		transfer_bitmap(_out, env, objBitmap.get(), clip);
	}

	void transfer_bitmap(bitmap<pixel_type>& _dest, JNIEnv* _env, jobject _bitmap, rect<int> _src)
	{
		// ピクセルを転送する
		AndroidBitmapInfo	info;
		AndroidBitmap_getInfo(_env, _bitmap, &info);

		void*	pixelsTop;
		switch (AndroidBitmap_lockPixels(_env, _bitmap, &pixelsTop))
		{
			case ANDROID_BITMAP_RESULT_SUCCESS  :
				break;
			case ANDROID_BITMAP_RESULT_BAD_PARAMETER   :
				AMTRS_DEBUG_LOG("ANDROID_BITMAP_RESULT_BAD_PARAMETER");
				break;
			case ANDROID_BITMAP_RESULT_JNI_EXCEPTION   :
				AMTRS_DEBUG_LOG("ANDROID_BITMAP_RESULT_JNI_EXCEPTION");
				break;
			case ANDROID_BITMAP_RESULT_ALLOCATION_FAILED   :
				AMTRS_DEBUG_LOG("ANDROID_BITMAP_RESULT_ALLOCATION_FAILED");
				break;
		}

		for (uint32_t y = _src.y; y < _src.y+_src.height; ++y)
		{
			rgba<uint8_t>*	pixels	= reinterpret_cast<rgba<uint8_t>*>(static_cast<char*>(pixelsTop) + info.stride * y) + _src.x;
			for (uint32_t x = _src.x; x < _src.x+_src.width; ++x)
			{
				_dest[vec2<uint32_t>(x, y)]	= *pixels++;
			}
		}
		AndroidBitmap_unlockPixels(_env, _bitmap);
	}


	virtual bounds compute_bounds(std::string_view _text) override
	{
		update();

		// 文字列
		auto		text		= java::jobj<String>(java::to_jstring(_text));

		// TextPaintを作成
		auto		tp			= java::jobj<TextPaint>(mTextPaint);

		// 値取得用
		auto		rect		= Rect::init();

		tp.getTextBounds(text, 0, text.length(), rect);

		bounds	retval;
		retval.area		= {(int)rect.get_left(), (int)rect.get_top(), (int)rect.get_right() - (int)rect.get_left(), (int)rect.get_bottom() - (int)rect.get_top()};
		retval.advance	= retval.area.width;
		return	retval;
	}

	virtual auto compute_braek(std::string_view _text, int _maxWidth) -> std::pair<typename std::string_view::size_type, int> override
	{
		update();

		// 文字列
		auto		text		= java::jobj<String>(java::to_jstring(_text));

		// TextPaintを作成
		auto		tp			= java::jobj<TextPaint>(mTextPaint);

		// 値取得用
		jint		ci			= tp.breakText(text, (jboolean)false, (jfloat)_maxWidth, (jfloatArray)nullptr);

		// キャラ数からバイト数へ変換
		auto*			cur		= _text.data();
		auto*			end		= cur + _text.size();
		while (cur < end && ci > 0)
		{
			int32_t	c;
			auto	nextCursor	= u8string_read_char(c, cur);
			if (!nextCursor)
			{
				++cur;
				continue;
			}
			cur	= nextCursor;
			--ci;
		}
		return	{(std::size_t)(cur - _text.data()), 0};
	}


	void release()
	{
		mTextPaint.clear();
		mFontMetrics.clear();
	}

	family_type						mFamily			= family_type::normal;
	std::string						mFontname;

	java::globalref<jobject>		mTextPaint;								//!< android.text.TextPaint
	java::globalref<jobject>		mFontMetrics;							//!< android.graphics.Paint.FontMetrics 
	java::globalref<jobject>		mTypeface;								//!< android.text.TextPaint

	//java::globalref<TextPaint>		mTextPaint;								//!< android.text.TextPaint
	//java::globalref<FontMetrics>	mFontMetrics;							//!< android.graphics.Paint.FontMetrics 
	font_size_type					mCurrentFontSize	= -1;				//!< 設定済みのフォントサイズ
	float							mAscent;
	float							mDescent;

	friend	ref<font> create(const char* _fontname, font_size_type _default_size, pixel_type _default_color);
};


inline ref<font> font::create(family_type _family, font_size_type _default_size, pixel_type _default_color)
{
	if (_default_size <= 0)
	{
		_default_size	= system_font_size();
	}
	
	ref<android_font>	f	= new android_font(_family, _default_size, _default_color);
	return	f;
}


inline ref<font> font::create(pixel_type _default_color)
{
	return	create(0, _default_color);
}


inline ref<font> font::create(font_size_type _default_size, pixel_type _default_color)
{
	if (_default_size <= 0)
	{
		_default_size	= system_font_size();
	}
	
	ref<android_font>	f	= new android_font(nullptr, _default_size, _default_color);
	return	f;
}


inline ref<font> font::create(const char* _fontname, font_size_type _default_size, pixel_type _default_color)
{
	ref<android_font>	f	= new android_font(_fontname, _default_size, _default_color);
	return	f;
}

inline auto font::system_font_size() -> font_size_type
{
	static int	fontSize	= -1;
	if (fontSize < 1)
	{
		auto	disp	= display::get_display();
		fontSize	= (int)(disp->pt_to_pixel_scale() * android_font::textSize_body);
	}
	return	fontSize;
}





AMTRS_NAMESPACE_END
#endif
