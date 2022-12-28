/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <map>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <amtrs/console.hpp>
#include <amtrs/optparse.hpp>
#include <amtrs/filesystem.hpp>
#include <amtrs/system.hpp>
#include <psnvm/lang-expression.hpp>
#include <psnvm/lang-lexer.hpp>
#include <psnvm/lang-parser.hpp>
#include <psnvm/lang-variable.hpp>
#include "apgen.hpp"
#include "functions/functions.hpp"
using namespace amtrs;
using namespace psnvm;
namespace 	fs	= amtrs::filesystem;
using		ll	= console::loglevel;


namespace apgen {

class	scriptengine
{
public:
	struct	symbolmanager
	{
		/*
		 *
		 */
		struct	symbol
		{
			std::string			name;
			variable			value;
		};

		struct	symbollist
		{
			symbollist*									parent	= nullptr;
			std::unordered_map<std::string, symbol>		items;
		};

		symbolmanager()
			: symbols(new symbollist())
		{
		}

		~symbolmanager()
		{
			while (symbols)
			{
				pop();
			}
		}

		void push()
		{
			auto	c	= new symbollist();
			c->parent	= symbols;
			symbols	= c;
		}

		void pop()
		{
			auto	c	= symbols;
			symbols	= symbols->parent;
			delete	c;
		}

		symbollist& top()
		{
			return	*symbols;
		}

		void add(std::string _name, variable _val)
		{
			top().items.insert({
				std::move(_name),
				symbol{
					.name	= _name,
					.value	= std::move(_val)
				}
			});
		}

		symbol* find(std::string const& _name)
		{
			for (symbollist* c = symbols; c; c = c->parent)
			{
				auto	it	= c->items.find(_name);
				if (it != c->items.end())
				{
					return	&it->second;
				}
			}
			return	nullptr;
		}

		symbollist*		symbols	= nullptr;
	};

	symbolmanager	mSymbols;

	scriptengine(status* _env)
		: mEnv(_env)
	{
		for (auto& def : mEnv->values)
		{
			auto&	name	= def.first;
			auto&	value	= def.second;

			// 引数が数値リテラルっぽいなら数値として扱う
			numeric_literal	num;
			if (read_numeric_literal(num, value))
			{
				bool	s;
				mSymbols.add(name, variable(to_intmax(s, num.code)));
			}
			// 数値っぽいくないので文字列扱い
			else
			{
				mSymbols.add(name, variable(std::string(value)));
			}
		}
	}

	~scriptengine()
	{
	}

	bool good() const noexcept { return mStatus == 0; }
	void clear() noexcept { mStatus = 0; }
	std::string outbuff() noexcept { return std::move(mOutbuff); }

	std::string	mFilename;
	char const*	mCode;
	errorcode	mErrorCode	= errorcode::ec_success;

