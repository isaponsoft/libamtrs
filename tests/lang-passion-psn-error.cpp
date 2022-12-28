/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <sstream>
#include <amtrs/tests.hpp>
#include <amtrs/console/escape.hpp>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/lang/psnvm.hpp>
#include <amtrs/string/convert.hpp>
#include <amtrs/string/join.hpp>
#include <amtrs/string/trnstext.hpp>
using namespace amtrs;
using namespace amtrs::filesystem;
using namespace psnvm;
using namespace psnvm::lang;
using	ec	= errorcode;
#define	_PRFX(_name)	lang_passion_psn_##_name


template<size_t N>
static size_t errsize(errorcode (&e)[N]) { return N; }
#define	TEST_PARSER_ERROR()	{\
			psn		p;\
			auto	codes	= p.parse(src);\
			int		c	= 0;\
			for (auto& e : p.errors)\
			{\
				AMTRS_TEST_EQ(error_string(e.err), c < errsize(err) ? error_string(err[c++]) : (text)"---");\
			}\
			while (c < errsize(err))\
			{\
				AMTRS_TEST_EQ((text)"---", error_string(err[c++]));\
			}\
		}


namespace
{
	sstring normalize(text s);
	sstring parse_test(text src, bool _docmode = false);
}


AMTRS_TEST_F(_PRFX(0_error))
{
	sstring	rootpath	= normalize_path<sstring>((sstring)parent_path(parent_path(__FILE__)) + "/share/psnvm/translate/");
	trnstext::set_from_file(rootpath, "psnvm.txt");

	{
		auto	src		= R"(
			class	X
			{
			public:
				X()
				{}
			};

			X*	x	= new X[4];

		)";

		validator	v;
		v.add("operator new", "void*", {"size_t"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
		{
			size_t	s	= _argv[0];
			void*	p	= operator new (s);
			vmachine::current()->add_memory(p, s);
			return	reinterpret_cast<intmax_t>(p);
		});
		v.add("operator delete", "void", {"void*"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
		{
			void*	p	= reinterpret_cast<void*>(_argv[0]);
			if (vmachine::current()->remove_memory(p))
			{
				operator delete (p);
			}
			return	0;
		});
		v.validate(src);
		for (auto& e : v.errors)
		{
			auto	loc	= compute_location(src, e.location.data());
			if (loc.filename.empty())	loc.filename	= "src";
			auto	m	= _TT(error_string(e.err));
			auto	fm	= format(m.data(), {(text)e.token1, (text)e.token2});
			std::cerr << console::fgcolor_light_red << loc.filename << "(" << loc.line << ") : E" << PSNVM_ERRORCODE(e.err) << " : " << (text)c8_to_cr(fm) << console::fgcolor_reset << std::endl;
			for (auto& amb : e.ambitiouses)
			{
				auto	loc	= compute_location(src, amb.location.data());
				if (loc.filename.empty())	loc.filename	= "src";
				std::cerr << console::fgcolor_light_magenta << loc.filename << "(" << loc.line << ") : " << (text)amb.token << console::fgcolor_reset << std::endl;
			}
		}
	}
}


AMTRS_TEST_F(_PRFX(1_error))
{
	// error
	{
		auto	src		= R"(	`aaaa`		 )";
		ec		err[]	= { ec::ec_unkown_token };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	foo::				)";
		ec		err[]	= { ec::ec_nothing_scope_right };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	foo<F>::				)";
		ec		err[]	= { ec::ec_nothing_scope_right };
		TEST_PARSER_ERROR();
	}
	// class error
	{
		auto	src	= R"(	class				)";
		ec		err[]	= { ec::ec_class_classname_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	class T				)";
		ec		err[]	= { ec::ec_class_terminate_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	class foo::			)";
		ec		err[]	= { ec::ec_nothing_scope_right, ec::ec_class_classname_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	class foo<bar>::	)";
		ec		err[]	= { ec::ec_nothing_scope_right, ec::ec_class_classname_nothing };
		TEST_PARSER_ERROR();
	}
	// class extends error
	{
		auto	src	= R"(	class T :								)";
		ec		err[]	= { ec::ec_class_baseclass_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	class T : base1,						)";
		ec		err[]	= { ec::ec_class_baseclass_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	class T : public base1,					)";
		ec		err[]	= { ec::ec_class_baseclass_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	class T : public virtual base1,			)";
		ec		err[]	= { ec::ec_class_baseclass_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	class T : virtual public base1,			)";
		ec		err[]	= { ec::ec_class_baseclass_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	class T : virtual public base1,	base2,	)";
		ec		err[]	= { ec::ec_class_baseclass_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum			)";
		ec		err[]	= { ec::ec_enum_typename_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum;			)";
		ec		err[]	= { ec::ec_enum_typename_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum class		)";
		ec		err[]	= { ec::ec_enum_typename_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum class;		)";
		ec		err[]	= { ec::ec_enum_typename_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct		)";
		ec		err[]	= { ec::ec_enum_typename_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct;	)";
		ec		err[]	= { ec::ec_enum_typename_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct	T { = 1 };	)";
		ec		err[]	= { ec::ec_enum_varname_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct	T { name = };	)";
		ec		err[]	= { ec::ec_unkown_token, ec::ec_enum_value_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct	T { name =, };	)";
		ec		err[]	= { ec::ec_unkown_token, ec::ec_enum_value_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct	T { n = 1, };	)";
		ec		err[]	= { ec::ec_enum_varname_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct	T { n = 1 v = 2 };	)";
		ec		err[]	= { ec::ec_enum_unkown_token };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct	T :	)";
		ec		err[]	= { ec::ec_enum_base_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct	T :;	)";
		ec		err[]	= { ec::ec_enum_base_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	enum struct	T : { };	)";
		ec		err[]	= { ec::ec_enum_base_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using		 	)";
		ec		err[]	= { ec::ec_using_aliasname_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using;		 	)";
		ec		err[]	= { ec::ec_using_aliasname_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using name	 	)";
		ec		err[]	= { ec::ec_using_alias_separator_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using name;	 	)";
		ec		err[]	= { ec::ec_using_alias_separator_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using name =	 	)";
		ec		err[]	= { ec::ec_using_alias_typeid_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using name =;	 	)";
		ec		err[]	= { ec::ec_using_alias_typeid_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using name = abc 	)";
		ec		err[]	= { ec::ec_using_terminate_nothing };
		TEST_PARSER_ERROR();
	}
