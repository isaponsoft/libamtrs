/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__char_iterator__hpp
#define	__libamtrs__string__char_iterator__hpp
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN


template<class CharT>
class	ascii_iterator
{
public:
	using	char_type		= CharT;
	using	int_type		= unsigned int;
	using	pointer			= char_type*;

	template<class It>
	It inc(It _cur, It _end) const noexcept
	{
		if (!empty(_cur, _end))
		{
			++_cur;
		}
		return	_cur;
	}


	template<class It>
	It read(int_type& _dest, It _cur, It _end) const noexcept
	{
		if (!empty(_cur, _end))
		{
			_dest	= (*_cur) & 0xff;
			++_cur;
		}
		else
		{
			_dest	= 0;
		}
		return	_cur;
	}


	template<class It>
	bool empty(It _cur, It _end) const noexcept
	{
		return	_cur == _end;
	}
};


template<class CharT>
class	mb_iterator
{
public:
	using	char_type	= CharT;
	using	int_type	= int;
	using	pointer		= char_type*;

	template<class It>
	It inc(It _cur, It _end) const noexcept
	{
		auto	s	= std::mbrlen(_cur, _end - _cur, &mState);
		if ((int)s > 0)
		{
			_cur += s;
		}
		return	_cur;
	}


	template<class It>
	It read(int_type& _dest, It _cur, It _end) const noexcept
	{
		int_type		c	= 0;
		if (_cur != _end)
		{
			std::mbstate_t	m	= std::mbstate_t();
			auto			s	= std::mbrlen(_cur, _end - _cur, &m);
			if ((int)s > 0)
			{
				for (decltype(s) i = 0; i < s; ++i)
				{
					c	= (c<<8) + (*reinterpret_cast<typename std::make_unsigned<const char_type>::type*>(&(_cur[i])));
				}
				_cur += s;
			}
		}
		_dest	= c;
		return	_cur;
	}


	template<class It>
	bool empty(It _cur, It _end) const noexcept
	{
		return	_cur == _end;
	}

protected:
	std::mbstate_t	mState	= std::mbstate_t();
};


AMTRS_NAMESPACE_END
#endif
