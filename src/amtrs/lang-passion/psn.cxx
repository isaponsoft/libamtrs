/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * 汎用ライブラリ
 * OSや環境問わずに使用できる。
 */
using namespace amtrs;
using namespace psnvm;
using namespace psnvm::lang;
#include <amtrs/typeutil/callback.hpp>

#include <iostream>
#include <sstream>
sstring test_dump(pflnode& n)
{
	std::stringstream	s;
	pfl{false}.serialize(n, [&](text t)
	{
		s << t;
	});
	return	sstring(text(s.str()));
}


namespace {

struct	psnparser_core
{
	enum blocktypes
	{
		globalblock	= 0,
		memberblock	= 1,
		codeblock	= 2,
		initstmt	= 3,
	};


	psnparser_core(psn* _parser, text _code)
		: owner(_parser)
		, code(_code)
		, templateSkip(-1)
	{}
	psnparser_core(psnparser_core const&) = default;
	psnparser_core& operator = (psnparser_core const&) = default;



	psn*			owner;
	text				code;
	int					templateSkip;
	bool				documentMode	= false;

	auto add_token(sstring _s) -> text
	{
		text	r(_s);
		owner->dynamics.push_back(std::move(_s));
		return	r;
	}

	auto bad() -> bool { return !good(); }
	auto error(errorinfo e) -> pflnode { owner->errors.push_back(std::move(e)); return pflnode(); }
	void error_clear() { owner->errors.clear(); }
	auto errors() -> auto& { return owner->errors; }
	template<class E> void errors(E&& _errs) { owner->errors = std::move(_errs); }
	auto good() -> bool { return owner->errors.empty(); }

	// キーワードを読み取る
	auto fetch_keyword() -> text
	{
		skip_blank();
		text	backup(code);
		auto	ret	= read_keyword();
		code	= backup;
		return	ret;
	}

	// 演算子を読み取る
	auto fetch_operator() -> operatordata
	{
		skip_blank();
		text	backup(code);
		auto	ret		= read_operator(code);
		code	= backup;
		return	ret;
	}

	// 関数名に使用可能な演算子を読み取る
	auto fetch_functions_operator() -> operatordata
	{
		skip_blank();
		text	backup(code);
		auto	ret		= read_operator(code);
		if (ret.type == operator_type::ot_none)
		{
			if (code.front() == '[')
			{
				++code;	skip_blank();
				if (code.front() == ']')
				{
					ret.type	= operator_type::ot_subscript;
					ret.s		= backup.s;
					ret.e		= code.s + 1;
				}
			}
			else if (code.front() == '(')
			{
				++code;	skip_blank();
				if (code.front() == ')')
				{
					ret.type	= operator_type::ot_function_call;
					ret.s		= backup.s;
					ret.e		= code.s + 1;
				}
			}
		}
		code	= backup;
		return	ret;
	}

	auto is_keywords(text _token) -> bool
	{
		return	_token == "auto"
			||	_token == "break"
			||	_token == "case"
			||	_token == "catch"
			||	_token == "class"
			||	_token == "consept"
			||	_token == "const"
			||	_token == "constexpr"
			||	_token == "const_cast"
			||	_token == "continue"
			||	_token == "co_wait"
			||	_token == "co_return"
			||	_token == "co_yield"
			||	_token == "decltype"
			||	_token == "default"
			||	_token == "delete"
			||	_token == "do"
			||	_token == "dynamic_cast"
			||	_token == "else"
			||	_token == "enum"
			||	_token == "friend"
			||	_token == "for"
			||	_token == "if"
			||	_token == "inline"
			||	_token == "new"
			||	_token == "nullptr"
			||	_token == "noexcept"
			||	_token == "operator"
			||	_token == "private"
			||	_token == "protected"
			||	_token == "public"
			||	_token == "requires"
			||	_token == "return"
			||	_token == "sizeof"
			||	_token == "static"
			||	_token == "static_assert"
			||	_token == "static_cast"
			||	_token == "struct"
			||	_token == "switch"
			||	_token == "template"
			||	_token == "this"
			||	_token == "thread_local"
			||	_token == "throw"
			||	_token == "try"
			||	_token == "typeid"
			||	_token == "typename"
			||	_token == "union"
			||	_token == "using"
			||	_token == "virtual"
			||	_token == "volatile"
			||	_token == "while"
		;
	}


	void nest_reset()
	{
		templateSkip	= -1;
	}


	auto parse() -> simplearray<pflnode>
	{
		simplearray<pflnode>	ret;
		while (!code.empty() && good())
		{
			skip_blank();
			if (code.front_at() == '#')
			{
				auto	line	= code.read_line();
				code	= code.substr(line.e);
				continue;
			}

			if (code.front() == ';')
			{
				++code;
				continue;
			}

			auto				stm	=	parse_statement(globalblock);
			if (stm)
			{
				ret.push_back(std::move(stm));
			}
		}
		return	ret;
	}

	auto parse_char_literal() -> pflnode
	{
		auto	location	= code.s;
		skip_blank();
		if (code.front() != '\'')
		{
			return	{};
		}
		++code;

		uint32_t		c	= 0;
		size_t			len	= 2;
		if (code.empty())
		{
			return	{};		// ERROR
		}
		if (code.front() != '\\')
		{
			c	= code.front();
			++code;
		}
		else
		{
			++code;
			switch (code.front())
			{
				case 'x' :
				{
					++code;
					int		shift	= 1;
					for (size_t l = 0; l < len; ++l)
					{
						char	v1	= code.front();	++code;
						v1	= (v1 >= '0' && v1 <= '9') ? v1 - '0'
							: (v1 >= 'a' && v1 <= 'f') ? v1 - 'a' + 10
							: (v1 >= 'A' && v1 <= 'F') ? v1 - 'A' + 10
							: -1;
						char	v2	= code.front();	++code;
						v2	= (v2 >= '0' && v2 <= '9') ? v2 - '0'
							: (v2 >= 'a' && v2 <= 'f') ? v2 - 'a' + 10
							: (v2 >= 'A' && v2 <= 'F') ? v2 - 'A' + 10
							: -1;
						c	= c + (((v1 << 4) + (v2)) * shift);
						shift <<= 8;
					}
					break;
				}
				case '0' :	++code; c = '\0';	break;
				case 'n' :	++code; c = '\n';	break;
				case 'r' :	++code; c = '\r';	break;
				case 't' :	++code; c = '\t';	break;
				default:	return {};		// ERROR
			}
		}

		if (code.front() != '\'')
		{
			return	{};
		}
		++code;
		return	(pflnode)pflnode::make<pn_literal_char>({location, code.s}, c);
	}


	auto parse_class(text _decltype, bool _require_term) -> pflnode
	{
		auto	location	= code.s;
		pflnode	classname	= parse_symbol();
		if (!classname || bad())
		{
			return	error({location, errorcode::ec_class_classname_nothing});
		}

		simplearray<pn_declare_class::base>	bases;
		simplearray<pflnode>					members;
		skip_blank();

		skip_blank();
		if (code.front() == ';')
		{
			return	(pflnode)pflnode::make<pn_declare_class>({location, code.s}, std::move(classname), std::move(bases), std::move(members));
		}


		// base classes
		if (code.front() == ':')
		{
			++code;	skip_blank();
			for (;;)
			{
				// access keyword
				auto					location	= code.s;
				pn_declare_class::base	base;
				for (;;)
				{
					auto	keyword	= fetch_keyword();
					     if (keyword == "public")		base.access	|= (uint32_t)pn_declare_specifier::spec_public;
					else if (keyword == "protected")	base.access	|= (uint32_t)pn_declare_specifier::spec_protected;
					else if (keyword == "private")		base.access	|= (uint32_t)pn_declare_specifier::spec_private;
					else if (keyword == "virtual")		base.access	|= (uint32_t)pn_declare_specifier::spec_virtual;
					else break;
					read_keyword();
				}
				base.name	= parse_typeid();
				if (!base.name)
				{
					return	error({location, errorcode::ec_class_baseclass_nothing});
				}
				bases.push_back(std::move(base));
				skip_blank();
				if (code.front() != ',')
				{
					break;
				}
				++code;
			}
			skip_blank();
		}

		// members
		if (code.front() == '{')
		{
			++code;

			uint32_t	access	= _decltype == "class"
								? (uint32_t)pn_declare_specifier::spec_private
								: (uint32_t)pn_declare_specifier::spec_public;
			for (;;)
			{
				skip_blank();
				if (code.front() == ';')
				{
					++code;
					continue;
				}

				if (code.front() == '}')
				{
					++code;
					break;
				}

				pflnode		member	= parse_member(access);
				if (bad())
				{
					return	member;
				}
				pn_label*	aclbl	= member.at<pn_label>();
				if (aclbl)
				{
					if (aclbl->name == "private")
					{
						access	= (uint32_t)pn_declare_specifier::spec_private;
						continue;
					}
					if (aclbl->name == "protected")
					{
						access	= (uint32_t)pn_declare_specifier::spec_protected;
						continue;
					}
					if (aclbl->name == "public")
					{
						access	= (uint32_t)pn_declare_specifier::spec_public;
						continue;
					}
				}


				pflnode	notermTest	= member;
				if (member.type() == pflnode_types::declare_template)
				{
					notermTest	= member.at<pn_declare_template>()->declare;
				}
				if (notermTest.type() == pflnode_types::label
				 || notermTest.type() == pflnode_types::declare_function_constractor
				 || notermTest.type() == pflnode_types::declare_function_destractor
				 || notermTest.type() == pflnode_types::declare_function_conversion
				 || notermTest.type() == pflnode_types::declare_function
				 || notermTest.type() == pflnode_types::declare_function_operator
				)
				{
				}
				else
				{
					if (code.front() != ';')
					{
						return	{};	// ERROR
					}
					++code;
				}
				members.push_back(std::move(member));
			}
		}
		if (code.front() != ';')
		{
			return	error({location, errorcode::ec_class_terminate_nothing});
		}
		return	(pflnode)pflnode::make<pn_declare_class>({location, code.s}, std::move(classname), std::move(bases), std::move(members));
	}


	auto parse_ref_ptr(pflnode _node) -> pflnode
	{
		if (_node)
		{
			skip_blank();
			if (code.front() == '*')
			{
				_node		= (pflnode)pflnode::make<pn_spec_symbol_pointer>({code.s, code.s+1}, std::move(_node));
				++code;
			}
			else if (code.front() == '&')
			{
				_node		= (pflnode)pflnode::make<pn_spec_symbol_reference>({code.s, code.s+1}, std::move(_node));
				++code;
			}
		}
		return	_node;
	}


