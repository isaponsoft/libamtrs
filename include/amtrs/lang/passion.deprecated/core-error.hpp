/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__sub__lang_error__hpp
#define	__libamtrs__passion__sub__lang_error__hpp
#include "namespace.hxx"
PSNVM_NAMESPACE_BEGIN
using namespace AMTRS_NAMESPACE;

#if	defined(DEBUG) || defined(_DEBUG)
#define	PSNVM_DEBUGLOG(m, ...)			AMTRS_OUTPUT_LOG(amtrs::system::loglevel::debug, m, __VA_ARGS__)
#define	PSNVM_DEBUGLOG_NOLOCATE(m, ...)	AMTRS_OUTPUT_LOG(amtrs::system::loglevel::debug, m, __VA_ARGS__)
#else
#define	PSNVM_DEBUGLOG(...)
#define	PSNVM_DEBUGLOG_NOLOCATE(...)
#endif

// ******************************************************************
//
// errors.
//
// ******************************************************************

#define	PSNVM_MAKE_ERRORCODE(_code)			(_code)
#define	PSNVM_MAKE_WARNCODE(_code)			(_code | (1<<24))
#define	PSNVM_MAKE_UNTERMERROR(_code)		(_code | (2<<24))					// 文などが途中で終わっている。対話モードで入力状態を維持するかどうかの判別に使用できる。
#define	PSNVM_IS_UNTERMERROR(_code)			(((_code>>24) & 0xff) == 2)			// 文などが途中で終わっている。対話モードで入力状態を維持するかどうかの判別に使用できる。

enum	errorcode
{
	ec_success							= 0,			// success

	ec_under_implementation					= PSNVM_MAKE_ERRORCODE(1),			// now working!!
	ec_module_notfound						= PSNVM_MAKE_ERRORCODE(2),			// module not found.
	ec_module_no_exported					= PSNVM_MAKE_ERRORCODE(3),			// module no exported.
	ec_module_format_error					= PSNVM_MAKE_ERRORCODE(4),			// module format error.
	ec_module_unkown_version				= PSNVM_MAKE_ERRORCODE(6),			// module unkown version.

	ec_nothing_closing_curry_brackets		= PSNVM_MAKE_UNTERMERROR(10000),	// '}' nothing.
	ec_nothing_closing_square_brackets		= PSNVM_MAKE_UNTERMERROR(10001),	// ']' nothing.
	ec_nothing_closing_circle_brackets		= PSNVM_MAKE_UNTERMERROR(10002),	// ')' nothing.

	ec_expression_unkown_identify			= PSNVM_MAKE_ERRORCODE(15000),		// unkown token.
	ec_not_document_mode					= PSNVM_MAKE_ERRORCODE(15001),		// Can't use <%%>
	ec_unkown_character						= PSNVM_MAKE_ERRORCODE(15002),		// unkown char
	ec_unkown_identify						= PSNVM_MAKE_ERRORCODE(15003),		// Unmatch variables.
	ec_unmatched_variables					= PSNVM_MAKE_ERRORCODE(15004),		// Unmatch variables.
	ec_unmatched_function					= PSNVM_MAKE_ERRORCODE(15005),		// Unmatch function.
	ec_notfound_function					= PSNVM_MAKE_ERRORCODE(15006),		// Unmatch function.
	ec_cant_assign_scalar					= PSNVM_MAKE_ERRORCODE(15007),		// Can't assign scalar type.
	ec_cant_cast_to							= PSNVM_MAKE_ERRORCODE(15008),		// Can't cast type.
	et_function_ambitious					= PSNVM_MAKE_ERRORCODE(15009),		// Many function match.
	ec_unmatch_function_parameter			= PSNVM_MAKE_ERRORCODE(15010),		// Many function match.
	ec_not_function_call					= PSNVM_MAKE_ERRORCODE(15011),		// Many function match.
	ec_not_assignable						= PSNVM_MAKE_ERRORCODE(15012),
	ec_nothing_bin_operator					= PSNVM_MAKE_ERRORCODE(15013),		// Nothing bin operator.

