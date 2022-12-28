/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/string/text.hpp>
#include <amtrs/string/sourcecode.hpp>
AMTRS_SOURCECODE_NAMESPACE_BEGIN
using namespace strutil;

void sourcecode_parser_initialize()
{
	using	err	= sourcecode_error;
	errorval::add_error_type(errorval_type<err>{}(), [](int e) -> char const*
	{
		switch ((err)e)
		{
			case err::success :				return	"success";
			case err::no_start_token :		return	"Have not start token.";
			case err::no_end_token :		return	"Have not end token.";
			case err::syntax_error :		return	"Syntax error.";
			default :						return	"Unkown";
		}
	});
}


result<srcdata> read_brackets(text _code, unsigned int _flags)
{
	char const*	beg	= _code.data();
	char const*	s	= beg;
	char const*	e	= beg + _code.size();
	if ((s == e) || (*s != '(' && *s != '[' && *s != '{'))
	{
		return	{sourcecode_error::no_start_token};
	}

	char	c	= *s == '(' ? ')'
				: *s == '[' ? ']'
				:             '}';
	++s;
	while (s != e)
	{
		if (*s == c)
		{
			++s;
			return	srcdata{
				text(_code.s, s),
				_code.substr(s)
			};
		}
		if (*s == '\\')
		{
			++s;
			if (s != e)
			{
				++s;
			}
			continue;
		}

		auto	brk	= read_brackets({s, e}, _flags);
		if (brk.good())
		{
			s	= brk.next.s;
			continue;
		}
		else if (!brk.err.is(sourcecode_error::no_start_token))
		{
			return	brk.err;
		}

		auto	str	= read_string({s, e}); 
		if (str.good())
		{
			s	= str.next.s;
			continue;
		}
		else if (!str.err.is(sourcecode_error::no_start_token))
		{
			return	str.err;
		}

		++s;
	}
	return	{sourcecode_error::no_end_token};
}



result<numeric_literal> read_numeric(text _code, unsigned int _flags)
{
	numeric_literal	retval;
	char const*		beg	= _code.data();
	char const*		cur	= beg;
	char const*		end	= beg + _code.size();

	retval.code				= {beg, cur};
	retval.code_int			= {beg, cur};
	retval.code_fraction	= {beg, cur};
	retval.code_exponent	= {beg, cur};
	if (!(*cur == '+' || *cur == '-' || (*cur >= '0' && *cur <= '9')))
	{
		return	errorval(sourcecode_error::no_start_token);
	}
	char const*	int_beg	= cur;
	if (*cur == '+' || *cur == '-')
	{
		++cur;
		cur	= skip_blank(cur, end);
		if (cur == end)
		{
			return	errorval(sourcecode_error::syntax_error);
		}
		int_beg	= cur;
	}
	bool	binaly	= *cur == '0';
	if (binaly)
	{
		++cur;
		if (cur == end)
		{
			// +0 or -0
			retval.type		= numeric_literal::nt_decimal;
			retval.code		= { beg, cur };
			retval.next		= _code.substr(cur);
			retval.code_int	= { int_beg, cur };
			retval.code_int	= { int_beg, cur };
			return	{errorval(sourcecode_error::success), std::move(retval)};
		}
		// Hex
		if (*cur == 'x')
		{
			++cur;
			if (cur == end)
			{
				return	errorval(sourcecode_error::syntax_error);
			}
			while (cur != end && ((*cur >= '0' && *cur <= '9') || (*cur >= 'a' && *cur <= 'f') || (*cur >= 'A' && *cur <= 'F')))
			{
				++cur;
			}
			retval.type		= numeric_literal::nt_hex;
			retval.code		= { beg, cur };
			retval.next		= _code.substr(cur);
			retval.code_int	= { int_beg, cur };
			return	{ errorval(sourcecode_error::success), std::move(retval) };
		}
		// Bin
		if (*cur == 'b')
		{
			++cur;
			if (cur == end)
			{
				return	errorval(sourcecode_error::syntax_error);
			}
			while (cur != end && ((*cur >= '0' && *cur <= '1')))
			{
				++cur;
			}
			retval.type		= numeric_literal::nt_bin;
			retval.code		= { beg, cur };
			retval.next		= _code.substr(cur);
			retval.code_int	= { int_beg, cur };
			return	{ errorval(sourcecode_error::success), std::move(retval) };
		}
		// Oct
		while (cur != end && ((*cur >= '0' && *cur <= '7')))
		{
			++cur;
		}
		retval.type		= numeric_literal::nt_oct;
		retval.code		= { beg, cur };
		retval.next		= _code.substr(cur);
		retval.code_int	= { int_beg, cur };
		return	{ errorval(sourcecode_error::success), std::move(retval) };
	}
	retval.type		= numeric_literal::nt_decimal;
	if (cur == end || (*cur < '0' || *cur > '9'))
	{
		return	{ errorval(sourcecode_error::syntax_error) };
	}
	while (cur != end && (*cur >= '0' && *cur <= '9'))
	{
		++cur;
	}
	retval.code_int	= { int_beg, cur };
	if (cur != end && *cur == '.')
	{
		++cur;
		char const*	frac_beg	= cur;
		while (cur != end && (*cur >= '0' && *cur <= '9'))
		{
			++cur;
		}
		retval.code_fraction	= { frac_beg, cur };
	}
	retval.code		= { beg, cur };
	retval.next		= _code.substr(cur);
	return	{ errorval(sourcecode_error::success), std::move(retval) };
}


