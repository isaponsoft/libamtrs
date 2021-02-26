/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
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
