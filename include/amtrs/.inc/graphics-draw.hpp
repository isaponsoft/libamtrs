/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__images__draw__hpp
#define	__libamtrs__images__draw__hpp
AMTRS_NAMESPACE_BEGIN

template<class... Params, class FuncT>
void filter_line_h(
		basic_bitmap<Params...>& 							_image,
		vec2<typename basic_bitmap<Params...>::point_type>	_p,
		typename basic_bitmap<Params...>::point_type			_width,
		FuncT&&												_func
)
{
	using	point_type	= typename basic_bitmap<Params...>::point_type;

	for (point_type x = _p.x; x < (_p.x + _width); ++x)
	{
		_func(_image[{x, _p.y}]);
	}
}

template<class... Params>
void draw_line_h(
		basic_bitmap<Params...>& 							_image,
		vec2<typename basic_bitmap<Params...>::point_type>	_p,
		typename basic_bitmap<Params...>::point_type			_width,
		typename basic_bitmap<Params...>::pixel_type			_color
)
{
	filter_line_h(_image, _p, _width, [=](auto& _pixel) { _pixel = _color; } );
}

template<class... Params, class FuncT>
void filter_line_v(
		basic_bitmap<Params...>& 							_image,
		vec2<typename basic_bitmap<Params...>::point_type>	_p,
		typename basic_bitmap<Params...>::point_type			_height,
		FuncT&&												_func
)
{
	using	point_type	= typename basic_bitmap<Params...>::point_type;

	for (point_type y = _p.y; y < (_p.y + _height); ++y)
	{
		_func(_image[{_p.x, y}]);
	}
}

template<class... Params>
void draw_line_v(
		basic_bitmap<Params...>& 							_image,
		vec2<typename basic_bitmap<Params...>::point_type>	_p,
		typename basic_bitmap<Params...>::point_type			_height,
		typename basic_bitmap<Params...>::pixel_type			_color
)
{
	filter_line_v(_image, _p, _height, [=](auto& _pixel) { _pixel = _color; } );
}


template<class... Params, class FuncT>
void filter_rect(
		basic_bitmap<Params...>& 							_image,
		rect<typename basic_bitmap<Params...>::point_type>	_rect,
		FuncT&&												_func
)
{
	using	point_type	= typename basic_bitmap<Params...>::point_type;
	filter_line_h(_image, {_rect.left(), _rect.top()     }, _rect.width, std::forward<FuncT>(_func));
	filter_line_h(_image, {_rect.left(), _rect.bottom()-1}, _rect.width, std::forward<FuncT>(_func));
	filter_line_v(_image, {_rect.left(), _rect.top()+1   }, _rect.height-2, std::forward<FuncT>(_func));
	filter_line_v(_image, {_rect.right()-1, _rect.top()+1}, _rect.height-2, std::forward<FuncT>(_func));
}

template<class... Params>
void draw_rect(
		basic_bitmap<Params...>& 							_image,
		rect<typename basic_bitmap<Params...>::point_type>	_rect,
		typename basic_bitmap<Params...>::pixel_type			_color
)
{
	filter_rect(_image, _rect, [=](auto& _pixel) { _pixel = _color; } );
}


AMTRS_NAMESPACE_END
#endif
