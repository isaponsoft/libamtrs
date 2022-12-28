/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__geometory__vec__hpp
#define	__libamtrs__graphics__geometory__vec__hpp
#include "_vector_tag.hxx"
AMTRS_GRAPHICS_NAMESPACE_BEGIN


template<class T>
struct	vec2 : AMTRS_IMPLEMENTS(vec)::_vector_impliments<T, AMTRS_IMPLEMENTS(vec)::vector_tag<2>, vec2<T>>
{
	using	value_type	= T;

	value_type	x, y;
	constexpr vec2() = default;
	constexpr vec2(const vec2& _r) = default;
	constexpr vec2(value_type _x, value_type _y)
		: x(_x), y(_y)
	{}
	template<class RT>
	constexpr explicit vec2(const vec2<RT>& _r) noexcept
		: x(static_cast<value_type>(_r.x)), y(static_cast<value_type>(_r.y))
	{}

	template<class RT>
	constexpr vec2 operator * (const vec2<RT>& _r) const noexcept
	{
		return	{
					static_cast<value_type>(x * _r.x),
					static_cast<value_type>(y * _r.y)
				};
	}

	template<class RT>
	constexpr vec2 operator *= (const vec2<RT>& _r) noexcept
	{
		x = static_cast<value_type>(x * _r.x);
		y = static_cast<value_type>(y * _r.y);
		return	*this;
	}


	constexpr vec2 operator /= (const vec2& _r) noexcept
	{
		x /= _r.x;
		y /= _r.y;
		return	*this;
	}

	constexpr value_type atan2() const noexcept
	{
		return	std::atan2(y, x);
	}

protected:
	using	_impl_type	= AMTRS_IMPLEMENTS(vec)::_vector_impliments<T, AMTRS_IMPLEMENTS(vec)::vector_tag<2>, vec2<T>>;

public:
	using	_impl_type::operator [];
	using	_impl_type::operator +;
	using	_impl_type::operator +=;
	using	_impl_type::operator -;
	using	_impl_type::operator -=;
	using	_impl_type::operator *;
	using	_impl_type::operator *=;
	using	_impl_type::operator /;
	using	_impl_type::operator /=;
	using	_impl_type::operator ==;
	using	_impl_type::operator !=;
};

template<class T>
struct	vec3 : AMTRS_IMPLEMENTS(vec)::_vector_impliments<T, AMTRS_IMPLEMENTS(vec)::vector_tag<3>, vec3<T>>
{
	using	value_type	= T;

	value_type	x, y, z;

	constexpr vec3() = default;
	constexpr vec3(value_type _x, value_type _y, value_type _z)
		: x(_x), y(_y), z(_z)
	{}
	template<class RT>
	explicit constexpr vec3(const vec2<RT> _r, value_type _z = 0)
		: x(static_cast<value_type>(_r.x)), y(static_cast<value_type>(_r.y)), z(_z)
	{}
	template<class RT>
	explicit constexpr vec3(const vec3<RT> _r)
		: x(static_cast<value_type>(_r.x)), y(static_cast<value_type>(_r.y)), z(static_cast<value_type>(_r.z))
	{}


	explicit operator vec2<value_type> () const noexcept { return {x, y}; }


	constexpr vec3 operator - (const vec3& _r) const noexcept
	{
		return	{x - _r.x, y - _r.y, z - _r.z };
	}

	constexpr vec3 operator -= (const vec3& _r) noexcept
	{
		x -= _r.x;
		y -= _r.y;
		z -= _r.z;
		return	*this;
	}

	constexpr vec3 operator * (const vec3& _r) const noexcept
	{
		return	{x * _r.x, y * _r.y, z * _r.z };
	}

	constexpr vec3 operator *= (const vec3& _r) noexcept
	{
		x *= _r.x;
		y *= _r.y;
		z *= _r.z;
		return	*this;
	}

	constexpr vec3 operator / (value_type _s) const noexcept
	{
		return	{x / _s, y / _s, z / _s };
	}

	constexpr vec3 operator / (const vec3& _r) const noexcept
	{
		return	{x / _r.x, y / _r.y, z / _r.z };
	}

	constexpr vec3 operator /= (const vec3& _r) noexcept
	{
		x /= _r.x;
		y /= _r.y;
		z /= _r.z;
		return	*this;
	}

protected:
	using	_impl_type	= AMTRS_IMPLEMENTS(vec)::_vector_impliments<T, AMTRS_IMPLEMENTS(vec)::vector_tag<3>, vec3<T>>;

public:
	using	_impl_type::operator [];
	using	_impl_type::operator +;
	using	_impl_type::operator +=;
	using	_impl_type::operator -;
	using	_impl_type::operator -=;
	using	_impl_type::operator *;
	using	_impl_type::operator *=;
	using	_impl_type::operator /;
	using	_impl_type::operator /=;
	using	_impl_type::operator ==;
	using	_impl_type::operator !=;
};


