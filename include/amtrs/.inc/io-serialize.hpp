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
#ifndef	__libamtrs__io__serialize__hpp
#define	__libamtrs__io__serialize__hpp
#include <string_view>
#include "io-stream_traits.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(serialize)
template<class T>
struct	is_containor
{
private:
	template<class C>
	static auto check(C& _c) -> decltype(_c.size(), _c.begin(), _c.end(),  std::true_type());
	static auto check(...) -> std::false_type;
public:
	using	type	= decltype(check(std::declval<T&>()));
	static constexpr type	value	= type{};
};
AMTRS_IMPLEMENTS_END(serialize)



//! 整数型の値を指定したエンディアンでシリアライズします。
template<endian E, class T, class OutT, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
bool serialize(OutT& _out, T const& _val)
{
	using	traits	= decltype(io::choise_stream_traits(std::declval<OutT&>()));
	if (!traits{}.good(_out))
	{
		return	false;
	}
	union
	{
		T			v;
		uint8_t		b[sizeof(T)];
	};
	v	= endian_util::encode<E>(_val);
	traits{}.write(_out, b, sizeof(T));
	return	traits{}.good(_out);
}


//! 整数型の値をリトルエンディアンでシリアライズします。
template<class T, class OutT, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
bool serialize(OutT& _out, T const& _val)
{
	return	serialize<endian::little, T, OutT>(_out, _val);
}



//! コンテナをシリアライズします。
template<class SizeT = uint32_t, class OutT, class T, typename std::enable_if<AMTRS_IMPLEMENTS(serialize)::is_containor<T>::value>::type* = nullptr>
bool serialize(OutT& _out, T const& _val)
{
	using	traits	= decltype(io::choise_stream_traits(std::declval<OutT&>()));
	if (!traits{}.good(_out))
	{
		return	false;
	}
	if (!serialize(_out, static_cast<SizeT>(_val.size())))
	{
		return	false;
	}
	for (auto& _v : _val)
	{
		if (!serialize(_out, _v))
		{
			return	false;
		}
	}
	return	traits{}.good(_out);
}



AMTRS_NAMESPACE_END
#endif
