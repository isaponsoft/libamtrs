/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__size__hpp
#define	__libamtrs__graphics__size__hpp
AMTRS_NAMESPACE_BEGIN


template<class T>
struct	size2 : vector_tag<2>
{
	enum class	scale_type
	{
		fill,				//!< 指定サイズがすべて埋まるようにスケールします。
		inside,				//!< 指定サイズに収まるようにスケールします。
	};


	using	value_type	= T;
	value_type	width, height;
	constexpr size2() = default;
	constexpr size2(const size2& _r) noexcept = default;
	constexpr size2(value_type _width, value_type _height)
		: width(_width), height(_height)
	{}

	template<class RT>
	constexpr explicit size2(const vec2<RT>& _r) noexcept
		: width(static_cast<value_type>(_r.x)), height(static_cast<value_type>(_r.y))
	{}

	template<class RT>
	constexpr explicit size2(const size2<RT>& _r) noexcept
		: width(static_cast<value_type>(_r.width)), height(static_cast<value_type>(_r.height))
	{}

	constexpr bool empty() const noexcept { return width == 0 || height == 0; }


	constexpr explicit operator vec2<value_type> () const noexcept { return {width, height}; }

	constexpr bool operator == (const size2& _r) const noexcept { return width == _r.width && height == _r.height; }
	constexpr bool operator != (const size2& _r) const noexcept { return !(*this == _r); }

	constexpr size2& operator = (const size2& _r) noexcept
	{
		width	= _r.width;
		height	= _r.height;
		return	*this;
	}

	constexpr size2 operator + (const size2& _r) const noexcept
	{
		return	{width + _r.width, height + _r.height };
	}

	constexpr size2 operator - (const size2& _r) const noexcept
	{
		return	{width - _r.width, height - _r.height };
	}

	template<class V>
	constexpr size2 operator * (V _scale) const noexcept
	{
		return	{static_cast<value_type>(width * _scale), static_cast<value_type>(height * _scale) };
	}

	template<class V>
	constexpr size2& operator *= (V _scale) noexcept
	{
		*this	= *this * _scale;
		return	*this;
	}

	template<class VT>
	constexpr size2 operator * (const vec2<VT>& _r) const noexcept
	{
		return	{width * _r.x, height * _r.y };
	}

	constexpr size2 operator / (value_type _scale) const noexcept
	{
		return	{width / _scale, height / _scale };
	}

	template<class VT>
	constexpr size2 operator / (const vec2<VT>& _r) const noexcept
	{
		return	{width / _r.x, height / _r.y };
	}

	template<class To>
	constexpr size2<To> cast_to() const noexcept
	{
		return	size2<To>(static_cast<To>(width), static_cast<To>(height));
	}

	constexpr bool is_inner(const vec2<value_type>& _pos) const noexcept
	{
		return	_pos.x >= 0 && _pos.x < width && _pos.y >= 0 && _pos.y < height;
	}

	//!	アスペクト比を計算します。
	//!	width / height
	constexpr double aspect_ratio() const noexcept { return static_cast<double>(width) / static_cast<double>(height); }


	//! _canvas で指定する大きさに合わせてスケールします。
	constexpr size2 scale_of(const size2& _canvas, scale_type _st) const noexcept
	{
		using namespace std;
		double	scale_w		= (double)_canvas.width  / (double)width;
		double	scale_h		= (double)_canvas.height / (double)height;
		double	scale_min	= std::min(scale_w, scale_h);
		double	scale_max	= std::max(scale_w, scale_h);
		return	(_st == scale_type::fill)
				? size2((value_type)((double)width * scale_max), (value_type)((double)height * scale_max))
				: size2((value_type)((double)width * scale_min), (value_type)((double)height * scale_min));
	}

	constexpr size2& set_width(value_type _v) noexcept { width = _v; return *this; }
	constexpr size2& set_height(value_type _v) noexcept { height = _v; return *this; }
};

template<class T>
struct	size3 : vector_tag<3>
{
	using	value_type	= T;
	value_type	width, height, depth;

	constexpr size3() = default;
	constexpr size3(const size3& _r) noexcept = default;
	constexpr size3(value_type _width, value_type _height, value_type _depth)
		: width(_width), height(_height), depth(_depth)
	{}

