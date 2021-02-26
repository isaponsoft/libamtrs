/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__lexar_rules__hpp
#define	__libamtrs__passion__lexar_rules__hpp
#include "string.hpp"
PASSION_NAMESPACE_BEGIN



struct	is_space
{
	constexpr bool operator () (int c) const noexcept
	{
		return	c == 0xfeff || c == ' ' || c == '\t';
	}
};

struct	is_return
{
	constexpr bool operator () (int c) const noexcept
	{
		return	c == '\n' || c == '\r';
	}
};

struct	is_blunk
{
	constexpr bool operator () (int c) const noexcept
	{
		return	is_space{}(c) || is_return{}(c);
	}
};


struct	keyword_rules
{
	constexpr bool first(int c) const noexcept
	{
		return	(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
	}

	constexpr bool second(int c) const noexcept
	{
		return	first(c) || (c >= '0' && c <= '9');
	}

	constexpr bool terminate(int c) const noexcept
	{
		return	true;
	}
};


struct	bracket_rules
{
	constexpr bool first(int c) const noexcept
	{
		return	c == '{' || c == '}'
			||	c == '[' || c == ']'
			||	c == '(' || c == ')';
	}

	constexpr bool second(int c) const noexcept
	{
		return	false;
	}

	constexpr bool terminate(int c) const noexcept
	{
		return	true;
	}
};


struct	string_rules
{
	constexpr bool first(int c) noexcept
	{
		start_	= c;
		return	c == '"';
	}

	constexpr bool second(int c) noexcept
	{
		if (escape_)
		{
			escape_	= false;
			return	true;
		}
		if (close_)
		{
			if (c != start_)
			{
				return	false;
			}
			close_	= false;
			return	true;
		}
		if (c == start_)
		{
			close_	= true;
		}
		if (c == '\\')
		{
			escape_	= true;
		}
		return	true;
	}

	constexpr bool terminate(int c) const noexcept
	{
		return	true;
	}

private:
	int		start_	= 0;
	bool	close_	= false;
	bool	escape_	= false;
};


struct	namespace_separator_rules
{
	constexpr bool first(int c) noexcept
	{
		return	c == ':';
	}

	constexpr bool second(int c) noexcept
	{
		if (count_ == 0)
		{
			if (c != ':')
			{
				return	false;
			}
			++count_;
			return	true;
		}
		return	false;
	}

	constexpr bool terminate(int c) const noexcept
	{
		return	c != ':' && count_  == 1;
	}

private:
	int		count_	= 0;
};


struct	lineterminate_rules
{
	constexpr bool first(int c) noexcept
	{
		return	c == ';';
	}

	constexpr bool second(int c) noexcept
	{
		return	false;
	}

	constexpr bool terminate(int c) const noexcept
	{
		return	true;
	}
};


struct	extends_rules
{
	constexpr bool first(int c) noexcept
	{
		return	c == ':';
	}

	constexpr bool second(int c) noexcept
	{
		return	false;
	}

	constexpr bool terminate(int c) const noexcept
	{
		return	true;
	}
};



PASSION_NAMESPACE_END
#endif
