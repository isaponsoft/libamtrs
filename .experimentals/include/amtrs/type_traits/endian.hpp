/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_utils__endian__hpp
#define __libamtrs__type_utils__endian__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

#ifdef	AMTRS_HAS_STD_ENDIAN
using	endian	= std::endian;
#else	// AMTRS_HAS_STD_ENDIAN
enum class endian
{
	#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
	#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
	#endif
};
#endif	// AMTRS_HAS_STD_ENDIAN


struct	is_big_endian
		: std::conditional<endian::native == endian::big,    std::true_type, std::false_type>::type
{};


struct	is_little_endian
		: std::conditional<endian::native == endian::little, std::true_type, std::false_type>::type
{};

AMTRS_NAMESPACE_IMPLIMENTS_BEGIN(endian)
template<amtrs::endian To, amtrs::endian From, class T, typename std::enable_if<sizeof(T) == 1 || To == From>::type* = 0>
constexpr auto convert_endian(const T& _v) -> const T&
{
	return	_v;
}

template<amtrs::endian To, amtrs::endian From, class T, typename std::enable_if<sizeof(T) != 1 && To != From>::type* = 0>
auto convert_endian(const T& _v) -> T
{
	T	tmp	= _v;
	for (int i = 0; i < sizeof(T)/2; ++i)
	{
		std::swap
		(
			reinterpret_cast<uint8_t&>(reinterpret_cast<uint8_t*>(&tmp)[i]),
			reinterpret_cast<uint8_t&>(reinterpret_cast<uint8_t*>(&tmp)[sizeof(T) - 1 - i])
		);
	}
	return	tmp;
}
AMTRS_NAMESPACE_IMPLIMENTS_END

template<endian To, endian From>
struct	basic_endiancvt
{
	template<class T>
	auto operator () (const T& _v) -> decltype(AMTRS_NAMESPACE_IMPLIMENTS(endian)::convert_endian<To, From>(std::declval<const T&>()))
	{
		return	AMTRS_NAMESPACE_IMPLIMENTS(endian)::convert_endian<To, From>(_v);
	}
};

template<endian ExternT>
struct	endiancvt
{
	template<class T>
	auto to (const T& _v) -> decltype(basic_endiancvt<ExternT, endian::native>{}(std::declval<const T&>()))
	{
		return	basic_endiancvt<ExternT, endian::native>{}(_v);
	}

	template<class T>
	auto from (const T& _v) -> decltype(basic_endiancvt<endian::native, ExternT>{}(std::declval<const T&>()))
	{
		return	basic_endiancvt<endian::native, ExternT>{}(_v);
	}
};


AMTRS_NAMESPACE_END

#ifdef	AMTRS_HAS_STD_ENDIAN
#else	// AMTRS_HAS_STD_ENDIAN
namespace std
{
	using	endian	= amtrs::endian;
}
#endif	// AMTRS_HAS_STD_ENDIAN

#endif