	ec_statament_if_nothing_conditions		= PSNVM_MAKE_UNTERMERROR(20000),	// if nothing conditions.
	ec_statament_if_nothing_statement		= PSNVM_MAKE_UNTERMERROR(20001),	// if nothing statement.
	ec_statament_if_else_nothing_statement	= PSNVM_MAKE_UNTERMERROR(20002),	// if else nothing statement.
	ec_statement_not_return_pass			= PSNVM_MAKE_ERRORCODE(20003),		// has not return.
	ec_statement_declvar_unknown_terminate	= PSNVM_MAKE_ERRORCODE(20004),		// decl var unkown terminate.
	ec_statement_declvar_expression_nothing	= PSNVM_MAKE_ERRORCODE(20005),		// decl var nothing initialize expression.

	ec_expr_cant_cast_to					= PSNVM_MAKE_ERRORCODE(21000),		// Can't cast to string.

	ec_can_not_type_cast					= PSNVM_MAKE_ERRORCODE(30002),		// cast error

	ec_prepro_message						= PSNVM_MAKE_UNTERMERROR(40001),		// pre processor "mesg" directive.
	ec_prepro_error							= PSNVM_MAKE_ERRORCODE(40002),			// pre processor "error" directive.
	ec_prepro_file_not_found				= PSNVM_MAKE_ERRORCODE(40003),			// pre processor "include" directive
	ec_prepro_include_syntax				= PSNVM_MAKE_ERRORCODE(40004),			// pre processor "include" directive
	ec_prepro_has_not_macroname				= PSNVM_MAKE_ERRORCODE(40005),			// pre processor define has not name
	ec_prepro_not_defined_undef				= PSNVM_MAKE_ERRORCODE(40006),			// pre processor undef not defined
	ec_prepro_endif_nothing					= PSNVM_MAKE_ERRORCODE(40007),			// pre processor endif nothing
	ec_prepro_else_duplicated				= PSNVM_MAKE_ERRORCODE(40008),			// pre processor else duplicated
	ec_prepro_defind_has_not_name			= PSNVM_MAKE_ERRORCODE(40009),			// pre processor
	ec_prepro_defind_have_not_name			= PSNVM_MAKE_ERRORCODE(40010),
};
#undef	PSNVMPARSER_ERRORCODE
#undef	PSNVMPARSER_WARNCODE
#undef	PSNVMPARSER_UNTERMERROR
#undef	PSNVM_IS_UNTERMERROR


/*
 * errorval.type() の値を取得します。
 */
PSNVM_API size_t error_type();

/*
 * amtrs errorval へエラーコードを登録します。
 */
PSNVM_API void register_error();

/*
 * エラーメッセージを直接取得します。
 */
PSNVM_API char const* get_error_string(errorcode _ec);


PSNVM_NAMESPACE_END
AMTRS_NAMESPACE_BEGIN
template<>
struct	errorval_type<::PSNVM_NAMESPACE::errorcode>
{
	size_t operator () () const noexcept
	{
		return	::PSNVM_NAMESPACE::error_type();
	}
};
AMTRS_NAMESPACE_END
PSNVM_NAMESPACE_BEGIN
struct	errorstate : amtrs::errorval
{
	errorstate()
	{}

	errorstate(errorcode _code, char const*	_locate, sstring _token = {}, sstring _token2 = {}, char const* _sender = nullptr, int _sender_line = 0)
		: errorval(error_type(), _code)
		, locate(_locate)
		, token(std::move(_token))
		, token2(std::move(_token2))
		, sender(_sender)
		, sender_line(_sender_line)
	{}

	char const*				locate;
	sstring					token;
	sstring					token2;
	simplearray<sstring>	optionals;

	// エラーを投げたプログラムの情報
	char const*	sender			= nullptr;
	int			sender_line		= 0;
};

/*
 * コンパイラーで発生したエラーを取得するためのハンドラ
 */
struct	errorhandler
{
	errorcode				lasterr		= errorcode::ec_success;
	simplearray<errorstate>	errors;

	errorstate* invoke(errorcode _ec, char const* _locate, sstring _token, sstring _deps, char const* _filename = nullptr, int _line = 0);

	bool bad() const noexcept { return !good(); }
	bool good() const noexcept { return lasterr == errorcode::ec_success; }
};
#if		defined(_DEBUG) || defined(DEBUG)
#	define	PSNVM_ERROR_INVOKE(eh,ec,loc,tk,dep)	(eh).invoke(ec,loc,tk,dep,__FILE__,__LINE__)
#else
#	define	PSNVM_ERROR_INVOKE(eh,ec,loc,tk,dep)	(eh).invoke(ec,loc,tk,dep,nullptr,0)
#endif



PSNVM_NAMESPACE_END
#endif
