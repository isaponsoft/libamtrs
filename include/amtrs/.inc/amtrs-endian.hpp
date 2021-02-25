/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__memory__endian__hpp
#define	__libamtrs__memory__endian__hpp
AMTRS_NAMESPACE_BEGIN

//! c++20 の STL が利用可能な場合は std::endian へのエイリアスになります。
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


namespace endian_util {

//! メモリに書き込んだ時にエンディアンが入れ替わるように値を変更します。
template<class T>
constexpr T bitswap(T _value) noexcept
{
	if constexpr (sizeof(T) > 1)
	{
		constexpr std::size_t	length	= sizeof(T);
		constexpr std::size_t	count	= length / 2;
		constexpr std::size_t	last	= length - 1;
		T	retval(_value);
		for (std::size_t i = 0; i < count; ++i)
		{
			std::swap(reinterpret_cast<char*>(&retval)[i], reinterpret_cast<char*>(&retval)[last - i]);
		}
		return	retval;
	}
	else
	{
		return	_value;
	}
}


//! メモリに書き込んだ時にエンディアンが入れ替わるように値を変更します。
template<class V, class T>
constexpr V bitswap(T const (&_value)[sizeof(V)]) noexcept
{
	return	bitswap(*reinterpret_cast<const V*>(_value));
}


//! メモリに書き込んだ時に指定したエンディアンになるように値を変更します。
template<endian E, class V, class T>
constexpr V bitswap_if(T const (&_value)[sizeof(V)]) noexcept
{
	if constexpr (endian::native != E)	return	bitswap<V>(_value);
	else								return	*reinterpret_cast<const V*>(_value);
}

//! メモリに書き込んだ時に指定したエンディアンになるように値を変更します。
template<endian E, class V>
constexpr V bitswap_if(V _value) noexcept
{
	if constexpr (endian::native != E)	return	bitswap(_value);
	else								return	_value;
}

//! メモリに書き込んだ時に指定したエンディアンになるように値を変更します。
template<endian E, class V, class T>
constexpr V encode(T const (&_value)[sizeof(V)]) noexcept { return bitswap_if<E, V>(_value); }

//! 指定したエンディアンで書き込まれたメモリから読み込んだデータをネイティブの値として使えるように値を修正します。
template<endian E, class V, class T>
constexpr V decode(T const (&_value)[sizeof(V)]) noexcept { return bitswap_if<E, V>(_value); }

//! メモリに書き込んだ時に指定したエンディアンになるように値を変更します。
template<endian E, class T>
constexpr T encode(T _value) noexcept { return bitswap_if<E>(_value); }

//! 指定したエンディアンで書き込まれたメモリから読み込んだデータをネイティブの値として使えるように値を修正します。
template<endian E, class V>
constexpr V decode(V _value) noexcept { return bitswap_if<E>(_value); }



} // endian_util
AMTRS_NAMESPACE_END
#endif