result<string_literal> read_string(text _code, unsigned int _flags)
{
	char const*	s	= _code.data();
	char const*	e	= _code.data() + _code.size();
	if (s == e)
	{
		return	errorval(sourcecode_error::no_start_token);
	}

	string_literal	retval;
	auto			cur	= s;
	// 通常の文字列
	if (*cur == '"')
	{
		++cur;
		for (;;)
		{
			// not closed.
			if (cur == e)
			{
				return	errorval(sourcecode_error::no_end_token);
			}
			if (*cur == '"')
			{
				++cur;
				retval.type	= string_literal::st_basic;
				retval.code	= {s, cur};
				retval.next	= _code.substr(cur);
				retval.str	= {s+1, cur-1};
				return	{ errorval(sourcecode_error::success), retval };
			}
			if (*cur == '\\')
			{
				++cur;
			}
			++cur;
		}
	}
	// ヒアドキュメント
	else if (starts_with(text(s, e), "<<<"))
	{
		auto	b	= s;
		s	+= 3;
		if (s == e)
		{
			return	{ sourcecode_error::no_end_token };
		}
		bool	indent	= *s == '-';
		if (indent)
		{
			++s;
		}
		text	eot;
		if (!read_identify_token(eot, {s, e}))
		{
			return	{ sourcecode_error::no_start_token };
		}
		s	+= eot.size();
		if (s != e)
		{
			auto	c	= *s;
			if (c == ' ')
			{
				++s;
			}
			else if (c == '\r' || c == '\n')
			{
				++s;
				if (s != e && (*s== '\r' || *s == '\n') && (c != *s))
				{
					++s;
				}
			}
		}
		if (s == e)
		{
			return	{ sourcecode_error::no_end_token };
		}
		auto	tb	= s;
		while ((s != e) && (!starts_with({s, e}, eot) || (read_identify_token({s, e}) != eot)))
		{
			++s;
		}
		if (s == e)
		{
			return	{ sourcecode_error::no_end_token };
		}
		retval.str		= {tb, s};


		s += eot.size();
		retval.code	= {b, s};
		if (indent && retval.str.size() > 0)
		{
			// indent checks.
			auto	ci	= retval.str.data();
			auto	c	= *ci++; 
			if (c == ' ' || c == '\t')
			{
				retval.indent_size	= 1;
				retval.indent_char	= c;
				while (*ci == c)
				{
					++retval.indent_size;
					++ci;
				}
			}	
			// 最後の１行を調べ、インデントのみだったら除外する
			text	witout_lastline(retval.str);
			while (!witout_lastline.empty() && witout_lastline.back() != '\r' && witout_lastline.back() != '\n')
			{
				witout_lastline	= witout_lastline.substr(0, witout_lastline.size()-1);
			}
			text	lastline	= retval.str.substr(witout_lastline.e);
			if (lastline.size() <= retval.indent_size && lastline.find_first_not_of(retval.indent_char) == text::npos)
			{
				retval.str	= witout_lastline;
			}
		}
		if (retval.str.back() == '\n' || retval.str.back() == '\r')
		{
			char	b	= retval.str.back();
			retval.str	= retval.str.substr(0, retval.str.size()-1);
			if (b != retval.str.back() && (retval.str.back() == '\n' || retval.str.back() == '\r'))
			{
				retval.str	= retval.str.substr(0, retval.str.size()-1);
			}
		}
		retval.type	= string_literal::st_hiredoc;
		retval.next	= _code.substr(s);
		return	{ sourcecode_error::success, retval };
	}
	// RAW文字列
	if (starts_with({cur, e}, "R\""))
	{
		s	= cur + 2;
		text	eot;
		read_identify_token(eot, {s, e});
		s	+= eot.size();
		if (s == e)
		{
			return	{ sourcecode_error::no_end_token };
		}
		if (*s != '(')
		{
			return	{ sourcecode_error::no_end_token };
		}
		++s;
		while ((s != e) && !(*s == ')' && text(s+1, e).starts_with(eot) && text(s+1, e).substr(eot.size()).front() == '"'))
		{
			++s;
		}
		if (s == e)
		{
			return	{ sourcecode_error::no_end_token };
		}
		s += eot.size() + 2;
		retval.type	= string_literal::st_rawstring;
		retval.code	= {cur, s};
		retval.next	= _code.substr(s);
		retval.str	= {cur+3+eot.size(), s-(2+eot.size())};
		return	{ sourcecode_error::success, retval };
	}
	return	{ sourcecode_error::no_start_token };
}


