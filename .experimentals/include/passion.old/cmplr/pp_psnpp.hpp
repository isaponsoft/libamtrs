/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__pp_psnpp__hpp
#define	__passion__pp_psnpp__hpp
#include <amtrs/filesystem/@>
#include "def.hpp"
#include "types.hpp"
#include "../err/error.hpp"
#include "../token/brunch_point.hpp"
#include "../token/token.hpp"
#include "../asm/assembler.hpp"
#include "../vm/vm.hpp"
#include "../vm/vmclass.hpp"
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! passion preprocesser.
// ----------------------------------------------------------------------------
//! c言語ライクなプリプロセッサ
// ----------------------------------------------------------------------------
template<class Compiler>
class	pp_clangex
{
public:
	using	value_type		= tokenreader::value_type;
	using	token_type		= tokenreader::token_type;
	using	compiler_type	= Compiler;

	struct	macro
	{
		std::vector<value_type>		tokens;

		bool						functional;
		std::vector<value_type>		parameters;
	};

	struct	argument
	{
		std::vector<value_type>		text;
	};


	bool operator () (compiler_type& _cmplr, tokenreader& _in)
	{
		if (_in == token_type::COMMAND)
		{
			auto	it	= mMacros.find(_in.str());
			if (it != mMacros.end())
			{
				const location&	loc	= _in;
				macro&			mc	= it->second;

				++_in;
				if (!mc.functional)
				{
					// 引数なし
					tokenbuffer	tb;
					for (value_type t : mc.tokens)
					{
						t.set_location(loc);
						tb.push_back(std::move(t));
					}
					_in.push(std::move(tb));
				}
				else
				{
					_in.skip(token_type::SPACE);
					if (_in != "(")
					{
						// error
					}
					++_in;
					_in.skip(token_type::SPACE);
					if (_in == ")")
					{
						// 引数０個
						++_in;

						tokenbuffer	tb;
						for (value_type t : mc.tokens)
						{
							t.set_location(loc);
							tb.push_back(std::move(t));
						}
						_in.push(std::move(tb));

					}
					else
					{
						std::vector<argument>	args;
						for (;;)
						{
							args.push_back({});
							std::size_t		brackets	= 0;
							argument&		arg			= args.back();
							for (;;)
							{
								_in.skip(token_type::SPACE);
								if (_in == ",")
								{
									if (brackets == 0)
									{
										break;
									}
								}
								else if (_in == ")")
								{
									if (brackets == 0)
									{
										break;
									}
									--brackets;
								}
								else if (_in == "(")
								{
									++brackets;
								}
								arg.text.push_back(_in);
								++_in;
							}

							if (_in == ")")
							{
								++_in;
								break;
							}
							++_in;
						}

						tokenbuffer	tb;
						for (value_type t : mc.tokens)
						{
							int		match	= -1;
							int		pidx	= 0;
							for (auto& p : mc.parameters)
							{
								if (p == t)
								{
									match	= pidx;
									break;
								}
								++pidx;
							}
							if (match != -1)
							{
								argument&		arg			= args[match];
								for (value_type tx : arg.text)
								{
									tx.set_location(loc);
									tb.push_back(std::move(tx));
								}
							}
							else
							{
								t.set_location(loc);
								tb.push_back(std::move(t));
							}
						}
						_in.push(std::move(tb));
					}
				}

				// 置き換えたので、自身を再度呼び出すことを呼び出し元へリクエスト
				return	true;
			}
		}

		if (_in == "__FILE__")
		{
			source_location	loc	= _in;
			std::string		fn	= format<std::string>("\"%s\"", _in.name().c_str());
			++_in;
			tokenbuffer	tb;
			tb.push_back(value_type(token_type::STRING, fn, loc));
			_in.push(std::move(tb));
			return	false;
		}

		if (_in == "__LINE__")
		{
			source_location	loc	= _in;
			std::string		fn	= _in.name();
			auto			fl	= _in.line();
			++_in;
			tokenbuffer	tb;
			tb.push_back(value_type(token_type::NUMBER, std::to_string(fl), fl, loc));
			_in.push(std::move(tb));
			return	false;
		}

		if (_in != "#")
		{
			return	false;
		}

		++_in;
		_in.skip(token_type::SPACE);
		if (shebang(_cmplr, _in))	{ return false; }
		if (define(_cmplr, _in))	{ return false; }
		if (undef(_cmplr, _in))		{ return false; }
		if (include(_cmplr, _in))	{ return true; }
		if (ifdef(_cmplr, _in))		{ return true; }

		PSNCMPLR_SYNTAX_ERROR(_cmplr, _in.location(), invalid_preprocessing_directive, _in.str().c_str());

		return	false;
	}


private:
	bool define(compiler_type& _cmplr, tokenreader& _in)
	{
		if (_in != "define")
		{
			return	false;
		}
		++_in;
		_in.skip(token_type::SPACE);

		if (_in != token_type::COMMAND)
		{
			if (_in == token_type::EOFTOKEN || _in == token_type::RETURN)
			{
				PSNCMPLR_SYNTAX_ERROR(_cmplr, _in.location(), macro_name_missing, "");
			}
			PSNCMPLR_SYNTAX_ERROR(_cmplr, _in.location(), macro_not_identifier, _in.str().c_str());
		}

		std::string	name	= _in.str();
		macro		mc;
		++_in;
		if (_in == token_type::SPACE)
		{
			// 引数なしマクロ
			mc.functional	= false;
		}
		else if (_in == "(")
		{
			// 引数ありマクロ
			mc.functional	= true;
			++_in;
			_in.skip(token_type::SPACE);
			if (_in == ")")
			{
				// パラメータ無し
				++_in;
			}
			else
			{
				for (;;)
				{
					mc.parameters.push_back(_in);
					++_in;
					_in.skip(token_type::SPACE);
					if (_in == ")")
					{
						++_in;
						break;
					}
					if (_in != ",")
					{
						// error
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _in.location(), expected_comma_in_macro_parameter_list, _in.str().c_str());
					}
					++_in;
					_in.skip(token_type::SPACE);
				}
			}
		}
		_in.skip(token_type::SPACE);