	char const* parse(std::string const& _filename, int _line, char const* _begin, char const* _end, bool _docmode = false)
	{
		mFilename	= _filename;
		mCode		= _begin;

		compiler	prc;
		prc.error.userparam	= this;
		prc.error.handler	= [](auto ec, auto msg, auto locate, void* up)
							{
								scriptengine*	thiz	= reinterpret_cast<scriptengine*>(up);
								sourcelocation	l		= compute_location(thiz->mCode, locate);
								thiz->mErrorCode		= ec;
								std::cerr << thiz->mFilename << "(" << l.line << "," << l.cols << "): " << msg << std::endl;
								return	0;
							};
		prc.userparam		= this;
		prc.load_variable	= [](auto& result, auto& proc, auto identify) -> compiler::result_code
							{
								scriptengine*	thiz	= reinterpret_cast<scriptengine*>(proc.userparam);
								auto			sym		= thiz->mSymbols.find((std::string)identify);
								if (sym)
								{
									bool	s;
									result	= sym->value;
								}
								return	compiler::rc_ok;
							};



		auto	it	= _begin;

		// docmode.
		if (_docmode)
		{
			// <% が出るまでを抜き出し、文字列として抽出する
			char const*	beg	= it;
			while (it != _end && !starts_with<std::string_view>({it, _end}, "<%"))
			{
				++it;
			}
			std::cout << std::string_view(beg, it) << std::flush;
			if (it == _end)
			{
				return	nullptr;
			}
			it += 2;
		}

		while (mErrorCode == errorcode::ec_success && (it = skip_blank(it, _end)) != _end)
		{
			exprnode	stm;
			if (starts_with<std::string_view>({it, _end}, "%>"))
			{
				// <% が出るまでを抜き出し、文字列として抽出する
				it = skip_one_return(it + 2, _end);
				char const*	beg	= it;
				while (it != _end && !starts_with<std::string_view>({it, _end}, "<%"))
				{
					++it;
				}
				std::cout << std::string_view(beg, it) << std::flush;
				if (it == _end)
				{
					return	nullptr;
				}
				it += 2;
				continue;
			}

			if (auto next = read_statements(stm, {it, _end}, prc.error); next)
			{
				if (stm.type == expr_type::st_expression)
				{
					if (starts_with(stm.expr.stm, "$("))
					{
						std::string_view	commands;
						read_brackes_block(commands, stm.expr.stm.substr(1), prc.error);
						commands	= trim_blank(trim(commands, "()"));

						std::string	cmd(commands);
						system::exec(cmd.c_str());
					}
					else if (starts_with(stm.expr.stm, "$<"))
					{
						variable	r	= proc_expression(prc, stm.expr.stm.substr(2));
						if (mErrorCode != errorcode::ec_success)
						{
							return	nullptr;
						}
						std::string	s;
						if (auto v = r.as<variable_string>(); v)
						{
							s	= v->value();
						}
						else if (auto v = r.as<variable_intmax>(); v)
						{
							s	= std::to_string(v->value());
						}
						std::cout << s << std::flush;
					}
				}
				if (stm.type == expr_type::st_compound)
				{
					parse(_filename, _line, stm.expr.stm.data(), stm.expr.stm.data() + stm.expr.stm.size());
				}
				if (stm.type == expr_type::st_if)
				{
					if (!stm.stm_if.init.empty())
					{
						proc_expression(prc, stm.stm_if.init);
						if (mErrorCode != errorcode::ec_success)
						{
							return	nullptr;
						}
					}

					variable	r	= proc_expression(prc, stm.stm_if.cond);
					if (mErrorCode != errorcode::ec_success)
					{
						return	nullptr;
					}
					if (is_true(r))
					{
						parse(_filename, _line, stm.stm_if.stm.data(), stm.stm_if.stm.data() + stm.stm_if.stm.size());
					}
					else
					{
						parse(_filename, _line, stm.stm_if.stm_else.data(), stm.stm_if.stm_else.data() + stm.stm_if.stm_else.size());
					}
				}

				it	= next;
				continue;
			}

			if (mErrorCode != errorcode::ec_success)
			{
				return	nullptr;
			}

			// 不明な文字列
			console::log(ll::ll_error, "unkown character '%c'.", *it);
			return	nullptr;
		}

		// パースを終えた位置。endと一致する。
		return	it;
	}

private:
	status*			mEnv;
	unsigned int	mStatus	= 0;
	std::string		mOutbuff;
};



std::string process(bool& _error, status& _status, std::string const& _filename, std::string const& _txt)
{
	std::string		txt;
	scriptengine	e(&_status);

	e.parse(_filename, 1, _txt.data(), _txt.data() + _txt.size(), true);
	if (!e.good())
	{
		_error	= true;
	}
	return	e.outbuff();
}


std::string process(status& _status, std::string const& _filename, std::string const& _txt)
{
	bool	e	= false;
	auto	r	= process(e, _status, _filename, _txt);
	if (e)
	{
		throw	1;
	}
	return	r;
}



std::string replace_macro(std::string const& _txt, apgen::status const& _status)
{
	std::string	txt	= _txt;
	for (;;)
	{
		bool	replaced	= false;
		txt	= amtrs::regex_replace_callback(txt, R"(\$\{(.*?)(:(.+))*\})", [&](auto m) -> std::string
			{
				std::string	vname	= m[1].str();
				replaced			= true;
				for (auto name : amtrs::split_chars((std::string_view)vname, (std::string_view)"|"))
				{
					for (auto& d : _status.values)
					{
						if (d.first == name)
						{
							std::string	val	= d.second;
							if (m[3] == "backslash-escape")
							{
								val	= amtrs::filesystem::normalize_path(val, true);
							}
							if (!val.empty())
							{
								return	val;
							}
						}
					}
				}
				if (std::string def = m[3].str(); !def.empty())
				{
					return	def;
				}
				return	"";
			});
		if (!replaced)
		{
			break;
		}
	}
	return	txt;
}

} // namespace