result<simplearray<text>> parsed_split(text _code, char _delm)
{
	simplearray<text>	retval;
	auto*				beg	= _code.begin();
	while (_code.size())
	{
		auto	c	= _code.front();
		if (c == _delm)
		{
			retval.push_back({beg, _code.begin()});
			++_code;
			beg	= _code.begin();
			continue;
		}

		// 括弧が見つかった場合は括弧を丸ごとスキップ
		auto	brk	= read_brackets(_code);
		if (brk.good())
		{
			_code	= brk.next;
			continue;
		}
		else if (!brk.err.is(sourcecode_error::no_start_token))
		{
			return	brk.err;
		}

		// 文字列が見つかった場合は文字列を丸ごとスキップ
		auto	str	= read_string(_code);
		if (str.good())
		{
			_code	= str.next;
			continue;
		}
		else if (!str.err.is(sourcecode_error::no_start_token))
		{
			return	str.err;
		}


		++_code;
	}
	retval.push_back({beg, _code.begin()});
	return	{ sourcecode_error::success, std::move(retval) };
}

sourcelocation compute_location(text _code, char const* _location, int _line)
{
	sourcelocation	r	= { nullptr, _code.s, _line, 0 };
	if (_code.s > _location || _code.e < _location)
	{
		return	r;
	}
	auto			cur		= _code.begin();
	auto			end		= _code.end();
	auto			linetop	= cur;
	while (cur != _location && cur != end)
	{
		auto	c	= *cur++;
		if (starts_with(text{cur, end}, "#line"))
		{
			cur += 5;
			// 改行コードまでを抜き出す
			char const*	beg	= skip_blank(cur, end);
			while (*cur && *cur != '\n' && *cur != '\r')
			{
				++cur;
			}
			linetop	= cur;
			char const*	lst	= cur;

			// まずは整数値（行番号）を抜き出す
			numeric_literal	line;
			char const*		next	= read_numeric_literal(line, {beg, lst});			
			if (next)
			{
				// 行番号があったので #line ディレクティブとして判断する
				bool	s;
				r.line	= (int)to_intmax(s, line.code);
				if (next != lst)
				{
					auto	str	= sourcecode::read_string({skip_blank(next, end), lst});
					if (str.good())
					{
						r.filename = {str.code.begin()+1, str.code.end()-1};
					}
				}
			}
			cur	= skip_one_return(cur, end);
			r.cols	= 0;
			continue;
		}
		if (c == '\n' || c == '\r')
		{
			++r.line;
			r.cols	= 0;
			if ((cur != _location) && (*cur != c) && (*cur == '\n' || *cur == '\r'))
			{	
				++cur;
			}
			linetop	= cur;
			continue;
		}
		++r.cols;
	}
	r.linetop	= linetop;
	r.cols		= static_cast<int>(cur - linetop);
	return	r;
}


