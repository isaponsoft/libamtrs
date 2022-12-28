/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__types__hpp
#define	__libamtrs__lang__passion__types__hpp
#include "../../string/sourcecode.hpp"
#include "../../string/text.hpp"
#include "namespace.hxx"
PSNVM_NAMESPACE_BEGIN
using namespace amtrs;

enum	accesstype
{
	access_none			= 0,
	access_private		= 1,
	access_protected	= 2,
	access_public		= 3,
};

enum class	opecode : int
{
	opecode_label	= -1,	// 疑似命令
	opecode_alloc	= 0,
	opecode_free,
	opecode_lea,			// lea	[mem]
	opecode_lea_r,			// lea	(rax|rbx|rcx), [mem]
	opecode_load_r,
	opecode_moff,
	opecode_reg_push,
	opecode_reg_pop,
	opecode_load,
	opecode_store,
	opecode_push_int,
	opecode_push_str,
	opecode_sizeof,
	opecode_add_i,
	opecode_sub_i,
	opecode_mul_i,
	opecode_imul_i,
	opecode_div_i,
	opecode_idiv_i,
	opecode_mod_i,
	opecode_imod_i,
	opecode_shl,
	opecode_shr,
	opecode_sar,
	opecode_cmpl,
	opecode_cmple,
	opecode_cmpg,
	opecode_cmpge,
	opecode_cmpe,
	opecode_cmpn,
	opecode_bit_and,
	opecode_bit_xor,
	opecode_bit_or,
	opecode_l_and,
	opecode_l_or,
	opecode_call,
	opecode_ret,
	opecode_jmp,
	opecode_jz,
	opecode_jnz,
	opecode_dup,
};
PSNVM_NAMESPACE_END
PSNVM_LANG_NAMESPACE_BEGIN
struct	codefunction;
class	compiler;


// ==================================================================
// 関数宣言時の引数
// ------------------------------------------------------------------
struct	parameterinfo
{
	text	type				= nullptr;		// 引数の型
	text	name				= nullptr;		// 仮引数名
	text	default_expression	= nullptr;		// デフォルト式

	bool empty() const noexcept { return type.empty() || name.empty(); }
};


using	parameterlist	= simplearray<parameterinfo>;


enum	expr_type
{
	et_none				= 0,
	st_none				= 0,		// 


	// https://en.cppreference.com/w/cpp/language/exprnode
	// Expression exprnode, Compound exprnode
	st_unparsed_expr,			// unparsed_expr
	st_substatements,			// compound exprnode witout {}
	st_compound,				// compound exprnode {}
	// Selection exprnode
	st_if,						// if() statement else statement
	st_switch,					// switch () {}
	// Iteration exprnode
	st_while,					// while (cond) statement, do statement while (cond);
	st_for,						// for(init; cond; inc) statement
	st_foreach,					// for(init; decl: val) statement
	// Jump exprnode
	st_break,					// break
	st_continue,				// continue
	st_return,					// return
	st_goto,					// goto
	// Declaration exprnode
	st_class_declare,			// class declare.
	st_function,				// function declare.
	st_function_prototype,		// function declare(not compound exprnode).
	st_variable,				// variable declare.

	// Try blocks

	// これ以降のノードは 2 pass 以降で処理できる
	et_statement_log,
	et_subexpression,
	et_static_number,
	et_static_string,
	et_reg_push,
	et_reg_pop,
	et_literal_number,
	et_literal_string,
	et_literal_sizeof,
	et_alloc_local,
	et_free_local,
	et_load_scalar,
	et_store_scalar,
	et_variable,
	et_variableref,
	et_lea_r,
	et_load_r,
	et_stackref,
	et_argumentref,
	et_localref,
	et_memberoff,
	et_function_name,
	et_invoke_function,
	et_scalar_copy,
	et_operator_new,
	et_operator_delete,
	et_operator_single,
	et_operator_bin,
	et_function_return,
	et_duplicate,
	et_statement_if,
	et_statement_for,
	et_typename_node,
};


enum class	value_type
{
	vt_none,			// 値無し(error etc..)
	vt_numeric,			// 数値
	vt_string,			// 文字列
	vt_identify,		// 識別名(変数)
	vt_expression,		// 何かしらの式(関数呼び出し、メンバ変数等)
	vt_function,		// 関数呼び出し
};



PSNVM_LANG_NAMESPACE_END
#endif
