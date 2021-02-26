/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/font.hpp>
#include AMTRS_DRIVER_INCLUDE(graphics-font.hpp)
AMTRS_NAMESPACE_BEGIN




font::font(const char* /*_fontname*/, font_size_type _default_size, pixel_type _default_color)
	: mUpdateDirty(true)
	, mFontColor(_default_color)
	, mFontSize(_default_size)
{
}

void font::set_color(pixel_type _font_color)
{
	mFontColor		= _font_color;
	mUpdateDirty	= true;
}


void font::set_edge(pixel_type _edge_color, int _edge_size)
{
	mEdgeColor		= _edge_color;
	mEdgeSize		= _edge_size;
	mUpdateDirty	= true;
}

void font::set_line_break_mode(line_break _lb)
{
	mLineBreak		= _lb;
	mUpdateDirty	= true;
}


auto font::create_bitmap(std::string_view _text, int _align_x, int _align_y) -> bitmap<pixel_type>
{
	bounds				b		= compute_bounds(_text);
	if (_align_x > 1)
	{
		b.area.width	+= b.area.width % _align_x;
	}
	if (_align_y > 1)
	{
		b.area.height	+= b.area.height % _align_y;
	}
	draw_info			di;
	di.color		= mFontColor;
	di.edgeColor	= mEdgeColor;
	di.edgeSize		= mEdgeSize;
	vec2<int>			drwpos((int)-b.area.x, (int)-b.area.y);
	size2<int>			bmpsize(b.area.width, b.area.height);
	if (mEdgeSize > 0)
	{
		drwpos.x		+= mEdgeSize;
		drwpos.y		+= mEdgeSize;
		bmpsize.width	+= mEdgeSize * 2;	// 左+右
		bmpsize.height	+= mEdgeSize * 2;	// 上+下
	}
	bitmap<pixel_type> 	bmp(bmpsize);
	draw(bmp, drwpos, _text, di);
	return	bmp;
}


void font::draw_mb(bitmap<pixel_type>& _out, std::string_view _text, const draw_info& _info)
{
	struct	line_info
	{
		std::string_view	text;
		rect<int>			lineRect;			// その行を作るのに必要な矩形
	};

	// まずは行を分割する
	auto					lines	= split_chars(_text, "\n");
	std::vector<line_info>	infos;
	for (auto line : lines)
	{
		line	= trim(line, "\r");

		auto*	cur		= line.data();
		auto*	end		= cur + line.size();
		while (cur < end)
		{
			auto	block	= std::string_view(cur, (std::size_t)(end - cur));
			auto	lb		= compute_braek(block, _out.size().width);
			if (lb.first == 0)
			{
				return;
			}
			block	= std::string_view(cur, lb.first);
			auto	bounds	= compute_bounds(block);
			infos.push_back({block, bounds.area});
			cur	+= lb.first;
		}
	}


	// 描画を行う
	int					y	= 0;
	for (auto& info : infos)
	{
		on_draw(_out, {0, -info.lineRect.y + y}, nullptr, info.text, _info);
		y += info.lineRect.height;
	}
}


