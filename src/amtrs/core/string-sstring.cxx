/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <algorithm>
#include <amtrs/string/sstring.hpp>
AMTRS_NAMESPACE_BEGIN
using namespace amtrs::strutil;

sstring::~sstring()
{
}

char const* sstring::zero()
{
	static char	z[1] = {0};
	return	z;
}

wsstring::~wsstring()
{
}

wchar_t const* wsstring::zero()
{
	static wchar_t	z[1] = {0};
	return	z;
}


sstring sstring::value_of(intmax_t v, int base)
{
	static const value_type	chs[]	= "01234567890abcdef";
	value_type				buff[sizeof(intmax_t) * 4 + 2];
	value_type*				dst		= buff + sizeof(buff);
	bool					minus	= false;
	if (v < 0)
	{
		minus	= true;
		v		= -v;
	}
	do
	{
		int		n	= v % base;
		v	= v / base;
		*(--dst) = chs[n];
	} while (v > 0);
	if (minus)
	{
		*(--dst) = '-';
	}

	return	text_type(dst, buff + sizeof(buff));
}

auto sstring::find(text_type _s) -> size_type
{
	if (size() < _s.size())
	{
		return	npos;
	}
	auto	c	= begin();
	auto	e	= end() - _s.size();
	do
	{
		if (starts_with(text_type(c, end()), _s))
		{
			return	c - begin();
		}
	} while (c++ != e);
	return	npos;
}

sstring sstring::replace(text_type _from, text_type _to) const
{
	sstring	retval;
	if (!empty())
	{
		value_type const*	beg	= data();
		value_type const*	cur	= beg;
		value_type const*	end	= data() + size();
		while (cur != end)
		{
			if (starts_with(text_type(cur, end), _from))
			{
				if (beg != cur)
				{
					retval.append(text_type(beg, cur));
				}
				if (_to.size())
				{
					retval.append(_to);
				}
				cur += _from.size();
				beg	= cur;
				continue;
			}
			++cur;
		}
		if (beg != cur)
		{
			retval.append(text_type(beg, cur));
		}
	}
	return	retval;
}


bool sstring::resize(size_type _n)
{
	if (_n > 0)
	{
		simplearray<value_type>	buff(_n+1);
		size_type			sz = mBuff.size(); 
		if (sz)
		{
			if (_n < sz)
			{
				sz	= _n;
			}
			std::copy_n(mBuff.data(), sz, buff.data());
		}
		buff[_n]	= 0;
		mBuff		= buff;
	}
	else
	{
		mBuff	= {};
	}
	return	true;
}

sstring::sstring(value_type const* _s, size_type _n)
{
	if (_n > 0)
	{
		mBuff		= simplearray<value_type>(_n + 1);
		if (_s)
		{
			std::copy_n(_s, size(), mBuff.data());
		}
		mBuff[_n]	= 0;
	}
}

sstring sstring::operator + (text_type _r) const
{
	sstring	r(size() + _r.size());
	value_type*	d	= r.data();
	std::copy_n(data(), size(), d); d += size();
	std::copy_n(_r.data(), _r.size(), d);
	return	r;
}


void sstring::append(text_type _r)
{
	if (_r.size() == 0)
	{
		return;
	}
	sstring	nb(size() + _r.size());
	value_type*	d	= nb.data();
	if (size() > 0)
	{
		std::copy_n(data(), size(), d);
		d += size();
	}
	std::copy_n(_r.data(), _r.size(), d);
	swap(nb);
}



wsstring wsstring::value_of(intmax_t v, int base)
{
	static const value_type	chs[]	= L"01234567890abcdef";
	value_type				buff[sizeof(intmax_t) * 4 + 2];
	value_type*				dst		= buff + sizeof(buff);
	bool				minus	= false;
	if (v < 0)
	{
		minus	= true;
		v		= -v;
	}
	do
	{
		int		n	= v % base;
		v	= v / base;
		*(--dst) = chs[n];
	} while (v > 0);
	if (minus)
	{
		*(--dst) = '-';
	}

	return	text_type(dst, buff + sizeof(buff));
}

auto wsstring::find(text_type _s) -> size_type
{
	if (size() < _s.size())
	{
		return	npos;
	}
	auto	c	= begin();
	auto	e	= end() - _s.size();
	do
	{
		if (starts_with(text_type(c, end()), _s))
		{
			return	c - begin();
		}
	} while (c++ != e);
	return	npos;
}

wsstring wsstring::replace(text_type _from, text_type _to)
{
	wsstring	retval;
	if (!empty())
	{
		value_type const*	beg	= data();
		value_type const*	cur	= beg;
		value_type const*	end	= data() + size();
		while (cur != end)
		{
			if (starts_with(text_type(cur, end), _from))
			{
				if (beg != cur)
				{
					retval.append(text_type(beg, cur));
				}
				if (_to.size())
				{
					retval.append(_to);
				}
				cur += _from.size();
				beg	= cur;
				continue;
			}
			++cur;
		}
		if (beg != cur)
		{
			retval.append(text_type(beg, cur));
		}
	}
	return	retval;
}


bool wsstring::resize(size_type _n)
{
	if (_n > 0)
	{
		simplearray<value_type>	buff(_n+1);
		size_type				sz = mBuff.size(); 
		if (sz)
		{
			if (_n < sz)
			{
				sz	= _n;
			}
			std::copy_n(mBuff.data(), sz, buff.data());
		}
		buff[_n]	= 0;
		mBuff		= buff;
	}
	else
	{
		mBuff	= {};
	}
	return	true;
}

wsstring::wsstring(value_type const* _s, size_type _n)
{
	if (_n > 0)
	{
		mBuff		= simplearray<wchar_t>(_n + 1);
		if (_s)
		{
			std::copy_n(_s, size(), mBuff.data());
		}
		mBuff[_n]	= 0;
	}
}

wsstring wsstring::operator + (text_type _r) const
{
	wsstring	r(size() + _r.size());
	value_type*	d	= r.data();
	std::copy_n(data(), size(), d); d += size();
	std::copy_n(_r.data(), _r.size(), d);
	return	r;
}


void wsstring::append(text_type _r)
{
	if (_r.size() == 0)
	{
		return;
	}
	wsstring	nb(size() + _r.size());
	value_type*	d	= nb.data();
	if (size() > 0)
	{
		std::copy_n(data(), size(), d);
		d += size();
	}
	std::copy_n(_r.data(), _r.size(), d);
	swap(nb);
}
AMTRS_NAMESPACE_END
