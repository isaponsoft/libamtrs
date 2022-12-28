/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__serialize__hpp
#define	__libamtrs__io__serialize__hpp
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
	if (!_out.good())
	{
		return	false;
	}
	union
	{
		T			v;
		uint8_t		b[sizeof(T)];
	};
	v	= endian_util::encode<E>(_val);
	write(_out, b, sizeof(T));
	return	_out.good();
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
	if (!_out.good())
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
	return	_out.good();
}



AMTRS_NAMESPACE_END
#endif