auto font::create_bitmap_adjust_size(vec2<int>* _origin, std::string_view _text, const draw_info& _info, size2<int> _size) -> bitmap<pixel_type>
{
	struct	line_info
	{
		std::string_view	text;
		rect<int>			lineRect;			// その行を作るのに必要な矩形
	};

	if (_size.width <= 0 || _size.height <= 0)
	{
		return	bitmap<pixel_type>(size2<int>(0, 0));
	}

	// まずは行を分割する
	ref<font>				f		= this;
	auto					lines	= split_chars(_text, "\n");
	std::vector<line_info>	infos;
	rect<int>				canvasSize;
	for (;;)
	{
		infos	= {};
		for (auto line : lines)
		{
			line	= trim(line, "\r");
			auto*	cur		= line.data();
			auto*	end		= cur + line.size();
			while (cur < end)
			{
				auto	block	= std::string_view(cur, (std::size_t)(end - cur));
				auto	lb		= f->compute_braek(block, _size.width);
				if (lb.first == 0)
				{
					return	{};
				}
				block	= std::string_view(cur, lb.first);
				auto	bounds	= f->compute_bounds(block);
				infos.push_back({block, bounds.area});
				cur	+= lb.first;
			}
		}

		// キャンバスの大きさを計算する
		canvasSize			= infos[0].lineRect;
		canvasSize.y		= 0;
		canvasSize.height	= 0;
		for (auto& info : infos)
		{
			int		right	= canvasSize.right();
			if (canvasSize.x > info.lineRect.x)
			{
				canvasSize.x		= info.lineRect.x;
				canvasSize.width	= right - canvasSize.x;
				right				= canvasSize.right();
			}
			
			if (right < info.lineRect.right())
			{
				canvasSize.width	= info.lineRect.right() - canvasSize.x;
			}
			canvasSize.height	+= info.lineRect.height;
		}

		float	scale	= std::min<float>((float)_size.width / (float)canvasSize.width, (float)_size.height / (float)canvasSize.height);
		if (scale >= 1.0f)
		{
			break;
		}
		f		= font::create((int)((float)f->size() * scale), f->color());
	}

	// 描画を行う
	bitmap<pixel_type>	retval(size2<int>(canvasSize.width, canvasSize.height));
	int					y	= 0;
	for (auto& info : infos)
	{
		f->on_draw(retval, {-canvasSize.x, -info.lineRect.y + y}, nullptr, info.text, _info);
		y += info.lineRect.height;
	}
	if (_origin)
	{
		_origin->x	= -canvasSize.x;
		_origin->y	= -infos[0].lineRect.y;
	}
	return	retval;
}


auto font::create_bitmap_mb(vec2<int>* _origin, std::string_view _text, const draw_info& _info, int _maxWidth) -> bitmap<pixel_type>
{
	struct	line_info
	{
		std::string_view	text;
		rect<int>			lineRect;			// その行を作るのに必要な矩形
	};

	// まずは行を分割する
	auto					lines	= split_chars(_text, "\n");
	std::vector<line_info>	infos;
	for (auto line : lines)
	{
		line	= trim(line, "\r");

		auto*	cur		= line.data();
		auto*	end		= cur + line.size();
		while (cur < end)
		{
			auto	block	= std::string_view(cur, (std::size_t)(end - cur));
			auto	lb		= compute_braek(block, _maxWidth);
			if (lb.first == 0)
			{
				return	{};
			}
			block	= std::string_view(cur, lb.first);
			auto	bounds	= compute_bounds(block);
			infos.push_back({block, bounds.area});
			cur	+= lb.first;
		}
	}

	// キャンバスの大きさを計算する
	rect<int>				canvasSize	= infos[0].lineRect;
	canvasSize.y		= 0;
	canvasSize.height	= 0;
	for (auto& info : infos)
	{
		int		right	= canvasSize.right();
		if (canvasSize.x > info.lineRect.x)
		{
			canvasSize.x		= info.lineRect.x;
			canvasSize.width	= right - canvasSize.x;
			right				= canvasSize.right();
		}
		
		if (right < info.lineRect.right())
		{
			canvasSize.width	= info.lineRect.right() - canvasSize.x;
		}
		canvasSize.height	+= info.lineRect.height;
	}

	// 描画を行う
	bitmap<pixel_type>	retval(size2<int>(canvasSize.width, canvasSize.height));
	int					y	= 0;
	for (auto& info : infos)
	{
		on_draw(retval, {-canvasSize.x, -info.lineRect.y + y}, nullptr, info.text, _info);
		y += info.lineRect.height;
	}
	if (_origin)
	{
		_origin->x	= -canvasSize.x;
		_origin->y	= -infos[0].lineRect.y;
	}
	return	retval;
}

AMTRS_NAMESPACE_END
