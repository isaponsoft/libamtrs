/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__memory__endian__hpp
#define	__libamtrs__memory__endian__hpp
#include <algorithm>
#include "def.hpp"
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
