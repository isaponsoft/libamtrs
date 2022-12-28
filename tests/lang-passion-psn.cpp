/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <sstream>
#include <amtrs/tests.hpp>
#include <amtrs/lang/psnvm.hpp>
#include <amtrs/string/join.hpp>
using namespace amtrs;
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

AMTRS_TEST_F(_PRFX(0))
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
	{
		auto	src	= R"(	a.b.m();			)";
		auto	dst	= R"(	(callthis (ope `.` `a` `b`) `m` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a->b->m();			)";
		auto	dst	= R"(	(callthisarrow (ope `->` `a` `b`) `m` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r<R> f(p1<P> a1 = 1, p2 a2 = foo<bar>, p3);				)";
		auto	dst	= R"(	(func `f` (symbol template `r` `R`) (parameter_list ((symbol template `p1` `P`) (name `a1`) (default (dec 1))) (`p2` (name `a2`) (default (symbol template `foo` `bar`))) (`p3`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r<R> f(p1<P> a1 = 1, p2 a2 = foo<bar>, p3);				)";
		auto	dst	= R"(	(func `f` (symbol template `r` `R`) (parameter_list ((symbol template `p1` `P`) (name `a1`) (default (dec 1))) (`p2` (name `a2`) (default (symbol template `foo` `bar`))) (`p3`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c< (a<2)>::m(1 + 2) > 3;	)";
		auto	dst	= R"(	(return (ope `>` (call (ns (symbol template `c` (ope `<` `a` (dec 2))) `m`) [(ope `+` (dec 1) (dec 2))]) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	::a;	)";
		auto	dst	= R"(	(nsabs `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c<a> > 3;	)";
		auto	dst	= R"(	(return (ope `>` (symbol template `c` `a`) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c< (a<2)>::m  (1 + 2) > 3;	)";
		auto	dst	= R"(	(return (ope `>` (call (ns (symbol template `c` (ope `<` `a` (dec 2))) `m`) [(ope `+` (dec 1) (dec 2))]) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new foo<L<R>::bar<A>B>();			)";
		auto	dst	= R"(	(new (ns (symbol template `foo` (ope `<` `L` `R`)) (symbol template `bar` (ope `>` `A` `B`))) (arguments))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
}





#if	0
AMTRS_TEST_F(_PRFX(0))
{
}

