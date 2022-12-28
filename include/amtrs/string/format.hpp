/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__format__hpp
#define	__libamtrs__string__format__hpp
#include <stdio.h>
#include "sstring.hpp"
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


template<>
struct	format_traits<sstring>
{
	auto operator () (sstring const& _value) noexcept
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
		return	string_type(buff, (size_t)len);
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


template<class T>
struct	vaargs
{
	using	type	= T;

	vaargs(intmax_t _size, T* _values)
		: mSize(_size)
		, mValues(_values)
	{}

	size_t size() const noexcept { return mSize; }
	T* data() noexcept { return mValues; }
protected:
	intmax_t	mSize;
	T*			mValues;
};


struct	fmtarg
{
	enum	types
	{
		ft_charptr	= 0,
		ft_intmax	= 1,
		ft_text		= 2,
		ft_ptr		= 3,
	};

	intmax_t	type;
	union
	{
		char const*	s;
		intmax_t	n;
	};
	char const*	e;

	fmtarg() = default;
	fmtarg(char const* _v) : type(ft_charptr), s(_v) {}
	fmtarg(intmax_t _v) : type(ft_intmax), n(_v) {}
	fmtarg(text _v) : type(ft_text), s(_v.s), e(_v.e) {}
	fmtarg(sstring const& _v) : type(ft_text), s(_v.c_str()), e(_v.c_str() + _v.size()) {}
	fmtarg(fmtarg const&) = default;
	fmtarg(void const* _v) : type(ft_ptr), n(reinterpret_cast<intmax_t>(_v)) {}
};


template<class StringT, class FmtT, class... Args>
auto strf(FmtT _format, Args... _args) -> StringT
{
	using	imp	= AMTRS_IMPLEMENTS(format)::format_impl<StringT>;
	return	imp().format(
			AMTRS_IMPLEMENTS(format)::get_pointer<typename StringT::value_type>(_format),
			AMTRS_IMPLEMENTS(format)::argget(_args)...);
};

template<class StringT, class FmtT>
auto strf(FmtT _format) -> StringT
{
	return	AMTRS_IMPLEMENTS(format)::get_pointer<typename StringT::value_type>(_format);
};


AMTRS_API sstring format(char const* _format, std::initializer_list<fmtarg> _args);
AMTRS_API sstring formatva(char const* _format, vaargs<fmtarg> _args, bool _reverse = false);	// _reverse = true の時は引数を後ろから読み取ります


AMTRS_NAMESPACE_END
#endif
