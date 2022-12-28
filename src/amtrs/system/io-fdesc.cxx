/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/io/fdesc.hpp>
#include <amtrs/string/convert.hpp>
AMTRS_IO_NAMESPACE_BEGIN


fdesc& fdesc::write_noconv(text v)
{
	auto	w	= write(v.s, (int)v.size());
	return	*this;
}

fdesc& fdesc::operator << (text	v)
{
	return	write_noconv(c8_to_cr(v));
}

fdesc& fdesc::operator << (char const* v)
{
	return	operator << (text(v));
}


fdesc& fdesc::operator << (void* v)
{
	operator << (text("0x"));
	return	write((intmax_t)v, 16);
}

fdesc& fdesc::write(char v)
{
	char	buf[2] = { v, 0 };
	return	operator << (text(buf, 1));
}

fdesc& fdesc::write(intmax_t v, int base)
{
	static const char	chs[]	= "01234567890abcdef";
	char				buff[sizeof(intmax_t) * 4 + 2];
	char*				dst		= buff + sizeof(buff);
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
	return	operator << (text(dst, buff + sizeof(buff)));
}

fdesc& fdesc::write(uintmax_t v, int base)
{
	static const char	chs[]	= "01234567890abcdef";
	char				buff[sizeof(intmax_t) * 4 + 2];
	char*				dst		= buff + sizeof(buff);
	do
	{
		int		n	= v % base;
		v	= v / base;
		*(--dst) = chs[n];
	} while (v > 0);
	return	operator << (text(dst, buff + sizeof(buff)));
}

fdesc& fdesc::flush()
{
	return	*this;
}


AMTRS_IO_NAMESPACE_END