template<class T>
struct	vec4 : AMTRS_IMPLEMENTS(vec)::_vector_impliments<T, AMTRS_IMPLEMENTS(vec)::vector_tag<4>, vec4<T>>
{
	using	value_type	= T;

	value_type	x, y, z, w;

	constexpr vec4() = default;
	constexpr vec4(value_type _x, value_type _y, value_type _z, value_type _w)
		: x(_x), y(_y), z(_z), w(_w)
	{}
	template<class RT>
	explicit constexpr vec4(const vec3<RT>& _r, value_type _w = 0)
		: x(static_cast<value_type>(_r.x)), y(static_cast<value_type>(_r.y)), z(static_cast<value_type>(_r.z)), w(_w)
	{}

protected:
	using	_impl_type	= AMTRS_IMPLEMENTS(vec)::_vector_impliments<T, AMTRS_IMPLEMENTS(vec)::vector_tag<4>, vec4<T>>;

public:
	using	_impl_type::operator [];
	using	_impl_type::operator +;
	using	_impl_type::operator +=;
	using	_impl_type::operator -;
	using	_impl_type::operator -=;
	using	_impl_type::operator *;
	using	_impl_type::operator *=;
	using	_impl_type::operator /;
	using	_impl_type::operator /=;
	using	_impl_type::operator ==;
	using	_impl_type::operator !=;
};


template<typename Type>
constexpr auto cross(const vec2<Type>& _v1, const vec2<Type>& _v2) noexcept -> vec2<Type>
{
	return	{
				_v1.x * _v2.y - _v1.y * _v2.x,
				_v1.x * _v2.y - _v1.y * _v2.x
			};
}

template<typename Type>
constexpr auto cross(const vec3<Type>& _v1, const vec3<Type>& _v2) noexcept -> vec3<Type>
{ 
	return	{
				_v1.y*_v2.z - _v1.z*_v2.y,
				_v1.z*_v2.x - _v1.x*_v2.z,
				_v1.x*_v2.y - _v1.y*_v2.x
			};
}

template<typename Type>
constexpr auto cross(const vec4<Type>& _v1, const vec4<Type>& _v2, const vec4<Type>& _v3) noexcept -> vec4<Type>
{
	return	{
				((_v2.z * _v3.w-_v2.w * _v3.z) * _v1.y) - ((_v2.y * _v3.w-_v2.w * _v3.y) * _v1.z) + ((_v2.y * _v3.z-_v2.z * _v3.y) * _v1.w),
				((_v2.w * _v3.z-_v2.z * _v3.w) * _v1.x) - ((_v2.w * _v3.x-_v2.x * _v3.w) * _v1.z) + ((_v2.z * _v3.x-_v2.x * _v3.z) * _v1.w),
				((_v2.y * _v3.w-_v2.w * _v3.y) * _v1.x) - ((_v2.x * _v3.w-_v2.w * _v3.x) * _v1.y) + ((_v2.x * _v3.y-_v2.y * _v3.x) * _v1.w),
				((_v2.z * _v3.y-_v2.y * _v3.z) * _v1.x) - ((_v2.z * _v3.x-_v2.x * _v3.z) * _v1.y) + ((_v2.y * _v3.x-_v2.x * _v3.y) * _v1.z)
			};
}

template<typename Type>
constexpr Type dot(const vec2<Type>& _v1, const vec2<Type>& _v2) noexcept
{
	return	(_v1.x * _v2.x) + (_v1.y * _v2.y);
}

template<typename Type>
constexpr Type dot(const vec3<Type>& _v1, const vec3<Type>& _v2) noexcept
{
	return	(_v1.x * _v2.x) + (_v1.y * _v2.y) + (_v1.z * _v2.z);
}

template<typename Type>
constexpr Type dot(const vec4<Type>& _v1, const vec4<Type>& _v2) noexcept
{
	return	(_v1.x * _v2.x) + (_v1.y * _v2.y) + (_v1.z * _v2.z) + (_v1.w * _v2.w);
}



template<class T, class Tag, class VecT>
auto make_backet(AMTRS_IMPLEMENTS(vec)::_vector_impliments<T, Tag, VecT>& _vec)
{
	struct	vec_backet
	{
		vec_backet(VecT& _vec)
			: vec(_vec)
		{}

		auto begin() noexcept { return &vec[0]; }
		auto end() noexcept { return &vec[VecT::vector_type::col_size]; }
		auto begin() const noexcept { return &vec[0]; }
		auto end() const noexcept { return &vec[VecT::vector_type::col_size]; }
		auto cbegin() const noexcept { return &vec[0]; }
		auto cend() const noexcept { return &vec[VecT::vector_type::col_size]; }

	private:
		VecT&	vec;
	};
	return	vec_backet(_vec);
}


AMTRS_GRAPHICS_NAMESPACE_END
#endif