sstring decode_escape_string(text _literal)
{
	sstring	retval;
	char const*	beg	= _literal.s;
	char const*	cur = beg;
	char const*	end	= cur + _literal.size();

	while (cur != end)
	{
		if (*cur == '\\')
		{
			if (beg != cur)
			{
				retval.append(text(beg, cur));
			}
			++cur;
			// oct
			if (*cur >= '0' && *cur <= '7')
			{
				int		val	= 0;
				for (int dig = 0; dig < 3 && cur != end; ++cur, ++dig)
				{
					if (*cur >= '0' && *cur <= '7') { val	= val * 8 + (*cur - '0'); }
					else { break; }
				}
				char	buf[1] = { (char)val };
				retval.append(text(buf, 1));
			}
			// hex
			else if (*cur == 'x')
			{
				int		val	= 0;
				++cur;
				for (int dig = 0; dig < 2 && cur != end; ++cur, ++dig)
				{
					     if (*cur >= '0' && *cur <= '8') { val = val * 16 + (*cur - '0'); }
					else if (*cur >= 'a' && *cur <= 'f') { val = val * 16 + (*cur - 'a'); }
					else if (*cur >= 'A' && *cur <= 'F') { val = val * 16 + (*cur - 'A'); }
					else { break; }
				}
				char	buf[1] = { (char)val };
				retval.append(text(buf, 1));
			}
			// other
			else
			{
				switch (*cur)
				{
					case 'a' :	retval.append("\a");			break;
					case 'b' :	retval.append("\b");			break;
					case 'f' :	retval.append("\f");			break;
					case 'n' :	retval.append("\n");			break;
					case 'r' :	retval.append("\r");			break;
					case 't' :	retval.append("\t");			break;
					case 'v' :	retval.append("\v");			break;
					case '{' :	retval.append("\\{");			break;
					case '}' :	retval.append("\\}");			break;
					case '\\' :	retval.append("\\");			break;
					default:	retval.append(text(cur, 1));	break;
				}
				++cur;
			}
			beg	= cur;
		}
		else
		{
			++cur;
		}
	}
	if (beg != cur)
	{
		retval.append(text(beg, cur));
	}
	return	retval;
}

sstring encode_escape_string(text _line)
{
	return	sstring(_line).replace("\\", "\\\\").replace("\n", "\\n").replace("\r", "\\r").replace("\"", "\\\"");
}



char const* read_numeric_literal(numeric_literal& _result, text _code)
{
	auto	r	= read_numeric(_code);
	if (r.bad())
	{
		return	nullptr;
	}
	_result	= r;
	return	r.next.s;
}



numeric_literal read_numeric_literal(text _code)
{
	numeric_literal	r;
	auto			next	= read_numeric_literal(r, _code);
	if (!next)
	{
		r.code	= nullptr;
	}
	return	r;
}



