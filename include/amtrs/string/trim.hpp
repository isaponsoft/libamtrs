/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__trim__hpp
#define	__libamtrs__string__trim__hpp
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(trim)
template<class T, class CharT>
bool has(T const& _delms, CharT _c)
{
	for (auto d : _delms)
	{
		if (d == _c)
		{
			return	true;
		}
	}
	return	false;
}

AMTRS_IMPLEMENTS_END(trim)


template<class T>
auto trim(T const& _string, T const& _delms = "\r\n\t ") -> T
{
	if (_string.empty())
	{
		return	_string;
	}

	auto	begin	= _string.begin();
	auto	end		= _string.end();
	while (begin != end && AMTRS_IMPLEMENTS(trim)::has<T>(_delms, *begin))
	{
		++begin;
	}
	if (begin != end)
	{
		while (begin != (end-1))
		{
			if (!AMTRS_IMPLEMENTS(trim)::has<T>(_delms, *(end-1)))
			{
				break;
			}
			--end;
		}
	}
	if (end == begin)
	{
		return	{};
	}
	return	T(&*begin, typename T::size_type(end - begin));
}


template<class T>
auto trim_first(T const& _string, T const& _delms = "\r\n\t ") -> T
{
	if (_string.empty())
	{
		return	_string;
	}

	auto	begin	= _string.begin();
	auto	end		= _string.end();
	while (begin != end && AMTRS_IMPLEMENTS(trim)::has<T>(_delms, *begin))
	{
		++begin;
	}
	return	T( &*begin, typename T::size_type(end - begin) );
}


AMTRS_NAMESPACE_END
#endif