	// 
	auto parse_concept(text _keyword) -> pflnode
	{
		auto	location	= _keyword.s;
		pflnode	name		= parse_symbol();
		if (!name)
		{
			return	{};	// ERROR
		}
		skip_blank();
		if (code.front() != '=')
		{
			return	{};	// ERROR
		}
		++code;
		pflnode	expr		= parse_expr(true);
		if (!expr)
		{
			return	{};	// ERROR
		}
		return	(pflnode)pflnode::make<pn_declare_concept>({location, code.s}, std::move(name), std::move(expr));
	}


	// ひとつの宣言文を読み取る
	auto parse_declare(blocktypes _blocktype, uint32_t _spec = 0) -> pflnode
	{
		auto	location	= code.s;
		auto	keyword		= fetch_keyword();
		auto	rollback	= code;
		bool	isFriend	= false;

		if (keyword == "friend")
		{
			isFriend	= true;
			read_keyword();
			keyword		= fetch_keyword();
			rollback	= code;
		}

		// class
		if (keyword == "class" || keyword == "struct" || keyword == "union")
		{
			read_keyword();
			auto	ret	= parse_class(keyword, true);
			if (!isFriend)
			{
				return	ret;
			}
			return	(pflnode)pflnode::make<pn_spec_member_friend>({location, code.s}, std::move(ret));
		}

		// concept
		if (keyword == "concept")
		{
			read_keyword();
			return	parse_concept(keyword);
		}

		// enum
		if (keyword == "enum")
		{
			read_keyword();
			return	parse_enum();
		}

		// thread_local
		if (keyword == "thread_local")
		{
		}

		// template
		if (keyword == "template")
		{
			psnparser_core	sub(*this);
			sub.read_keyword(); sub.skip_blank();
			if (sub.code.front() == '<')
			{
				code	= sub.code;
				++code;

				pflnode		decl;
				pflnode		params	= parse_template_parameter_list();
				skip_blank();
				if (code.front() != '>'|| bad())
				{
					return	error({location, errorcode::ec_declare_template_paramlist_error});
				}
				++code;

				decl	= parse_declare(_blocktype);
				if (decl.type() != pflnode_types::declare_class
				 && decl.type() != pflnode_types::declare_concept
				 && decl.type() != pflnode_types::declare_function
				 && decl.type() != pflnode_types::declare_function_constractor
				 && decl.type() != pflnode_types::declare_function_conversion
				 && decl.type() != pflnode_types::declare_function_operator
				 && decl.type() != pflnode_types::declare_template
				 && decl.type() != pflnode_types::using_alias
				 && decl.type() != pflnode_types::spec_member_friend
				)
				{
					return	error({location, errorcode::ec_declare_template_unkown_statement_error});
				}
				return	(pflnode)pflnode::make<pn_declare_template>({location, code.s}, params.at<pn_parameter_list>(), std::move(decl));
			}
		}
 
		if (keyword == "namespace")
		{
			read_keyword();
			pflnode	name	= parse_symbol();
			if (bad())
			{
				return	name;
			}
			skip_blank();
			if (code.front() != '{')
			{
				return	{};
			}
			++code;

			simplearray<pflnode>	stms;
			for (;;)
			{
				skip_blank();
				if (code.front() == '}')
				{
					break;
				}

				if (code.empty() && bad())
				{
					return	{};
				}

				auto				stm	=	parse_statement(_blocktype);
				if (stm)
				{
					stms.push_back(std::move(stm));
				}
			}
			++code;

			return	(pflnode)pflnode::make<pn_declare_namespace>({location, code.s}, std::move(name), std::move(stms));
		}

		if (keyword == "using")
		{
			read_keyword();
			// namespace
			if (fetch_keyword() == "namespace")
			{
				read_keyword();
				pflnode	symbol	= parse_typeid(false);
				if (!symbol)
				{
					return	error({location, errorcode::ec_using_namespace_scope_nothing});
				}
				if (code.front() != ';')
				{
					return	error({location, errorcode::ec_using_namespace_terminate_nothing});
				}
				return	(pflnode)pflnode::make<pn_using_namespace>({location, code.s}, std::move(symbol));
			}
			// alias
			else
			{
				pflnode	symbol	= parse_symbol();
				if (!symbol)
				{
					return	error({location, errorcode::ec_using_aliasname_nothing});

				}
				skip_blank();
				if (code.front() != '=')
				{
					return	error({location, errorcode::ec_using_alias_separator_nothing});
				}
				++code;	skip_blank();
				pflnode	target	= parse_typeid(true);
				if (!target)
				{
					return	error({location, errorcode::ec_using_alias_typeid_nothing});
				}
				skip_blank();
				target	= parse_ref_ptr(std::move(target));
				if (code.front() != ';')
				{
					return	error({location, errorcode::ec_using_terminate_nothing});
				}
				return	(pflnode)pflnode::make<pn_using_alias>({location, code.s}, std::move(symbol), std::move(target));
			}
		}


		unsigned int	specflags	= _spec;
		for (;;)
		{
			skip_blank();
			text	token	= fetch_keyword();
			if (token == "constexpr")
			{
				read_keyword();
				specflags	|= (uint32_t)pn_declare_specifier::spec_constexpr;
				continue;
			}
			if (token == "inline")
			{
				read_keyword();
				specflags	|= (uint32_t)pn_declare_specifier::spec_inline;
				continue;
			}
			if (token == "static")
			{
				read_keyword();
				specflags	|= (uint32_t)pn_declare_specifier::spec_static;
				continue;
			}
			if (token == "virtual")
			{
				read_keyword();
				specflags	|= (uint32_t)pn_declare_specifier::spec_virtual;
				continue;
			}
			if (token == "volatile")
			{
				read_keyword();
				specflags	|= (uint32_t)pn_declare_specifier::spec_volatile;
				continue;
			}
			break;
		}


		// conversion function
		skip_blank();
		if ((_blocktype == memberblock) && fetch_keyword() == "operator")
		{
			read_keyword();
			pflnode		to;
			psnparser_core	backup(*this);

			// コンバート先が関数ポインタと仮定して取得
			to	= parse_ref_ptr(parse_typeid());	skip_blank();
			if (bad() || !to || code.front() != '(')
			{
				// コンバート先が関数ポインタだと整合性がとれないので関数ポインタを無視して調査
				*this	= backup;
				to		= parse_typeid(false);	skip_blank();
				if (bad() || !to)
				{
					return	{};	// ERROR
				}
			}
			pflnode	none;
			return	parse_function(location, specflags, none, to);
		}

		nest_reset();
		text	reload	= code;
		pflnode	first	= (_blocktype == memberblock) ? parse_typeid() : parse_statement_expr(operator_type::ot_none);
		if (bad())
		{
			return	{};
		}
		if (_blocktype == memberblock)
		{
			first	= parse_ref_ptr(first);
		}
		skip_blank();
		error_clear();
		nest_reset();
		pflnode			name;
		if (!first)
		{
			if (_blocktype == memberblock)
			{
				return	{};
			}
			code	= reload;
			first	= parse_symbol();
			if (!first || bad())
			{
				return	{};
			}
			skip_blank();
			nest_reset();
		}
		if (!first.is_symbol())
		{
			if (code.front() != ';')
			{
				return	{};
			}
			return	first;
		}
		else if (fetch_keyword() == "operator")
		{
			name	= (pflnode)pflnode::make<pn_symbol_name>({location, code.s}, read_keyword());
		}
		// コンストラクタの可能性があるのでシンボルの後ろに名前以外のあるばあいはシンボルとして読み直す
		else if (_blocktype == memberblock)
		{
			if (fetch_keyword().empty() || is_keywords(fetch_keyword()))
			{
				code	= reload;
				first	= parse_symbol();
				skip_blank();
				nest_reset();
			}
		}

		if (!name)
		{
			name	= parse_symbol();
		}
		if (!name)
		{

			// constractor or destractor
			if (_blocktype == memberblock)
			{
				pflnode		dummy;
				psnparser_core	sub(owner, code);
				pflnode		func		= sub.parse_function(location, specflags, first, dummy);
				if (func)
				{
					code	= sub.code;
					return	func;
				}
			}
			skip_blank();
			if (code.front() != ';')
			{
				return	{};		// ERROR
			}

			// sym(); という文になっているので、関数呼び出しの可能性で読み取りなおす
			if (first.is_symbol())
			{
				code	= reload;
				error_clear();
				return	parse_statement_expr(operator_type::ot_none);
			}
			return	first;
		}
		skip_blank();

		// declarer function or declare variables.

		// declare function test.
		if (_blocktype != codeblock)
		{
			psnparser_core		sub(owner, code);
			pflnode			func		= sub.parse_function(location, specflags, name, first);
			if (func || (bad() && errors().front().err != ec_declare_function_parameter_notclosed))
			{
				if (good()) code = sub.code;
				return	func;
			}
		}

		// declare variables.
		simplearray<pn_declare_variable::var>	variables;


		skip_blank();
		if (code.front() == '[')
		{
			simplearray<pflnode>	sizes;
			for (;;)
			{
				if (code.front() != '[')
				{
					break;
				}
				++code;	skip_blank();
				pflnode	s;
				if (code.front() != ']')
				{
					s	= parse_expr(true);
				}
				sizes.push_back(std::move(s));
				if (code.front() != ']')
				{
					return	{};	// ERROR
				}
				++code;
				skip_blank();
			}
			first	= (pflnode)pflnode::make<pn_spec_symbol_array>({location, code.s}, std::move(first), std::move(sizes));
		}


		for (;;)
		{
			pflnode	init;
			if (fetch_operator().type == operator_type::ot_assign)
			{
				++code;
				skip_blank();
				nest_reset();
				init	= parse_statement_expr(operator_type::ot_comma);
				if (!init)
				{
					return	error({location, errorcode::ec_declare_variable_initexpr_nothing});
				}
			}
			else if (code.front() == '(')
			{
				++code;
				skip_blank();
				init	= parse_expr(true);
				skip_blank();
				if (!init || code.front() != ')')
				{
					return	error({location, errorcode::ec_declare_variable_initexpr_nothing});
				}
				++code;
			}
			else if (code.front() == '{')
			{
				init	= parse_initializer_list();
			}

			variables.push_back(pn_declare_variable::var{ std::move(name), std::move(init)});

			if (fetch_operator().type != operator_type::ot_comma)
			{
				break;
			}
			++code;
			skip_blank();

			location	= code.s;
			name		= parse_statement_expr(operator_type::ot_assign);
			if (!name)
			{
				return	error({location, errorcode::ec_declare_varname_nothing});
			}
		}

		return	(pflnode)pflnode::make<pn_declare_variable>({location, code.s}, specflags, (pflnode)first, std::move(variables));
	}