errorval string_literal::decode(amtrs_bufferif_one_init _destinate) const noexcept
{
	switch (type)
	{
		// 通常の文字列をデコード
		case string_literal::st_basic :
		{
			auto	r	= decode_escape_string(str);
			// バッファを確保
			auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, r.size()));
			if (!data)
			{
				return	ENOMEM;
			}
			// 転送
			std::copy(r.begin(), r.end(), data);
			return	{ };
		}


		// Raw string の場合はそのまま返す
		case string_literal::st_rawstring :
		{
			// バッファを確保
			auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, str.size()));
			if (!data)
			{
				return	ENOMEM;
			}
			std::copy(str.s, str.e, data);
			return	{ };
		}

		// hiredoc の文字列をデコード
		case string_literal::st_hiredoc :
		{
			size_t	total	= 0;
			auto	cur		= str;
			while (cur.size())
			{
				// skip indent.
				auto	cnt		= indent_size;
				while (cur.front() == indent_char && cnt > 0)
				{
					++cur;
					--cnt;
				}

				// extract to return.
				auto	rpos	= cur.find_first_of("\r\n");
				size_t	sz		= 0;
				if (rpos != text::npos)
				{
					sz		= skip_one_return(cur.s + rpos, str.e) - cur.s;
				}
				else
				{
					sz		= cur.size();
					rpos	= 0;
				}
				total += sz;
				cur   =  cur.substr(sz);
			}
			// バッファを確保
			auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, total));
			if (!data)
			{
				return	ENOMEM;
			}

			// 転送
			cur		= str;
			while (cur.size())
			{
				// skip indent.
				auto	cnt		= indent_size;
				while (cur.front() == indent_char && cnt > 0)
				{
					++cur;
					--cnt;
				}
			
				// extract to return.
				auto	rpos	= cur.find_first_of("\r\n");
				size_t	sz		= cur.size();
				if (rpos != text::npos)
				{
					sz		= skip_one_return(cur.s + rpos, str.e) - cur.s;
				}
				else
				{
					sz		= cur.size();
					rpos	= 0;
				}
				std::copy_n(cur.s, sz, data);
				data  += sz;
				cur   =  cur.substr(sz);
			}
			return	{};
		}

		default:
		{
			return	{ sourcecode_error::unsupported };
		}
	}
}



intmax_t to_intmax(bool& _success, text _code, unsigned int _flags)
{
	char const*	s		= _code.data();
	char const*	e		= _code.data() + _code.size();
	_success	= false;
	if (s == e)
	{
		return	0;
	}
	intmax_t	n		= 0;
	int			base	= 10;
	bool		minus	= *s == '-';
	if (*s == '-' || *s == '+')
	{
		++s;
		if (s == e)
		{
			return	0;
		}
	}

	if (*s == '0')
	{
		++s;
		if (s == e)
		{
			_success	= true;
			return	0;
		}
		if (*s == 'x' || *s == 'X')
		{
			base	= 16;
			++s;
		}
		else if (*s == 'b' || *s == 'B')
		{
			base	= 2;
			++s;
		}
		else
		{
			base	= 8;
		}
		if (s == e)
		{
			return	0;
		}
	}

	while (s != e)
	{
		int	cn	= 99;
		if (*s >= '0' && *s <= '9')
		{
			cn	= *s - '0';
		}
		else if (*s >= 'a' && *s <= 'f')
		{
			cn	= *s - 'a' + 10;
		}
		else if (*s >= 'A' && *s <= 'F')
		{
			cn	= *s - 'A' + 10;
		}
		if (cn >= base)
		{
			return	0;
		}
		n	= n * base + cn;
		++s;
	}

	if (minus)
	{
		n	= -n;
	}
	_success	= true;
	return	n;
}

/*
 * 識別子を抜き出す
 */
