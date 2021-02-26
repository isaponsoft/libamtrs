/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__basic__format__hpp
#define	__libamtrs__basic__format__hpp
#include <string>
AMTRS_NAMESPACE_BEGIN

template<class...>
struct	format_traits;


template<class T>
struct	format_traits<T>
{
	auto operator () (T const& _value) noexcept
	{
		return	_value;
	}
};


template<class T>
struct	format_traits<T*>
{
	auto operator () (T const* _value) noexcept
	{
		return	_value;
	}
};


template<class CharT, class Traits>
struct	format_traits<std::basic_string<CharT, Traits>>
{
	auto operator () (std::basic_string<CharT, Traits> const& _value) noexcept
	{
		return	_value.c_str();
	}
};



AMTRS_IMPLEMENTS_BEGIN(format)

template<class StringT>
struct	format_impl
{
	template<class... Args>
	StringT format(typename StringT::value_type const* _format, Args&&... _args)
	{
		using	string_type	= StringT;
		char	buff[1024];
		auto	len	= ::snprintf(buff, sizeof(buff), _format, format_traits<typename std::remove_const<Args>::type>{}(std::forward<Args>(_args))...);
		return	string_type(std::string_view(buff, len));
	}
};


template<class CharT>
CharT const* get_pointer(CharT const* _p)
{
	return	_p;
}

template<class CharT, size_t N>
CharT const* get_pointer(CharT const (&_s)[N])
{
	return	&_s[0];
}

#if	__cplusplus > 201703
template<class CharT>
CharT const* get_pointer(char8_t const* _p)
{
	return	reinterpret_cast<CharT const*>(_p);
}
#endif

template<class T>
auto argget(T _arg)
{
	return	format_traits<typename std::remove_const<T>::type>{}(_arg);
}

AMTRS_IMPLEMENTS_END(format)




template<class StringT, class FmtT, class... Args>
auto format(FmtT _format, Args... _args) -> StringT
{
	using	imp	= AMTRS_IMPLEMENTS(format)::format_impl<StringT>;
	return	imp().format(
			AMTRS_IMPLEMENTS(format)::get_pointer<typename StringT::value_type>(_format),
			AMTRS_IMPLEMENTS(format)::argget(_args)...);
};

template<class StringT, class FmtT>
auto format(FmtT _format) -> StringT
{
	return	AMTRS_IMPLEMENTS(format)::get_pointer<typename StringT::value_type>(_format);
};





AMTRS_NAMESPACE_END
#endif
