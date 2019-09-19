#ifndef	__libamtrs__graphics__color__hpp
#define	__libamtrs__graphics__color__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

enum class color_format
{
	rgba8,
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
	using	_base_type			= rgb<T>;
public:
	using	value_type			= T;
	using	color_level_type	= typename _base_type::color_level_type;

	value_type		a;

	constexpr rgba() = default;
	constexpr rgba(const rgba&) = default;
	constexpr rgba& operator = (const rgba&) = default;
	~rgba() = default;

	constexpr rgba(value_type _r, value_type _g, value_type _b, value_type _a = color_level_type::max)
		: _base_type(_r, _g, _b)
		, a(_a)
	{}

	constexpr rgba(const rgb<T>& _c, value_type _a)
		: _base_type(_c)
		, a(_a)
	{}

	constexpr bool operator == (const rgba<T>& _r) const noexcept
	{
		return	(_base_type::operator == (_r)) && (this->a == _r.a);
	}

	constexpr bool operator != (const rgba<T>& _r) const noexcept
	{
		return	!(*this == _r);
	}

};


AMTRS_NAMESPACE_END
#endif