AMTRS_TEST_F(_PRFX(0_success_test_example))
{
	// success test example
	{
		auto	src	= R"(	int	c{.a = 1, .b = 2};	)";
		auto	dst	= R"(	 (var `int` [(`c` (initializer_list (`a` (dec 1)) (`b` (dec 2))))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
}


AMTRS_TEST_F(_PRFX(0_errormsg_test_example))
{
	// error test example
	{
		auto	src		= R"(	int a =		)";
		ec		err[]	= { ec::ec_nothing_token, ec::ec_declare_variable_initexpr_nothing };
		TEST_PARSER_ERROR();
	}
}
#endif



// token node parse test.
// not test typeinfo.
AMTRS_TEST_F(_PRFX(parse_and_serialize))
{
	{
		auto	src	= R"(
			// 9 8 7 6 5 4 3 2 1
			1
			// 9 8 7 6 5 4 3 2 1
			;
		)";
		auto	dst	= R"(	(dec 1)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			/* 9 8 7 6 5 4 3 2 1 */
			1;
			/* 9 8 7 6 5 4 3 2 1
		)";
		auto	dst	= R"(	(dec 1)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	/*1*/2;	)";
		auto	dst	= R"(	(dec 2)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	1/*2*/;	)";
		auto	dst	= R"(	(dec 1)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	1;//hello	)";
		auto	dst	= R"(	(dec 1)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}


//AMTRS_TEST_F(_PRFX(1_literal)) {
	{
		auto	src	= R"(	1;		)";
		auto	dst	= R"(	(dec 1)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	'a';	)";
		auto	dst	= R"(	'a'		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	true;		)";
		auto	dst	= R"(	(true)		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	true;		)";
		auto	dst	= R"(	(true)		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	'\n';		)";
		auto	dst	= R"(	'\n'		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// string
	{
		auto	src	= R"(	"Hello";	)";
		auto	dst	= R"(	"Hello"		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	<<<EOT
hello world
EOT;	)";
		auto	dst	= R"(	"hello world"	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	<<<-EOT
								hello world
							EOT;	)";
		auto	dst	= R"(	"hello world"	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	R"aaa(Hello world)aaa";	)";
		auto	dst	= R"(	"Hello world"	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_address)){
	{
		auto	src	= R"(	&a;		)";
		auto	dst	= R"(	(prefix `&` `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	*a;		)";
		auto	dst	= R"(	(prefix `*` `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_assign)){
	{
		auto	src	= R"(	a += 2;		)";
		auto	dst	= R"(	(ope `+=` `a` (dec 2))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_bitwise)) {
	{
		auto	src	= R"(	z = a & 2;			)";
		auto	dst	= R"(	(ope `=` `z` (ope `&` `a` (dec 2)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	z = a | 2;		)";
		auto	dst	= R"(	(ope `=` `z` (ope `|` `a` (dec 2)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	z = a ^ 2;		)";
		auto	dst	= R"(	(ope `=` `z` (ope `^` `a` (dec 2)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_call)){
	{
		auto	src	= R"(	decltype(x)::bar(1, 2);	)";
		auto	dst	= R"(	(call (ns (decltype `x`) `bar`) [(dec 1) (dec 2)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a.data() + a.size();			)";
		auto	dst	= R"(	(ope `+` (callthis `a` `data` []) (callthis `a` `size` []))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// operator function
	{
		auto	src	= R"(	a = operator ++ ();							)";
		auto	dst	= R"(	(ope `=` `a` (call (symbol operator `++`) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	operator ++ ();										)";
		auto	dst	= R"(	(call (symbol operator `++`) [])					)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_increments)){
	{
		auto	src	= R"(	a++;		)";
		auto	dst	= R"(	(suffix `++` `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a--;	)";
		auto	dst	= R"(	(suffix `--` `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	++a;		)";
		auto	dst	= R"(	(prefix `++` `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	--a;	)";
		auto	dst	= R"(	(prefix `--` `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_initializer_list)){
	{
		auto	src	= R"(	a = {1, 2, 3, 4, 5};	)";
		auto	dst	= R"(	(ope `=` `a` (initializer_list ((dec 1)) ((dec 2)) ((dec 3)) ((dec 4)) ((dec 5))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a = { .x = 1, .y = 2};	)";
		auto	dst	= R"(	(ope `=` `a` (initializer_list (`x` (dec 1)) (`y` (dec 2))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	st{ .x = 1, .y = 2};	)";
		auto	dst	= R"(	(struct_initializer `st` (initializer_list (`x` (dec 1)) (`y` (dec 2))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	st{ .x = 1, .y = 2}(99);	)";
		auto	dst	= R"(	(call (struct_initializer `st` (initializer_list (`x` (dec 1)) (`y` (dec 2)))) [(dec 99)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	c{.a = 1, .b = 2};	)";
		auto	dst	= R"(	(struct_initializer `c` (initializer_list (`a` (dec 1)) (`b` (dec 2))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int	c{.a = 1, .b = 2};	)";
		auto	dst	= R"(	 (var `int` [(`c` (initializer_list (`a` (dec 1)) (`b` (dec 2))))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_lambda)){
	{
		auto	src	= R"(	[](){ return 1; };		)";
		auto	dst	= R"(	(lambda (parameter_list) [ (return (dec 1))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	[](auto a, auto b){ return 1; };		)";
		auto	dst	= R"(	(lambda (parameter_list (`auto` (name `a`)) (`auto` (name `b`))) [ (return (dec 1))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	[](auto a, auto b)->int{ return 1; };		)";
		auto	dst	= R"(	(lambda `int` (parameter_list (`auto` (name `a`)) (`auto` (name `b`))) [ (return (dec 1))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	auto pos = find_if(t, [](auto a, auto b)->int { return 1; });	)";
		auto	dst	= R"(	(var `auto` [(`pos` (call `find_if` [`t` (lambda `int` (parameter_list (`auto` (name `a`)) (`auto` (name `b`))) [ (return (dec 1))])]))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_list)){
	{
		auto	src	= R"(	1, 2;	)";
		auto	dst	= R"(	(list (dec 1) (dec 2))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	1, 2, 3, 4;	)";
		auto	dst	= R"(	(list (dec 1) (dec 2) (dec 3) (dec 4))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	"1", "2";		)";
		auto	dst	= R"(	(list "1" "2")	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	"Hello", "World", 1, 2, 3;		)";
		auto	dst	= R"(	(list "Hello" "World" (dec 1) (dec 2) (dec 3))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	"Hello", "World", (1, 2, 3);		)";
		auto	dst	= R"(	(list "Hello" "World" (list (dec 1) (dec 2) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	("Hello", "World"), (1, 2, 3);		)";
		auto	dst	= R"(	(list (list "Hello" "World") (list (dec 1) (dec 2) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	1, 2, 3, "Hello";	)";
		auto	dst	= R"(	(list (dec 1) (dec 2) (dec 3) "Hello")	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	"1", 2, 3, "Hello";	)";
		auto	dst	= R"(	(list "1" (dec 2) (dec 3) "Hello")	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_member)){
	{
		auto	src	= R"(	a.test();		)";
		auto	dst	= R"(	(callthis `a` `test` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a.~test();		)";
		auto	dst	= R"(	(callthis `a` `~test` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	(a.~test()).~T();		)";
		auto	dst	= R"(	(callthis (callthis `a` `~test` []) `~T` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(2_expr_member_arrow)){
	{
		auto	src	= R"(	a->test();		)";
		auto	dst	= R"(	(callthisarrow `a` `test` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a->test();		)";
		auto	dst	= R"(	(callthisarrow `a` `test` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	(a->*mptr)(1, 2);		)";
		auto	dst	= R"(	(call (ope `->` `a` (prefix `*` `mptr`)) [(dec 1) (dec 2)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a->~dest();		)";
		auto	dst	= R"(	(callthisarrow `a` `~dest` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(2_expr_calculation)){
	{
		auto	src	= R"(	a = 1 + 2 * 3;			)";
		auto	dst	= R"(	(ope `=` `a` (ope `+` (dec 1) (ope `*` (dec 2) (dec 3))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a = (1 + 2) * 3;			)";
		auto	dst	= R"(	(ope `=` `a` (ope `*` (ope `+` (dec 1) (dec 2)) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a = (A < B) * 3;			)";
		auto	dst	= R"(	(ope `=` `a` (ope `*` (ope `<` `A` `B`) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a = (A > B) * 3;			)";
		auto	dst	= R"(	(ope `=` `a` (ope `*` (ope `>` `A` `B`) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	1 + 2 + 3;		)";
		auto	dst	= R"(	(ope `+` (ope `+` (dec 1) (dec 2)) (dec 3))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	1 + 2 * 3;		)";
		auto	dst	= R"(	(ope `+` (dec 1) (ope `*` (dec 2) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	1 + 2 * 3 / 4;		)";
		auto	dst	= R"(	(ope `+` (dec 1) (ope `/` (ope `*` (dec 2) (dec 3)) (dec 4)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	"Hello" + "world";	)";
		auto	dst	= R"(	(ope `+` "Hello" "world")	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(2_expr_conditions)){
	{
		auto	src	= R"(	(1 + 2) ? 99 : 100;	)";
		auto	dst	= R"(	(conditions (ope `+` (dec 1) (dec 2)) (dec 99) (dec 100))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	(1 + 2) ? 99 : (0) ? 100 : (1) ? 200 : 300;	)";
		auto	dst	= R"(	(conditions (ope `+` (dec 1) (dec 2)) (dec 99) (conditions (dec 0) (dec 100) (conditions (dec 1) (dec 200) (dec 300))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(2_expr_new_delete)){
	{
		auto	src	= R"(	new test();					)";
		auto	dst	= R"(	(new `test` (arguments))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new (p) test();								)";
		auto	dst	= R"(	(new `test` (placement `p`) (arguments))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new test("hello");					)";
		auto	dst	= R"(	(new `test` (arguments "hello"))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new test("hello", "world");					)";
		auto	dst	= R"(	(new `test` (arguments "hello" "world"))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new  test  ("hello", "world");				)";
		auto	dst	= R"(	(new `test` (arguments "hello" "world"))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new (p) test(1, 2, 3);												)";
		auto	dst	= R"(	(new `test` (placement `p`) (arguments (dec 1) (dec 2) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new (p) test(1, 2, 3);												)";
		auto	dst	= R"(	(new `test` (placement `p`) (arguments (dec 1) (dec 2) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new(p)test  (1, 2, 3);												)";
		auto	dst	= R"(	(new `test` (placement `p`) (arguments (dec 1) (dec 2) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new foo::bar();						)";
		auto	dst	= R"(	(new (ns `foo` `bar`) (arguments))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	new foo<L<R>::bar<A>B>();			)";
		auto	dst	= R"(	(new (ns (symbol template `foo` (ope `<` `L` `R`)) (symbol template `bar` (ope `>` `A` `B`))) (arguments))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	delete	p;			)";
		auto	dst	= R"(	(delete `p`)		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	delete[]	p;			)";
		auto	dst	= R"(	(delete[] `p`)		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	delete []p;			)";
		auto	dst	= R"(	(delete[] `p`)		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	delete [] new T();					)";
		auto	dst	= R"(	(delete[] (new `T` (arguments)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_expr_subscript)){
	{
		auto	src	= R"(	a[1];					)";
		auto	dst	= R"(	(ope `[]` `a` (dec 1))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_symbol)){
	{
		auto	src	= R"(	a;				)";
		auto	dst	= R"(	`a`				)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a::b;			)";
		auto	dst	= R"(	(ns `a` `b`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	::a;				)";
		auto	dst	= R"(	(nsabs `a`)			)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a::b::c;			)";
		auto	dst	= R"(	(ns `a` `b` `c`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	::a::b::c;					)";
		auto	dst	= R"(	(nsabs (ns `a` `b` `c`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// decltype
	{
		auto	src	= R"(	decltype(1);	)";
		auto	dst	= R"(	(decltype (dec 1))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(1)::a;	)";
		auto	dst	= R"(	(ns (decltype (dec 1)) `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(A + B);				)";
		auto	dst	= R"(	(decltype (ope `+` `A` `B`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(foo<F1, F2>);							)";
		auto	dst	= R"(	(decltype (symbol template `foo` `F1` `F2`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(foo<F1, F2>)::bar;				)";
		auto	dst	= R"(	(ns (decltype (symbol template `foo` `F1` `F2`)) `bar`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(foo<F1, F2>)::bar<B1<B2>;				)";
		auto	dst	= R"(	(ns (decltype (symbol template `foo` `F1` `F2`)) (symbol template `bar` (ope `<` `B1` `B2`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(1 + 2);						)";
		auto	dst	= R"(	(decltype (ope `+` (dec 1) (dec 2)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(1 + 2)::hello;								)";
		auto	dst	= R"(	(ns (decltype (ope `+` (dec 1) (dec 2))) `hello`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// template
	{
		auto	src	= R"(	using namespace A<B>;	)";
		auto	dst	= R"(	(using namespace (symbol template `A` `B`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace A;	)";
		auto	dst	= R"(	(using namespace `A`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace A<B<C>;	)";
		auto	dst	= R"(	(using namespace (symbol template `A` (ope `<` `B` `C`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace A<B>C>;	)";
		auto	dst	= R"(	(using namespace (symbol template `A` (ope `>` `B` `C`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace A<B<C>>;	)";
		auto	dst	= R"(	(using namespace (symbol template `A` (symbol template `B` `C`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<1 < 2>;											)";
		auto	dst	= R"(	(symbol template `foo` (ope `<` (dec 1) (dec 2)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<1 > 2>;											)";
		auto	dst	= R"(	(symbol template `foo` (ope `>` (dec 1) (dec 2)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<a > b>;									)";
		auto	dst	= R"(	(symbol template `foo` (ope `>` `a` `b`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<a<b>>;											)";
		auto	dst	= R"(	(symbol template `foo` (symbol template `a` `b`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// template and scope
	{
		auto	src	= R"(	a<A1>::b::c;	)";
		auto	dst	= R"(	(ns (symbol template `a` `A1`) `b` `c`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a<A1>::b<B1>::c<C1>;	)";
		auto	dst	= R"(	(ns (symbol template `a` `A1`) (symbol template `b` `B1`) (symbol template `c` `C1`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	a<>::b<>::c<>;			)";
		auto	dst	= R"(	(ns (symbol template `a`) (symbol template `b`) (symbol template `c`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<L<R>::bar<A>B>;		)";
		auto	dst	= R"(	(ns (symbol template `foo` (ope `<` `L` `R`)) (symbol template `bar` (ope `>` `A` `B`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	A<B,C<D>;		)";
		auto	dst	= R"(	(symbol template `A` `B` (ope `<` `C` `D`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	A<B,C,D<E>;		)";
		auto	dst	= R"(	(symbol template `A` `B` `C` (ope `<` `D` `E`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	basic_ostream < char, char_traits < char > >;	)";
		auto	dst	= R"(	(symbol template `basic_ostream` `char` (symbol template `char_traits` `char`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	basic_ostream<char, char_traits<char>>;	)";
		auto	dst	= R"(	(symbol template `basic_ostream` `char` (symbol template `char_traits` `char`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	A< 1, B < C, D < E, F, G > >, 3, H>;	)";
		auto	dst	= R"(	(symbol template `A` (dec 1) (symbol template `B` `C` (symbol template `D` `E` `F` `G`)) (dec 3) `H`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	A< 1, B < C, D < E, F, G >>, 3, H>;	)";
		auto	dst	= R"(	(symbol template `A` (dec 1) (symbol template `B` `C` (symbol template `D` `E` `F` `G`)) (dec 3) `H`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	A<1, B<C, D<E, F, G>>, 3, H>;	)";
		auto	dst	= R"(	(symbol template `A` (dec 1) (symbol template `B` `C` (symbol template `D` `E` `F` `G`)) (dec 3) `H`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_class)){
	// class
	{
		auto	src	= R"(	class T;			)";
		auto	dst	= R"(	(class `T`)			)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T {};			)";
		auto	dst	= R"(	(class `T`)			)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class	ns1::ns2::C;			)";
		auto	dst	= R"(	(class (ns `ns1` `ns2` `C`)) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class	n1<1,2>::ns2::C<C1,C2>;		)";
		auto	dst	= R"(	(class (ns (symbol template `n1` (dec 1) (dec 2)) `ns2` (symbol template `C` `C1` `C2`))) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// class extends
	{
		auto	src	= R"(	class T : base1 {};					)";
		auto	dst	= R"(	(class `T` (extends (`base1`)))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : base1, base2 {};					)";
		auto	dst	= R"(	(class `T` (extends (`base1`) (`base2`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : private base1 {};					)";
		auto	dst	= R"(	(class `T` (extends (private `base1`)))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : private virtual base1 {};					)";
		auto	dst	= R"(	(class `T` (extends (virtual private `base1`)))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : public  base1, protected virtual base2 {};					)";
		auto	dst	= R"(	(class `T` (extends (public `base1`) (virtual protected `base2`)))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// class extends template
	{
		auto	src	= R"(	class MyClass : Base1<A<2> {};		)";
		auto	dst	= R"(	(class `MyClass` (extends ((symbol template `Base1` (ope `<` `A` (dec 2))))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class MyClass : public virtual Base1<A<2>, protected Base2<A>2>, private Base3<c, d> {};	)";
		auto	dst	= R"(	(class `MyClass` (extends (virtual public (symbol template `Base1` (ope `<` `A` (dec 2)))) (protected (symbol template `Base2` (ope `>` `A` (dec 2)))) (private (symbol template `Base3` `c` `d`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : public  base1<A<B>, protected virtual base2<C>D> {};					)";
		auto	dst	= R"(	(class `T` (extends (public (symbol template `base1` (ope `<` `A` `B`))) (virtual protected (symbol template `base2` (ope `>` `C` `D`)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : base1<A<B>, base2<C>D> {};					)";
		auto	dst	= R"(	(class `T` (extends ((symbol template `base1` (ope `<` `A` `B`))) ((symbol template `base2` (ope `>` `C` `D`)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : template base1<A<B>, template base2<C>D> {};					)";
		auto	dst	= R"(	(class `T` (extends ((symbol template `base1` (ope `<` `A` `B`))) ((symbol template `base2` (ope `>` `C` `D`)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : base1<A<B>, base2<C>D> {};	)";
		auto	dst	= R"(	(class `T` (extends ((symbol template `base1` (ope `<` `A` `B`))) ((symbol template `base2` (ope `>` `C` `D`)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : base1<A<B> {};	)";
		auto	dst	= R"(	(class `T` (extends ((symbol template `base1` (ope `<` `A` `B`)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : template base1<A<B> {};	)";
		auto	dst	= R"(	(class `T` (extends ((symbol template `base1` (ope `<` `A` `B`)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : template base1<A<B>, template base2<C>D> {};					)";
		auto	dst	= R"(	(class `T` (extends ((symbol template `base1` (ope `<` `A` `B`))) ((symbol template `base2` (ope `>` `C` `D`)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T : base1<A<B>, template base2<C>D> {};					)";
		auto	dst	= R"(	(class `T` (extends ((symbol template `base1` (ope `<` `A` `B`))) ((symbol template `base2` (ope `>` `C` `D`)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class MyClass : public virtual Base1, protected Base2<a, b>, private Base3<c, d> {};		)";
		auto	dst	= R"(	(class `MyClass` (extends (virtual public `Base1`) (protected (symbol template `Base2` `a` `b`)) (private (symbol template `Base3` `c` `d`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// class extends and declare variables
	{
		auto	src	= R"(	class T : public  base1, protected virtual base2 { int a; };								)";
		auto	dst	= R"(	(class `T` (extends (public `base1`) (virtual protected `base2`)) (var `int` [(`a`)]))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// class constractor
	{
		auto	src	= R"(	class T { T(); };										)";
		auto	dst	= R"(	(class `T` (func constractor `T` (parameter_list)))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { T() : a(0), b(1) {};  int a = 3, b = 4; };			)";
		auto	dst	= R"(	(class `T` (func constractor `T` (parameter_list) (initializers (`a` (dec 0)) (`b` (dec 1))) []) (var `int` [(`a` (dec 3)) (`b` (dec 4))]))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { T() : a(0, 1), b(2, 3) {}; };			)";
		auto	dst	= R"(	(class `T` (func constractor `T` (parameter_list) (initializers (`a` (dec 0) (dec 1)) (`b` (dec 2) (dec 3))) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class It> struct has_datasizable { template<class T> static constexpr auto test(T& o) -> decltype(std::declval<It&>() == (o.data() + o.size())); };		)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `It`))) (class `has_datasizable` (template (parameter_list (`class` (name `T`))) (func `test` (spec static constexpr) (decltype (ope `==` (call (ns `std` (symbol template `declval` (type `It` &))) []) (ope `+` (callthis `o` `data` []) (callthis `o` `size` [])))) (parameter_list ((type `T` &) (name `o`)))))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// class destractor
	{
		auto	src	= R"(	class T { ~T() {}; };			)";
		auto	dst	= R"(	(class `T` (func destractor `~T` (parameter_list) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// class conversion
	{
		auto	src	= R"(	class T { operator bool () const noexcept {}; };								)";
		auto	dst	= R"(	(class `T` (func conversion [const noexcept] `bool` (parameter_list) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { operator foo::bar () {}; };									)";
		auto	dst	= R"(	(class `T` (func conversion (ns `foo` `bar`) (parameter_list) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { operator foo<T>::bar () {}; };								)";
		auto	dst	= R"(	(class `T` (func conversion (ns (symbol template `foo` `T`) `bar`) (parameter_list) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { operator foo<T>::bar<U> () {}; };								)";
		auto	dst	= R"(	(class `T` (func conversion (ns (symbol template `foo` `T`) (symbol template `bar` `U`)) (parameter_list) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// class operator
	{
		auto	src	= R"(	class T { T& operator ++ () {}; };					)";
		auto	dst	= R"(	(class `T` (func operator `++` (type `T` &) (parameter_list) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { T& operator ++ (int) const noexcept {}; };	)";
		auto	dst	= R"(	(class `T` (func operator `++` [const noexcept] (type `T` &) (parameter_list (`int`)) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { auto operator ++ (int r) const noexcept -> T& {}; };	)";
		auto	dst	= R"(	(class `T` (func operator `++` [const noexcept] (type `T` &) (parameter_list (`int` (name `r`))) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { foo::bar<int>::T* operator ++ () const noexcept {}; };	)";
		auto	dst	= R"(	(class `T` (func operator `++` [const noexcept] (type (ns `foo` (symbol template `bar` `int`) `T`) *) (parameter_list) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { auto operator ++ () const noexcept -> foo::bar<int>::T* {}; };	)";
		auto	dst	= R"(	(class `T` (func operator `++` [const noexcept] (type (ns `foo` (symbol template `bar` `int`) `T`) *) (parameter_list) []))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// template class
	{
		auto	src	= R"(	template<> class Class;		)";
		auto	dst	= R"(	(template  (parameter_list ) (class `Class`)) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<> class NS::Class;		)";
		auto	dst	= R"(	(template (parameter_list ) (class (ns `NS` `Class`))) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class	NS<A>::Class;		)";
		auto	dst	= R"(	(class (ns (symbol template `NS` `A`) `Class`)) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<> class NS<A>::Class;		)";
		auto	dst	= R"(	(template (parameter_list ) (class (ns (symbol template `NS` `A`) `Class`))) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<A> class Class;							)";
		auto	dst	= R"(	(template (parameter_list (`A`)) (class `Class`)) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class A> class	Class;		)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `A`))) (class `Class`)) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class A=1> class Class;		)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `A`) (default (dec 1)))) (class `Class`)) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<X<A>> class Class;		)";
		auto	dst	= R"(	(template (parameter_list ((symbol template `X` `A`))) (class `Class`)) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class A=traits<A>> class Class;		)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `A`) (default (symbol template `traits` `A`)))) (class `Class`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<A, B<B1, B2>> class N<N1,2,3,N2>::Class;		)";
		auto	dst	= R"(	(template (parameter_list (`A`)((symbol template `B` `B1` `B2`))) (class (ns (symbol template `N` `N1` (dec 2) (dec 3) `N2`) `Class`))) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<typename = 1-1> class N<N1,2,3,N2>::Class;		)";
		auto	dst	= R"(	(template (parameter_list (`typename` (default (ope `-` (dec 1) (dec 1))))) (class (ns (symbol template `N` `N1` (dec 2) (dec 3) `N2`) `Class`))) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class = 1-1> class N<N1,2,3,N2>::Class;		)";
		auto	dst	= R"(	(template (parameter_list (`class` (default (ope `-` (dec 1) (dec 1))))) (class (ns (symbol template `N` `N1` (dec 2) (dec 3) `N2`) `Class`))) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<A, B<B1, B2>, class = 1-1> class N<N1,2,3,N2>::Class;		)";
		auto	dst	= R"(	(template (parameter_list (`A`)((symbol template `B` `B1` `B2`))(`class` (default (ope `-` (dec 1) (dec 1))))) (class (ns (symbol template `N` `N1` (dec 2) (dec 3) `N2`) `Class`))) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class MyClass { int	a = 1; int b = 1, c = 2; };		)";
		auto	dst	= R"(	(class `MyClass` (var `int` [(`a` (dec 1))]) (var `int` [(`b` (dec 1)) (`c` (dec 2))]))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			class MyClass
			{
				int	a	= 1;			// MyClass.a
				int	b	= 1, c = 2;		// MyClass b, MyClass c
				char m();				// char m();
			};
		)";
		auto	dst	= R"(	(class `MyClass` (var `int` [(`a` (dec 1))]) (var `int` [(`b` (dec 1)) (`c` (dec 2))]) (func `m` `char` (parameter_list)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			// nested class
			class MyClass
			{
				class Child
				{
					int	a	= 1;			// MyClass.a
					int	b	= 1, c = 2;		// MyClass.b and MyClass.c
					char m();				// char m();
				};
			};
		)";
		auto	dst	= R"(	(class `MyClass` (class `Child` (var `int` [(`a` (dec 1))]) (var `int` [(`b` (dec 1)) (`c` (dec 2))]) (func `m` `char` (parameter_list))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			// nested class
			class MyClass
			{
				using	value_type	= foo::bar;
			};
		)";
		auto	dst	= R"(	(class `MyClass` (using alias `value_type` (ns `foo` `bar`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			// nested class
			class MyClass
			{
			public:
			protected:
			private:
			};
		)";
		auto	dst	= R"(	(class `MyClass` (label `public`) (label `protected`) (label `private`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			// nested class
			class MyClass
			{
				template<class X>
				using	type	= X::value_type;
			};
		)";
		auto	dst	= R"(	(class `MyClass` (template (parameter_list (`class` (name `X`))) (using alias `type` (ns `X` `value_type`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			template<class T1, class T2>
			class	MyClass
			{
			public:
				template<class A>
				int f1();

			private:
				template<class A>
				int f2();
			};
		)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `T1`))(`class` (name `T2`))) (class `MyClass` (label `public`) (template (parameter_list (`class` (name `A`))) (func `f1` `int` (parameter_list))) (label `private`) (template (parameter_list (`class` (name `A`))) (func `f2` `int` (parameter_list)))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			template<class T, class Traits = myclass_traits<T>>
			class	MyClass
			{
				int v1	= 11 + 22;

				template<class A, class B>
				int m1()
				{
					return	33;
				}
			};
		)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `T`))(`class` (name `Traits`) (default (symbol template `myclass_traits` `T`)))) (class `MyClass` (var `int` [(`v1` (ope `+` (dec 11) (dec 22)))]) (template (parameter_list (`class` (name `A`))(`class` (name `B`))) (func `m1` `int` (parameter_list) [ (return (dec 33))]))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			class	MyClass
			{
				int m1() { return 99; }
				int m2() { return 12; }
			};
		)";
		auto	dst	= R"(	(class `MyClass` (func `m1` `int` (parameter_list) [ (return (dec 99))]) (func `m2` `int` (parameter_list) [ (return (dec 12))]))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			class	Cls
			{
				Cls();
				Cls(Cls const&);
				Cls(Cls&&);
				~Cls();
			};
		)";

		auto	dst	= R"(	(class `Cls` (func constractor `Cls` (parameter_list)) (func constractor `Cls` (parameter_list ((type (type `Cls` const) &)))) (func constractor `Cls` (parameter_list ((type `Cls` &&)))) (func destractor `~Cls` (parameter_list)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			class	Cls
			{
				Cls()
					: a()
					, b(1)
					, c(2 * 3 + 1)
				{}
			};
		)";


		auto	dst	= R"(	(class `Cls` (func constractor `Cls` (parameter_list) (initializers (`a`) (`b` (dec 1)) (`c` (ope `+` (ope `*` (dec 2) (dec 3)) (dec 1)))) []))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			class	Cls
			{
				template<class T>
				Cls(T i)
					: a(i)
				{}
			};
		)";
		auto	dst	= R"(	(class `Cls` (template (parameter_list (`class` (name `T`))) (func constractor `Cls` (parameter_list (`T` (name `i`))) (initializers (`a` `i`)) [])))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			class	Cls
			{
				template<class T>
				Cls(){}
			};
		)";
		auto	dst	= R"(	(class `Cls` (template (parameter_list (`class` (name `T`))) (func constractor `Cls` (parameter_list) [])))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			class	Cls
			{
				Cls(foo<F>::bar<> a){}
			};
		)";
		auto	dst	= R"(	(class `Cls` (func constractor `Cls` (parameter_list ((ns (symbol template `foo` `F`) (symbol template `bar`)) (name `a`))) []))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(1_template_declare)){
	{
		auto	src	= R"(	template<class = T*> class C;								)";
		auto	dst	= R"(	(template (parameter_list (`class` (default (type `T` *)))) (class `C`))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	xyz*	ary[12]; 									)";
		auto	dst	= R"(	(var (type (type `xyz` *) [] ((dec 12))) [(`ary`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	xyz*	ary[12][]; 										)";
		auto	dst	= R"(	(var (type (type `xyz` *) [] ((dec 12)) ()) [(`ary`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	xyz*	ary[1][2][3]; 													)";
		auto	dst	= R"(	(var (type (type `xyz` *) [] ((dec 1)) ((dec 2)) ((dec 3))) [(`ary`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	xyz*	ary[]	= {1,2,3} 									)";
		auto	dst	= R"(	(var (type (type `xyz` *) [] ()) [(`ary` (initializer_list ((dec 1)) ((dec 2)) ((dec 3))))])		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	xyz*	ary[]	= "hello" 							)";
		auto	dst	= R"(	(var (type (type `xyz` *) [] ()) [(`ary` "hello")])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	xyz*	ary[]	= { 'h', 'e', 'l', 'l', 'o' } 												)";
		auto	dst	= R"(	(var (type (type `xyz` *) [] ()) [(`ary` (initializer_list ('h') ('e') ('l') ('l') ('o')))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	std::hash<key_type>{}(_key);																				)";
		auto	dst	= R"(	(call (struct_initializer (ns `std` (symbol template `hash` `key_type`)) (initializer_list)) [`_key`])		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_enum)){
	{
		auto	src	= R"(	enum class myenum {};	)";
		auto	dst	= R"(	(enum `myenum`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	enum class myenum { v1 };	)";
		auto	dst	= R"(	(enum `myenum` [(`v1`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	enum class myenum { v1 = 1 };	)";
		auto	dst	= R"(	(enum `myenum` [(`v1` (dec 1))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	enum class myenum { v1 = 1, v2 = 2 };	)";
		auto	dst	= R"(	(enum `myenum` [(`v1` (dec 1)) (`v2` (dec 2))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	enum class myenum : base { v1 = 1, v2 = 2 };	)";
		auto	dst	= R"(	(enum `myenum` `base` [(`v1` (dec 1)) (`v2` (dec 2))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_template_class)){
	{
		auto	src	= R"(	template<typename A=1> class Class;		)";
		auto	dst	= R"(	(template (parameter_list (`typename` (name `A`) (default (dec 1)))) (class `Class`)) 	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_namespace)){
	{
		auto	src	= R"(	namespace foo {}						)";
		auto	dst	= R"(	(namespace `foo`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	namespace foo { namespace {} }			)";
		auto	dst	= R"(	(namespace `foo` (namespace))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	namespace foo { namespace {} namespace bar{} }			)";
		auto	dst	= R"(	(namespace `foo` (namespace) (namespace `bar`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	namespace foo { class T; }		)";
		auto	dst	= R"(	(namespace `foo` (class `T`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	namespace ns1::ns2 { }		)";
		auto	dst	= R"(	(namespace (ns `ns1` `ns2`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	namespace ns1::ns2 { namespace ns3 {} }		)";
		auto	dst	= R"(	(namespace (ns `ns1` `ns2`) (namespace `ns3`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	namespace ns1::ns2 { int a;  }		)";
		auto	dst	= R"(	(namespace (ns `ns1` `ns2`) (var `int` [(`a`)]) )	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_return)){
	{
		auto	src	= R"(	{ return 1; }		)";
		auto	dst	= R"(	[ (return (dec 1))]	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f() { return; }	)";
		auto	dst	= R"(	(func `f` `int` (parameter_list) [ (return)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f() { return 1; }	)";
		auto	dst	= R"(	(func `f` `int` (parameter_list) [ (return (dec 1))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return 1;			)";
		auto	dst	= R"(	(return (dec 1))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return;	)";
		auto	dst	= R"(	(return)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c<a < 2>::m(1 + 2);	)";
		auto	dst	= R"(	(return (call (ns (symbol template `c` (ope `<` `a` (dec 2))) `m`) [(ope `+` (dec 1) (dec 2))]))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c<a < 2>::m(1 + 2) < 3;	)";
		auto	dst	= R"(	(return (ope `<` (call (ns (symbol template `c` (ope `<` `a` (dec 2))) `m`) [(ope `+` (dec 1) (dec 2))]) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c<(a<2)>;	)";
		auto	dst	= R"(	(return (symbol template `c` (ope `<` `a` (dec 2))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c<a> > 3;	)";
		auto	dst	= R"(	(return (ope `>` (symbol template `c` `a`) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c<a>::m > 3;	)";
		auto	dst	= R"(	(return (ope `>` (ns (symbol template `c` `a`) `m`) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c< (a<2)>::m(1 + 2) > 3;	)";
		auto	dst	= R"(	(return (ope `>` (call (ns (symbol template `c` (ope `<` `a` (dec 2))) `m`) [(ope `+` (dec 1) (dec 2))]) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	return	c<a < 2>::m(1 + 2) > 3;	)";
		auto	dst	= R"(	(return (ope `>` (call (ns (symbol template `c` (ope `<` `a` (dec 2))) `m`) [(ope `+` (dec 1) (dec 2))]) (dec 3)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_using_alias)){
	{
		auto	src	= R"(	using aaa = foo;					)";
		auto	dst	= R"(	(using alias `aaa` `foo`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using aaa = foo::bar ;					)";
		auto	dst	= R"(	(using alias `aaa` (ns `foo` `bar`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using aaa = foo<F>;						)";
		auto	dst	= R"(	(using alias `aaa` (symbol template `foo` `F`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using aaa = foo<F>::bar<B> ;					)";
		auto	dst	= R"(	(using alias `aaa` (ns (symbol template `foo` `F`) (symbol template `bar` `B`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using aaa = foo<A<B> ;					)";
		auto	dst	= R"(	(using alias `aaa` (symbol template `foo` (ope `<` `A` `B`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using aaa = foo<A<B>::bar<C>D>;					)";
		auto	dst	= R"(	(using alias `aaa` (ns (symbol template `foo` (ope `<` `A` `B`)) (symbol template `bar` (ope `>` `C` `D`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	fpr = void();											)";
		auto	dst	= R"(	(using alias `fpr` (symbol func `void` (parameter_list)))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	fpr = decltype(1 + 2)(decltype(2 + 3));			)";
		auto	dst	= R"(	(using alias `fpr` (symbol func (decltype (ope `+` (dec 1) (dec 2))) (parameter_list ((decltype (ope `+` (dec 2) (dec 3)))))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	fpr = void(int a, int b);			)";
		auto	dst	= R"(	(using alias `fpr` (symbol func `void` (parameter_list (`int` (name `a`)) (`int` (name `b`)))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	int_ptr	= int;					)";
		auto	dst	= R"(	(using alias `int_ptr` `int`)			)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	int_ptr	= int*;					)";
		auto	dst	= R"(	(using alias `int_ptr` (type `int` *))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	int_ptr	= int**;						)";
		auto	dst	= R"(	(using alias `int_ptr` (type (type `int` *) *))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	int_ptr	= int*const*const;											)";
		auto	dst	= R"(	(using alias `int_ptr` (type (type (type (type `int` *) const) *) const))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	int_ptr	= int const*;							)";
		auto	dst	= R"(	(using alias `int_ptr` (type (type `int` const) *))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	int_ptr	= const int*;							)";
		auto	dst	= R"(	(using alias `int_ptr` (type (type `int` const) *))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	ptr	= void()*;												)";
		auto	dst	= R"(	(using alias `ptr` (type (symbol func `void` (parameter_list)) *))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	ptr	= void(*)();												)";
		auto	dst	= R"(	(using alias `ptr` (type (symbol func `void` (parameter_list)) *))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	ptr	= int&;					)";
		auto	dst	= R"(	(using alias `ptr` (type `int` &))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using	ptr	= int&&;					)";
		auto	dst	= R"(	(using alias `ptr` (type `int` &&))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_using_namespace)){
	{
		auto	src	= R"(	using namespace T;		)";
		auto	dst	= R"(	(using namespace `T`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace T;		)";
		auto	dst	= R"(	(using namespace `T`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace A::B::C;				)";
		auto	dst	= R"(	(using namespace (ns `A` `B` `C`))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// This is not supported syntax. But, psn is enable.
	{
		auto	src	= R"(	using namespace A<B>;	)";
		auto	dst	= R"(	(using namespace (symbol template `A` `B`))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace A<B>C>;	)";
		auto	dst	= R"(	(using namespace (symbol template `A` (ope `>` `B` `C`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace A<B<C>>;	)";
		auto	dst	= R"(	(using namespace (symbol template `A` (symbol template `B` `C`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace foo<F>::bar<B> ;					)";
		auto	dst	= R"(	(using namespace (ns (symbol template `foo` `F`) (symbol template `bar` `B`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	using namespace foo<A<B>::bar<C>D>;					)";
		auto	dst	= R"(	(using namespace (ns (symbol template `foo` (ope `<` `A` `B`)) (symbol template `bar` (ope `>` `C` `D`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_concept)){
	{
		auto	src	= R"(	template<class T> concept myconcept = sizeof(T) == sizeof(int);						)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `T`))) (concept `myconcept` (ope `==` (sizeof `T`) (sizeof `int`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stm_concept_requires)){
	{
		auto	src	= R"(	template<class T> concept	myconcept = requires { ++a;  };					)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `T`))) (concept `myconcept` (requires [(expr (prefix `++` `a`))])))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class T> concept	myconcept = requires {{++a;}->int;{--a;}->int;};					)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `T`))) (concept `myconcept` (requires [(match `int` (prefix `++` `a`)) (match `int` (prefix `--` `a`))])))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class T> concept	myconcept = requires (T a) { ++a;  };					)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `T`))) (concept `myconcept` (requires (parameter_list (`T` (name `a`)))[(expr (prefix `++` `a`))])))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class T> concept	myconcept = requires (T a) { { ++a; } -> int;  };					)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `T`))) (concept `myconcept` (requires (parameter_list (`T` (name `a`)))[(match `int` (prefix `++` `a`))])))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class T> concept	myconcept = requires (T a) { { ++a; } -> int; { --a; } -> int; };					)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `T`))) (concept `myconcept` (requires (parameter_list (`T` (name `a`)))[(match `int` (prefix `++` `a`)) (match `int` (prefix `--` `a`))])))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_strm_do_while)){
	{
		auto	src	= R"(	do { } while(a);	)";
		auto	dst	= R"(	(do_while `a` [])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	do { return 1; } while(a);	)";
		auto	dst	= R"(	(do_while `a` [ (return (dec 1))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//ATRS_TEST_F(_PRFX(3_strm_for)){
	{
		auto	src	= R"(	for(;;);	)";
		auto	dst	= R"(	(for )	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	for(;;){}		)";
		auto	dst	= R"(	(for [])		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	for( a ; b ; c );	)";
		auto	dst	= R"(	(for `a` `b` `c`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	for( a ; b ; c ) { return 1; }	)";
		auto	dst	= R"(	(for `a` `b` `c` [ (return (dec 1))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_strm_for_rangedbase)){
	{
		auto	src	= R"(	for (auto& i : v) {}	)";
		auto	dst	= R"(	(foreach (var (type `auto` &) [(`i`)]) `v` [])			)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_friend)){
	{
		auto	src	= R"(	friend class F; };				)";
		auto	dst	= R"(	(friend (class `F`))				)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class C { template<T> friend class F; };				)";
		auto	dst	= R"(	(class `C` (template (parameter_list (`T`)) (friend (class `F`))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_strm_if)){
	{
		auto	src	= R"(	if (a);	)";
		auto	dst	= R"(	(if `a`)	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	if (a) { return 1; } else { return 2; }	)";
		auto	dst	= R"(	(if `a` [ (return (dec 1))] [ (return (dec 2))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	if (a) { return 1; } else if (b) { return 2; } else {}	)";
		auto	dst	= R"(	(if `a` [ (return (dec 1))] (if `b` [ (return (dec 2))] []))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_strm_switch)){
	{
		auto	src	= R"(	switch (a) { case 1: case 2: }	)";
		auto	dst	= R"(	(switch `a` ((dec 1) []) ((dec 2) []))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			switch (a)
			{
				default:
					break;
				case 1:
					return	9;
				case 2:
					return	10;
			}
		)";
		auto	dst	= R"(	(switch `a` (default (break)) ((dec 1) (return (dec 9))) ((dec 2) (return (dec 10))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(3_stmt_while)){
	{
		auto	src	= R"(	while(a);		)";
		auto	dst	= R"(	(while `a` )	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	while(a) { continue; }	)";
		auto	dst	= R"(	(while `a` [ (continue)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(4_decl_func)){
	{
		auto	src	= R"(	int f();					)";
		auto	dst	= R"(	(func `f` `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	auto f() -> int;			)";
		auto	dst	= R"(	(func `f` `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	auto f() -> int*;			)";
		auto	dst	= R"(	(func `f` (type `int` *) (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r f();							)";
		auto	dst	= R"(	(func `f` `r` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r f(p1);				)";
		auto	dst	= R"(	(func `f` `r` (parameter_list (`p1`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f(int (*) = nullptr);	)";
		auto	dst	= R"(	(func `f` `int` (parameter_list ((type `int` *) (default (nullptr)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f(int (*f) = nullptr);	)";
		auto	dst	= R"(	(func `f` `int` (parameter_list ((type `int` *) (name `f`) (default (nullptr)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f(int (*_fp)() = nullptr);	)";
		auto	dst	= R"(	(func `f` `int` (parameter_list ((type (symbol func `int` (parameter_list)) *) (name `_fp`) (default (nullptr)))))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f(int (***a));	)";
		auto	dst	= R"(	(func `f` `int` (parameter_list ((type (type (type `int` *) *) *) (name `a`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f(int (*_fp)(int a, int, short c) = nullptr);	)";
		auto	dst	= R"(	(func `f` `int` (parameter_list ((type (symbol func `int` (parameter_list (`int` (name `a`)) (`int`) (`short` (name `c`)))) *) (name `_fp`) (default (nullptr)))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f(int (*_fp)[N] = nullptr);	)";
		auto	dst	= R"(	(func `f` `int` (parameter_list ((type (type `int` [] (`N`)) *) (name `_fp`) (default (nullptr)))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r f(p1, p2);				)";
		auto	dst	= R"(	(func `f` `r` (parameter_list (`p1`) (`p2`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r f(p1, p2, p3);				)";
		auto	dst	= R"(	(func `f` `r` (parameter_list (`p1`) (`p2`) (`p3`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r f(p1 a1);				)";
		auto	dst	= R"(	(func `f` `r` (parameter_list (`p1` (name `a1`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	void f(int* a); 												)";
		auto	dst	= R"(	(func `f` `void` (parameter_list ((type `int` *) (name `a`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	void f(int*& a); 														)";
		auto	dst	= R"(	(func `f` `void` (parameter_list ((type (type `int` *) &) (name `a`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r f(p1 a1, p2 a2, p3 a3);				)";
		auto	dst	= R"(	(func `f` `r` (parameter_list (`p1` (name `a1`)) (`p2` (name `a2`)) (`p3` (name `a3`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r f(p1 a1 = 1);				)";
		auto	dst	= R"(	(func `f` `r` (parameter_list (`p1` (name `a1`) (default (dec 1)))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r f(p1 a1 = 1, p2 a2 = foo<bar>, p3);				)";
		auto	dst	= R"(	(func `f` `r` (parameter_list (`p1` (name `a1`) (default (dec 1))) (`p2` (name `a2`) (default (symbol template `foo` `bar`))) (`p3`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	r<R> f(p1<P> a1 = 1, p2 a2 = foo<bar>, p3);				)";
		auto	dst	= R"(	(func `f` (symbol template `r` `R`) (parameter_list ((symbol template `p1` `P`) (name `a1`) (default (dec 1))) (`p2` (name `a2`) (default (symbol template `foo` `bar`))) (`p3`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F1,F2>::bar<B1,B2>	f();					)";
		auto	dst	= R"(	 (func `f` (ns (symbol template `foo` `F1` `F2`) (symbol template `bar` `B1` `B2`)) (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F1,F2>::bar<B1,B2>	f(p1, p2, p3);				)";
		auto	dst	= R"(	 (func `f` (ns (symbol template `foo` `F1` `F2`) (symbol template `bar` `B1` `B2`)) (parameter_list (`p1`) (`p2`) (`p3`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F1,F2>::bar<B1,B2>	ns::ns::f(p1, p2, p3);				)";
		auto	dst	= R"(	 (func (ns `ns` `ns` `f`) (ns (symbol template `foo` `F1` `F2`) (symbol template `bar` `B1` `B2`)) (parameter_list (`p1`) (`p2`) (`p3`)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F1,F2>::bar<B1,B2>	ns::ns::f(p1 a1, p2, p3 a3);				)";
		auto	dst	= R"(	 (func (ns `ns` `ns` `f`) (ns (symbol template `foo` `F1` `F2`) (symbol template `bar` `B1` `B2`)) (parameter_list (`p1` (name `a1`)) (`p2`) (`p3` (name `a3`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F1,F2>::bar<B1,B2>	ns::ns::f(p1<P1, P2> a1, p2, p3 a3);				)";
		auto	dst	= R"(	 (func (ns `ns` `ns` `f`) (ns (symbol template `foo` `F1` `F2`) (symbol template `bar` `B1` `B2`)) (parameter_list ((symbol template `p1` `P1` `P2`) (name `a1`)) (`p2`) (`p3` (name `a3`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F1,F2>::bar<B1,C<C1>,B2>	ns::ns::f(p1<P1, P2> a1, p2, p3 a3);			)";
		auto	dst	= R"(	(func (ns `ns` `ns` `f`) (ns (symbol template `foo` `F1` `F2`) (symbol template `bar` `B1` (symbol template `C` `C1`) `B2`)) (parameter_list ((symbol template `p1` `P1` `P2`) (name `a1`)) (`p2`) (`p3` (name `a3`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F1,F2,X<>>::bar<B1,C<C1>,B2>	ns::ns::f(p1<P1, P2> a1, p2, p3 a3);				)";
		auto	dst	= R"(	(func (ns `ns` `ns` `f`) (ns (symbol template `foo` `F1` `F2` (symbol template `X`)) (symbol template `bar` `B1` (symbol template `C` `C1`) `B2`)) (parameter_list ((symbol template `p1` `P1` `P2`) (name `a1`)) (`p2`) (`p3` (name `a3`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(1 + 2)::hello f();															)";
		auto	dst	= R"(	(func `f` (ns (decltype (ope `+` (dec 1) (dec 2))) `hello`) (parameter_list))		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class CharT, class Traits = char_traits<CharT>> void func() { return 1231; }	)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `CharT`))(`class` (name `Traits`) (default (symbol template `char_traits` `CharT`)))) (func `func` `void` (parameter_list) [ (return (dec 1231))]))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	inline int f();		)";
		auto	dst	= R"(	(func `f` (spec inline) `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	inline static virtual volatile int f();		)";
		auto	dst	= R"(	(func `f` (spec static virtual inline volatile) `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int f(int&&... argv);		)";
		auto	dst	= R"(	(func `f` `int` (parameter_list ((type (type `int` &&)...) (name `argv`))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	decltype(1) func(int a, Z<X,decltype("hello")> b = 1 + 2);	)";
		auto	dst	= R"(	(func `func` (decltype (dec 1)) (parameter_list (`int` (name `a`)) ((symbol template `Z` `X` (decltype "hello")) (name `b`) (default (ope `+` (dec 1) (dec 2))))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class A> template<class B, size_t N> int s<A>::f(int (*_fp)[N] = nullptr);	)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `A`))) (template (parameter_list (`class` (name `B`))(`size_t` (name `N`))) (func (ns (symbol template `s` `A`) `f`) `int` (parameter_list ((type (type `int` [] (`N`)) *) (name `_fp`) (default (nullptr)))))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	template<class A> template<class B, size_t N> int s<A>::f(int (*_fp)[N][1][][x] = nullptr);	)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `A`))) (template (parameter_list (`class` (name `B`))(`size_t` (name `N`))) (func (ns (symbol template `s` `A`) `f`) `int` (parameter_list ((type (type `int` [] (`N`) ((dec 1)) () (`x`)) *) (name `_fp`) (default (nullptr)))))))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			// return type is int
			decltype(1)
				// function name is func
				func(
					int						a,			/* parameter 1 */
					Z<X,decltype("hello")>	b = 1 + 2	// parameter 2 : default value is 3
				)
				{
					return 7 + 9;
				}
		)";
		auto	dst	= R"(	(func `func` (decltype (dec 1)) (parameter_list (`int` (name `a`)) ((symbol template `Z` `X` (decltype "hello")) (name `b`) (default (ope `+` (dec 1) (dec 2))))) [ (return (ope `+` (dec 7) (dec 9)))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			template<class CharT, class Traits = char_traits<CharT>>
			void func()
			{
				return	1231;
			}
		)";
		auto	dst	= R"(	(template (parameter_list (`class` (name `CharT`))(`class` (name `Traits`) (default (symbol template `char_traits` `CharT`)))) (func `func` `void` (parameter_list) [ (return (dec 1231))]))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			auto f() -> int
			{
				return	m1(m2(1 + 2) + 3);
			}
		)";
		auto	dst	= R"(	(func `f` `int` (parameter_list) [ (return (call `m1` [(ope `+` (call `m2` [(ope `+` (dec 1) (dec 2))]) (dec 3))]))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(
			auto f() -> int
			{
				return	ns::c<a < 2>::m(1 + 2) + 3;
			}
		)";
		auto	dst	= R"(	(func `f` `int` (parameter_list) [ (return (ope `+` (call (ns `ns` (symbol template `c` (ope `<` `a` (dec 2))) `m`) [(ope `+` (dec 1) (dec 2))]) (dec 3)))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(4_decl_conversion)){
	{
		auto	src	= R"(	class T { operator bool (); };		)";
		auto	dst	= R"(	(class `T` (func conversion `bool` (parameter_list)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class T { operator foo<F>::bar<B> const* (); };		)";
		auto	dst	= R"(	(class `T` (func conversion (type (type (ns (symbol template `foo` `F`) (symbol template `bar` `B`)) const) *) (parameter_list)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// convert to function ptr.
	{
		auto	src	= R"(	class T { operator bool()* (); };		)";
		auto	dst	= R"(	(class `T` (func conversion (type (symbol func `bool` (parameter_list)) *) (parameter_list)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	// convert ro c/c++ style function ptr.
	{
		auto	src	= R"(	class T { operator bool(*)() (); };		)";
		auto	dst	= R"(	(class `T` (func conversion (type (symbol func `bool` (parameter_list)) *) (parameter_list)))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(4_decl_operator)){
	{
		auto	src	= R"(	int operator ++ ();							)";
		auto	dst	= R"(	(func operator `++` `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	auto operator ++ ();						)";
		auto	dst	= R"(	(func operator `++` `auto` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	auto operator ++ () -> int;		)";
		auto	dst	= R"(	(func operator `++` `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int operator ++ () const noexcept;							)";
		auto	dst	= R"(	(func operator `++` [const noexcept] `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class C { int operator ++ () const noexcept; }; C c; c(1);		)";
		auto	dst	= R"(	(class `C` (func operator `++` [const noexcept] `int` (parameter_list))) (var `C` [(`c`)]) (call `c` [(dec 1)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	class C { int operator () (int a) { return a; } }; C c; c(1);		)";
		auto	dst	= R"(	 (class `C` (func operator `()` `int` (parameter_list (`int` (name `a`))) [ (return `a`)])) (var `C` [(`c`)]) (call `c` [(dec 1)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int operator () () const noexcept;							)";
		auto	dst	= R"(	(func operator `()` [const noexcept] `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int operator [] () const noexcept;							)";
		auto	dst	= R"(	(func operator `[]` [const noexcept] `int` (parameter_list))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(4_decl_variable)){
	{
		auto	src	= R"(	A<> value;									)";
		auto	dst	= R"(	(var (symbol template `A`) [(`value`)])		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	A<B> value;									)";
		auto	dst	= R"(	(var (symbol template `A` `B`) [(`value`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	A<B<>> value;													)";
		auto	dst	= R"(	(var (symbol template `A` (symbol template `B`)) [(`value`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<1>	a = val<1>2>;					)";
		auto	dst	= R"(	(var (symbol template `foo` (dec 1)) [(`a` (symbol template `val` (ope `>` (dec 1) (dec 2))))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<L>	a = val<1>2>;					)";
		auto	dst	= R"(	(var (symbol template `foo` `L`) [(`a` (symbol template `val` (ope `>` (dec 1) (dec 2))))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<L>	a = val<A>2>, x = 991;					)";
		auto	dst	= R"(	(var (symbol template `foo` `L`) [(`a` (symbol template `val` (ope `>` `A` (dec 2)))) (`x` (dec 991))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int	a = 2;						)";
		auto	dst	= R"(	(var `int` [(`a` (dec 2))])		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int	a = 1 + 2;									)";
		auto	dst	= R"(	(var `int` [(`a` (ope `+` (dec 1) (dec 2)))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int	a, b;					)";
		auto	dst	= R"(	(var `int` [(`a`) (`b`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int	a = 1+2, b = 9;											)";
		auto	dst	= R"(	(var `int` [(`a` (ope `+` (dec 1) (dec 2))) (`b` (dec 9))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<a<b>> value						)";
		auto	dst	= R"(	(var (symbol template `foo` (symbol template `a` `b`)) [(`value`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo::bar	v1, v2							)";
		auto	dst	= R"(	(var (ns `foo` `bar`) [(`v1`) (`v2`)])		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo::bar::hoge::huga	v1, v2								)";
		auto	dst	= R"(	(var (ns `foo` `bar` `hoge` `huga`) [(`v1`) (`v2`)])		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo::bar<B>	v1, v2													)";
		auto	dst	= R"(	(var (ns `foo` (symbol template `bar` `B`)) [(`v1`) (`v2`)])		)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<L > R>	a, b, c;												)";
		auto	dst	= R"(	(var (symbol template `foo` (ope `>` `L` `R`)) [(`a`) (`b`) (`c`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<L > R>	a = 123, b = val<A>2>, c = 991;					)";
		auto	dst	= R"(	(var (symbol template `foo` (ope `>` `L` `R`)) [(`a` (dec 123)) (`b` (symbol template `val` (ope `>` `A` (dec 2)))) (`c` (dec 991))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo::bar<L > R>	a					)";
		auto	dst	= R"(	 (var (ns `foo` (symbol template `bar` (ope `>` `L` `R`))) [(`a`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<L > R>::bar	a					)";
		auto	dst	= R"(	 (var (ns (symbol template `foo` (ope `>` `L` `R`)) `bar`) [(`a`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F>::bar	value					)";
		auto	dst	= R"(	 (var (ns (symbol template `foo` `F`) `bar`) [(`value`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F>::bar<B>	value					)";
		auto	dst	= R"(	 (var (ns (symbol template `foo` `F`) (symbol template `bar` `B`)) [(`value`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	foo<F1,F2>::bar<B1,B2>	value					)";
		auto	dst	= R"(	 (var (ns (symbol template `foo` `F1` `F2`) (symbol template `bar` `B1` `B2`)) [(`value`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	inline static foo	value1, value2		)";
		auto	dst	= R"(	(var `foo` (spec static inline) [(`value1`) (`value2`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	inline static foo<F>::bar<B1,B2>	value1, value2		)";
		auto	dst	= R"(	(var (ns (symbol template `foo` `F`) (symbol template `bar` `B1` `B2`)) (spec static inline) [(`value1`) (`value2`)])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	int	a(1);		)";
		auto	dst	= R"(	(var `int` [(`a` (dec 1))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	C	a(1, 2, 3);	)";
		auto	dst	= R"(	(var `C` [(`a` (list (dec 1) (dec 2) (dec 3)))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(5_documentmode)){
	{
		auto	src	= R"(	a.test(); <% int a; %>							)";
		auto	dst	= R"(	(echo "a.test(); ") (var `int` [(`a`)]) (echo "")	)";
		AMTRS_TEST_EQ((text)parse_test(src, true), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(5_string_extract)){
	{
		auto	src	= R"(	"Hello${x}world";	)";
		auto	dst	= R"(	(ope `+` (ope `+` "Hello" `x`) "world")	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
	{
		auto	src	= R"(	"Hello${1 + 2}world";	)";
		auto	dst	= R"(	(ope `+` (ope `+` "Hello" (ope `+` (dec 1) (dec 2))) "world")	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//AMTRS_TEST_F(_PRFX(8_multitest)){
	{
		auto	src	= R"(	auto insert(value_type const& _value) -> std::pair<iterator, bool> { return _emplace(tail, _value); }	)";
		auto	dst	= R"(	(func `insert` (ns `std` (symbol template `pair` `iterator` `bool`)) (parameter_list ((type (type `value_type` const) &) (name `_value`))) [ (return (call `_emplace` [`tail` `_value`]))])	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
//}
//}
//AMTRS_TEST_F(_PRFX(9_test)){
	{
		auto		src	= R"(
			namespace ns {
				enum class enum1
				{
				};

				enum class enum2
				{
				};


				template<class T>
				concept	test = requires (T a) {
					{ ++a; } -> int;
				};
				

				enum class Enum : int
				{
					e1	= 2,
					e2
				};

				class	cls
						: public virtual base1
						, private base2
				{
					int		val1, val2;

					cls()
						: val1(0)
						, val2(1)
					{
						if (val1 == val2) { return; }
					}

					static inline auto func() const noexcept -> int
					{
						return	console::log("{} {} {}", 1, 2 , 3);
					}

					operator bool ()
					{
						return	true;
					}

					int operator + (int a)
					{
						return	(int)(val1 + val2 + a + 'a');
					}

				};

				int decltype(gptr)::get()
				{
					return	1;
				}

				int func()
				{
					switch (m())
					{
						case 1 :	return	1;
						case 2 :	return	2;
					}
					v1	= new (x) T (a, b, c);
					v2	= new (x) T [ a +  b + c ];
					while (a < b)
					{
						++x;
						++y;
						break;
						continue;
					}
					for (int i = 0; i < 10; ++i) { ++z; }
					for (auto& v : values) { ++v; }
					a	= operator ++ (1);
					a	= a[1239];
					x	= (c < 0) ? 1 : 2;
				}
			}
		)";
		auto		dst	= R"(	(namespace `ns` (enum `enum1`) (enum `enum2`) (template (parameter_list (`class` (name `T`))) (concept `test` (requires (parameter_list (`T` (name `a`)))[(match `int` (prefix `++` `a`))]))) (enum `Enum` `int` [(`e1` (dec 2)) (`e2`)]) (class `cls` (extends (virtual public `base1`) (private `base2`)) (var `int` [(`val1`) (`val2`)]) (func constractor `cls` (parameter_list) (initializers  (`val1` (dec 0)) (`val2` (dec 1))) [ (if (ope `==` `val1` `val2`) [ (return)])]) (func `func` [const noexcept] (spec static inline) `int` (parameter_list)  [ (return (call (ns `console` `log`) ["{} {} {}" (dec 1) (dec 2) (dec 3)]))]) (func conversion  `bool` (parameter_list)  [ (return (true))]) (func operator `+` `int` (parameter_list (`int` (name `a`)))  [ (return (cast c `int` (ope `+` (ope `+` (ope `+` `val1` `val2`) `a`) 'a')))])) (func (ns (decltype `gptr`) `get`) `int` (parameter_list)  [ (return (dec 1))]) (func `func` `int` (parameter_list)  [ (switch (call `m` []) ((dec 1) (return (dec 1))) ((dec 2) (return (dec 2)))) (ope `=` `v1` (new `T` (placement `x`) (arguments `a` `b` `c`))) (ope `=` `v2` (new[] `T` (placement `x`) (arguments (ope `+` (ope `+` `a` `b`) `c`)))) (while (ope `<` `a` `b`) [ (prefix `++` `x`) (prefix `++` `y`) (break) (continue)]) (for  (var `int` [(`i` (dec 0))]) (ope `<` `i` (dec 10)) (prefix `++` `i`) [ (prefix `++` `z`)]) (foreach (var (type `auto` &) [(`v`)]) `values` [ (prefix `++` `v`)]) (ope `=` `a` (call (symbol operator `++`) [(dec 1)])) (ope `=` `a` (ope `[]` `a` (dec 1239))) (conditions (ope `=` `x` (ope `<` `c` (dec 0))) (dec 1) (dec 2))]))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
}

#if	0
AMTRS_TEST_F(_PRFX(parse_and_serialize_1_view_class))
{
	{
		auto	src	= R"(

template<class T, class Iterator = T*, class Traits = view_traits<Iterator>>
struct	view
{
	using	traits_type				= Traits;
	using	value_type				= typename traits_type::value_type;
	using	const_value_type		= typename traits_type::const_value_type;
	using	size_type				= typename traits_type::size_type;
	using	reference				= typename traits_type::reference;
	using	const_reference			= typename traits_type::const_reference;
	using	iterator				= typename traits_type::iterator;
	using	const_iterator			= typename traits_type::const_iterator;

	static constexpr size_type	npos	= static_cast<size_type>(~0);

	iterator		s		= traits_type::init();
	iterator		e		= traits_type::init();

private:

public:
	constexpr view() = default;
	constexpr view(view const&) = default;
	constexpr view(view&&) = default;
	constexpr view& operator = (view const&) = default;
	constexpr view& operator = (view&&) = default;
	constexpr view(std::nullptr_t) noexcept : s(nullptr), e(nullptr) {}

	constexpr view(iterator _s) noexcept : s(_s), e(tail(_s)) {}
	constexpr view(iterator _s, iterator _e) noexcept : s(_s), e(_e) {}
	constexpr view(iterator _s, size_type _n) noexcept : s(_s), e(_s + _n) {}

	template<size_type N>
	constexpr view(value_type (&_src)[N]) noexcept
		: s(_src)
		, e(_src + N)
	{}

	template<typename  R>
	constexpr view(R const& _src, std::enable_if_t<is_datasizable_v<R, iterator>>** _= nullptr) noexcept
		: s(_src.data())
		, e(_src.data() + _src.size())
	{}

	template<typename  R>
	constexpr view(R const& _src, std::enable_if_t<is_begin_end_v<R, iterator>>** _= nullptr) noexcept
		: s(_src.begin())
		, e(_src.end())
	{}

	static auto eof() noexcept -> const_reference { static value_type c = traits_type::eof(); return c; }

	constexpr auto data() const noexcept -> iterator { return s; }
	constexpr auto back() const noexcept -> reference { return e[-1]; }
	constexpr auto back_at() const noexcept -> const_reference { return empty() ? eof() : e[-1]; }
	constexpr auto begin() const noexcept -> iterator { return s; }
	constexpr auto end() const noexcept -> iterator { return e; }
	constexpr auto front() const noexcept -> reference { return *s; }
	constexpr auto front_at() const noexcept -> const_reference { return empty() ? eof() : *s; }
	constexpr auto size() const noexcept -> size_type { return e - s; }

	constexpr bool empty() const noexcept { return s == e || bad(); }				// 異常終了しているか終端まで読み込んでいるか調べます
	constexpr bool endof() const noexcept { return s == e && !bad(); }			// 正常に終わっている状態で終端まで読み込みが終わっているか調べます
	constexpr bool good() const noexcept { return !bad(); }						// 正常終了しているか調べます
	constexpr bool bad() const noexcept { if constexpr (std::is_pointer<iterator>::value) { return !s || !e; } else { return false; } }						// 異常終了しているか調べます

	constexpr auto operator * () const noexcept -> reference { return front(); }						// 異常終了しているか調べます

	constexpr value_type const& operator [] (int _r) const noexcept { return data()[_r]; }

	// **************************************************************
	// check

	constexpr bool in_bounds(iterator _it) const noexcept { return _it >= s && _it < e; }
	constexpr bool out_bounds(iterator _it) const noexcept { return !in_bounds(_it); }

	constexpr auto hash() const noexcept -> size_type;

	// **************************************************************
	// ccompare

	// 文字列を比較します。
	constexpr auto compare(view _r) const noexcept -> int;

	// 指定した文字列で始まっているかどうか調べます
	constexpr bool starts_with(value_type const* _text) const noexcept;
	constexpr bool starts_with(view _text) const noexcept;

	// 指定した文字列で終わっているかどうか調べます
	constexpr bool ends_with(value_type const* _text) const noexcept;
	constexpr bool ends_with(view _text) const noexcept;

	// 二つの文字列を比較し、一致する部分を返します。
	constexpr auto match_first_of(value_type const* _text) const noexcept -> view;
	constexpr auto match_first_of(view _text) const noexcept -> view;


	// **************************************************************
	// find

	// 指定した文字列が最初に出てくる場所を返します。
	constexpr auto find(view _s) const noexcept -> size_type;

	// 指定した文字列が最後に出てくる場所を返します。
	constexpr auto rfind(view _s) const noexcept -> size_type;


	// 指定した文字が最初に出てくる場所を返します。
	constexpr auto find_first_of(value_type _c) const noexcept -> size_type;

	// 指定した文字の一覧の中から最初に出てくる場所を返します。
	constexpr auto find_first_of(value_type const* _c) const noexcept -> size_type;
	constexpr auto find_first_of(view _c) const noexcept -> size_type;

	// 指定した文字以外の文字が最初に出てくる場所を返します。
	constexpr auto find_first_not_of(value_type _c) const noexcept -> size_type;

	// 指定した文字の一覧の中以外の文字が最初に出てくる場所を返します。
	constexpr auto find_first_not_of(value_type const* _c) const noexcept -> size_type;
	constexpr auto find_first_not_of(view _c) const noexcept -> size_type;

	// 指定した文字が最後に出てくる場所を返します。
	constexpr auto find_last_of(value_type _c) const noexcept -> size_type;

	// 指定した文字の一覧の中から最後に出てくる場所を返します。
	constexpr auto find_last_of(value_type const* _c) const noexcept -> size_type;
	constexpr auto find_last_of(view _c) const noexcept -> size_type;

	// 指定した文字以外の文字が最後に出てくる場所を返します。
	constexpr auto find_last_not_of(value_type _c) const noexcept -> size_type;

	// 指定した文字の一覧の中以外の文字が最後に出てくる場所を返します。
	constexpr auto find_last_not_of(value_type const* _c) const noexcept -> size_type;
	constexpr auto find_last_not_of(view _c) const noexcept -> size_type;


	// **************************************************************
	// subview

	// 改行が出るまでを抜き出します。
	constexpr auto read_line() const noexcept -> view;

	// 指定した位置から後ろを返します。
	template<class F>
	constexpr auto substr(F _first) const noexcept -> view;

	// 指定した位置から指定したサイズ（または終端位置）を返します。
	template<class F, class L>
	constexpr auto substr(F _first, L _last) const noexcept -> view;

	// 指定する文字を前方、後方から取り除きます
	constexpr auto trim(value_type _c) const noexcept -> view { return trim_first(_c).trim_last(_c); }

	// 指定する文字の一覧を前方、後方から取り除きます
	constexpr auto trim(value_type const* _list) const noexcept -> view { return trim(view(_list)); }
	constexpr auto trim(view _list) const noexcept -> view { return trim_first(_list).trim_last(_list); }

	// 指定する文字を前方から取り除きます
	constexpr auto trim_first(value_type _c) const noexcept -> view;

	// 指定する文字の一覧を前方から取り除きます
	constexpr auto trim_first(value_type const* _list) const noexcept -> view;
	constexpr auto trim_first(view _list) const noexcept -> view;

	// 指定する文字を後方から取り除きます
	constexpr auto trim_last(value_type _c) const noexcept -> view;

	// 指定する文字の一覧を後方から取り除きます
	constexpr auto trim_last(value_type const* _list) const noexcept -> view;
	constexpr auto trim_last(view _list) const noexcept -> view;

	// 前方からひとつだけ改行を取り除きます
	constexpr auto trim_first_one_return() const noexcept -> view;


	constexpr bool operator == (std::nullptr_t) const noexcept { return !s || !e; }
	constexpr bool operator != (std::nullptr_t) const noexcept { return s && e; }
	constexpr bool operator == (value_type const* _txt) const noexcept { if (empty()) { return _txt == nullptr || *_txt == '\0' ? true : false; } auto cur = s; while (*_txt) { if (*_txt++ != *cur++) { return false; } } return cur == e; }
	constexpr bool operator != (value_type const* _txt) const noexcept { return !operator == (_txt); }
	constexpr bool operator == (view _r) const noexcept { if (size() != _r.size()) { return false; } auto s1 = s, s2 = _r.s; while (s1 != e) { if (*s1++ != *s2++) { return false; } } return true; }
	constexpr bool operator != (view _r) const noexcept { return !operator == (_r); }
	constexpr bool operator <  (view _r) const noexcept { return compare(_r) <  0; }
	constexpr bool operator <= (view _r) const noexcept { return compare(_r) <= 0; }
	constexpr bool operator >  (view _r) const noexcept { return compare(_r) >  0; }
	constexpr bool operator >= (view _r) const noexcept { return compare(_r) >= 0; }

	// 読み取り開始位置を１文字進めます。終端位置は考慮しません。
	constexpr view& operator ++ () noexcept { ++s; return *this; }	
	constexpr view operator ++ (int) noexcept { view r(*this); ++s; return r; }	

	template<class CTraits>
	friend std::basic_ostream<typename std::remove_const<value_type>::type, CTraits>& operator << (std::basic_ostream<typename std::remove_const<value_type>::type, CTraits>& o, view<T, Iterator, Traits> const& thiz)
	{
		o.write(thiz.data(), thiz.size());
		return	o;
	}

private:
	constexpr iterator pos(value_type* _pos) const noexcept { return _pos < e ? _pos : e; }
	constexpr iterator pos(size_type   _pos) const noexcept { return s + _pos < e ? s + _pos : e; }
	constexpr iterator tail(iterator _p) noexcept { if (_p) {while (*_p) ++_p;} return _p; }
};


		)";
		auto	dst	= R"((template (parameter_list (`class` (name `T`))(`class` (name `Iterator`) (default (type `T` *)))(`class` (name `Traits`) (default (symbol template `view_traits` `Iterator`)))) (class `view` (using alias `traits_type` `Traits`) (using alias `value_type` (ns `traits_type` `value_type`)) (using alias `const_value_type` (ns `traits_type` `const_value_type`)) (using alias `size_type` (ns `traits_type` `size_type`)) (using alias `reference` (ns `traits_type` `reference`)) (using alias `const_reference` (ns `traits_type` `const_reference`)) (using alias `iterator` (ns `traits_type` `iterator`)) (using alias `const_iterator` (ns `traits_type` `const_iterator`)) (var `size_type` (spec static constexpr) [(`npos` (cast c `size_type` (prefix `~` (dec 0))))]) (var `iterator` [(`s` (call (ns `traits_type` `init`) []))]) (var `iterator` [(`e` (call (ns `traits_type` `init`) []))]) (label `private`) (label `public`) (func constractor `view` [default delete] (spec constexpr) (parameter_list)) (func constractor `view` [default delete] (spec constexpr) (parameter_list ((type (type `view` const) &)))) (func constractor `view` [default delete] (spec constexpr) (parameter_list ((type `view` &&)))) (func operator `=` [default delete] (spec constexpr) (type `view` &) (parameter_list ((type (type `view` const) &)))) (func operator `=` [default delete] (spec constexpr) (type `view` &) (parameter_list ((type `view` &&)))) (func constractor `view` [noexcept] (spec constexpr) (parameter_list ((ns `std` `nullptr_t`))) (initializers (`s` (nullptr)) (`e` (nullptr))) []) (func constractor `view` [noexcept] (spec constexpr) (parameter_list (`iterator` (name `_s`))) (initializers (`s` `_s`) (`e` (call `tail` [`_s`]))) []) (func constractor `view` [noexcept] (spec constexpr) (parameter_list (`iterator` (name `_s`)) (`iterator` (name `_e`))) (initializers (`s` `_s`) (`e` `_e`)) []) (func constractor `view` [noexcept] (spec constexpr) (parameter_list (`iterator` (name `_s`)) (`size_type` (name `_n`))) (initializers (`s` `_s`) (`e` (ope `+` `_s` `_n`))) []) (template (parameter_list (`size_type` (name `N`))) (func constractor `view` [noexcept] (spec constexpr) (parameter_list ((type (type `value_type` [] (`N`)) &) (name `_src`))) (initializers (`s` `_src`) (`e` (ope `+` `_src` `N`))) [])) (template (parameter_list (`typename` (name `R`))) (func constractor `view` [noexcept] (spec constexpr) (parameter_list ((type (type `R` const) &) (name `_src`)) ((type (type (ns `std` (symbol template `enable_if_t` (symbol template `is_datasizable_v` `R` `iterator`))) *) *) (name `_`) (default (nullptr)))) (initializers (`s` (call (ope `.` `_src` `data`) [])) (`e` (ope `+` (call (ope `.` `_src` `data`) []) (call (ope `.` `_src` `size`) [])))) [])) (template (parameter_list (`typename` (name `R`))) (func constractor `view` [noexcept] (spec constexpr) (parameter_list ((type (type `R` const) &) (name `_src`)) ((type (type (ns `std` (symbol template `enable_if_t` (symbol template `is_begin_end_v` `R` `iterator`))) *) *) (name `_`) (default (nullptr)))) (initializers (`s` (call (ope `.` `_src` `begin`) [])) (`e` (call (ope `.` `_src` `end`) []))) [])) (func `eof` [noexcept] (spec static) `const_reference` (parameter_list) [ (var `value_type` (spec static) [(`c` (call (ns `traits_type` `eof`) []))]) (return `c`)]) (func `data` [const noexcept] (spec constexpr) `iterator` (parameter_list) [ (return `s`)]) (func `back` [const noexcept] (spec constexpr) `reference` (parameter_list) [ (return (ope `[]` `e` (dec -1)))]) (func `back_at` [const noexcept] (spec constexpr) `const_reference` (parameter_list) [ (return (conditions (call `empty` []) (call `eof` []) (ope `[]` `e` (dec -1))))]) (func `begin` [const noexcept] (spec constexpr) `iterator` (parameter_list) [ (return `s`)]) (func `end` [const noexcept] (spec constexpr) `iterator` (parameter_list) [ (return `e`)]) (func `front` [const noexcept] (spec constexpr) `reference` (parameter_list) [ (return (prefix `*` `s`))]) (func `front_at` [const noexcept] (spec constexpr) `const_reference` (parameter_list) [ (return (conditions (call `empty` []) (call `eof` []) (prefix `*` `s`)))]) (func `size` [const noexcept] (spec constexpr) `size_type` (parameter_list) [ (return (ope `-` `e` `s`))]) (func `empty` [const noexcept] (spec constexpr) `bool` (parameter_list) [ (return (ope `||` (ope `==` `s` `e`) (call `bad` [])))]) (func `endof` [const noexcept] (spec constexpr) `bool` (parameter_list) [ (return (ope `&&` (ope `==` `s` `e`) (prefix `!` (call `bad` []))))]) (func `good` [const noexcept] (spec constexpr) `bool` (parameter_list) [ (return (prefix `!` (call `bad` [])))]) (func `bad` [const noexcept] (spec constexpr) `bool` (parameter_list) [ (if (ns `std` (symbol template `is_pointer` `iterator`) `value`) [ (return (ope `||` (prefix `!` `s`) (prefix `!` `e`)))] [ (return (false))])]) (func operator `*` [const noexcept] (spec constexpr) `reference` (parameter_list) [ (return (call `front` []))]) (func operator `[]` [const noexcept] (spec constexpr) (type (type `value_type` const) &) (parameter_list (`int` (name `_r`))) [ (return (ope `[]` (call `data` []) `_r`))]) (func `in_bounds` [const noexcept] (spec constexpr) `bool` (parameter_list (`iterator` (name `_it`))) [ (return (ope `&&` (ope `>=` `_it` `s`) (ope `<` `_it` `e`)))]) (func `out_bounds` [const noexcept] (spec constexpr) `bool` (parameter_list (`iterator` (name `_it`))) [ (return (prefix `!` (call `in_bounds` [`_it`])))]) (func `hash` [const noexcept] (spec constexpr) `size_type` (parameter_list)) (func `compare` [const noexcept] (spec constexpr) `int` (parameter_list (`view` (name `_r`)))) (func `starts_with` [const noexcept] (spec constexpr) `bool` (parameter_list ((type (type `value_type` const) *) (name `_text`)))) (func `starts_with` [const noexcept] (spec constexpr) `bool` (parameter_list (`view` (name `_text`)))) (func `ends_with` [const noexcept] (spec constexpr) `bool` (parameter_list ((type (type `value_type` const) *) (name `_text`)))) (func `ends_with` [const noexcept] (spec constexpr) `bool` (parameter_list (`view` (name `_text`)))) (func `match_first_of` [const noexcept] (spec constexpr) `view` (parameter_list ((type (type `value_type` const) *) (name `_text`)))) (func `match_first_of` [const noexcept] (spec constexpr) `view` (parameter_list (`view` (name `_text`)))) (func `find` [const noexcept] (spec constexpr) `size_type` (parameter_list (`view` (name `_s`)))) (func `rfind` [const noexcept] (spec constexpr) `size_type` (parameter_list (`view` (name `_s`)))) (func `find_first_of` [const noexcept] (spec constexpr) `size_type` (parameter_list (`value_type` (name `_c`)))) (func `find_first_of` [const noexcept] (spec constexpr) `size_type` (parameter_list ((type (type `value_type` const) *) (name `_c`)))) (func `find_first_of` [const noexcept] (spec constexpr) `size_type` (parameter_list (`view` (name `_c`)))) (func `find_first_not_of` [const noexcept] (spec constexpr) `size_type` (parameter_list (`value_type` (name `_c`)))) (func `find_first_not_of` [const noexcept] (spec constexpr) `size_type` (parameter_list ((type (type `value_type` const) *) (name `_c`)))) (func `find_first_not_of` [const noexcept] (spec constexpr) `size_type` (parameter_list (`view` (name `_c`)))) (func `find_last_of` [const noexcept] (spec constexpr) `size_type` (parameter_list (`value_type` (name `_c`)))) (func `find_last_of` [const noexcept] (spec constexpr) `size_type` (parameter_list ((type (type `value_type` const) *) (name `_c`)))) (func `find_last_of` [const noexcept] (spec constexpr) `size_type` (parameter_list (`view` (name `_c`)))) (func `find_last_not_of` [const noexcept] (spec constexpr) `size_type` (parameter_list (`value_type` (name `_c`)))) (func `find_last_not_of` [const noexcept] (spec constexpr) `size_type` (parameter_list ((type (type `value_type` const) *) (name `_c`)))) (func `find_last_not_of` [const noexcept] (spec constexpr) `size_type` (parameter_list (`view` (name `_c`)))) (func `read_line` [const noexcept] (spec constexpr) `view` (parameter_list)) (template (parameter_list (`class` (name `F`))) (func `substr` [const noexcept] (spec constexpr) `view` (parameter_list (`F` (name `_first`))))) (template (parameter_list (`class` (name `F`))(`class` (name `L`))) (func `substr` [const noexcept] (spec constexpr) `view` (parameter_list (`F` (name `_first`)) (`L` (name `_last`))))) (func `trim` [const noexcept] (spec constexpr) `view` (parameter_list (`value_type` (name `_c`))) [ (return (call (ope `.` (call `trim_first` [`_c`]) `trim_last`) [`_c`]))]) (func `trim` [const noexcept] (spec constexpr) `view` (parameter_list ((type (type `value_type` const) *) (name `_list`))) [ (return (call `trim` [(call `view` [`_list`])]))]) (func `trim` [const noexcept] (spec constexpr) `view` (parameter_list (`view` (name `_list`))) [ (return (call (ope `.` (call `trim_first` [`_list`]) `trim_last`) [`_list`]))]) (func `trim_first` [const noexcept] (spec constexpr) `view` (parameter_list (`value_type` (name `_c`)))) (func `trim_first` [const noexcept] (spec constexpr) `view` (parameter_list ((type (type `value_type` const) *) (name `_list`)))) (func `trim_first` [const noexcept] (spec constexpr) `view` (parameter_list (`view` (name `_list`)))) (func `trim_last` [const noexcept] (spec constexpr) `view` (parameter_list (`value_type` (name `_c`)))) (func `trim_last` [const noexcept] (spec constexpr) `view` (parameter_list ((type (type `value_type` const) *) (name `_list`)))) (func `trim_last` [const noexcept] (spec constexpr) `view` (parameter_list (`view` (name `_list`)))) (func `trim_first_one_return` [const noexcept] (spec constexpr) `view` (parameter_list)) (func operator `==` [const noexcept] (spec constexpr) `bool` (parameter_list ((ns `std` `nullptr_t`))) [ (return (ope `||` (prefix `!` `s`) (prefix `!` `e`)))]) (func operator `!=` [const noexcept] (spec constexpr) `bool` (parameter_list ((ns `std` `nullptr_t`))) [ (return (ope `&&` `s` `e`))]) (func operator `==` [const noexcept] (spec constexpr) `bool` (parameter_list ((type (type `value_type` const) *) (name `_txt`))) [ (if (call `empty` []) [ (return (conditions (ope `||` (ope `==` `_txt` (nullptr)) (ope `==` (prefix `*` `_txt`) '\0')) (true) (false)))]) (var `auto` [(`cur` `s`)]) (while (prefix `*` `_txt`) [ (if (ope `!=` (prefix `*` (suffix `++` `_txt`)) (prefix `*` (suffix `++` `cur`))) [ (return (false))])]) (return (ope `==` `cur` `e`))]) (func operator `!=` [const noexcept] (spec constexpr) `bool` (parameter_list ((type (type `value_type` const) *) (name `_txt`))) [ (return (prefix `!` (call (symbol operator `==`) [`_txt`])))]) (func operator `==` [const noexcept] (spec constexpr) `bool` (parameter_list (`view` (name `_r`))) [ (if (ope `!=` (call `size` []) (call (ope `.` `_r` `size`) [])) [ (return (false))]) (var `auto` [(`s1` `s`) (`s2` (ope `.` `_r` `s`))]) (while (ope `!=` `s1` `e`) [ (if (ope `!=` (prefix `*` (suffix `++` `s1`)) (prefix `*` (suffix `++` `s2`))) [ (return (false))])]) (return (true))]) (func operator `!=` [const noexcept] (spec constexpr) `bool` (parameter_list (`view` (name `_r`))) [ (return (prefix `!` (call (symbol operator `==`) [`_r`])))]) (func operator `<` [const noexcept] (spec constexpr) `bool` (parameter_list (`view` (name `_r`))) [ (return (ope `<` (call `compare` [`_r`]) (dec 0)))]) (func operator `<=` [const noexcept] (spec constexpr) `bool` (parameter_list (`view` (name `_r`))) [ (return (ope `<=` (call `compare` [`_r`]) (dec 0)))]) (func operator `>` [const noexcept] (spec constexpr) `bool` (parameter_list (`view` (name `_r`))) [ (return (ope `>` (call `compare` [`_r`]) (dec 0)))]) (func operator `>=` [const noexcept] (spec constexpr) `bool` (parameter_list (`view` (name `_r`))) [ (return (ope `>=` (call `compare` [`_r`]) (dec 0)))]) (func operator `++` [noexcept] (spec constexpr) (type `view` &) (parameter_list) [ (prefix `++` `s`) (return (prefix `*` (this)))]) (func operator `++` [noexcept] (spec constexpr) `view` (parameter_list (`int`)) [ (var `view` [(`r`)]) (prefix `*` (this)) (prefix `++` `s`) (return `r`)]) (template (parameter_list (`class` (name `CTraits`))) (func operator `<<` (type (ns `std` (symbol template `basic_ostream` (ns `std` (symbol template `remove_const` `value_type`) `type`) `CTraits`)) &) (parameter_list ((type (ns `std` (symbol template `basic_ostream` (ns `std` (symbol template `remove_const` `value_type`) `type`) `CTraits`)) &) (name `o`)) ((type (type (symbol template `view` `T` `Iterator` `Traits`) const) &) (name `thiz`))) [ (call (ope `.` `o` `write`) [(call (ope `.` `thiz` `data`) []) (call (ope `.` `thiz` `size`) [])]) (return `o`)])) (label `private`) (func `pos` [const noexcept] (spec constexpr) `iterator` (parameter_list ((type `value_type` *) (name `_pos`))) [ (return (conditions (ope `<` `_pos` `e`) `_pos` `e`))]) (func `pos` [const noexcept] (spec constexpr) `iterator` (parameter_list (`size_type` (name `_pos`))) [ (return (conditions (ope `<` (ope `+` `s` `_pos`) `e`) (ope `+` `s` `_pos`) `e`))]) (func `tail` [noexcept] (spec constexpr) `iterator` (parameter_list (`iterator` (name `_p`))) [ (if `_p` [ (while (prefix `*` `_p`) (prefix `++` `_p`))]) (return `_p`)])))	)";
		AMTRS_TEST_EQ((text)parse_test(src), (text)normalize(dst));
	}
}
#endif

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
