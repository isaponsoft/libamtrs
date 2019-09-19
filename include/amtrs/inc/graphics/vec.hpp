#ifndef	__libamtrs__graphics__vec__hpp
#define	__libamtrs__graphics__vec__hpp
#include <cmath>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


template<std::size_t Size>
struct	vector_tag
{
	static constexpr std::size_t	col_size	= Size;
};

template<class T>
struct	size2;



template<class T, class Tag, class VecT>
struct	_vector_impliments : Tag
{
	using	value_type		= T;
	using	vector_type		= VecT;
	using	iterator		= value_type*;
	using	const_iterator	= const value_type*;

	static constexpr std::size_t	col_size	= Tag::col_size;

	      value_type& operator [] (int _index)       noexcept { return reinterpret_cast<      value_type*>(this)[_index]; }
	const value_type& operator [] (int _index) const noexcept { return reinterpret_cast<const value_type*>(this)[_index]; }


	vector_type operator - () const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = -(*this)[c];
		}
		return	retval;
	}


	vector_type operator + (const vector_type& _r) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] + _r[c];
		}
		return	retval;
	}

	vector_type operator + (value_type _v) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] + _v;
		}
		return	retval;
	}

	vector_type& operator += (const vector_type& _r) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] += _r[c];
		}
		return	*static_cast<vector_type*>(this);
	}

	vector_type& operator += (value_type _v) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] += _v;
		}
		return	*static_cast<vector_type*>(this);
	}

	vector_type operator - (const vector_type& _r) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] - _r[c];
		}
		return	retval;
	}

	vector_type operator - (value_type _v) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] - _v;
		}
		return	retval;
	}

	vector_type& operator -= (const vector_type& _r) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] -= _r[c];
		}
		return	*static_cast<vector_type*>(this);
	}

	vector_type& operator -= (value_type _v) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] -= _v;
		}
		return	*static_cast<vector_type*>(this);
	}

	vector_type operator * (const vector_type& _r) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] * _r[c];
		}
		return	retval;
	}

	vector_type operator * (value_type _v) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] * _v;
		}
		return	retval;
	}

	vector_type& operator *= (const vector_type& _r) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] *= _r[c];
		}
		return	*static_cast<vector_type*>(this);
	}

	vector_type& operator *= (value_type _v) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] *= _v;
		}
		return	*static_cast<vector_type*>(this);
	}

	vector_type operator / (const vector_type& _r) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] / _r[c];
		}
		return	retval;
	}

	vector_type operator / (value_type _v) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] / _v;
		}
		return	retval;
	}

	vector_type& operator /= (const vector_type& _r) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] /= _r[c];
		}
		return	*static_cast<vector_type*>(this);
	}

	vector_type& operator /= (value_type _v) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] /= _v;
		}
		return	*static_cast<vector_type*>(this);
	}

	bool operator == (const vector_type& _r) const noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			if ((*this)[c] != _r[c])
			{
				return	false;
			}
		}
		return	true;
	}

	bool operator != (const vector_type& _r) const noexcept
	{
		return	!(*this == _r);
	}

	static vector_type one() noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = 1;
		}
		return	retval;
	}

	static vector_type zero() noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = 0;
		}
		return	retval;
	}


	value_type length() const noexcept
	{
		value_type	a	= 0;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			a += (*this)[c] * (*this)[c];
		}
		return	 value_type(sqrt(a));
	}

	iterator begin() noexcept { return &(*this)[0]; }
	iterator end() noexcept { return &(*this)[vector_type::col_size]; }
	const_iterator begin() const noexcept { return &(*this)[0]; }
	const_iterator end() const noexcept { return &(*this)[vector_type::col_size]; }
	const_iterator cbegin() const noexcept { return &(*this)[0]; }
	const_iterator cend() const noexcept { return &(*this)[vector_type::col_size]; }
};


template<class T>
struct	vec2 : _vector_impliments<T, vector_tag<2>, vec2<T>>
{
	using	value_type	= T;
	using	_impl_type	= _vector_impliments<T, vector_tag<2>, vec2<T>>;
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
/*
	constexpr vec2 operator + (const vec2& _r) const noexcept
	{
		return	{x + _r.x, y + _r.y };
	}

	constexpr vec2& operator += (const vec2& _r) noexcept
	{
		x += _r.x;
		y += _r.y;
		return	*this;
	}
*/
	constexpr vec2 operator - (const vec2& _r) const noexcept
	{
		return	{x - _r.x, y - _r.y };
	}

	constexpr vec2 operator -= (const vec2& _r) noexcept
	{
		x -= _r.x;
		y -= _r.y;
		return	*this;
	}

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
};

template<class T>
struct	vec3 : _vector_impliments<T, vector_tag<3>, vec3<T>>
{
	using	value_type	= T;
	using	_impl_type	= _vector_impliments<T, vector_tag<3>, vec3<T>>;

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
};


template<class T>
struct	vec4 : _vector_impliments<T, vector_tag<4>, vec4<T>>
{
	using	value_type	= T;
	using	_impl_type	= _vector_impliments<T, vector_tag<4>, vec4<T>>;

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

	value_type	x, y, z, w;

	constexpr vec4() = default;
	constexpr vec4(value_type _x, value_type _y, value_type _z, value_type _w)
		: x(_x), y(_y), z(_z), w(_w)
	{}
	template<class RT>
	explicit constexpr vec4(const vec3<RT>& _r, value_type _w = 0)
		: x(static_cast<value_type>(_r.x)), y(static_cast<value_type>(_r.y)), z(static_cast<value_type>(_r.z)), w(_w)
	{}
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
auto make_backet(_vector_impliments<T, Tag, VecT>& _vec)
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


AMTRS_NAMESPACE_END
#endif
