/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__math__muldiv__hpp
#define	__libamtrs__math__muldiv__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


//! 割り算を行い、余りも同時に求めます。
template<class T, class T2>
std::pair<T, T> div_mod(T _operand1, T2 _operand2)
{
	T	div	= _operand1 / _operand2;
	T	mod	= _operand1 % _operand2;
	return	{ div, mod };
}


template<class T>
struct	mul_traits;

template<>
struct	mul_traits<int8_t>
{
	using	double_type	= int16_t;
};

template<>
struct	mul_traits<int16_t>
{
	using	double_type	= int32_t;
	using	harf_type	= int8_t;
};

template<>
struct	mul_traits<int32_t>
{
	using	double_type	= int64_t;
	using	harf_type	= int16_t;
};

template<>
struct	mul_traits<int64_t>
{
	using	double_type	= __int128;
	using	harf_type	= int32_t;
};

template<>
struct	mul_traits<uint8_t>
{
	using	double_type	= uint16_t;
};

template<>
struct	mul_traits<uint16_t>
{
	using	double_type	= uint32_t;
	using	harf_type	= uint8_t;
};

template<>
struct	mul_traits<uint32_t>
{
	using	double_type	= uint64_t;
	using	harf_type	= uint16_t;
};

template<>
struct	mul_traits<uint64_t>
{
	using	double_type	= __uint128_t;
	using	harf_type	= uint32_t;
};


//! 倍精度の掛け算を行い、上位と下位を返します。
template<class T, class T2>
std::pair<T, T> mul(T _operand1, T2 _operand2)
{
	// 倍精度に変換することで imul, mul の拡張サイズを扱うヒントにする。
	using	dt	= typename mul_traits<T>::double_type;
	T	h	= (dt)_operand1 * (dt)_operand2 >> (sizeof(T) * 8);
	T	l	= (dt)_operand1 * (dt)_operand2;
	return	{ h, l };
}


AMTRS_NAMESPACE_END
#endif