	auto parse_enum() -> pflnode
	{
		auto	location	= code.s;
		bool	classMode	= (fetch_keyword() == "class" || fetch_keyword() == "struct");
		if (classMode)
		{
			read_keyword();
		}
		auto	symbol		= parse_symbol();
		if (!symbol)
		{
			return	error({location, errorcode::ec_enum_typename_nothing});
		}
		skip_blank();

		// have basetype
		pflnode	base;
		if (code.front() == ':')
		{
			++code;	skip_blank();
			base	= parse_typeid();
			if (!base)
			{
				return	error({location, errorcode::ec_enum_base_nothing});
			}
			skip_blank();
		}

		simplearray<pn_declare_enum::item>	values;

		if (code.front() == '{')
		{
			++code;	skip_blank();
			if (code.front() != '}')
			{
				for (;;)
				{
					skip_blank();
					pn_declare_enum::item	item;

					item.name	= read_keyword();
					if (item.name.empty())
					{
						return	error({location, errorcode::ec_enum_varname_nothing});
					}
					skip_blank();
					if (code.front() == '=')
					{
						++code;	skip_blank();
						item.value	= parse_expr(true, operator_type::ot_comma);
						if (!item.value)
						{
							return	error({location, errorcode::ec_enum_value_nothing});
						}
						error_clear();
					}
					values.push_back(std::move(item));
					skip_blank();
					if (code.front() == '}')
					{
						break;
					}
					if (code.front() != ',')
					{
						return	error({location, errorcode::ec_enum_unkown_token});
					}
					++code;
				}
			}
			++code;
		}

		if (code.front() != ';')
		{
//			return	error({location, errorcode::ec_enum_terminate_nothing});
		}
		return	(pflnode)pflnode::make<pn_declare_enum>({location, code.s}, std::move(symbol), std::move(base), std::move(values));
	}


	// ひとつの式を読み取る
	//	_rval : true の時は辛なず値を返却する必要がある。
	auto parse_expr_value() -> pflnode
	{
		auto	location	= code.s;
		pflnode	ret;

		skip_blank();
		error_clear();

		auto	fetched	= fetch_keyword();

		if (fetched == "typename")
		{
			code	= code.substr(fetched.e);
			return	parse_typeid();
		}


		if (code.starts_with("..."))
		{
			code	= code.substr(3);
			ret		= (pflnode)pflnode::make<pn_operator_single>({code.s-3, code.s}, operator_type::ot_unary_left_fold, std::move(parse_expr(true)));
			fetched	= fetch_keyword();
		}

		if (code.front() == '(')
		{
			++code;
			psnparser_core	sub(*this);
			ret	= sub.parse_expr(true);
			sub.skip_blank();
			if (sub.code.front() != ')')
			{
				return	{};	// 
			}
			++sub.code;
			code	= sub.code;
			if (ret.is_symbol())
			{
				pflnode	val	= parse_expr(true);
				ret	= (pflnode)pflnode::make<pn_expr_cast>({location, code.s}, pn_expr_cast::cast_types::cast_ctype, std::move(ret), std::move(val));
			}
		}


		// initializer list
		if (ret.empty())
		{
			ret	= parse_initializer_list();
			if (bad())
			{
				return	ret;
			}
		}


		// literal: integer, realnumber
		if (ret.empty())
		{
			auto	num	= read_numeric_literal(code);
			if (!num.empty())
			{
				bool	success;
				code	= code.substr(num.code.end());
				ret		= (pflnode)pflnode::make<pn_literal_integer>({location, code.s}, to_intmax(success, num.code), std::move(num));
			}
		}

		// literal: string
		if (ret.empty())
		{
			result<string_literal>	str	= sourcecode::read_string(code);
			if (str.good())
			{
				ret	= parse_string(str);
			}
		}

		// literal: char
		if (ret.empty())
		{
			ret	= parse_char_literal();
		}

		// embebed symbols

		// nullptr
		if (fetched == "nullptr")
		{
			read_keyword();	skip_blank();
			ret	= (pflnode)pflnode::make<pn_literal_nullptr>({location, code.s});
		}

		// bool
		if (fetched == "true" || fetched == "false")
		{
			read_keyword();	skip_blank();
			ret	= (pflnode)pflnode::make<pn_literal_bool>({location, code.s}, (fetched == "true" ? true : false));
		}

		// sizeof
		if (fetched == "sizeof")
		{
			read_keyword();	skip_blank();
			if (code.front() != '(') { return {}; }
			++code;	skip_blank();
			pflnode		val	= parse_expr(true);
			if (code.front() != ')') { return {}; }
			++code;	skip_blank();
			ret	= (pflnode)pflnode::make<pn_literal_sizeof>({location, code.s}, std::move(val));
		}

		// this
		if (fetched == "this")
		{
			read_keyword();	skip_blank();
			ret	= (pflnode)pflnode::make<pn_literal_this>({location, code.s});
		}

		// new
		if (fetched == "new")
		{
			read_keyword();	skip_blank();
			skip_blank();
			simplearray<pflnode>	values;
			if (code.front() == '(')
			{
				++code;	skip_blank();
				if (code.front() != ')')
				{
					for (;;)
					{
						pflnode	n	= parse_expr(true, operator_type::ot_comma);
						if (!n)
						{
							return	{};
						}
						values.push_back(std::move(n));
						skip_blank();
						if (code.front() == ',')
						{
							++code;
							continue;
						}
						if (code.front() == ')')
						{
							break;
						}
						return	{};
					}
				}
				if (code.front() != ')')
				{
					return	{};
				}
				++code;
			}
			psnparser_core	prv(*this);
			pflnode		totype	= parse_typeid();
			skip_blank();
			error_clear();
			if (code.front() != '(')
			{
				*this	= prv;
				totype	= parse_typeid(false);
			}

			skip_blank();
			if (code.front() == '(')
			{
				++code;	skip_blank();
				simplearray<pflnode>	arguments;
				if (code.front() != ')')
				{
					for (;;)
					{
						pflnode	n	= parse_expr(true, operator_type::ot_comma);
						if (!n)
						{
							return	{};
						}
						arguments.push_back(std::move(n));
						skip_blank();
						if (code.front() == ',')
						{
							++code;
							continue;
						}
						if (code.front() == ')')
						{
							break;
						}
						return	{};
					}
				}
				if (code.front() != ')')
				{
					return	{};
				}
				++code;
				ret	= (pflnode)pflnode::make<pn_operator_new>({location, code.s}, false, std::move(totype), std::move(values), std::move(arguments));
			}
			else if (code.front() == '[')
			{
				++code;	skip_blank();
				pflnode	sz	= parse_expr(true);
				skip_blank();
				if (code.front() != ']')
				{
					return	{};
				}
				++code;
				simplearray<pflnode>	arguments;
				arguments.push_back(std::move(sz));
				ret	= (pflnode)pflnode::make<pn_operator_new>({location, code.s}, true, std::move(totype), std::move(values), std::move(arguments));
			}
			else
			{
				return	{};
			}
		}

		// delete
		if (fetched == "delete")
		{
			read_keyword();	skip_blank();

			bool	is_array	= false;
			if (code.front() == '[')
			{
				++code;	skip_blank();
				if (code.front() != ']')
				{
					return	{};
				}
				++code;
				is_array	= true;
			}
			pflnode	val			= parse_expr(true);
			if (!val)
			{
				return	{};
			}
			ret	= (pflnode)pflnode::make<pn_operator_delete>({location, code.s}, is_array, std::move(val));
		}


		// cast
		if (fetched == "const_cast" || fetched == "reinterpret_cast" || fetched == "static_cast")
		{
			read_keyword();	skip_blank();
			if (code.front() != '<')
			{
				return	{};	// ERR
			}
			++code;
			pflnode	to	= parse_ref_ptr(parse_typeid());
			read_keyword();	skip_blank();
			if (code.front() != '>')
			{
				return	{};	// ERR
			}
			++code;

			read_keyword();	skip_blank();
			if (code.front() != '(')
			{
				return	{};	// ERR
			}
			++code;
			pflnode	e	= parse_expr(true);
			read_keyword();	skip_blank();
			if (code.front() != ')')
			{
				return	{};	// ERR
			}
			++code;
			pn_expr_cast::cast_types	t	= fetch_keyword() == "const_cast" ? pn_expr_cast::cast_types::cast_const
									: fetch_keyword() == "reinterpret_cast" ? pn_expr_cast::cast_types::cast_reinterpret
									: fetch_keyword() == "static_cast" ? pn_expr_cast::cast_types::cast_static
									: pn_expr_cast::cast_types::cast_ctype;
			ret	= (pflnode)pflnode::make<pn_expr_cast>({location, code.s}, t, std::move(to), std::move(e));
		}

		// operator
		if (fetched == "operator")
		{
			psnparser_core	sub(*this);
			sub.read_keyword();
			auto		ope	= sub.fetch_functions_operator();
			if (ope.type != operator_type::ot_none)
			{
				code	= code.substr(ope.e);
				ret		= (pflnode)pflnode::make<pn_symbol_operator>({location, code.s}, ope.type);
			}
		}

		// requires
		if (fetched == "requires")
		{
			read_keyword();	skip_blank();
			ret	= parse_expr_requires();
		}


		// symbol
		if (ret.empty())
		{
			auto	errs	= std::move(errors());
			ret	= parse_typeid(false);
		}

		// expr_lambda
		if (!ret)
		{
			ret	= parse_expr_lambda();
			if (bad())
			{
				return	ret;
			}
		}


		if (!ret)
		{
			auto	ope	= read_prefix_operator(code);
			if (ope.type != operator_type::ot_none)
			{
				code	= code.substr(ope.e);
				pflnode	e	= parse_expr(true, operator_type::ot_bitwise_not);
				if (bad() || !e)
				{
					return	{};
				}
				if (ope.type == operator_type::ot_address_of)
				{
					ret	= (pflnode)pflnode::make<pn_operator_address_of>({location, code.s}, std::move(e));
				}
				else if (ope.type == operator_type::ot_indirection)
				{
					ret	= (pflnode)pflnode::make<pn_operator_indirection>({location, code.s}, std::move(e));
				}
				else if (ope.type == operator_type::ot_dec_prefix || ope.type == operator_type::ot_inc_prefix)
				{
					ret	= (pflnode)pflnode::make<pn_operator_incdec>({location, code.s}, ope.type, std::move(e));
				}
				else
				{
					ret	= (pflnode)pflnode::make<pn_operator_single>({location, code.s}, ope.type, std::move(e));
				}
			}
		}


		if (!ret)
		{
			auto	ope			= fetch_operator();
			if (ope.type == operator_type::ot_inc_prefix || ope.type == operator_type::ot_dec_prefix || ope.type == operator_type::ot_inc_suffix || ope.type == operator_type::ot_dec_suffix)
			{
				code	= code.substr(ope.e);
				operator_type	o	= ope.type == operator_type::ot_inc_prefix ? operator_type::ot_inc_prefix
									: ope.type == operator_type::ot_dec_prefix ? operator_type::ot_dec_prefix
									: ope.type == operator_type::ot_inc_suffix ? operator_type::ot_inc_prefix
									: ope.type == operator_type::ot_dec_suffix ? operator_type::ot_dec_prefix
									: ope.type;
				ret	= parse_expr(true, o);
				if (bad())
				{
					return	ret;
				}
				ret	= (pflnode)pflnode::make<pn_operator_incdec>({location, code.s}, o, std::move(ret));
			}
		}
		return	ret;
	}