		// データ部分を読み取る
		while (!_in.empty() && _in != token_type::RETURN)
		{
			mc.tokens.push_back(_in);
			++_in;
		}

		mMacros[name]	= std::move(mc);

		return	true;
	}

	bool shebang(compiler_type& _cmplr, tokenreader& _in)
	{
		if (_in != "!")
		{
			return	false;
		}

		do
		{
			++_in;
		} while (_in != Token::EOFTOKEN && _in != Token::RETURN);
		return	true;
	}


	bool undef(compiler_type& _cmplr, tokenreader& _in)
	{
		if (_in != "undef")
		{
			return	false;
		}
		++_in;
		_in.skip(token_type::SPACE);

		auto&	name	= _in.str();
		if (auto it = mMacros.find(name); it != mMacros.end())
		{
			mMacros.erase(it);
		}
		++_in;
		return	true;
	}


	bool include(compiler_type& _cmplr, tokenreader& _in)
	{
		if (_in != "include")
		{
			return	false;
		}
		++_in;
		_in.skip(token_type::SPACE);

		auto	filename	= removeQuotation(_in.str());
		auto	dir			= (std::string)filesystem::parent_path((std::string_view)_in.name());
		auto	filepath	= dir+"/"+filename;
		++_in;
		
		tokenbuffer	tb;
		auto		source	= filesystem::file_get_contents<std::string>((std::string_view)filepath);
		TextReader	reader({source, filepath, 1});
		reader.setCommentEnable(false);
		for (;;)
		{
			Token	token = reader.read();
			if (token == Token::EOFTOKEN)
			{
				break;
			}
			tb.push_back(token);
		}
		_in.push(std::move(tb));

		return	true;
	}


	value_type ifdef_read_block(compiler_type& _cmplr, tokenbuffer& _block, tokenreader& _in)
	{
		value_type	retval;
		std::size_t	nestCount	= 0;
		for (;;)
		{
			if (_in != "#")
			{
				_block.push_back(_in);
				++_in;
			}
			else
			{
				tokenbuffer	bf;
				bf.push_back(_in);
				++_in;
				_in.skip(bf, token_type::SPACE);
				if (_in == "define")
				{
					// defineがあった場合、定義をすべてバッファに蓄積し、何も加工しない。
					for (;;)
					{
						bool	ret	= _in == token_type::RETURN;
						bf.push_back(_in);
						++_in;
						if (ret)
						{
							break;
						}
					}
					_block.push_back(bf);
					continue;
				}
				else if (_in == "if" || _in == "ifdef")
				{
					++nestCount;
				}
				else if (_in == "endif")
				{
					if (nestCount == 0)
					{
						retval	= _in;
						++_in;
						return	retval;
					}
					--nestCount;
				}
				else if (_in == "else" || _in == "elif")
				{
					if (nestCount == 0)
					{
						retval	= _in;
						++_in;
						return	retval;
					}
				}
				bf.push_back(_in);
				_block.push_back(bf);
				++_in;
			}
		}
	}



	std::intmax_t compute_if_expression(compiler_type& _cmplr, tokenbuffer& _in, opecode_priority _level)
	{
		std::intmax_t	answer	= 0;
		_in.skip(token_type::SPACE);
		if (_in.fetch() == token_type::COMMAND)
		{
			auto	it	= mMacros.find(_in.fetch().str());
			if (it != mMacros.end())
			{
				if (it->second.tokens.size() == 1)
				{
					answer	= it->second.tokens[0].number();
				}
				else
				{
					tokenbuffer	tb;
					tb.push_back(it->second.tokens.begin(), it->second.tokens.end());
					answer	= compute_if_expression(_cmplr, tb, _level);
				}
			}
			_in.read();
		}
		else if (_in.fetch() == token_type::NUMBER)
		{
			answer	= _in.fetch().number();
			_in.read();
		}
		_in.skip(token_type::SPACE);

		if (!_in.empty())
		{
			auto	opecode	= _in.fetch();
			if (opecode == "+")
			{
				if (_level >= opecode_priority::add_sub)
				{
					return	answer;
				}
				_in.read();
				answer	+= compute_if_expression(_cmplr, _in, opecode_priority::add_sub);
			}
			else if (opecode == "-")
			{
				if (_level >= opecode_priority::add_sub)
				{
					return	answer;
				}
				_in.read();
				answer	-= compute_if_expression(_cmplr, _in, opecode_priority::add_sub);
			}
			else if (opecode == "*")
			{
				if (_level >= opecode_priority::mul_div)
				{
					return	answer;
				}
				_in.read();
				answer	*= compute_if_expression(_cmplr, _in, opecode_priority::mul_div);
			}
			else if (opecode == "/")
			{
				if (_level >= opecode_priority::mul_div)
				{
					return	answer;
				}
				_in.read();
				answer	/= compute_if_expression(_cmplr, _in, opecode_priority::mul_div);
			}
			else if (opecode == "%")
			{
				if (_level >= opecode_priority::mul_div)
				{
					return	answer;
				}
				_in.read();
				answer	%= compute_if_expression(_cmplr, _in, opecode_priority::mul_div);
			}
			else if (opecode == "&")
			{
				if (_level >= opecode_priority::bitwise)
				{
					return	answer;
				}
				_in.read();
				answer	&= compute_if_expression(_cmplr, _in, opecode_priority::bitwise);
			}
			else if (opecode == "|")
			{
				if (_level >= opecode_priority::bitwise)
				{
					return	answer;
				}
				_in.read();
				answer	|= compute_if_expression(_cmplr, _in, opecode_priority::bitwise);
			}
			else if (opecode == "^")
			{
				if (_level >= opecode_priority::bitwise)
				{
					return	answer;
				}
				_in.read();
				answer	^= compute_if_expression(_cmplr, _in, opecode_priority::bitwise);
			}
			else if (opecode == "<<")
			{
				if (_level >= opecode_priority::shift)
				{
					return	answer;
				}
				_in.read();
				answer	<<= compute_if_expression(_cmplr, _in, opecode_priority::shift);
			}
			else if (opecode == ">>")
			{
				if (_level >= opecode_priority::shift)
				{
					return	answer;
				}
				_in.read();
				answer	>>= compute_if_expression(_cmplr, _in, opecode_priority::shift);
			}
			else if (opecode == "&&")
			{
				if (_level >= opecode_priority::logic_and)
				{
					return	answer;
				}
				_in.read();
				answer	= answer && compute_if_expression(_cmplr, _in, opecode_priority::none);
			}
			else if (opecode == "||")
			{
				if (_level >= opecode_priority::logic_and)
				{
					return	answer;
				}
				_in.read();
				answer	= answer || compute_if_expression(_cmplr, _in, opecode_priority::none);
			}
			else if (opecode == "<")
			{
				if (_level >= opecode_priority::compare)
				{
					return	answer;
				}
				_in.read();
				answer	= answer < compute_if_expression(_cmplr, _in, opecode_priority::compare);
			}
			else if (opecode == "<=")
			{
				if (_level >= opecode_priority::compare)
				{
					return	answer;
				}
				_in.read();
				answer	= answer <= compute_if_expression(_cmplr, _in, opecode_priority::compare);
			}
			else if (opecode == ">")
			{
				if (_level >= opecode_priority::compare)
				{
					return	answer;
				}
				_in.read();
				answer	= answer > compute_if_expression(_cmplr, _in, opecode_priority::compare);
			}
			else if (opecode == ">=")
			{
				if (_level >= opecode_priority::compare)
				{
					return	answer;
				}
				_in.read();
				answer	= answer >= compute_if_expression(_cmplr, _in, opecode_priority::compare);
			}
			else if (opecode == "?")
			{
				if (_level >= opecode_priority::conditions)
				{
					return	answer;
				}
				_in.read();
				_in.skip(token_type::SPACE);
				auto	v1	= compute_if_expression(_cmplr, _in, opecode_priority::none);
				if (_in.fetch() != ":")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _in.fetch().location(), token_is_not_a_valid_binary_operator_in_a_preprocessor_subexpression, _in.fetch().str().c_str());
				}
				_in.skip(token_type::SPACE);
				_in.read();
				auto	v2	= compute_if_expression(_cmplr, _in, opecode_priority::none);
				answer	= answer ? v1 : v2;
			}
			else
			{
				return	answer;
			}
		}
		return	answer;
	}


	std::intmax_t compute_if_expression(compiler_type& _cmplr, tokenreader& _in)
	{
		// とりあえず式を全部読み取る
		tokenbuffer	expr;
		for (;;)
		{
			if (_in == token_type::RETURN)
			{
				break;
			}
			expr.push_back(_in);
			++_in;
		}
		return	compute_if_expression(_cmplr, expr, opecode_priority::none);
	}


	bool ifdef(compiler_type& _cmplr, tokenreader& _in)
	{
		bool	use	= false;
		if (_in == "ifdef")
		{
			++_in;
			_in.skip(token_type::SPACE);
			use	= mMacros.find(_in.str()) != mMacros.end();
			++_in;
		}
		else if (_in == "if")
		{
			++_in;
			_in.skip(token_type::SPACE);
			use	= compute_if_expression(_cmplr, _in) != 0;
			++_in;
		}
		else
		{
			return	false;
		}
		_in.skip(token_type::SPACE);

		tokenbuffer	enableBlock;
		bool		haveEnable	= false;
		for (;;)
		{
			tokenbuffer	block;
			auto		end	= ifdef_read_block(_cmplr, block, _in);
			if (use)
			{
				enableBlock	= std::move(block);
				haveEnable	= true;
				use			= false;
			}
			if (end == "elif")
			{
				use	= compute_if_expression(_cmplr, _in) != 0;
			}
			if (end == "else")
			{
				if (!haveEnable)
				{
					use			= true;
				}
			}
			if (end == "endif")
			{
				break;
			}
		}
		_in.push(std::move(enableBlock));
		return	true;
	}


	std::unordered_map<std::string, macro>	mMacros;

};

PASSION_NAMESPACE_END
#endif

