/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
// https://en.cppreference.com/w/cpp/language/operator_precedence
#include <amtrs/filesystem/path.hpp>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/string/text.hpp>
PSNVM_LANG_NAMESPACE_BEGIN
using namespace amtrs;
using namespace amtrs::filesystem;
using namespace amtrs::strutil;
using namespace lang;

struct	ppimpl
{
	prepro*	pp;

	sstring error(errorinfo _err)
	{
		pp->errors.push_back(std::move(_err));
		return	{};
	}

	sstring _process_block(prepro& _pp, text _code, text _filename, int _linenumber, bool _insertAddCmd)
	{
		using namespace amtrs::strutil;

		sstring		retval;
		sstring		rewrited;
		int			linenum	= _linenumber;
		text		code	= _code;
		char const*	beg		= code.s;
		if (_insertAddCmd)
		{
			if (pp->line_directive)
			{
				retval.append(_pp.removed_line_begin);
				retval.append(format("#line {} \"{}\"\n", {_linenumber, _filename}));
				retval.append(_pp.removed_line_end);
			}
		}
		while (!code.empty())
		{
			text	line	= code.read_line();
			text	cmd		= skip_blank(line);
			code	= code.substr(line.e);
			if (cmd.front() != '#')
			{
				// replace keywords.
				sstring	l;
				if (_extract_macro(l, _pp, _filename, _code, line.s, line, false))
				{
					// マクロを置換したので出力
					if (beg != line.s)
					{
						retval.append(text(beg, line.s));
					}
					retval.append(l);
					beg	= line.e;
				}
				continue;
			}
			skip_blank(++cmd);

			// siban
			if (starts_with(cmd, "!"))
			{
				if (beg != line.s)
				{
					retval.append(text(beg, line.s));
				}
				beg	= line.e;
				retval.append(_pp.removed_line);
				continue;
			}

			// define
			if (starts_with(cmd, "define"))
			{
				// 直前のデータを出力
				if (beg != line.s)
				{
					retval.append(text(beg, line.s));
				}
				beg	= line.e;
				retval.append(_pp.removed_line);

				// マクロ名
				cmd	= skip_blank(cmd.substr(6));
				auto	name	= read_identify_token(cmd);
				if (name.empty())
				{
					return	error({line.s, errorcode::ec_prepro_has_not_macroname});
				}
				auto	body	= skip_blank(cmd.substr(name.e));
				auto	g		= _pp.find_macro_group(name);
				if (!g)
				{

					group	newg;
					newg.identify	= name;
					_pp.defines.push_back(std::move(newg));
					g	= _pp.find_macro_group(name);
				}

				declare	dec;
				dec.filename	= _filename;
				dec.line		= compute_location(_code, name.s, 1).line;
				dec.code		= trim_blank(replace_return_to_space(body));	// 改行コードはマクロを置換した時に行数が変わってしまうので削除
				g->decls.push_back(std::move(dec));
				continue;
			}

			// undef
			if (starts_with(cmd, "undef"))
			{
				// 直前のデータを出力
				if (beg != line.s)
				{
					retval.append(text(beg, line.s));
				}
				beg	= line.e;
				retval.append(_pp.removed_line);

				// マクロ名
				cmd	= skip_blank(cmd.substr(6));
				auto	name	= read_identify_token(cmd);
				if (name.empty())
				{
					return	error({line.s, errorcode::ec_prepro_has_not_macroname});
				}
				cmd	= skip_blank(cmd.substr(name.e));

				auto	fg	= _pp.find_macro_group(name);
				if (!fg)
				{
					return	error({line.s, errorcode::ec_prepro_not_defined_undef});
				}

				simplearray<group>	gp;
				for (auto& g : _pp.defines)
				{
					if (&g != fg)
					{
						gp.push_back(std::move(g));
					}
				}
				_pp.defines	= std::move(gp);
				continue;
			}

			// error, mesg
			if (starts_with(cmd, "error") || starts_with(cmd, "mesg"))
			{
				// 直前のデータを出力
				if (beg != line.s)
				{
					retval.append(text(beg, line.s));
				}
				beg	= line.e;
				bool	er	= starts_with(cmd, "error");
				cmd	= skip_blank(cmd.substr(er ? 5 : 4));

				error({line.s, er ? errorcode::ec_prepro_error : errorcode::ec_prepro_message, cmd});
				if (_pp.errors.size() > 0 || er)
				{
					return	{};
				}
				continue;
			}

			// include
			if (starts_with(cmd, "include"))
			{
				// 直前のデータを出力
				if (beg != line.s)
				{
					retval.append(text(beg, line.s));
				}
				beg	= line.e;

				// ファイル名を抽出
				cmd	= skip_blank(cmd.substr(7));

				text				incname;
				simplearray<text>	findpaths;
				if (cmd.front() == '<')
				{
					++cmd;
					auto	pos	= cmd.find_first_of('>');
					if (pos == text::npos)
					{
						return	error({line.s, errorcode::ec_prepro_include_syntax});
					}
					incname	= text(cmd.s, pos);
					for (auto& p : _pp.paths)
					{
						findpaths.push_back(p);
					}
				}
				else if (cmd.front() == '"')
				{
					++cmd;
					auto	pos	= cmd.find_first_of('"');
					if (pos == text::npos)
					{
						return	error({line.s, errorcode::ec_prepro_include_syntax});
					}
					incname	= text(cmd.s, pos);
					// 現在のファイルに対する相対位置のみ
					findpaths.push_back(amtrs::filesystem::parent_path(_filename));
				}
				else
				{
					return	error({line.s, errorcode::ec_prepro_include_syntax});
				}
				sstring	opend;
				bool	errorf	= false;
				sstring	incdata	= _read_include(opend, errorf, _pp, incname, findpaths);
				if (errorf)
				{
					return	error({line.s, errorcode::ec_prepro_file_not_found, incname});
				}
				if (opend.empty())
				{
					return	error({line.s, errorcode::ec_prepro_file_not_found, incname});
				}
				if (!incdata.empty())
				{
					incdata	= pp->process(incdata, opend);
					retval.append(incdata);
					retval.append(_pp.removed_line);
					if (pp->line_directive)
					{
						retval.append(_pp.removed_line_begin);
						retval.append(format("#line {} \"{}\"\n", { compute_location(_code, line.s, 1).line + 1, _filename }));
						retval.append(_pp.removed_line_end);
					}
				}
				continue;
			}


			// if - elif - else - endif
			if (starts_with(cmd, "if") || starts_with(cmd, "ifdef"))
			{
				// 直前のデータを出力
				if (beg != line.s)
				{
					retval.append(text(beg, line.s));
				}
				retval.append(_pp.removed_line);

				struct	nestinfo
				{
					char const*	start_if	= nullptr;
					char const*	start		= nullptr;
					bool		end_true	= false;
					bool		have_else	= false;
					int			nest		= 0;
				};
				nestinfo	info;
				retval.append(_pp.removed_line);
				bool	cond	= false;
				if (!starts_with(cmd, "ifdef"))
				{
					cond	= _if_expression(_pp, _filename, _code, trim_blank(line.substr(3))) != 0;
				}
				else
				{
					auto	name	= read_identify_token(skip_blank(cmd.substr(6)));
					if (name.empty())
					{
						return	error({line.s, errorcode::ec_prepro_has_not_macroname});
					}
					cond	= _pp.find_macro_group(name) != nullptr;
				}
				info.start_if	= line.s;
				info.start		= cond ? line.e : nullptr;
				info.end_true	= cond;

				for (;;)
				{
					code	= code.substr(line.e);
					line	= code.read_line();

					if (starts_with(line, "#if"))
					{
						++info.nest;
					}
					else if (info.nest == 0 && starts_with(line, "#elif"))
					{
						if (info.start)
						{
							retval.append(_process_block(_pp, {info.start, line.s}, _filename, linenum, false));
							info.start	= nullptr;
						}
						retval.append(_pp.removed_line);
						if (!info.end_true)
						{
							bool	cond	= _if_expression(_pp, _filename, _code, trim_blank(line.substr(5))) != 0;
							info.start		= cond ? line.e : nullptr;
							info.end_true	= cond;
						}
					}
					else if (info.nest == 0 && starts_with(line, "#else"))
					{
						if (info.have_else)
						{
							return	error({line.s, errorcode::ec_prepro_else_duplicated});
						}
						if (info.start)
						{
							retval.append(_process_block(_pp, {info.start, line.s}, _filename, linenum, false));
							info.start	= nullptr;
						}
						retval.append(_pp.removed_line);
						info.have_else	= true;
						if (!info.end_true)
						{
							info.start		= line.e;
							info.end_true	= true;
						}
					}
					else if (starts_with(line, "#endif"))
					{
						if (info.nest == 0)
						{
							if (info.start)
							{
								retval.append(_process_block(_pp, {info.start, line.s}, _filename, linenum, false));
								info.start	= nullptr;
							}
							retval.append(_pp.removed_line);
							code	= code.substr(line.e);
							break;
						}
						else
						{
							--info.nest;
							if (!info.start)
							{
								retval.append(_pp.removed_line);
							}
						}
					}
					else
					{
						if (!info.start)
						{
							retval.append(_pp.removed_line);
						}
					}
				}
				beg	= code.s;
				continue;
			}
		}

		if (beg != code.s)
		{
			retval.append(text(beg, code.s));
		}
		return	retval;
	}