	auto parse_expr(bool _rval, operator_type _previous = operator_type::ot_none) -> pflnode
	{
		auto	location	= code.s;
		auto	ret			= parse_expr_value();
		if (!ret)
		{
			if (_rval)
			{
				if (code.empty())
				{
					return	error({location, errorcode::ec_nothing_token});
				}
				else
				{
					return	error({location, errorcode::ec_unkown_token});
				}
			}
			return	ret;
		}

		if (ret.is_symbol())
		{
			pflnode	init	= parse_initializer_list();
			if (bad())
			{
				return	{};
			}
			if (init)
			{
				ret	= (pflnode)pflnode::make<pn_struct_initializer>(ret.location(), std::move(ret), std::move(init));
			}
		}

		// operator test
		operator_type	last	= operator_type::ot_none;
		for (;;)
		{
			skip_blank();
			auto	location	= code.s;

			if (code.starts_with("..."))
			{
				code	= code.substr(3);
				ret		= (pflnode)pflnode::make<pn_operator_single>({code.s-3, code.s}, operator_type::ot_unary_right_fold, std::move(ret));
			}


			if (code.front() == '.' && !code.starts_with(".."))
			{
				if (((int)_previous&0xff) <= ((int)operator_type::ot_member_access&0xff))
				{
					return	ret;
				}
				++code;
				pflnode	sym	= parse_expr(true, operator_type::ot_member_access);
				if (!sym || bad())
				{
					return	{};
				}

				// 関数呼び出しがあったので this コールに変換する
				if (sym.type() == pn_operator_call::type)
				{
					auto	call	= sym.at<pn_operator_call>();
					ret	= (pflnode)pflnode::make<pn_operator_member_call>({location, code.s}, std::move(call->function), std::move(ret), std::move(call->arguments));
				}
				// this callがあったので、this式を変換する
				else if (sym.type() == pn_operator_member_call::type)
				{
					auto	call	= sym.at<pn_operator_member_call>();
					call->thiz	= (pflnode)pflnode::make<pn_operator_member_access>({location, code.s}, std::move(ret), std::move(call->thiz));
					ret	= sym;
				}
				// this callがあったので、this式を変換する
				else if (sym.type() == pn_operator_member_call_arrow::type)
				{
					auto	call	= sym.at<pn_operator_member_call_arrow>();
					call->thiz	= (pflnode)pflnode::make<pn_operator_member_access>({location, code.s}, std::move(ret), std::move(call->thiz));
					ret	= sym;
				}
				else
				{
					ret	= (pflnode)pflnode::make<pn_operator_member_access>({location, code.s}, std::move(ret), std::move(sym));
				}
				continue;
			}

			if (code.starts_with("->"))
			{
				if (((int)_previous&0xff) <= ((int)operator_type::ot_member_access_arrow&0xff))
				{
					return	ret;
				}
				code	= code.substr(2);
				pflnode	sym	= parse_expr(true, operator_type::ot_member_access_arrow);
				if (!sym || bad())
				{
					return	{};
				}

				// 関数呼び出しがあったので this コールに変換する
				if (sym.type() == pn_operator_call::type)
				{
					auto	call	= sym.at<pn_operator_call>();
					ret	= (pflnode)pflnode::make<pn_operator_member_call_arrow>({location, code.s}, std::move(call->function), std::move(ret), std::move(call->arguments));
				}
				// this callがあったので、this式を変換する
				else if (sym.type() == pn_operator_member_call::type)
				{
					auto	call	= sym.at<pn_operator_member_call>();
					call->thiz	= (pflnode)pflnode::make<pn_operator_member_access_arrow>({location, code.s}, std::move(ret), std::move(call->thiz));
					ret	= sym;
				}
				// this callがあったので、this式を変換する
				else if (sym.type() == pn_operator_member_call_arrow::type)
				{
					auto	call	= sym.at<pn_operator_member_call_arrow>();
					call->thiz	= (pflnode)pflnode::make<pn_operator_member_access_arrow>({location, code.s}, std::move(ret), std::move(call->thiz));
					ret	= sym;
				}
				else
				{
					ret	= (pflnode)pflnode::make<pn_operator_member_access_arrow>({location, code.s}, std::move(ret), std::move(sym));
				}
				continue;
			}


			auto	ope			= fetch_operator();

			// function operator_call
			if (code.front() == '(')
			{
				if (((int)_previous&0xff) < ((int)operator_type::ot_function_call&0xff))
				{
					return	ret;
				}

				simplearray<pflnode>	arguments;
				++code;	skip_blank();
				if (code.front() != ')')
				{
					for (;;)
					{
						pflnode	n	= parse_expr(true, operator_type::ot_comma);
						if (!n)
						{
							return	{};
						}
						arguments.push_back(std::move(n));
						skip_blank();
						if (code.front() == ',')
						{
							++code;
							continue;
						}
						if (code.front() == ')')
						{
							break;
						}
						return	{};
					}
				}
				++code;
				ret		= (pflnode)pflnode::make<pn_operator_call>({location, code.s}, std::move(ret), std::move(arguments));
				last	= operator_type::ot_function_call;
				continue;
			}

			if (code.front() == '[')
			{
				if (((int)_previous&0xff) <= ((int)operator_type::ot_subscript&0xff))
				{
					return	ret;
				}
				++code;
				pflnode	idx	= parse_expr(true, operator_type::ot_subscript);
				if (bad() || !idx)
				{
					return	idx;
				}
				skip_blank();
				if (code.front() != ']')
				{
					return	{};
				}
				++code;
				ret		= (pflnode)pflnode::make<pn_operator_subscriptl>({location, code.s}, std::move(ret), std::move(idx));
				last	= operator_type::ot_subscript;
				continue;
			}

			if (((int)_previous&0xff) <= ((int)ope.type&0xff))
			{
				return	ret;
			}

			if ((code.front() == '>'))
			{
				if (templateSkip == 0)
				{

					return	ret;
				}

				// 右辺値が無い場合はテンプレートの終端記号の可能性がある
				psnparser_core	sub(*this);
				sub.code	= sub.code.substr(sub.fetch_operator().e);
				pflnode		rval	= sub.parse_expr_value();
				if (rval.empty() || sub.bad())
				{
					error_clear();
					return	ret;
				}
				--templateSkip;
			}

			// ２項演算子
			if (is_bin_operator(ope.type) || is_assign_operator(ope.type))
			{
				if (((ope.type == ot_leter) || (ope.type == ot_leter_equals) || (ope.type == ot_grater) || (ope.type == ot_grater_equals))
				 && ((last == ot_leter) || (last == ot_leter_equals) || (last == ot_grater) || (last == ot_grater_equals))
				)
				{
					return	ret;
				}

				psnparser_core	rp(owner, code.substr(ope.e));
				auto		re	= rp.parse_expr(true, ope.type);
				if (!re || bad())
				{
					if (bad() && errors().size() == 1 && errors().front().err == errorcode::ec_unkown_token)
					{
						skip_blank();
						if (code.front() == '&' || code.front() == '*')
						{
							error_clear();
						}
					}
					return	ret;
				}
				*this	= rp;
				ret		= (pflnode)pflnode::make<pn_operator_bin>({location, code.s}, ope.type, std::move(ret), std::move(re));
				last	= ope.type;
				continue;
			}



			// インクリメンタル、デクリメンタル
			if (ope.type == operator_type::ot_inc_prefix || ope.type == operator_type::ot_dec_prefix || ope.type == operator_type::ot_inc_suffix || ope.type == operator_type::ot_dec_suffix)
			{
				if (((int)_previous&0xff) <= ((int)ope.type&0xff))
				{
					return	ret;
				}
				code	= code.substr(ope.e);
				operator_type	o	= ope.type == operator_type::ot_inc_prefix ? operator_type::ot_inc_suffix
									: ope.type == operator_type::ot_dec_prefix ? operator_type::ot_dec_suffix
									: ope.type == operator_type::ot_inc_suffix ? operator_type::ot_inc_suffix
									: ope.type == operator_type::ot_dec_suffix ? operator_type::ot_dec_suffix
									: ope.type;
				ret	= (pflnode)pflnode::make<pn_operator_incdec>({location, code.s}, o, std::move(ret));
				last	= ope.type;
				continue;
			}



			// カンマ演算子
			if (ope.type == operator_type::ot_comma)
			{
				simplearray<pflnode>	list({std::move(ret)});
				code	= code.substr(ope.e);

				for (;;)
				{
					pflnode	v	= parse_expr(true, operator_type::ot_comma);
					if (!v)
					{
						return	{};
					}
					list.push_back(std::move(v));
					skip_blank();
					if (code.front() != ',')
					{
						break;
					}
					++code;
				}
				ret		= (pflnode)pflnode::make<pn_list>({location, code.s}, std::move(list));
				last	= ope.type;
				continue;
			}

			// ? : 
			if (ope.type == operator_type::ot_ternary_conditional)
			{
				code	= code.substr(ope.e);
				pflnode	v1	= parse_expr(true, operator_type::ot_ternary_conditional);
				if (!v1)
				{
					return	{};
				}
				if (code.front() != ':')
				{
					return	{};
				}
				++code;

				pflnode	v2	= parse_expr(true, operator_type::ot_none);
				if (!v2)
				{
					return	{};
				}
				ret		= (pflnode)pflnode::make<pn_operator_conditions>({location, code.s}, std::move(ret), std::move(v1), std::move(v2));
				last	= ope.type;
				continue;
			}


			// 不明な演算子なので解析終了
			return	ret;
		}
	}