//AMTRS_TEST_F(_PRFX(9_syntax_error)){
	{
		auto	src		= R"(	int a =		)";
		ec		err[]	= { ec::ec_nothing_token, ec::ec_declare_variable_initexpr_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	int a,	 	)";
		ec		err[]	= { ec::ec_nothing_token, ec::ec_declare_varname_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	int a =, b	)";
		ec		err[]	= { ec::ec_unkown_token, ec::ec_declare_variable_initexpr_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	int a()	 	)";
		ec		err[]	= { ec::ec_declare_functin_terminate_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	int a() : x() {}	 		)";
		ec		err[]	= { ec::ec_declare_function_have_intializers };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class X { int f() : x() {} };	 		)";
		ec		err[]	= { ec::ec_declare_function_have_intializers };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class X { f() : {} };	 		)";
		ec		err[]	= { ec::ec_declare_constractor_intializers_varname_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class X { f() : a		 		)";
		ec		err[]	= { ec::ec_declare_constractor_intializers_value_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class X { f() : a( 		 		)";
		ec		err[]	= { ec::ec_declare_constractor_intializers_value_unclosed };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class X { f() : a();		 		)";
		ec		err[]	= { ec::ec_declare_constractor_codeblock_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	int f() -> int;		 		)";
		ec		err[]	= { ec::ec_declare_function_after_returntype_not_auto };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	auto f() -> ;		 		)";
		ec		err[]	= { ec::ec_declare_function_after_returntype_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	auto f() {			 		)";
		ec		err[]	= { ec::ec_codeblock_unclosed, ec::ec_declare_functin_codeblock_unclosed };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class T { template f; }		 		)";
		ec		err[]	= { ec::ec_template_has_not_params };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class T { template< f; }		 		)";
		ec		err[]	= { ec::ec_declare_template_paramlist_error };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class T { template<> }		 		)";
		ec		err[]	= { ec::ec_declare_template_unkown_statement_error };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	class T { template<`aaaa`> }		 )";
		ec		err[]	= { ec::ec_declare_template_paramlist_error };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using namespace		 )";
		ec		err[]	= { ec::ec_using_namespace_scope_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src		= R"(	using namespace foo<bar>::zoo	 )";
		ec		err[]	= { ec::ec_using_namespace_terminate_nothing };
		TEST_PARSER_ERROR();
	}
	{
		auto	src	= R"(	template<`typename`> class Cls	)";
		ec		err[]	= { ec::ec_declare_template_paramlist_error };
		TEST_PARSER_ERROR();
	}
}

namespace
{


sstring _normalize(text s)
{
	return	sstring(s.trim("\r\n\t "))
			.replace("\r", " ")
			.replace("\n", " ")
			.replace("\t", " ")
			.replace("  ", " ")
			.replace(") )", "))")
	;
}

sstring normalize(text _s)
{
	sstring	s(_s);
	sstring	r;
	for (;;)
	{
		r	= _normalize(s);
		if (r == s)
		{
			return	r;
		}
		s	= r;
	}
}


sstring parse_test(text _src, bool _docmode)
{
	sstring				src(_src.trim(" \t\r\n"));
	std::stringstream	s;
	text		code	= src;
	psn		p;
	p.mode.document_mode	= _docmode;
	auto		codes	= p.parse(code);
	int	count	= 0;
	for (auto& n : codes)
	{
		if (count++ > 0) { s << " "; }
		pfl{false}.serialize(n, [&](text t)
		{
			s << t;
		});
	}
	return	normalize(s.str());
}

}
