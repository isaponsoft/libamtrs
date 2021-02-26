/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__bitmaps__font_ios__hpp
#define	__libamtrs__bitmaps__font_ios__hpp
#import <UIKit/UIKit.h>
#include <type_traits>
#include "../../graphics/bitmap.hpp"


AMTRS_NAMESPACE_BEGIN

//! このクラスはプラットフォームの機能を利用します。
class	ios_font
		: public font
{
public:
	ios_font(const char* _fontname, font_size_type _default_size, pixel_type _default_color)
		: font(_fontname, _default_size, _default_color)
	{
		if (_fontname)
		{
			mFontname	= _fontname;
		}
	}

	UIFont*		mFont		= nullptr;
	float		mAscent		= 0;
	float		mDescent	= 0;


	void update()
	{
		if (!mFont)
		{
			mFont		= [UIFont systemFontOfSize: mFontSize];
			mAscent		= mFont.ascender;
			mDescent	= mFont.descender;
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


	virtual bounds compute_bounds(std::string_view _text) override
	{@autoreleasepool{
		update();

		std::string		cstr(_text);
		NSString*		text		= [NSString stringWithCString: cstr.c_str() encoding:NSUTF8StringEncoding];
		CGSize			textRect	= [text sizeWithAttributes:@{ NSFontAttributeName: mFont } ];
		bounds	retval;
		retval.area		= {
			(int)0,
			(int)0,
			(int)textRect.width,
			(int)textRect.height
		};
		retval.advance	= textRect.width;
		//AMTRS_DEBUG_LOG("(%d,%d)-(%dx%d) %s", retval.area.x, retval.area.y, retval.area.width, retval.area.height, cstr.c_str());
		return	retval;
	}}


	virtual auto compute_braek(std::string_view _text, int _maxWidth) -> std::pair<typename std::string_view::size_type, int> override
	{@autoreleasepool{
		update();

		std::size_t		pos		= 0;
		float			advance	= 0;

		std::string		cstr(_text);
		NSString*		text		= [NSString stringWithCString: cstr.c_str() encoding:NSUTF8StringEncoding];
		int				maxSize		= static_cast<int>(text.length);
		for (int i = 1; i <= maxSize; ++i)
		{
			NSString*	str	= [text substringToIndex: i];
//			int			adv	= (int)[str sizeWithFont: mFont].width;
			int			adv	= (int)[str sizeWithAttributes:@{NSFontAttributeName: mFont}].width;
			if (adv > _maxWidth) 
			{
				break;
			}
			pos		= strlen([str UTF8String]);
			advance	= adv;
		}
		return	{pos, advance};
	}}


	virtual void on_draw(bitmap<pixel_type>& _out, vec2<int> _origin, size2<int>* _clip, std::string_view _text, const draw_info& _info) override
	{@autoreleasepool{
		update();

		std::string					cstr(_text);
		NSString*					text		= [NSString stringWithCString: cstr.c_str() encoding:NSUTF8StringEncoding];

		NSMutableParagraphStyle*	style	= [[NSMutableParagraphStyle alloc] init];
		style.lineBreakMode	= NSLineBreakByClipping;

		// 描画範囲を計算
		NSMutableAttributedString*	attr		= [[NSMutableAttributedString alloc] initWithString:text attributes:@{
													NSForegroundColorAttributeName:	[UIColor colorWithRed:((float)_info.color.r/255.0f) green:((float)_info.color.g/255.0f) blue: ((float)_info.color.b/255.0f) alpha:1.0],
													NSFontAttributeName:			mFont,
													NSParagraphStyleAttributeName:	style
												}];
		CGSize						bbox		= [text sizeWithAttributes:@{
													NSForegroundColorAttributeName:	[UIColor colorWithRed:((float)_info.color.r/255.0f) green:((float)_info.color.g/255.0f) blue: ((float)_info.color.b/255.0f) alpha:1.0],
													NSFontAttributeName:			mFont,
													NSParagraphStyleAttributeName:	style
												}];

		// 描画用のBitmapを生成
		bitmap<pixel_type>			tmp(size2<int>(bbox.width, bbox.height));
		for (auto& p : tmp) { p = {0,0,0,0}; }

		// 描画用コンテキストを生成
		CGRect						imageRect		= CGRectMake(0, 0, bbox.width, bbox.height);
		CGColorSpaceRef				colorSpace		= CGColorSpaceCreateDeviceRGB();
		CGContextRef				bitmapContext	= CGBitmapContextCreate(
															(char*)tmp.pixels().data(),
															tmp.size().width, tmp.size().height,
															8, 4 * tmp.size().width,
															colorSpace, kCGImageAlphaPremultipliedLast
													);

		CGContextSetRGBFillColor(bitmapContext, 0.0, 0.0, 0.0, 0.0);
		CGContextFillRect(bitmapContext, imageRect);
		CGContextSetAllowsAntialiasing(bitmapContext, true);
		CGContextSetShouldAntialias(bitmapContext, true);
	
		UIGraphicsPushContext(bitmapContext);

		// コンテキストに対して描画する
		if (_info.edgeSize > 0)
		{
			NSMutableAttributedString*	attr	=	[[NSMutableAttributedString alloc] initWithString:text attributes:@{
														NSFontAttributeName:		mFont,
														NSStrokeColorAttributeName: [UIColor
																						colorWithRed: (float)_info.edgeColor.r/255.0f
																							   green: (float)_info.edgeColor.g/255.0f
																								blue: (float)_info.edgeColor.b/255.0f
																							   alpha: (float)_info.edgeColor.a/255.0f],
														NSStrokeWidthAttributeName: [NSNumber numberWithFloat: mEdgeSize]
													}];
			[attr drawInRect:imageRect];

		}
		[attr drawInRect:imageRect];

		UIGraphicsPopContext();
		CGImageRelease(CGBitmapContextCreateImage(bitmapContext));

		// 生成した画像を転送する
		for (int y = 0; y < tmp.size().height; ++y)
		{
			for (int x = 0; x < tmp.size().width; ++x)
			{
				_out[_origin + vec2<int>(x,y)] = tmp[vec2<int>(x,tmp.size().height-1-y)];
			}
		}
	}}



private:

	std::string		mFontname;


	friend	ref<font> create(const char* _fontname, font_size_type _default_size, pixel_type _default_color);
};


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
	
	ref<ios_font>	f	= new ios_font(nullptr, _default_size, _default_color);
	return	f;
}


inline ref<font> font::create(const char* _fontname, font_size_type _default_size, pixel_type _default_color)
{
	ref<ios_font>	f	= new ios_font(_fontname, _default_size, _default_color);
	return	f;
}

inline auto font::system_font_size() -> font_size_type
{
	static int	fontSize	= -1;
	if (fontSize < 1)
	{
		//auto	disp	= display::get_display();
		fontSize	= (int)[UIFont systemFontSize];
		//AMTRS_DEBUG_LOG("fontSize = %d", fontSize);
	}
	return	fontSize;
}



AMTRS_NAMESPACE_END
#endif