	auto parse_expr_requires() -> pflnode
	{
		auto	location	= code.s;
		skip_blank();

		pflnode									params;
		simplearray<pn_expr_requires::condinfo>	conditions;

		// have parameter
		if (code.front() == '(')
		{
			++code; skip_blank();
			params	= parse_function_parameter_list();
			skip_blank();
			if (code.front() != ')')
			{
				return	error({location, errorcode::ec_declare_function_parameter_notclosed});
			}
			++code;
		}
		skip_blank();
		if (code.front() != '{')
		{
			return	{};
		}
		++code;
		for (;;)
		{
			skip_blank();
			if (code.front() == '}')
			{
				break;
			}

			pn_expr_requires::condinfo	cond;
			if (code.front() != '{')
			{
				cond.expr	= parse_expr(true);
			}
			else
			{
				++code;
				cond.expr	= parse_expr(true);
				skip_blank();
				if (code.front() != ';')
				{
					return	{};
				}
				++code; skip_blank();
				if (code.front() != '}')
				{
					return	{};
				}
				++code; skip_blank();
				if (!code.starts_with("->"))
				{
					return	{};
				}
				code		= code.substr(2);	skip_blank();
				cond.symbol	= parse_ref_ptr(parse_typeid());
			}
			skip_blank();
			if (code.front() != ';')
			{
				return	{};		// ERROR
			}
			++code;
			conditions.push_back(std::move(cond));
		}
		++code;
		return	(pflnode)pflnode::make<pn_expr_requires>({location, code.s}, std::move(params), std::move(conditions));
	}


	// 関数宣言を読み取る
	auto parse_function(char const* _location, unsigned int _specifier, pflnode& _name, pflnode& _rettype) -> pflnode
	{
		using			initinfo	= pn_declare_function_constractor::initinfo;
		auto			location	= code.s;
		operator_type	ope			= operator_type::ot_none;

		if (_name.type() == pflnode_types::symbol_name && _name.at<pn_symbol_name>()->name == "operator")
		{
			auto	opn	= fetch_functions_operator();
			if (opn.type != operator_type::ot_none)
			{
				ope		= opn.type;
				code	= code.substr(opn.e);
			}
			else
			{
				return	{};		// ERROR
			}
		}

		skip_blank();
		if (code.front() != '(')
		{
			return	{};	// not function
		}
		++code;

		simplearray<initinfo>	inits;
		pflnode					params	= parse_function_parameter_list();
		pflnode					codes;
		if (params.empty())
		{
			return	{};
		}
		skip_blank();
		if (code.front() != ')')
		{
			return	error({location, errorcode::ec_declare_function_parameter_notclosed});
		}
		++code;	skip_blank();
		error_clear();


		for (;;)
		{
			text	tkn	= fetch_keyword();
			     if (tkn == "const")	_specifier	|= (uint32_t)pn_declare_specifier::spec_const;
			else if (tkn == "noexcept")	_specifier	|= (uint32_t)pn_declare_specifier::spec_noexcept;
			else	break;
			code	= code.substr(tkn.e);
		}
		for (;;)
		{
			text	tkn	= fetch_keyword();
			     if (tkn == "override")	_specifier	|= (uint32_t)pn_declare_specifier::spec_virt_override;
			else if (tkn == "final")	_specifier	|= (uint32_t)pn_declare_specifier::spec_virt_final;
			else	break;
			code	= code.substr(tkn.e);
		}
		skip_blank();

		// initializers
		if (code.front() == ':')
		{
			if (_rettype)
			{
				return	error({location, errorcode::ec_declare_function_have_intializers});
			}
			++code;	skip_blank();
			for (;;)
			{
				initinfo	param;
				param.name	= parse_symbol();
				if (!param.name)
				{
					return	error({location, errorcode::ec_declare_constractor_intializers_varname_nothing});
				}
				error_clear();
				skip_blank();
				if (code.front() != '(')
				{
					return	error({location, errorcode::ec_declare_constractor_intializers_value_nothing});
				}
				++code;
				auto	argv	= parse_expr(false);
				auto	nlst	= argv.at<pn_list>();
				if (nlst)
				{
					param.init		= std::move(nlst->values);
				}
				else if (argv)
				{
					param.init.push_back(argv);
				}

				skip_blank();
				if (code.front() != ')')
				{
					return	error({location, errorcode::ec_declare_constractor_intializers_value_unclosed});
				}
				error_clear();

				++code;
				skip_blank();
				inits.push_back(std::move(param));
				if (code.front() != ',')
				{
					break;
				}
				++code;
			}
			skip_blank();
		}

		if (code.front() == '=')
		{
			++code;	skip_blank();
			for (;;)
			{
				text	tkn	= fetch_keyword();
					 if (tkn == "default")	_specifier	|= (uint32_t)pn_declare_specifier::spec_default;
				else if (tkn == "delete")	_specifier	|= (uint32_t)pn_declare_specifier::spec_delete;
				else	break;
				code	= code.substr(tkn.e);
			}
		}


		pflnode		autoret;
		if (code.starts_with("->"))
		{
			if (_rettype.type() != pflnode_types::symbol_name || _rettype.at<pn_symbol_name>()->name != "auto")
			{
				return	error({location, errorcode::ec_declare_function_after_returntype_not_auto});
			}
			code = code.substr(2);
			skip_blank();
			autoret	= parse_ref_ptr(parse_typeid());
			if (!autoret)
			{
				return	error({location, errorcode::ec_declare_function_after_returntype_nothing});
			}
			skip_blank();
		}

		if (inits.size() > 0 && code.front() != '{')
		{
			return	error({location, errorcode::ec_declare_constractor_codeblock_nothing});
		}

		if (code.front() == '{')
		{
			codes	= parse_statement(codeblock);
			if (bad())
			{
				return	error({location, errorcode::ec_declare_functin_codeblock_unclosed});
			}
		}
		else if (code.front() != ';')
		{
			return	error({location, errorcode::ec_declare_functin_terminate_nothing});
		}

		if (_name.empty())
		{
			return	(pflnode)pflnode::make<pn_declare_function_conversion>({_location, code.s}, std::move(_rettype), std::move(params), std::move(codes), _specifier);
		}
		else if (_rettype.empty())
		{
			if (is_destractor_symbol(_name))
					return	(pflnode)pflnode::make<pn_declare_function_destractor>({_location, code.s}, std::move(_name), pflnode(), std::move(params), std::move(codes), _specifier);
			else	return	(pflnode)pflnode::make<pn_declare_function_constractor>({_location, code.s}, std::move(_name), pflnode(), std::move(inits), std::move(params), std::move(codes), _specifier);
		}
		else if (ope != operator_type::ot_none)
		{
			return	(pflnode)pflnode::make<pn_declare_function_operator>({_location, code.s}, (operator_type)ope, autoret ? std::move(autoret) : std::move(_rettype), std::move(params), std::move(codes), _specifier);
		}
		else
		{
			return	(pflnode)pflnode::make<pn_declare_function>({_location, code.s}, std::move(_name), autoret ? std::move(autoret) : std::move(_rettype), std::move(params), std::move(codes), _specifier);
		}
	}


	auto parse_function_parameter_list() -> pflnode
	{
		auto	parseone = [](psnparser_core* _thiz) -> pflnode
		{
			const char*									location	= _thiz->code.s;
			simplearray<pn_parameter_list::parameter>	arguments;
			for (;;)
			{
				pn_parameter_list::parameter	arg;
				psnparser_core						ap(*_thiz);
				arg.first	= ap.parse_typeid();
				if (arg.first.empty())
				{
					break;
				}
				arg.first	= ap.parse_ref_ptr(arg.first);
				_thiz->error_clear();
				*_thiz	= ap;
				// 変数名。読み取れたら読み取る。エラーになったらたぶん変数名ではないので無視する
				if (_thiz->fetch_operator().type != operator_type::ot_comma && _thiz->fetch_operator().type != operator_type::ot_assign)
				{
					ap			= *_thiz;
					arg.second	= ap.parse_expr(true, operator_type::ot_scope);
					if (arg.second)
					{
						_thiz->skip_blank();
						_thiz->error_clear();
						operator_type	o		= operator_type::ot_none;
						size_t			pointer	= 0;
						if (arg.second.type() == pn_operator_address_of::type)
						{
							auto	n	= arg.second.at<pn_operator_address_of>();
							arg.second	= std::move(n->value);
							o			= operator_type::ot_address_of;
						}
						else if (arg.second.type() == pn_operator_indirection::type)
						{
							o			= operator_type::ot_indirection;
							while (arg.second.type() == pn_operator_indirection::type)
							{
								auto	n	= arg.second.at<pn_operator_indirection>();
								arg.second	= std::move(n->value);
								++pointer;
							}
						}
						ap.skip_blank();
						if (ap.code.front() == '(')
						{
							++ap.code;
							pflnode	parameters	= ap.parse_function_parameter_list();
							if (_thiz->bad())
							{
								return	parameters;
							}
							ap.skip_blank();
							if (ap.code.front() != ')')
							{
								return	_thiz->error({_thiz->code.s, errorcode::ec_circlebracket_unclosed});
							}
							++ap.code;
							arg.first	= (pflnode)pflnode::make<pn_symbol_functype>(arg.second.location(), std::move(arg.first), std::move(parameters));
						}
						else if (ap.code.front() == '[')
						{
							simplearray<pflnode>	sizes;
							for (;;)
							{
								pflnode	n;
								if (ap.code.front() != '[')
								{
									break;
								}
								++ap.code; ap.skip_blank();
								if (ap.code.front() != ']')
								{
									n	= ap.parse_expr(true);
									if (_thiz->bad())
									{
										return	n;
									}
									ap.skip_blank();
								}
								if (ap.code.front() != ']')
								{
									return	_thiz->error({_thiz->code.s, errorcode::ec_qauarebracket_unclosed});
								}
								sizes.push_back(std::move(n));
								++ap.code;
							}
							arg.first	= (pflnode)pflnode::make<pn_spec_symbol_array>(arg.second.location(), std::move(arg.first), std::move(sizes));
						}
						if (o == operator_type::ot_address_of)	arg.first	= (pflnode)pflnode::make<pn_spec_symbol_reference>(arg.second.location(), std::move(arg.first));
						if (o == operator_type::ot_indirection)
						{
							while (pointer > 0)
							{
								--pointer;
								arg.first	= (pflnode)pflnode::make<pn_spec_symbol_pointer>(arg.second.location(), std::move(arg.first));
							}
						}
						*_thiz	= ap;
					}
					_thiz->error_clear();
				}
				if (_thiz->fetch_operator().type == operator_type::ot_assign)
				{
					ap			= *_thiz;
					++ap.code;
					arg.init	= ap.parse_expr(true, operator_type::ot_assign);
					if (arg.init)
					{
						_thiz->error_clear();
						*_thiz	= ap;
					}
				}
				arguments.push_back(std::move(arg));
				if (_thiz->fetch_operator().type != operator_type::ot_comma)
				{
					break;
				}
				++_thiz->code;
			}
			return	(pflnode)pflnode::make<pn_parameter_list>({location, _thiz->code.s}, std::move(arguments));
		};
		psnparser_core	sub(*this);
		sub.templateSkip	= 0;
		pflnode		r	= parseone(&sub);
		code	= sub.code;
		return	r;
	}