	bool _extract_macro(sstring& _retval, prepro& _pp, text _filename, text _code, char const* _locate, text _line, bool _if_mode)
	{
		using namespace amtrs::strutil;
		sstring			retval;
		auto			code	= _line;
		char const*		beg		= code.s;
		while (!code.empty())
		{
			auto	token	= read_identify_token(code);
			if (!token.empty())
			{
				if (_if_mode && token == "defined")
				{
					auto	definedchk	= skip_blank(code.substr(token.e));
					if (definedchk.front() != '(')
					{
						error({code.s, errorcode::ec_prepro_defind_has_not_name, _locate ? _locate : code.s});
						return	false;
					}
					text			cond	= trim_blank(trim_circle_brackets(read_brackes_block(definedchk)));
					if (cond.empty())
					{
						error({code.s, errorcode::ec_prepro_defind_has_not_name, _locate ? _locate : code.s});
						return	false;
					}
					text			name	= read_identify_token(cond);
					if (name != cond)
					{
						error({code.s, errorcode::ec_prepro_defind_has_not_name, _locate ? _locate : code.s, cond});
						return	false;
					}
					if (beg != token.s)
					{
						retval.append(text(beg, token.s));
					}
					retval.append(_pp.find_macro_group(name) ? "1" : "0");
					code	= code.substr(definedchk.e);
					beg		= code.s;
					continue;
				}
				else if (token == "__FILE__")
				{
					if (beg != token.s)
					{
						retval.append(text(beg, token.s));
					}
					retval.append(format(" \"{}\" ", {_filename}));
					code		= code.substr(token.e);
					beg			= code.s;
					continue;
				}
				else if (token == "__LINE__")
				{
					int		linenumber	= compute_location(_code, _locate ? _locate : token.s, 1).line;
					if (beg != token.s)
					{
						retval.append(text(beg, token.s));
					}
					retval.append(format(" {} ", {linenumber}));
					code		= code.substr(token.e);
					beg			= code.s;
					continue;
				}
				else
				{
					auto	g		= _pp.find_macro_group(token);
					if (g)
					{
						sstring	gen;
						if (beg != token.s)
						{
							gen.append(text(beg, token.s));
						}
						auto		pos	= gen.size();
						declare*	dec	= &g->decls[0];

						// 再帰的に変換するためにコード位置を書き換える
						gen.append(dec->code);
						gen.append({token.e, code.e});
						retval		= std::move(gen);
						code		= text(retval).substr(pos);
						beg			= retval.data();
						continue;
					}
				}
				code	= code.substr(token.e);
				continue;
			}
			++code;
		}
		if (beg == _line.s)
		{
			return	false;
		}
		_retval	= std::move(retval);
		return	true;
	}

