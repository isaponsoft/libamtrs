/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */#ifndef	__libamtrs__lang__passion__types__hpp
#define	__libamtrs__lang__passion__types__hpp
#include "../../memory/ref_object.hpp"
#include "../../memory/simplearray.hpp"
#include "../../string/sstring.hpp"
#include "../../string/text.hpp"
#include "namespace.hpp"
PASSION_NAMESPACE_BEGIN


static constexpr char const	spaces[]	= " \t\r\n";

enum class	expr_types
{
	none				= 0,
	identify			= 1,		// `name`
	name_and_type		= 2,		// `name`:expr
	number				= 3,		// $1234
	string				= 4,		// "string"
	command				= 5,		// (identify
	c_function			= 6,		// (function [(parameters)] [expr...])
	c_parameters		= 7,		// (parameters `name`:expr...)
	c_decltype			= 8,		// (decltype expr)
	c_return			= 9,		// (return expr)
	c_class				= 10,		// (class `name` (extends) [expr...])
	c_extends			= 11,		// (extends `name`:(extendsattr)...)
	c_extendsattr		= 12,		// (extendsattr [private|protected:public])
	c_single_operator	= 13,		// (lnot|reverse|minus)
	c_bin_operator		= 14,		// (add|sub|mul|div|mod|shr|shl|sar|sal|and|or|xor|eq|ne|grater|letter|ge|le|lor|land|ship expr expr)
	c_call				= 15,		// (call `function-symbol` expr...)
	c_callm				= 16,		// (callm `function-symbol` this-expr expr...)
	c_signature			= 17,		// (signature)
};


enum class	bin_operator_types
{
	o_add			= 0,
	o_sub			= 1,
	o_mul			= 2,
	o_div			= 3,
	o_mod			= 4,
	o_imul			= 5,
	o_idiv			= 6,
	o_imod			= 7,
	o_bit_shl		= 8,
	o_bit_shr		= 9,
	o_bit_sar		= 10,
	o_bit_and		= 11,
	o_bit_xor		= 12,
	o_bit_or		= 13,
};


enum class	extends_attr_types
{
	a_default		= 0,		// not choise
	a_private		= 1,		// `private`
	a_protected		= 2,		// `protected`
	a_public		= 3,		// `public`
};


enum class	psnfl_errno
{
	success							= 0,
	no_open							= 1,		// ( nothing
	no_close						= 2,		// ( nothing
	no_identify						= 3,		// nothing keyword after (
	no_identify_space				= 4,		// 
	no_close_token					= 5,		// ` nothing
	no_type							= 6,		// `name`: TYPE nothing
	no_numeric						= 7,		// #numeric error
	function_hasnot_name			= 8,		// function has not name
	function_hasnot_returntype		= 9,		// function has not return type
	function_mis_parameters			= 10,		// function not parameters
	parameters_not_name_and_type	= 11,		// parameters unkown node
	decltype_hsnot_expr				= 12,		// decltype has not expression
	decltype_many_expr				= 13,		// decltyoe have a many expression
	return_many_expr				= 14,		// return have a many expression
	class_has_not_name				= 15,		// class has not classname
	extends_unkown_node				= 16,		// (extends) have a unkown child node.
	extends_not_attribute			= 17,		// (extends) `name`:(extendsattr) is not extendsattr
	extendsattr_unkown_node			= 18,		// (extendsattr) have a unkown child node.
	extendsattr_unkown_attribute	= 19,		// (extendsattr) unkown attribute.
	bin_operator_has_not_expression	= 20,		// (add|sub...) has not 2 expression.
	no_string_closed,
};


struct	psnfl_error
{
	psnfl_errno	err			{psnfl_errno::success};
	char const*	location	{nullptr};
	text		token1		{nullptr};
	text		token2		{nullptr};

	psnfl_error() = default;
	psnfl_error(psnfl_error const&) = default;
	psnfl_error(psnfl_error&&) = default;
	psnfl_error& operator = (psnfl_error const&) = default;
	psnfl_error& operator = (psnfl_error&&) = default;
	psnfl_error(psnfl_errno _err, char const* _locate)
		: err(_err)
		, location(_locate)
	{}
};


struct	psnfl_status
{
	simplearray<psnfl_error>	errors;

	bool bad() const noexcept { return !good(); }
	bool good() const noexcept { return errors.empty(); }


	void error(psnfl_errno _err, char const* _locate) { errors.push_back(psnfl_error(_err, _locate)); }
};


PASSION_NAMESPACE_END
#endif