	// initializer list
	//	{ で始まっていない場合は empty()
	//  { で始まっているが閉じられていない場合は bad()
	auto parse_initializer_list() -> pflnode
	{
		auto	location	= code.s;
		skip_blank();
		if (code.front() != '{')
		{
			return	{};
		}
		++code;	skip_blank();

		simplearray<pn_initializer_list::initinfo>	values;
		if (code.front() != '}')
		{
			for (;;)
			{
				pn_initializer_list::initinfo	info;
				info.name	= nullptr;

				skip_blank();
				if (code.front() == '.')
				{
					++code;
					info.name	= read_keyword();
					if (info.name.empty())
					{
						return	{};	// ERROR
					}
					skip_blank();
					if (code.front() != '=')
					{
						return	{};	// ERROR
					}
					++code;
				}

				info.value	= parse_expr(true, operator_type::ot_comma);
				if (!info.value || bad())
				{
					return	{};
				}
				values.push_back(std::move(info));
				skip_blank();
				if (code.front() == ',')
				{
					++code;
					continue;
				}
				if (code.front() == '}')
				{
					break;
				}
				return	{};	// ERROR
			}
		}
		++code;
		return	(pflnode)pflnode::make<pn_initializer_list>({location, code.s}, std::move(values));
	}


	auto parse_expr_lambda() -> pflnode
	{
		auto	location	= code.s;
		pflnode	rettype;
		pflnode	captures;
		pflnode	parameters;
		pflnode	statements;

		skip_blank();
		if (code.front() != '[')
		{
			return	{};
		}
		++code;	skip_blank();




		if (code.front() != ']')
		{
			// ERROR
			return	{};
		}
		++code;	skip_blank();
		if (code.front() != '(')
		{
			// ERROR
			return	{};
		}
		++code;	skip_blank();
		parameters	= parse_function_parameter_list();
		if (bad())
		{
			return	parameters;
		}
		if (code.front() != ')')
		{
			return	{};
		}
		++code;	skip_blank();
		if (code.starts_with("->"))
		{
			code	= code.substr(2);
			skip_blank();
			rettype	= parse_typeid();
		}

		skip_blank();
		if (code.front() != '{')
		{
			// ERROR
			return	{};
		}

		statements	= parse_statement(codeblock);
		if (bad())
		{
			
			return	{};
		}

		return	(pflnode)pflnode::make<pn_expr_lambda>({location, code.s}, std::move(rettype), std::move(captures), std::move(parameters), std::move(statements));
	}


	// ひとつのメンバを読み取る
	auto parse_member(uint32_t _access) -> pflnode
	{
		auto	location	= code.s;
		auto	keyword		= fetch_keyword();

		if (keyword == "private" || keyword == "protected" || keyword == "public")
		{
			psnparser_core	acc(*this);
			acc.read_keyword();
			acc.skip_blank();
			if (acc.code.front() == ':')
			{
				++acc.code;
				*this	= acc;
				return	(pflnode)pflnode::make<pn_label>({location, code.s}, keyword);
			}
		}

		return	parse_declare(memberblock, _access);
	}



	auto parse_template_parameter_list() -> pflnode
	{
		const char*									location	= code.s;
		text										backup(code);
		simplearray<pn_parameter_list::parameter>	arguments;
		for (;;)
		{
			skip_blank();
			pn_parameter_list::parameter	arg;
			psnparser_core						ap(*this);
			auto							token	= ap.fetch_keyword();

			// read "class" | "typename" | typeid
			if (token == "class" || token == "typename")
			{
				auto	pos	= code.s;
				ap.read_keyword();
				arg.first	= (pflnode)pflnode::make<pn_symbol_name>({pos, code.s}, token);
				if (ap.code.starts_with("..."))
				{
					ap.code		= ap.code.substr(3);
					arg.first	= (pflnode)pflnode::make<pn_spec_symbol_parampack>({pos, code.s}, std::move(arg.first));
				}
			}
			else
			{
				arg.first	= ap.parse_typeid();
			}
			if (arg.first.empty())
			{
				break;
			}

			// name
			*this	= ap;
			if (fetch_operator().type != operator_type::ot_comma && fetch_operator().type != operator_type::ot_assign)
			{
				ap			= *this;
				arg.second	= ap.parse_symbol();
				if (arg.second)
				{
					*this	= ap;
				}
			}
			skip_blank();

			// default value
			if (fetch_operator().type == operator_type::ot_assign)
			{
				ap			= *this;
				++ap.code;
				psnparser_core	backup(ap);
				arg.init	= ap.parse_typeid();
				if (!arg.init)
				{
					ap			= backup;
					arg.init	= ap.parse_expr(true, operator_type::ot_assign);
					if (!arg.init)
					{
						return	{};
					}
				}
				arg.init	= ap.parse_ref_ptr(arg.init);
				*this	= ap;
			}
			arguments.push_back(std::move(arg));
			if (fetch_operator().type != operator_type::ot_comma)
			{
				break;
			}
			++code;
		}
		return	(pflnode)pflnode::make<pn_parameter_list>({location, code.s}, std::move(arguments));
	}


	// ひとつの文を読み取る
	auto parse_statement(blocktypes _block) -> pflnode
	{
		skip_blank();
		auto	location	= code.s;
		auto	keyword		= fetch_keyword();

		if (owner->mode.document_mode)
		{
			if (!documentMode && code.starts_with("%>"))
			{
				documentMode	= true;
				code			= code.substr(2).trim_first_one_return();
			}

			// <% が見つかるまでを抜き出す
			if (documentMode)
			{
				text	cur(code);
				text	doc(code);
				for (;;)
				{
					if (cur.starts_with("<%") || cur.empty())
					{
						break;
					}
					++cur;
				}
				doc		= doc.substr(0, cur.s);
				code	= code.substr(cur.s);
				if (code.starts_with("<%"))
				{
					code	= code.substr(2);
				}
				documentMode	= false;
				return	(pflnode)pflnode::make<pn_echo>({location, code.s}, doc);
			}
		}

		if (code.front() == ';')
		{
			++code;
			return	{};
		}


		if (keyword == "break")
		{
			read_keyword();
			skip_blank();
			if (code.front() != ';')
			{
				return	{};
			}
			++code;
			return	(pflnode)pflnode::make<pn_stmt_branch_break>({location, code.s});
		}

		if (keyword == "co_await")
		{
		}

		if (keyword == "co_return")
		{
		}

		if (keyword == "co_yield")
		{
		}
 
		if (keyword == "continue")
		{
			read_keyword();
			skip_blank();
			if (code.front() != ';')
			{
				return	{};
			}
			++code;
			return	(pflnode)pflnode::make<pn_stmt_branch_continue>({location, code.s});
		}

		if (keyword == "do")
		{
			read_keyword();
			return	parse_stmt_do_while();
		}

		if (keyword == "for")
		{
			read_keyword();
			return	parse_stmt_for();
		}

		if (keyword == "if")
		{
			read_keyword();
			return	parse_stmt_if();
		}

		if (keyword == "return")
		{
			read_keyword();
			skip_blank();

			pflnode	val;
			if (code.front() != ';')
			{
				val	= parse_expr(true);
				if (bad() || code.front() != ';')
				{
					return	{};
				}
				++code;
			}
			return	(pflnode)pflnode::make<pn_stmt_return>({location, code.s}, std::move(val));
		}

		if (keyword == "static_assert")
		{
		}

		if (keyword == "switch")
		{
			read_keyword();
			return	parse_stmt_switch();
		}

		if (keyword == "while")
		{
			read_keyword();
			return	parse_stmt_while();
		}

		// check label
		if (!keyword.empty())
		{
			psnparser_core	sub(owner, code);
			sub.read_keyword();
			sub.skip_blank();
			if (sub.code.front() == ':' && !sub.code.starts_with("::"))
			{
				code	= sub.code;
				return	(pflnode)pflnode::make<pn_label>({location, code.s}, keyword);
			}
		}

		skip_blank();
		if (code.front() == '{')
		{
			++code;
			simplearray<pflnode>	stms;
			for (;;)
			{
				skip_blank();
				if (code.empty())
				{
					return	error({location, errorcode::ec_codeblock_unclosed});
				}
				if (code.front() == '}')
				{
					++code;
					return	(pflnode)pflnode::make<pn_stmt_compound>({location, code.s}, std::move(stms));
				}
				pflnode	s	= parse_statement(codeblock);
				if (s)
				{
					stms.push_back(std::move(s));
				}
			}
		}

		return	parse_declare(_block);
	}


	// 文に使用される式を返す
	auto parse_statement_expr(operator_type _previous) -> pflnode
	{
		pflnode	n;
		nest_reset();
		psnparser_core	backup(*this);

		// とりあえず最初のシンボルを読み取る
		pflnode		f	= parse_typeid();
		if (bad())
		{
			return	f;
		}
		f	= parse_ref_ptr(f);
		if (f.type() == pflnode_types::spec_symbol_reference
		 || f.type() == pflnode_types::sepc_symbol_pointer)
		{
			// 最初のシンボルが & か * で終わっているなら強制的に型名として扱う
			return	f;
		}
		else
		{
			error_clear();
			*this	= backup;
			n		= parse_expr(true, _previous);
			if (bad())
			{
				return	n;
			}
		}
		nest_reset();
		return	n;
	}


