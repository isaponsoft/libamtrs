/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/string/text.hpp>
AMTRS_STRING_NAMESPACE_BEGIN

void lowercase(char* _begin, char* _end)
{
	while (_begin != _end)
	{
		if (*_begin >= 'A' && *_begin <= 'Z')
		{
			*_begin	= *_begin - 'A' + 'a';
		}
		++_begin;
	}
}


void uppercase(char* _begin, char* _end)
{
	while (_begin != _end)
	{
		if (*_begin >= 'a' && *_begin <= 'z')
		{
			*_begin	= *_begin - 'a' + 'A';
		}
		++_begin;
	}
}

text skip_blank(text _code)
{
	return	_code.substr(skip_blank(_code.s, _code.e));
}


char const* skip_blank(char const* _begin, char const* _end)
{
	while (_begin != _end)
	{
		if (*_begin == ' ' || *_begin == '\t' || *_begin == '\r' || *_begin == '\n')
		{
			++_begin;
			continue;
		}
		if (starts_with({_begin, _end}, "//") || starts_with({_begin, _end}, "#line"))
		{
			while (_begin != _end)
			{
				if (*_begin == '\n' || *_begin == '\r')
				{
					++_begin;
					break;
				}
				++_begin;
			}
			continue;
		}
		if (starts_with({_begin, _end}, "/*"))
		{
			_begin += 2;
			while (_begin != _end)
			{
				if (starts_with({_begin, _end}, "*/"))
				{
					_begin += 2;
					break;
				}
				++_begin;
			}
			continue;
		}
		break;
	}
	return	_begin;
}



char const* skip_one_return(char const* _begin, char const* _end)
{
	char const*	s	= _begin;
	auto c = *s; 
	if (c == '\r' || c == '\n')
	{
		++s;
		if (s != _end && (*s == '\r' || *s == '\n') && (c != *s))
		{
			++s;
		}
	}
	return	s;
}

text skip_one_return(text _code)
{
	auto	next	= skip_one_return(_code.s, _code.e);
	if (!next)
	{
		return	_code.substr(0, 0);
	}
	return	_code.substr(next);
}


text trim_blank(text _code)
{
	auto	e	= _code.e;
	auto	s	= skip_blank(_code.s, e);
	if (s != e)
	{
		while ((e-1) != s)
		{
			char	c	= *(e-1);
			if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
			{
				break;
			}
			--e;
		}
	}
	return	{s, e};
}


text trim_brackets(text _code)
{
	char const*	s	= _code.s;
	char const*	e	= _code.e;
	if ((e - s) >= 2)
	{
		if (s[0] == '(' && e[-1] == ')')
		{
			return	trim_brackets({s+1, e-1});
		}
		if (s[0] == '[' && e[-1] == ']')
		{
			return	trim_brackets({s+1, e-1});
		}
		if (s[0] == '{' && e[-1] == '}')
		{
			return	trim_brackets({s+1, e-1});
		}
	}
	return	_code;
}


text trim_circle_brackets(text _code)
{
	auto		code	= trim_blank(_code);
	char const*	s		= code.data();
	char const*	e		= code.data() + code.size();
	if ((e - s) >= 2)
	{
		if (s[0] == '(' && e[-1] == ')')
		{
			return	trim_blank(trim_brackets({s+1, e-1}));
		}
	}
	return	_code;
}

text skip_utf8_bom(text _code)
{
	return	_code.starts_with(text(utf8bom, 3)) ? _code.substr(3) : _code;
}


AMTRS_STRING_NAMESPACE_END
