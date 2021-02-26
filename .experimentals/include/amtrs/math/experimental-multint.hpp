/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__math__multiint__hpp
#define	__libamtrs__math__multiint__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

//! 多倍長整数
template<class T, size_t N>
class	basic_multi_int;

template<class T, class R>
void _mul(T* _dst, T const* _src, size_t _size, R _num)
{
	using	type1	= T;
	using	type2	= typename mul_traits<T>::double_type;
	T	t1	= 0;
	T	t2	= 0;
	for (size_t i = 0; i < _size; ++i)
	{
		auto [mh, ml] = mul(_src[i], _num);
		type2	t	= (type2)ml + (type2)t1 + (type2)t2;
		type1	th	= t>>8;
		_dst[i]	= t;
		t1	= mh;
		t2	= th;
	}
}


template<class T, class R>
void _mul_add(T* _dst, T const* _src, size_t _size, R _num)
{
	using	type1	= T;
	using	type2	= typename mul_traits<T>::double_type;
	T	t1	= 0;
	T	t2	= 0;
	for (size_t i = 0; i < _size; ++i)
	{
		auto [mh, ml] = mul(_src[i], _num);
		type2	t	= (type2)_dst[i] + (type2)ml + (type2)t1 + (type2)t2;
		type1	th	= t>>(sizeof(type1)*8);
		_dst[i]	= t;
		t1	= mh;
		t2	= th;
	}
}


template<class T>
void _add(T* _dst, T const* _src, size_t _size, T _num)
{
	using	type1	= T;
	using	type2	= typename mul_traits<T>::double_type;

	type2	t		= (type2)_src[0] + _num;
	type1	carry	= (type1)(t>>(sizeof(type1)*8));
	_dst[0]			= (type1)t;
	for (size_t i = 1; i < _size && carry; ++i)
	{
		t			= (type2)_src[i] + carry;
		carry		= (type1)(t>>(sizeof(type1)*8));
		_dst[i]		= (type1)t;
	}
}



template<class T, size_t N>
class	basic_multi_int
{
public:
	basic_multi_int() = default;
	basic_multi_int(basic_multi_int const&) = default;
	basic_multi_int& operator = (basic_multi_int const&) = default;

	template<class It>
	basic_multi_int(It _first, It _last, int _base)
	{
		while (_first != _last)
		{
			int	c	= *_first++;
			c	= (c >= '0' && c <= '9') ? (c - '0'     )
				: (c >= 'a' && c <= 'f') ? (c - 'a' + 10)
				: (c >= 'A' && c <= 'F') ? (c - 'A' + 10)
				: 0;
			_mul(mValues, mValues, N, _base);
			_add(mValues, mValues, N, static_cast<T>(c));
		}
	}

	template<class S>
	basic_multi_int(S _value)
	{
		int	i	= 0;
		while (_value > 0)
		{
			mValues[i++] = static_cast<T>(_value);
			_value >>= (sizeof(T)*8);
		}
	}


	size_t size() const noexcept { return N; }


	template<class INDEX> T& operator [] (INDEX _index) noexcept { return mValues[_index]; }
	template<class INDEX> T const& operator [] (INDEX _index) const noexcept { return mValues[_index]; }


	basic_multi_int operator * (basic_multi_int const& _r) const noexcept
	{
		basic_multi_int	retval = {0};
		for (int a = 0; a < N; ++a)
		{
			T	t1	= 0;
			T	t2	= 0;
			for (int b = 0; b < N; ++b)
			{
				auto [mh, ml] = mul(mValues[b], _r.mValues[a]);
				uint16_t	t	= (uint16_t)retval.mValues[a + b]	+ (uint16_t)ml + (uint16_t)t1 + (uint16_t)t2;
				uint8_t		th	= t>>8;
				retval.mValues[a + b]	= t;
				t1	= mh;
				t2	= th;
			}
		}
		return	retval;
	}


	basic_multi_int operator / (basic_multi_int const& _r) const noexcept
	{
		basic_multi_int	retval = {0};
		return	retval;
	}

private:
	T	mValues[N]	= {0};
};

AMTRS_NAMESPACE_END
#endif