	auto parse_stmt_for() -> pflnode
	{
		pflnode	init;
		pflnode	cond;
		pflnode	incr;
		pflnode	stmt;
		auto	location	= code.s;


		skip_blank();
		if (code.front() != '(')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();

		// init
		if (code.front() != ';')
		{
			init	= parse_declare(initstmt);
			if (bad())
			{
				return	{};
			}
			skip_blank();
		}

		// ranged base for loop
		skip_blank();
		if (code.front() == ':')
		{
			++code;
			cond	= parse_expr(true);
			skip_blank();
			if (code.front() != ')')
			{
				return	error({location, errorcode::ec_for_circle_unopened});
			}
			++code;
			if (code.front() == ';')
			{
				++code;
			}
			else
			{
				stmt	= parse_statement(codeblock);
			}
			return	(pflnode)pflnode::make<pn_stmt_for_rangedbase>({location, code.s}, std::move(init), std::move(cond), std::move(stmt));
		}


		skip_blank();
		if (code.front() != ';')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;


		// cond
		skip_blank();
		if (code.front() != ';')
		{
			cond	= parse_expr(true);
			if (bad())
			{
				return	{};
			}
			skip_blank();
		}
		if (code.front() != ';')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;

		// incr
		skip_blank();
		if (code.front() != ')')
		{
			incr	= parse_expr(false);
			if (bad())
			{
				return	{};
			}
			skip_blank();
		}
		if (code.front() != ')')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();
		if (code.front() == ';')
		{
			++code;
		}
		else
		{
			// stmt
			stmt	= parse_statement(codeblock);
			if (bad())
			{
				return	{};
			}
		}
		return	(pflnode)pflnode::make<pn_stmt_for>({location, code.s}, std::move(init), std::move(cond), std::move(incr), std::move(stmt));
	}

	auto parse_stmt_if() -> pflnode
	{
		pflnode	init;
		pflnode	cond;
		pflnode	stmt1;
		pflnode	stmt2;
		auto	location	= code.s;

		if (fetch_keyword() == "constexpr")
		{
			read_keyword();
		}

		skip_blank();
		if (code.front() != '(')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();

		// conditions
		psnparser_core	sub(*this);
		cond	= sub.parse_declare(initstmt);
		if (cond)
		{
			*this	= sub;
		}
		else
		{
			cond	= parse_expr(true);
		}
		if (code.front() == ';')
		{
			++code;	skip_blank();
			init	= std::move(cond);
			cond	= parse_expr(true);
		}
		skip_blank();
		if (code.front() != ')')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();
		if (code.front() == ';')
		{
			++code;
		}
		else
		{
			// stmt
			stmt1	= parse_statement(codeblock);
			if (bad())
			{
				return	{};
			}
			++code;	skip_blank();
			if (fetch_keyword() == "else")
			{
				read_keyword();
				stmt2	= parse_statement(codeblock);
			}
		}
		return	(pflnode)pflnode::make<pn_stmt_if>({location, code.s}, std::move(init), std::move(cond), std::move(stmt1), std::move(stmt2));
	}

	auto parse_stmt_do_while() -> pflnode
	{
		pflnode	cond;
		pflnode	stmt;
		auto	location	= code.s;

		stmt	= parse_statement(codeblock);
		if (bad())
		{
			return	{};
		}

		if (fetch_keyword() != "while")
		{
			return	{};
		}
		read_keyword();	skip_blank();
		if (code.front() != '(')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();

		// conditions
		cond	= parse_expr(true);
		skip_blank();
		if (code.front() != ')')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();
		if (code.front() != ';')
		{
			return	{};
		}
		return	(pflnode)pflnode::make<pn_stmt_do_while>({location, code.s}, std::move(cond), std::move(stmt));
	}


	auto parse_stmt_switch() -> pflnode
	{
		pflnode									init;
		pflnode									cond;
		simplearray<pn_stmt_switch::casedata>	cases;
		auto									location	= code.s;


		skip_blank();
		if (code.front() != '(')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();

		// conditions
		cond	= parse_expr(true);
		skip_blank();
		if (code.front() != ')')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();
		if (code.front() != '{')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;
		for (;;)
		{
			skip_blank();
			if (code.front() == '}')
			{
				break;
			}
			auto	tkn	= read_keyword();
			if (tkn != "case" && tkn != "default")
			{
				return	{};
			}
			skip_blank();
			pn_stmt_switch::casedata	c;
			if (tkn != "default")
			{
				c.value	= parse_expr(true);
				if (bad() || !c.value)
				{
					return	{};
				}
			}
			skip_blank();
			if (code.front() != ':')
			{
				return	{};
			}
			++code;	skip_blank();
			simplearray<pflnode>	statements;
			for (;;)
			{
				if (fetch_keyword() == "case" || fetch_keyword() == "default" || code.front() == '}')
				{
					break;
				}
				pflnode	n	= parse_statement(codeblock);
				if (bad())
				{
					return	n;
				}
				statements.push_back(std::move(n));
			}
			if (statements.size() == 1)
			{
				c.statements	= std::move(statements.front());
			}
			else
			{
				c.statements	= (pflnode)pflnode::make<pn_stmt_compound>({location, code.s}, std::move(statements));
			}
			cases.push_back(std::move(c));
		}
		++code;
		return	(pflnode)pflnode::make<pn_stmt_switch>({location, code.s}, std::move(cond), std::move(cases));
	}


	auto parse_stmt_while() -> pflnode
	{
		pflnode	cond;
		pflnode	stmt;
		auto	location	= code.s;


		skip_blank();
		if (code.front() != '(')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();

		// conditions
		cond	= parse_expr(true);
		skip_blank();
		if (code.front() != ')')
		{
			return	error({location, errorcode::ec_for_circle_unopened});
		}
		++code;	skip_blank();
		if (code.front() == ';')
		{
			++code;
		}
		else
		{
			// stmt
			stmt	= parse_statement(codeblock);
			if (bad())
			{
				return	{};
			}
		}
		return	(pflnode)pflnode::make<pn_stmt_while>({location, code.s}, std::move(cond), std::move(stmt));
	}


	auto parse_string(result<string_literal> _str) -> pflnode
	{
		auto		location	= _str.code.s;
		pflnode		ret;
		text		s		= add_token(std::move(_str.decode<sstring>()));
		char const*	beg		= s.s;
		while (!s.empty())
		{
			if (s.front() == '\\')
			{
				++s;
				if (!s.empty())
				{
					++s;
				}
				continue;
			}
			// extract expression
			if (s.starts_with("${"))
			{
				// check close
				text		backup(s);
				s	= s.substr(2); 
				char const*	open	= s.s;
				while (!s.empty() && s.front() != '}')
				{
					if (s.front() == '\\')
					{
						++s;
						if (!s.empty())
						{
							++s;
						}
						continue;
					}
					++s;
				}
				if (s.front() != '}')
				{
					s	= backup;
					break;
				}
				const char*	close	= s.s;
				++s;

				if (beg != backup.s)
				{
					pflnode	n	= (pflnode)pflnode::make<pn_literal_string>({location, code.s}, add_token(sstring(beg, backup.s)));
					if (ret)
					{
						ret	= (pflnode)pflnode::make<pn_operator_bin>(ret.location(), operator_type::ot_addition, std::move(ret), std::move(n));
					}
					else
					{
						ret	= std::move(n);
					}
				}
				psnparser_core	in(owner, text(open, close));
				in.skip_blank();
				if (!in.code.empty())
				{
					pflnode		n	= in.parse_expr(true);
					if (!n)
					{
						return	{};
					}
					ret	= (pflnode)pflnode::make<pn_operator_bin>(ret.location(), operator_type::ot_addition, std::move(ret), std::move(n));
				}
				beg	= s.s;
				continue;
			}

			++s;
		}
		if (beg != s.s)
		{
			pflnode	n	= (pflnode)pflnode::make<pn_literal_string>({location, code.s}, add_token(sstring(beg, s.s)));
			if (ret)
			{
				ret	= (pflnode)pflnode::make<pn_operator_bin>(ret.location(), operator_type::ot_addition, std::move(ret), std::move(n));
			}
			else
			{
				ret	= std::move(n);
			}
		}
		code	= _str.next;
		return	ret;
	}