char const* read_identify_token(text& _result, text _code)
{
	char const*	s	= _code.data();
	char const*	e	= _code.data() + _code.size();
	if (s == e)
	{
		_result	= { s, (size_t)0 };
		return	nullptr;
	}
	if (!((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') || (*s == '_') || (static_cast<unsigned char>(*s) >= 0x80)))
	{
		_result	= { s, (size_t)0 };
		return	nullptr;
	}
	auto	b	= s;
	while ((s != e) && ((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') || (*s == '_') || (*s >= '0' && *s <= '9') || (static_cast<unsigned char>(*s) >= 0x80)))
	{
		++s;
	}
	_result	= { b, s };
	return	s;
}


text read_identify_token(text _code)
{
	text	r;
	if (read_identify_token(r, _code))
	{
		return	r;
	}
	return	nullptr;
}




char const* get_operator_string(operator_type _ot)
{
	switch (_ot)
	{
		case ot_scope:					return	"::";
		case ot_inc_suffix:				return	"++";
		case ot_dec_suffix:				return	"--";
		case ot_functional_cast:		return	"{}";
		case ot_function_call:			return	"()";
		case ot_subscript:				return	"[]";
		case ot_member_access:			return	".";
		case ot_member_access_arrow:	return	"->";
		case ot_inc_prefix:				return	"++";
		case ot_dec_prefix:				return	"--";
		case ot_plus:					return	"+";
		case ot_minus:					return	"-";
		case ot_logical_not:			return	"!";
		case ot_bitwise_not:			return	"~";
		case ot_c_style_cast:			return	"()";
		case ot_indirection:			return	"*";
		case ot_address_of:				return	"&";
		case ot_size_of:				return	"sizeof";
		case ot_await_expression:		return	"co_await";
		case ot_dynamic_allocation:		return	"new";
		case ot_dynamic_deallocation:	return	"delete";
		case ot_pointer_to_member:		return	".*";
		case ot_multiplication:			return	"*";
		case ot_division:				return	"/";
		case ot_remainder:				return	"%";
		case ot_addition:				return	"+";
		case ot_subtraction:			return	"-";
		case ot_left_shift:				return	"<<";
		case ot_right_shift:			return	">>";
		case ot_three_way_compare:		return	"<=>";
		case ot_leter:					return	"<";
		case ot_leter_equals:			return	"<=";
		case ot_grater:					return	">";
		case ot_grater_equals:			return	">=";
		case ot_equals:					return	"==";
		case ot_not_equals:				return	"!=";
		case ot_bitwise_and:			return	"&";
		case ot_bitwise_xor:			return	"^";
		case ot_bitwise_or:				return	"|";
		case ot_logical_and:			return	"&&";
		case ot_logical_or:				return	"||";
		case ot_ternary_conditional:	return	"?:";
		case ot_throw:					return	"throw";
		case ot_co_yield:				return	"co_yield";
		case ot_assign:					return	"=";
		case ot_assign_sum:				return	"+=";
		case ot_assign_difference:		return	"-=";
		case ot_assign_product:			return	"*=";
		case ot_assign_quotient:		return	"/=";
		case ot_assign_remainder:		return	"%=";
		case ot_assign_left_shift:		return	"<<=";
		case ot_assign_right_shift:		return	">>=";
		case ot_assign_and:				return	"&=";
		case ot_assign_xor:				return	"^=";
		case ot_assign_or:				return	"|=";
		case ot_unary_right_fold:		return	"...";
		case ot_unary_left_fold:		return	"...";
		case ot_comma:					return	",";
		default:						return	"";
	}
}

operatordata read_operator(text _code)
{
	if (_code.starts_with("::"))
	{
		operatordata	retval;
		retval.type	= operator_type::ot_scope;
		retval		= text{ _code.s, _code.s+2 };
		return	retval;
	}

	if (_code.starts_with("->"))
	{
		operatordata	retval;
		retval.type	= operator_type::ot_member_access_arrow;
		retval		= text{ _code.s, _code.s+2 };
		return	retval;
	}

	operatordata	retval	= read_binary_operator(_code);
	if (!retval.empty())
	{
		return	retval;
	}
	if (_code.front() == '.')
	{
		text	back(_code);
		++_code;
		if (_code.front() != '.')
		{
			retval.type	= operator_type::ot_member_access;
			retval		= text{ _code.s, _code.s+1 };
			return	retval;
		}
		_code	= back;
	}
	if (_code.front() == ',')
	{
		retval.type	= operator_type::ot_comma;
		retval		= text{ _code.s, _code.s+1 };
		return	retval;
	}
	if (_code.front() == '?')
	{
		retval.type	= operator_type::ot_ternary_conditional;
		retval		= text{ _code.s, _code.s+1 };
		return	retval;
	}
	return	nullptr;
}


operatordata read_binary_operator(text _code)
{
	operatordata	r;
	auto			n	= read_binary_operator(r, _code);
	if (!n)
	{
		r	= text(nullptr);
	}
	return	r;
}


/*
 * 2項演算子を読み取る。
 */
char const* read_binary_operator(operatordata& exp, text _code)
{
	char const*	beg	= skip_blank(_code.data(), _code.data() + _code.size());
	char const*	s	= beg;
	char const*	e	= _code.data() + _code.size();
	if (s == e)
	{
		return	nullptr;
	}

	if (*s == '+')
	{
		++s;
		exp.type	= operator_type::ot_addition;
		exp			= text{ beg, s };
		if (*s == '=')
		{
			++s;
			exp.type	= operator_type::ot_assign_sum;
			exp			= text{ beg, s };
			return	s;
		}
		if (*s == '+')
		{
			++s;
			exp.type	= operator_type::ot_inc_suffix;
			exp			= text{ beg, s };
			return	s;
		}
		return	s;
	}

	if (*s == '-')
	{
		++s;
		exp.type	= operator_type::ot_subtraction;
		exp			= text{ beg, s };
		if (*s == '=')
		{
			++s;
			exp.type	= operator_type::ot_assign_difference;
			exp			= text{ beg, s };
			return	s;
		}
		if (*s == '>')
		{
			++s;
			exp.type	= operator_type::ot_assign_difference;
			exp			= text{ beg, s };
			return	s;
		}
		if (*s == '-')
		{
			++s;
			exp.type	= operator_type::ot_dec_suffix;
			exp			= text{ beg, s };
			return	s;
		}
		return	s;
	}

	if (*s == '*')
	{
		++s;
		exp.type	= operator_type::ot_multiplication;
		exp			= text{ beg, s };
		if (*s == '=')
		{
			++s;
			exp.type	= operator_type::ot_assign_product;
			exp			= text{ beg, s };
			return	s;
		}
		return	s;
	}

	if (*s == '/')
	{
		++s;
		exp.type	= operator_type::ot_division;
		exp			= text{ beg, s };
		if (*s == '=')
		{
			++s;
			exp.type	= operator_type::ot_assign_quotient;
			exp			= text{ beg, s };
			return	s;
		}
		return	s;
	}

	if (*s == '%')
	{
		++s;
		exp.type	= operator_type::ot_remainder;
		exp			= text{ beg, s };
		if (*s == '=')
		{
			++s;
			exp.type	= operator_type::ot_assign_remainder;
			exp			= text{ beg, s };
			return	s;
		}
		return	s;
	}

	if (*s == '&')
	{
		++s;
		exp.type	= operator_type::ot_bitwise_and;
		exp			= text{ beg, s };
		if (s != e)
		{
			if (*s == '&')
			{
				++s;
				exp.type	= operator_type::ot_logical_and;
				exp			= text{ beg, s };
				return	s;
			}
			else if (*s == '=')
			{
				++s;
				exp.type	= operator_type::ot_assign_and;
				exp			= text{ beg, s };
				return	s;
			}
		}
		return	s;
	}

	if (*s == '|')
	{
		++s;
		exp.type	= operator_type::ot_bitwise_or;
		exp			= text{ beg, s };
		if (s != e)
		{
			if (*s == '|')
			{
				++s;
				exp.type	= operator_type::ot_logical_or;
				exp			= text{ beg, s };
				return	s;
			}
			else if (*s == '=')
			{
				++s;
				exp.type	= operator_type::ot_assign_or;
				exp			= text{ beg, s };
				return	s;
			}
		}
		return	s;
	}

	if (*s == '^')
	{
		++s;
		exp.type	= operator_type::ot_bitwise_xor;
		exp			= text{ beg, s };
		if (s != e)
		{
			if (*s == '=')
			{
				++s;
				exp.type	= operator_type::ot_assign_xor;
				exp			= text{ beg, s };
				return	s;
			}
		}
		return	s;
	}

	if (*s == '=')
	{
		++s;
		exp.type	= operator_type::ot_assign;
		exp			= text{ beg, s };
		if (s != e)
		{
			if (*s == '=')
			{
				++s;
				exp.type	= operator_type::ot_equals;
				exp			= text{ beg, s };
				return	s;
			}
		}
		return	s;
	}

	if (*s == '<')
	{
		++s;
		exp.type	= operator_type::ot_leter;
		exp			= text{ beg, s };
		if (s != e)
		{
			if (*s == '<')
			{
				++s;
				exp.type	= operator_type::ot_left_shift;
				exp			= text{ beg, s };
				return	s;
			}
			else if (*s == '=')
			{
				++s;
				exp.type	= operator_type::ot_leter_equals;
				exp			= text{ beg, s };
				return	s;
			}
		}
		return	s;
	}

	if (*s == '>')
	{
		++s;
		exp.type	= operator_type::ot_grater;
		exp			= text{ beg, s };
		if (s != e)
		{
			if (*s == '>')
			{
				++s;
				exp.type	= operator_type::ot_right_shift;
				exp			= text{ beg, s };
				return	s;
			}
			else if (*s == '=')			{
				++s;
				exp.type	= operator_type::ot_grater_equals;
				exp			= text{ beg, s };
				return	s;
			}
		}
		return	s;
	}

	if (*s == '!')
	{
		++s;
		if (s != e)
		{
			if (*s == '=')
			{
				++s;
				exp.type	= operator_type::ot_not_equals;
				exp			= text{ beg, s };
				return	s;
			}
		}
		return	s;
	}
	return	nullptr;
}



operatordata read_prefix_operator(text _code)
{
	struct	operatordef
	{
		operator_type	type;
		char const*		code;
	};
	static constexpr operatordef	prefix_op[] =
	{
		{operator_type::ot_inc_prefix,				"++"},
		{operator_type::ot_dec_prefix,				"--"},
		{operator_type::ot_logical_not,				"!" },
		{operator_type::ot_bitwise_not,				"~" },
		{operator_type::ot_indirection,				"*" },
		{operator_type::ot_address_of,				"&" },
		{operator_type::ot_dynamic_allocation,		"new" },
		{operator_type::ot_dynamic_deallocation,	"delete" },
	};

	auto	code	= trim_blank(_code);
	for (auto o : prefix_op)
	{
		if (!_code.starts_with(o.code)) { continue; }
		operatordata	ret;
		size_t			sz	= text(o.code).size();
		ret.type		= o.type;
		ret				= _code.substr(0, sz);
		return	ret;
	}
	return	nullptr;
}


sstring replace_return_to_space(text _text)
{
	return	sstring(_text).replace("\r\n", " ").replace("\n", " ").replace("\r", " ");
}



bool is_assign_operator(operator_type _ope)
{
	if (_ope == ot_assign
	 || _ope == ot_assign_sum
	 || _ope == ot_assign_difference
	 || _ope == ot_assign_product
	 || _ope == ot_assign_quotient
	 || _ope == ot_assign_remainder
	 || _ope == ot_assign_left_shift
	 || _ope == ot_assign_right_shift
	 || _ope == ot_assign_and
	 || _ope == ot_assign_xor
	 || _ope == ot_assign_or
	) {
		return	true;
	}
	return	false;
}


bool is_bin_operator(operator_type _ope)
{
	if (_ope == ot_multiplication
	 || _ope == ot_division
	 || _ope == ot_remainder
	 || _ope == ot_addition
	 || _ope == ot_subtraction
	 || _ope == ot_left_shift
	 || _ope == ot_right_shift
	 || _ope == ot_three_way_compare
	 || _ope == ot_leter
	 || _ope == ot_leter_equals
	 || _ope == ot_grater
	 || _ope == ot_grater_equals
	 || _ope == ot_equals
	 || _ope == ot_not_equals
	 || _ope == ot_bitwise_and
	 || _ope == ot_bitwise_xor
	 || _ope == ot_bitwise_or
	 || _ope == ot_logical_and
	 || _ope == ot_logical_or
	) {
		return	true;
	}
	return	false;
}


AMTRS_SOURCECODE_NAMESPACE_END