	intmax_t _if_expression(prepro& _pp, text _filename, text _code, text _expression)
	{
		// 先にマクロを展開しておく
		sstring	expr;
		text	code(_expression);
		if (_extract_macro(expr, _pp, _filename, _code, nullptr, _expression, true))
		{
			code	= expr;
		}
		return	_if_expression(_pp, _filename, _code, code, operator_type::ot_none);
	}


	intmax_t _if_expression(prepro& _pp, text _filename, text _code, text& _expression, operator_type _prev)
	{
		using namespace amtrs::strutil;

		auto&			code	= _expression;
		intmax_t		retval	= 0;
		code	= trim_blank(code);

		// 値を読み込む
		numeric_literal	num;
		text			r;

		// 優先式
		if (code.front() == '(')
		{
			text	subexpr;
			auto	next	= read_brackes_block(subexpr, code);
			if (!next)
			{
				return	0;
			}
			code	= code.substr(next);
			retval	= _if_expression(_pp, _filename, _code, subexpr);
		}
		// 数値リテラル
		else if (!(num = read_numeric_literal(code)).empty())
		{
			bool	s;
			code	= code.substr(num.code.end());
			retval	= to_intmax(s, num.code);
		}
		// それ以外
		else
		{
			error({code.s, errorcode::ec_unkown_token, text(code.s, code.s+1)});
			return	0;
		}

		// 演算子と第２オペランド、第３オペランドを読み取る
		while (!(code = trim_blank(code)).empty())
		{
			operatordata	ope	= read_binary_operator(code);
			if (ope.empty() || operator_priority(_prev) <= operator_priority(ope.type))
			{
				// 解析終了
				return	retval;
			}
			code	= code.substr(ope.e);

			// 第２オペランドを読み取る
			intmax_t		ope2	= _if_expression(_pp, _filename, _code, code, ope.type);
			if (ope.type == operator_type::ot_addition)
			{
				retval	+= ope2;
			}
			else if (ope.type == operator_type::ot_subtraction)
			{
				retval	-= ope2;
			}
			else if (ope.type == operator_type::ot_multiplication)
			{
				retval	*= ope2;
			}
			else if (ope.type == operator_type::ot_division)
			{
				retval	/= ope2;
			}
			else if (ope.type == operator_type::ot_remainder)
			{
				retval	%= ope2;
			}
			else if (ope.type == operator_type::ot_left_shift)
			{
				retval	<<= ope2;
			}
			else if (ope.type == operator_type::ot_right_shift)
			{
				retval	>>= ope2;
			}
			else if (ope.type == operator_type::ot_leter)
			{
				retval	= retval < ope2;
			}
			else if (ope.type == operator_type::ot_leter_equals)
			{
				retval	= retval <= ope2;
			}
			else if (ope.type == operator_type::ot_grater)
			{
				retval	= retval > ope2;
			}
			else if (ope.type == operator_type::ot_grater_equals)
			{
				retval	= retval >= ope2;
			}
			else if (ope.type == operator_type::ot_equals)
			{
				retval	= retval == ope2;
			}
			else if (ope.type == operator_type::ot_not_equals)
			{
				retval	= retval != ope2;
			}
			else if (ope.type == operator_type::ot_bitwise_and)
			{
				retval	= retval & ope2;
			}
			else if (ope.type == operator_type::ot_bitwise_xor)
			{
				retval	= retval ^ ope2;
			}
			else if (ope.type == operator_type::ot_bitwise_or)
			{
				retval	= retval | ope2;
			}
			else if (ope.type == operator_type::ot_bitwise_and)
			{
				retval	= retval && ope2;
			}
			else if (ope.type == operator_type::ot_bitwise_or)
			{
				retval	= retval || ope2;
			}
		}
		return	retval;

		return	0;
	}