	auto parse_typeid(bool _typemode = true) -> pflnode
	{
		struct	matchstatus
		{
			struct	scope
			{
				text				symbol;
				simplearray<pflnode>	params;
				int					paramsize	= -1;
			};

			struct	matchinfo
			{
				matchinfo() : depth(-1) {}
				matchinfo(matchinfo&&) = default;
				matchinfo& operator = (matchinfo&&) = default;
				auto push(scope scope)
				{
					if (depth == -1)
					{
						scopes.resize(10);
						depth	= 0;
					}
					else if (scopes.size() == depth)
					{
						scopes.resize(depth + 20);
					}
					scopes[depth++] = std::move(scope);
				}
				simplearray<scope>	scopes;
				int					depth;
				char const*			position	= nullptr;
			};


			matchinfo				matched;
			char const*				location;


			auto parse(psnparser_core& p) -> pflnode
			{
				location	= p.code.s;
				text	startpos(p.code);
				pflnode	decl;
				bool	abs	= false;

				p.skip_blank();
				if (p.code.starts_with("::"))
				{
					if (p.templateSkip <= 0)
					{
						abs		= true;
						p.code	= p.code.substr(2);
					}
					else
					{
						--p.templateSkip;
						return	{};
					}
				}

				text	first	= p.fetch_keyword();

				if (first == "auto")
				{
					p.code	= p.code.substr(first.e);	p.skip_blank();
					decl	= (pflnode)pflnode::make<pn_symbol_name>({location, p.code.s}, first);
					auto	ope	= p.fetch_operator();
					if (ope.type != operator_type::ot_scope)
					{
						return	decl;
					}
					p.code	= p.code.substr(ope.e);
				}
				else if (first == "decltype")
				{
					p.code	= p.code.substr(first.e);	p.skip_blank();
					if (p.code.front() != '(')
					{
						p.error({location, errorcode::ec_decltype_has_not_expr});
						return	{};
					}
					++p.code;
					decl	= p.parse_expr(true);
					if (!decl || p.bad())
					{
						p.error({location, errorcode::ec_decltype_has_not_expr});
						return	{};
					}
					p.skip_blank();
					if (p.code.front() != ')')
					{
						p.error({location, errorcode::ec_decltype_has_not_expr});
						return	{};
					}
					++p.code;
					decl	= (pflnode)pflnode::make<pn_symbol_decltye>({location, p.code.s}, std::move(decl));

					auto	ope	= p.fetch_operator();
					if (ope.type != operator_type::ot_scope)
					{
						return	decl;
					}
					p.code	= p.code.substr(ope.e);
				}
				else if (p.is_keywords(first) && first != "template")
				{
					if (abs)	p.code = startpos;
					return	{};
				}

				matchinfo	match;
				matchinfo	work;
				if (p.bad() || !parse_one(match, work, p))
				{
					if (abs)	p.code = startpos;
					return	{};
				}

				// match.depth は必ず 1 以上になっている（そうでないならエラー）
				if (!decl && match.depth == 1)
				{
					auto	ret	= make_node(match.scopes.front());
					if (abs)
					{
						ret	= (pflnode)pflnode::make<pn_symbol_nameabs>({location, p.code.s}, std::move(ret));
					}
					return	ret;
				}

				size_t				off	= decl ? 1 : 0;
				simplearray<pflnode>	scopes((size_t)match.depth + off);
				if (decl)
				{
					scopes[0]	= std::move(decl);
				}
				for (size_t i = 0; i < (size_t)match.depth; ++i)
				{
					scopes[i + off] = make_node(match.scopes[i]);
				}
				auto	ret	= (pflnode)pflnode::make<pn_symbol_scope>({location, p.code.s}, std::move(scopes));
				if (abs)
				{
					ret	= (pflnode)pflnode::make<pn_symbol_nameabs>({location, p.code.s}, std::move(ret));
				}
				return	ret;
			}

			auto make_node(scope& _scope) -> pflnode
			{
				if (_scope.paramsize < 0)
				{
					return	(pflnode)pflnode::make<pn_symbol_name>(_scope.symbol, _scope.symbol);
				}

				simplearray<pflnode>	params((size_t)_scope.paramsize);
				for (size_t i = 0; i < (size_t)_scope.paramsize; ++i)
				{
					params[i] = std::move(_scope.params[i]);
				}
				return	(pflnode)pflnode::make<pn_symbol_template>(_scope.symbol, _scope.symbol, std::move(params));
			}

			auto parse_one(matchinfo& matched, matchinfo& current, psnparser_core& p) -> bool
			{
				for (int depth = 0; ; ++depth)
				{
					scope		s;
					text		begin(p.code);
					bool		force		= false;
					s.symbol		= p.read_keyword();
					if (s.symbol == "template")
					{
						s.symbol	= p.read_keyword();
						force		= true;
						if (s.symbol.empty())
						{
							p.error({location, errorcode::ec_nothing_scope_right});
							return	false;
						}

					}
					else if (p.is_keywords(s.symbol))
					{
						p.code	= begin;
						return	false;
					}
					else if (p.code.front() == '~')
					{
						++p.code;
						s.symbol	= p.read_keyword();
						if (s.symbol.empty())
						{
							if (depth > 0)
							{
								p.error({location, errorcode::ec_nothing_scope_right});
							}
							else
							{
								p.code	= begin;
							}
							return	false;
						}
						s.symbol	= p.add_token(sstring("~") + s.symbol);
					}
					else if (s.symbol.empty())
					{
						if (!current.scopes.empty())
						{
							p.error({location, errorcode::ec_nothing_scope_right});
						}
						return	false;
					}


					p.skip_blank();
					if (p.code.front() == '<')
					{
						const char*	beg	= p.code.s;
						++p.code; p.skip_blank();
						// none parameter template
						if (p.code.front() == '>')
						{
							++p.code; p.skip_blank();
							s.paramsize	= 0;
						}
						// template チェックをスキップする
						else if (p.templateSkip > 0)
						{
							--p.templateSkip;
							p.code.s	= beg;		// 位置を戻す
						}
						// template パラメータをテスト
						else
						{
							char const*		matched	= nullptr;
							char const*		lastpos	= nullptr;
							for (int skip = 0; ; ++skip)
							{
								simplearray<pflnode>	args;
								psnparser_core			sub(p);
								sub.error_clear();
								sub.templateSkip	= skip;
								for (;;)
								{
									pflnode	n	= sub.parse_expr(false, operator_type::ot_comma);
									if (sub.bad())
									{
										if (p.errors().size() >= 1 && p.errors().front().err != errorcode::ec_nothing_scope_right && sub.code.front() == '>')
										{
											p.error_clear();
										}
										break;
									}
									n	= sub.parse_ref_ptr(n);
									args.push_back(std::move(n));
									sub.skip_blank();
									if (sub.code.front() == '>' || sub.code.front() != ',')
									{
										break;
									}
									++sub.code;	sub.skip_blank();
								}

								// 検索範囲が変化しなくなったら最後まで調べ切っているので終了
								if (matched == sub.code.s)
								{
									break;
								}

								// 直後に値が続いている場合はテンプレートパラメータとしては無効にする

								matched	= sub.code.s;
								if (!sub.bad() && sub.code.front() == '>')
								{
									// 前回の検索よりも更新できなくなった終了
									if (lastpos >= sub.code.s)
									{
										break;
									}
									s.params	= std::move(args);
									s.paramsize	= (int)s.params.size();
									lastpos		= sub.code.s + 1;
								}
							}
							if (lastpos)
							{
								p.code.s	= lastpos;
							}
							else
							{
								p.code.s	= beg;
							}
						}

					}
					else if (force)
					{
						return	p.error({location, errorcode::ec_template_has_not_params});
					}
					current.push(std::move(s));

					//
					p.skip_blank();
					auto	ope	= p.fetch_operator();
					if (ope.type == operator_type::ot_scope)
					{
						p.code	= p.code.substr(ope.e);
						continue;
					}
					current.position	= p.code.s;

					// 解析終了。より長くまで解析出来ているなら結果を格納
					if (matched.position < current.position)
					{
						matched	= std::move(current);
						return	true;
					}
					return	false;
				}
			}

		}	m;



		auto	rollback	= code;
		auto	location	= code.s;
		bool	isConst		= false;
		bool	isPointer	= false;
		if (_typemode)
		{
			if (fetch_keyword() == "class" || fetch_keyword() == "typename")
			{
				read_keyword();
				skip_blank();
			}

			if (fetch_keyword() == "const")
			{
				isConst	= true;
				read_keyword();
			}
		}

		auto	ret	= 	m.parse(*this);
		if (!ret || bad())
		{
			return	ret;
		}

		// 関数型の確認
		skip_blank();
		text	backup(code);
		if (_typemode && code.front() == '(')
		{
			++code; skip_blank();
			// c lang compatible
			if (code.front() == '*')
			{
				++code;	skip_blank();
				if (code.front() == ')')
				{
					++code;	skip_blank();
					isPointer	= true;
				}
			}
			if (!isPointer)
			{
				code	= backup;
			}
			skip_blank();
			if (code.front() == '(')
			{
				++code;
				text	roll	= code;
				pflnode	params	= parse_function_parameter_list();
				if (code.front() != ')')
				{
					code	= backup;
				}
				else
				{
					++code;
					ret	= (pflnode)pflnode::make<pn_symbol_functype>({location, code.s}, std::move(ret), std::move(params));
				}
			}
			else if (!isPointer)
			{
				code		= backup;
			}
		}

		if (isConst)
		{
			ret	= (pflnode)pflnode::make<pn_spec_symbol_const>({location, code.s}, std::move(ret));
		}
		if (isPointer)
		{
			ret	= (pflnode)pflnode::make<pn_spec_symbol_pointer>({location, code.s}, std::move(ret));
		}

		if (_typemode)
		{
			psnparser_core	backpos(*this);
			pflnode		commit	= ret;
			for (;;)
			{
				skip_blank();
				if (code.front() == '*')
				{
					commit	= ret;
					backpos	= *this;
					++code;
					ret	= (pflnode)pflnode::make<pn_spec_symbol_pointer>({location, code.s}, std::move(ret));
				}
				else if (fetch_keyword() == "const")
				{
					read_keyword();
					ret		= (pflnode)pflnode::make<pn_spec_symbol_const>({location, code.s}, std::move(ret));
					commit	= ret;
					backpos	= *this;
				}
				else
				{
					break;
				}
			}
			skip_blank();

			if (code.starts_with("&&"))
			{
				code	= code.substr(2);
				ret		= (pflnode)pflnode::make<pn_spec_symbol_move>({location, code.s}, std::move(ret));
				commit	= ret;
				backpos	= *this;
			}
			else if (code.starts_with("&"))
			{
				commit	= ret;
				backpos	= *this;
				++code;
				ret		= (pflnode)pflnode::make<pn_spec_symbol_reference>({location, code.s}, std::move(ret));
			}
			skip_blank();
			if (code.starts_with("..."))
			{
				code	= code.substr(3);
				ret		= (pflnode)pflnode::make<pn_spec_symbol_parampack>(ret.location(), std::move(ret));
				skip_blank();
				commit	= ret;
				backpos	= *this;
			}

			// & か * で終わっているなら後続のコードによって処理を分けるひつようがあるのでいったん backpos に戻す
			*this	= backpos;
			ret		= commit;
		}
		return	ret;
	}


	// 一つのシンボルを読み取る
	//	example)	foo<bar, expr>::huge::fuga<x, y, z>
	auto parse_symbol() -> pflnode
	{
		return	parse_typeid(false);
	}


	// キーワードを読み取る
	auto read_keyword() -> text
	{
		if (!skip_blank())
		{
			return	code.substr(0, 0);
		}
		auto	beg	= code.s;
		auto	c	= code.front();
		if (!(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_')))
		{
			return	code.substr(0, 0);
		}
		do
		{
			++code;
			c	= code.front();
		} while (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'));
		return	{beg, code.s};
	}


	// 空白を読み飛ばす
	auto skip_blank() -> bool
	{
		text	prev;
		do
		{
			prev	= code;
			code	= code.trim_first(" \t\r\n");
			if (code.starts_with("//"))
			{
				auto	pos	= code.find_first_of("\r\n");
				if (pos == text::npos)
				{
					code	= code.substr(code.size());
				}
				else
				{
					code	= code.substr(pos);
				}
			}
			else if (code.starts_with("/*"))
			{
				auto	pos	= code.find("*/");
				if (pos == text::npos)
				{
					code	= code.substr(code.size());
				}
				else
				{
					code	= code.substr(pos + 2);
				}
			}

		} while (prev != code);
		return	!code.empty();
	}

	// 終端市が不要か終端子があればtrue
	auto read_terminate(bool _require_terminate) -> bool
	{
		if (code.front() == ';')
		{
			++code;
			return	true;
		}
		return	_require_terminate ? false : true;
	}
};


}

PSNVM_LANG_NAMESPACE_BEGIN

auto psn::parse(text _code) -> simplearray<pflnode>
{
	psnparser_core	p(this, _code.trim("\r\n\t "));
	p.documentMode	= mode.document_mode;
	return	p.parse();
}

auto psn::parse_expr(text& _code) -> pflnode
{
	psnparser_core	p(this, _code);
	pflnode		r	= p.parse_expr(true);
	_code	= p.code;
	return	r;
}

PSNVM_LANG_NAMESPACE_END