	template<class RT>
	constexpr explicit size3(const size2<RT>& _r, value_type _depth = 0) noexcept
		: width(static_cast<value_type>(_r.width)), height(static_cast<value_type>(_r.height)), depth(_depth)
	{}

	template<class RT>
	constexpr explicit size3(const vec3<RT>& _r) noexcept
		: width(_r.x), height(_r.y), depth(_r.z)
	{}
	template<class RT>
	constexpr explicit size3(const size3<RT>& _r) noexcept
		: width(_r.width), height(_r.height), depth(_r._depth)
	{}

	constexpr bool empty() const noexcept { return width == 0 || height == 0 || depth == 0; }

	constexpr explicit operator size2<value_type> () const noexcept { return {width, height}; }
	constexpr explicit operator vec3<value_type> () const noexcept { return {width, height, depth}; }

	constexpr bool operator == (const size3& _r) const noexcept { return width == _r.width && height == _r.height && depth == _r.depth; }
	constexpr bool operator != (const size3& _r) const noexcept { return !(*this == _r); }

	template<class I>
	constexpr value_type& operator [] (I _index) noexcept { return (&width)[_index]; }

	constexpr size3& operator = (const size3& _r) noexcept
	{
		width	= _r.width;
		height	= _r.height;
		depth	= _r.depth;
		return	*this;
	}

	constexpr size3 operator + (const size3& _r) const noexcept
	{
		return	{width + _r.width, height + _r.height, depth + _r.depth };
	}

	constexpr size3 operator - (const size3& _r) const noexcept
	{
		return	{width - _r.width, height - _r.height, depth - _r.depth };
	}

	constexpr size3 operator * (value_type _scale) const noexcept
	{
		return	{width * _scale, height * _scale, depth * _scale };
	}

	template<class VT>
	constexpr size3 operator * (const vec3<VT>& _r) const noexcept
	{
		return	{width * _r.x, height * _r.y, depth * _r.z };
	}

	template<class VT>
	constexpr size3 operator / (const vec3<VT>& _r) const noexcept
	{
		return	{width / _r.x, height / _r.y, depth / _r.z };
	}

	constexpr size3 operator / (value_type _r) const noexcept
	{
		return	{width / _r, height / _r, depth / _r };
	}

	template<class To>
	constexpr size3<To> cast_to() const noexcept
	{
		return	size3<To>(static_cast<To>(width), static_cast<To>(height), static_cast<To>(depth));
	}

	constexpr bool is_inner(const vec2<value_type>& _pos) const noexcept
	{
		return	(_pos.x >= 0) && (_pos.x < width) && (_pos.y >= 0) && (_pos.y < height);
	}

	constexpr bool is_inner(const vec3<value_type>& _pos) const noexcept
	{
		return	_pos.x >= 0 && _pos.x < width && _pos.y >= 0 && _pos.y < height &&  _pos.z >= 0 && _pos.z < depth;
	}

	constexpr size3& set_width(value_type _v) noexcept { width = _v; return *this; }
	constexpr size3& set_height(value_type _v) noexcept { height = _v; return *this; }
	constexpr size3& set_depth(value_type _v) noexcept { depth = _v; return *this; }

	//! アスペクト比を維持し指定する幅になるように縦と奥行きを調整します。
	constexpr size3& adjust_to_width(value_type _width)
	{
		double	scale	= _width / width;
		width	= _width;
		height	= static_cast<value_type>(static_cast<double>(height) * scale);
		depth	= static_cast<value_type>(static_cast<double>(depth) * scale);
		return	*this;
	}

	//! アスペクト比を維持し指定する高さになるように幅と奥行きを調整します。
	constexpr size3& adjust_to_height(value_type _height)
	{
		double	scale	= _height / height;
		width	= static_cast<value_type>(static_cast<double>(width) * scale);
		height	= _height;
		depth	= static_cast<value_type>(static_cast<double>(depth) * scale);
		return	*this;
	}
};


template<class T>
static constexpr vec2<T> make_vec2(const size2<T>& _sz)
{
	return	{_sz.width, _sz.height};
}


AMTRS_NAMESPACE_END
#endif