	sstring _read_include(sstring& _opend, bool& _error, prepro& _pp, text _filename, simplearray<text> const& _paths)
	{
		if (amtrs::filesystem::is_absolute(_filename))
		{
			auto	filename	= amtrs::filesystem::normalize_path<sstring>(_filename);
			sstring	val			= amtrs::filesystem::file_get_contents<sstring>(_filename);
			if (!val.empty())
			{
				_opend	= _filename;
				return	val;
			}
			return	{};
		}

		for (auto& dir : _paths)
		{
			auto	filename	= amtrs::filesystem::normalize_path<sstring>(sstring(dir) + "/" + _filename);
			sstring	val			= amtrs::filesystem::file_get_contents<sstring>(filename);
			if (!val.empty())
			{
				_opend	= std::move(filename);
				return	val;
			}
		}
		return	{};
	}

	char const* read_brackes_block(text& _result, text _code)
	{
		char const*	beg	= _code.data();
		char const*	s	= beg;
		char const*	e	= beg + _code.size();
		if ((s != e) && (*s == '(' || *s == '[' || *s == '{'))
		{
			char	c	= *s == '(' ? ')'
						: *s == '[' ? ']'
						:             '}';
			++s;
			while (s != e)
			{
				if (*s == c)
				{
					++s;
					_result	= {beg, s};
					return	s;
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
				if (*s == '(' || *s == '[' || *s == '{')
				{
					text	tmp;
					s	= read_brackes_block(tmp, {s, e});
					if (!s)
					{
						return	nullptr;
					}
				}
				else
				{
					auto			str = sourcecode::read_string({s, e}); 
					if (str.good())
					{
						s	= str.next.s;
					}
					else
					{
						++s;
					}
				}
			}

			error({beg
				, c == '(' ? ec_prepro_nothing_closing_circle_brackets
				: c == '[' ? ec_prepro_nothing_closing_square_brackets
				:            ec_prepro_nothing_closing_curry_brackets
			});
		}
		return	nullptr;
	}


	text read_brackes_block(text _code)
	{
		text	r;
		if (read_brackes_block(r, _code))
		{
			return	r;
		}
		return	nullptr;
	}
};


sstring prepro::process(text _code, text _filename)
{
	ppimpl	pi;
	pi.pp	= this;

	return	pi._process_block(*this, skip_utf8_bom(_code), amtrs::filesystem::normalize_path<sstring>(_filename), 1, true);
}

PSNVM_LANG_NAMESPACE_END
