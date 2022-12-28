/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <cwchar>
#include <amtrs/string/format.hpp>
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(format)

static fmtarg get(std::initializer_list<fmtarg>& _args, int _i)
{
	for (auto& a : _args)
	{
		if (_i == 0)
		{
			return	a;
		}
		--_i;
	}
	return	fmtarg("");
}

AMTRS_IMPLEMENTS_END(format)

struct	fmtinfo
{
	int		prm	= 0;
	char	flg	= 0;
	int		wid	= -1;
	int		pre	= -1;
	char	typ	= 0;
};
static char const* read_fmt(fmtinfo& _fmt, text _str)
{
	if (_str.empty())
	{
		return	nullptr;
	}
	if (_str.front() >= '1' && _str.front() <= '9')
	{
		int		num	= 0;
		do
		{
			num = num * 10 + _str.front() - '0';
			++_str;
		} while (_str.front() >= '0' && _str.front() <= '9');
		if (_str.front() == '$')
		{
			++_str;
			_fmt.prm	= num;
		}
		else
		{
			_fmt.wid	= num;
		}
		if (_str.empty())
		{
			return	nullptr;
		}
	}
	// flags
	if (_fmt.wid == -1)
	{
		char	c	= _str.front();
		if (c == '-' || c == '+' || c == ' ' || c == '0' || c == '\'' || c == '#')
		{
			_fmt.flg	= c;
			++_str;
			if (_str.empty())
			{
				return	nullptr;
			}
		}
		// width
		if (_str.front() >= '1' && _str.front() <= '9')
		{
			int		num	= 0;
			do
			{
				num = num * 10 + _str.front() - '0';
				++_str;
			} while (_str.front() >= '0' && _str.front() <= '9');
			_fmt.wid	= num;
			if (_str.empty())
			{
				return	nullptr;
			}
		}
	}
	// precision
	if (_str.front() == '.')
	{
		++_str;
		if (_str.front() >= '1' && _str.front() <= '9')
		{
			int		num	= 0;
			do
			{
				num = num * 10 + _str.front() - '0';
				++_str;
			} while (_str.front() >= '0' && _str.front() <= '9');
			_fmt.pre	= num;
			if (_str.empty())
			{
				return	nullptr;
			}
		}
		else
		{
			return	nullptr;
		}
	}
	// type
	char	c	= _str.front();
	if (c == 'd' || c == 'i' || c == 'u' || c == 'f' || c == 'e' || c == 'E' || c == 'g' || c == 'G'
	 || c == 'x' || c == 'X' || c == 'o' || c == 's' || c == 'c' || c == 'p' || c == 'P'
	)
	{
		_fmt.typ	= c;
		++_str;
	}
	if (_str.front() != '}')
	{
		return	nullptr;
	}
	return	_str.s+1;
}

sstring formatva(char const* _format, vaargs<fmtarg> _args, bool _reverse)
{
	sstring	t;
	char const*	beg	= _format;
	text		cur	= _format;
	int			idx	= 0;
	fmtarg*		arg	= _args.data();
	while (!cur.empty())
	{
		if (cur.front() == '\\')
		{
			t.append(text(beg, cur.s));
			++cur;
			if (!cur.empty())
			{
				t.append(text(cur.s, cur.s+1));
				++cur;
				beg	= cur.s;
			}
			continue;
		}
		if (cur.front() == '{')
		{
			fmtinfo	fmt;
			fmt.prm	= idx+1;
			auto	next	= read_fmt(fmt, cur.substr(1));
			if (!next)
			{
				++cur;
				continue;
			}
			if (beg != cur.s)
			{
				t.append(text(beg, cur.s));
			}
			cur	= cur.substr(next);

			idx	= fmt.prm-1;
			if (idx >= _args.size())
			{
				beg	= cur.s;
				continue;
			}

			auto	a	= arg[(_reverse ? -(1+idx) : idx)];
			if ((a.type == fmtarg::ft_charptr || a.type == fmtarg::ft_text) && (fmt.typ != 'p' && fmt.typ != 'P'))
			{
				text	val	= a.type == fmtarg::ft_charptr ? text(a.s) : text(a.s, a.e);
				if (fmt.wid == -1 || (fmt.wid > 0 && fmt.wid <= val.size()))
				{
					t.append(val);
					
				}
				else
				{
					size_t	padding	= fmt.wid - val.size();
					if (fmt.flg == '-')
					{
						t.append(val);
						for (size_t i = 0; i < padding; ++i) { t.append(" "); }
					}
					else
					{
						for (size_t i = 0; i < padding; ++i) { t.append(" "); }
						t.append(val);
					}
				}
			}
			else if (a.type == fmtarg::ft_intmax || a.type == fmtarg::ft_ptr)
			{
				if (fmt.typ == 'c')
				{
					char		buff[10] = {(char)a.n};
					t.append(text(buff, 1));
				}
				else
				{
					static constexpr char	C[] = "0123456789ABCDEF";
					static constexpr char	c[] = "0123456789abcdef";
					char		buff[sizeof(intmax_t) * 8];
					uintmax_t	num		= (uintmax_t)((a.type == fmtarg::ft_intmax && a.n < 0) ? -a.n : a.n);
					unsigned	base	= (fmt.typ == 'o'                  ) ? 8
										: (fmt.typ == 'x' || fmt.typ == 'X') ? 16
										: (fmt.typ == 'p' || fmt.typ == 'P') ? 16
										: 10;

					// 数値部分をバッファへ書き出し、text val へセットする
					char*		cur		= buff + sizeof(buff);
					char const*	chrs	= (fmt.typ == 'X' || fmt.typ == 'P') ? C : c;
					do
					{
						*(--cur) = chrs[(num % base)];
						num = num / base;
					} while (num > 0);
					text	val(cur, buff + sizeof(buff));

					// フォーマットに従って書き出す
					if (fmt.typ == 'p' || fmt.typ == 'P')
					{
						if ((int)fmt.wid < (int)(sizeof(void*)*2))
						{ 
							fmt.wid = sizeof(void*)*2; 
						}
						fmt.flg	= '0';
					}
					// 幅が無い、もしくは幅が桁数より少ない
					if (fmt.wid == -1 || (fmt.wid > 0 && fmt.wid <= val.size()))
					{
						// 10進数なら - を負荷する
						if (base == 10 && a.type == fmtarg::ft_intmax && a.n < 0)
						{
							*(--cur) = '-';
							val	= text(cur, buff + sizeof(buff));
						}
						t.append(val);
					}
					else
					{
						// 10進数なら - を負荷する
						if (base == 10 && a.type == fmtarg::ft_intmax && a.n < 0)
						{
							*(--cur) = '-';
							val	= text(cur, buff + sizeof(buff));
						}
						size_t	padding	= fmt.wid - val.size();
						if (fmt.flg == '-')
						{
							t.append(val);
							for (size_t i = 0; i < padding; ++i) { t.append(" "); }
						}
						else
						{
							for (size_t i = 0; i < padding; ++i) { t.append(fmt.flg == '0' ? "0" : " "); }
							t.append(val);
						}
					}
				}
			}
			beg	= cur.s;
			++idx;
			continue;
		}
		++cur;
	}
	if (beg != cur.s)
	{
		t.append(text(beg, cur.s));
	}
	return	t;
}


sstring format(char const* _format, std::initializer_list<fmtarg> _args)
{
	simplearray<fmtarg>	args(_args);
	return	formatva(_format, vaargs<fmtarg>(args.size(), args.data()));
}


AMTRS_NAMESPACE_END
