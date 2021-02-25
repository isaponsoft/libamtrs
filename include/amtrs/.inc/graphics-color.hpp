/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__color__hpp
#define	__libamtrs__graphics__color__hpp
AMTRS_NAMESPACE_BEGIN

enum class color_format
{
	unkown		= -1,
	gray8		= 0,		//!< 8bit gray scale
	rgb8		= 1,		//!< 24bit R8 G8 B8
	rgba8		= 2,		//!< 32bit R8 G8 B8 A8
	rgbx8		= 3,		//!< 32bit R8 G8 B8 x8
	rgbf		= 4,		//!< 48bit Rfloat Gfloat Bfloat
	rgbaf		= 5,		//!< 64bit Rfloat Gfloat Bfloat Afloat
};


template<class T>
struct	color_level;

template<>
struct	color_level<uint8_t>
{
	static constexpr uint8_t	min	= 0;
	static constexpr uint8_t	max	= 255;
};

template<>
struct	color_level<float>
{
	static constexpr float		min	= 0.0f;
	static constexpr float		max	= 1.0f;
};

template<>
struct	color_level<double>
{
	static constexpr double		min	= 0.0f;
	static constexpr double		max	= 1.0f;
};


template<class T>
struct	rgb
{
	using	value_type			= T;
	using	color_level_type	= color_level<T>;

	value_type		r;
	value_type		g;
	value_type		b;

	constexpr rgb() = default;
	constexpr rgb(const rgb&) = default;
	constexpr rgb& operator = (const rgb&) = default;
	~rgb() = default;

	constexpr rgb(value_type _r, value_type _g, value_type _b)
		: r(_r), g(_g), b(_b)
	{}

	constexpr rgb& operator += (rgb const& _r) noexcept
	{
		r	+= _r.r;
		g	+= _r.g;
		b	+= _r.b;
		return	*this;
	}

	template<class V>
	constexpr rgb& operator *= (V _alpha) noexcept
	{
		r	= static_cast<value_type>(r * _alpha);
		g	= static_cast<value_type>(g * _alpha);
		b	= static_cast<value_type>(b * _alpha);
		return	*this;
	}

	constexpr rgb operator + (rgb const& _r) const noexcept
	{
		rgb	ret(*this);
		ret += _r;
		return	ret;
	}

	template<class V>
	constexpr rgb operator * (V _alpha) const noexcept
	{
		rgb	ret(*this);
		ret.r	= static_cast<value_type>(ret.r * _alpha);
		ret.g	= static_cast<value_type>(ret.g * _alpha);
		ret.b	= static_cast<value_type>(ret.b * _alpha);
		return	ret;
	}

	constexpr rgb operator * (rgb const& _r) const noexcept
	{
		rgb	ret(*this);
		ret *= _r;
		return	ret;
	}

	constexpr bool operator == (const rgb<T>& _r) const noexcept
	{
		return	this->r == _r.r && this->g == _r.g && this->b == _r.b;
	}

	constexpr bool operator != (const rgb<T>& _r) const noexcept
	{
		return	!(*this == _r);
	}

};


template<class T>
struct	rgba : rgb<T>
{
private:
	using	super_type			= rgb<T>;
public:
	using	value_type			= T;
	using	color_level_type	= typename super_type::color_level_type;

	value_type		a;

	constexpr rgba() = default;
	constexpr rgba(const rgba&) = default;
	constexpr rgba& operator = (const rgba&) = default;
	~rgba() = default;

	constexpr rgba(value_type _r, value_type _g, value_type _b, value_type _a = color_level_type::max)
		: super_type(_r, _g, _b)
		, a(_a)
	{}

	constexpr rgba(const rgb<T>& _c, value_type _a)
		: super_type(_c)
		, a(_a)
	{}

	constexpr rgba& operator += (rgba const& _r) noexcept
	{
		super_type::operator += (_r);
		a	+= _r.a;
		return	*this;
	}

	template<class V>
	constexpr rgba& operator *= (V _alpha) noexcept
	{
		super_type::operator *= (_alpha);
		a	= static_cast<value_type>(a * _alpha);
		return	*this;
	}

	constexpr rgba operator + (rgba const& _r) const noexcept
	{
		rgba	ret(*this);
		ret		+= _r;
		ret.a	+= _r.a;
		return	ret;
	}

	constexpr rgba operator * (rgba const& _r) const noexcept
	{
		rgba	ret(*this);
		ret *= _r;
		return	ret;
	}

	template<class V>
	constexpr rgba operator * (V _alpha) const noexcept
	{
		rgba	ret(*this);
		ret		*= _alpha;
		return	ret;
	}

	constexpr bool operator == (const rgba<T>& _r) const noexcept
	{
		return	(super_type::operator == (_r)) && (this->a == _r.a);
	}

	constexpr bool operator != (const rgba<T>& _r) const noexcept
	{
		return	!(*this == _r);
	}

};


inline constexpr size_t pixel_size(color_format _fmt) noexcept
{
	return	_fmt == color_format::gray8	? (sizeof(uint8_t) * 1)
		:	_fmt == color_format::rgb8	? (sizeof(uint8_t) * 3)
		:	_fmt == color_format::rgba8	? (sizeof(uint8_t) * 4)
		:	_fmt == color_format::rgbx8	? (sizeof(uint8_t) * 5)
		:	_fmt == color_format::rgbf	? (sizeof(float)   * 3)
		:	_fmt == color_format::rgbaf ? (sizeof(float)   * 4)
		:	                              0;
}



AMTRS_NAMESPACE_END
#endif
