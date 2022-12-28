/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/string/numeric.hpp>
AMTRS_STRING_NAMESPACE_BEGIN

static constexpr char const*	gNumChars	= "01234567890abcdef";

size_t write_int(char* _buffer, size_t _bufferSize, intmax_t _num, int _base) noexcept
{
	size_t		len	= 0;
	intmax_t	t;
	if (_base >= 16)
	{
		return	0;
	}


	t	= _num;
	do
	{
		++len;
		t	= t / _base;
	} while (t > 0);

	if (_bufferSize < len)
	{
		return	0;
	}

	char*		dest	= _buffer + len;
	t	= _num;
	do
	{
		*--dest	= gNumChars[t % _base];
		t	= t / _base;
	} while (t > 0);

	return	len;
}



AMTRS_STRING_NAMESPACE_END
