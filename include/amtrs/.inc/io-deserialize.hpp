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
#ifndef	__libamtrs__io__deserialize__hpp
#define	__libamtrs__io__deserialize__hpp
#include <string_view>
#include "io-stream_traits.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(deserialize)
template<class T>
struct	is_containor
{
private:
	template<class C>
	static auto check(C& _c) -> decltype(_c.resize(0), _c.begin(), std::true_type());
	static auto check(...) -> std::false_type;
public:
	using	type	= decltype(check(std::declval<T&>()));
	static constexpr type	value	= type{};
};
template<class T>
struct	is_buffable
{
private:
	template<class C>
	static auto check(C& _c) -> decltype(_c.resize(0), _c.data(), std::true_type());
	static auto check(...) -> std::false_type;
public:
	using	type	= decltype(check(std::declval<T&>()));
	static constexpr type	value	= type{};
};
AMTRS_IMPLEMENTS_END(deserialize)


//! 整数型の値を指定したエンディアンでデシリアライズします。
template<endian E = endian::little, class T, class In, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
bool deserialize(T& _buff, In& _in)
{
	using	traits	= decltype(io::choise_stream_traits(std::declval<In&>()));
	if (!traits{}.good(_in))
	{
		return	false;
	}
	union
	{
		T			v;
		uint8_t		b[sizeof(T)];
	};
	traits{}.read(b, sizeof(T), _in);
	_buff	= endian_util::encode<E>(v);
	return	traits{}.good(_in);
}




//! コンテナをデシリアライズします。
template<class SizeT = uint32_t, endian E = endian::little, class T, class In, typename std::enable_if<AMTRS_IMPLEMENTS(deserialize)::is_containor<T>::value>::type* = nullptr>
bool deserialize(T& _buff, In& _in)
{
	SizeT	len;
	if (!deserialize<E>(len, _in))
	{
		return	false;
	}
	if (len == 0)
	{
		return	true;
	}
	_buff.resize(static_cast<size_t>(len));

	if constexpr (std::is_integral<typename T::value_type>::value && AMTRS_IMPLEMENTS(deserialize)::is_buffable<T>::value)
	{
		// 整数型かつdata()をサポートする場合は一括で転送する
		using	traits	= decltype(io::choise_stream_traits(std::declval<In&>()));

		traits{}.read(_buff.data(), len * sizeof(typename T::value_type), _in);

		// エンディアンの変換が必要ならエンディアンの変更を行う
		if constexpr (E == endian::native && sizeof(typename T::value_type) > 1)
		{
			for (auto& v : _buff)
			{
				v	= endian_util::encode<E>(v);
			}
		}
	}
	else
	{
		auto	it	= _buff.begin();
		for (SizeT i = 0; i < len; ++i)
		{
			if (!deserialize(*it++, _in))
			{
				return	false;
			}
		}
	}
	return	true;
}


AMTRS_NAMESPACE_END
#endif
