/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__sourcecode__hpp
#define	__libamtrs__string__sourcecode__hpp
#include "../memory/bufferif.h"
#include "text.hpp"
#include "sstring.hpp"
AMTRS_SOURCECODE_NAMESPACE_BEGIN

/*
 * 汎用的なプログラムのソースコードを読み込むためのライブラリ
 */


/*
 * 解析時のエラーコード
 */
enum class	sourcecode_error
{
	success				= 0,			// 成功
	unkown_error		= 1,			// 不明なエラー
	no_start_token		= 2,			// 開始トークンが見つからなかった。このエラーが発生した場合は処理を続けても構わない。
	no_end_token		= 3,			// 終了トークンが見つからなかった。
	syntax_error		= 4,			// 開始トークンは見つかったが構文が不正。
	unsupported			= 5,			// 未サポートまたは使用方法が間違っている。
};




#define	_OPERATOR_DEF(_pri, _num)		((_num << 8) + _pri)
enum operator_type
{
	// Precedence 1
	ot_scope				= _OPERATOR_DEF(1, 1),			// Left-to-right :: 
	// Precedence 2
	ot_inc_suffix			= _OPERATOR_DEF(2, 1),			// a++
	ot_dec_suffix			= _OPERATOR_DEF(2, 2),			// a--
	ot_functional_cast		= _OPERATOR_DEF(2, 3),			// type(), type{}
	ot_function_call		= _OPERATOR_DEF(2, 4),			// a()
	ot_subscript			= _OPERATOR_DEF(2, 5),			// a[]
	ot_member_access		= _OPERATOR_DEF(2, 6),			// .
	ot_member_access_arrow	= _OPERATOR_DEF(2, 7),			// ->
	// Precedence 3
	ot_inc_prefix			= _OPERATOR_DEF(3, 1),			// ++a
	ot_dec_prefix			= _OPERATOR_DEF(3, 2),			// --a
	ot_plus					= _OPERATOR_DEF(3, 3),			// +a
	ot_minus				= _OPERATOR_DEF(3, 4),			// -a
	ot_logical_not			= _OPERATOR_DEF(3, 5),			// !
	ot_bitwise_not			= _OPERATOR_DEF(3, 6),			// ~
	ot_c_style_cast			= _OPERATOR_DEF(3, 7),			// (type)
	ot_indirection			= _OPERATOR_DEF(3, 8),			// *a
	ot_address_of			= _OPERATOR_DEF(3, 9),			// &a
	ot_size_of				= _OPERATOR_DEF(3, 10),			// sizeof
	ot_await_expression		= _OPERATOR_DEF(3, 11),			// co_await
	ot_dynamic_allocation	= _OPERATOR_DEF(3, 12),			// new, new[]
	ot_dynamic_deallocation	= _OPERATOR_DEF(3, 13),			// delete, delete[]
	// Precedence 4
	ot_pointer_to_member	= _OPERATOR_DEF(4, 1),			// .*   ->*
	// Precedence 5
	ot_multiplication		= _OPERATOR_DEF(5, 1),			// a*b
	ot_division				= _OPERATOR_DEF(5, 2),			// a/b
	ot_remainder			= _OPERATOR_DEF(5, 3),			// a%b
	// Precedence 6
	ot_addition				= _OPERATOR_DEF(6, 1),			// a+b
	ot_subtraction			= _OPERATOR_DEF(6, 2),			// a-b
	// Precedence 7
	ot_left_shift			= _OPERATOR_DEF(7, 1),			// <<
	ot_right_shift			= _OPERATOR_DEF(7, 2),			// >>
	// Precedence 8
	ot_three_way_compare	= _OPERATOR_DEF(8, 1),			// <=>
	// Precedence 9
	ot_leter				= _OPERATOR_DEF(9, 1),			// <
	ot_leter_equals			= _OPERATOR_DEF(9, 2),			// <=
	ot_grater				= _OPERATOR_DEF(9, 3),			// >
	ot_grater_equals		= _OPERATOR_DEF(9, 4),			// >=
	// Precedence 10
	ot_equals				= _OPERATOR_DEF(10, 1),			// ==
	ot_not_equals			= _OPERATOR_DEF(10, 2),			// !=
	// Precedence 11
	ot_bitwise_and			= _OPERATOR_DEF(11, 1),			// &
	// Precedence 12
	ot_bitwise_xor			= _OPERATOR_DEF(12, 1),			// ^
	// Precedence 13
	ot_bitwise_or			= _OPERATOR_DEF(13, 1),			// |
	// Precedence 14
	ot_logical_and			= _OPERATOR_DEF(14, 1),			// &&
	// Precedence 15
	ot_logical_or			= _OPERATOR_DEF(15, 1),			// ||
	// Precedence 16
	ot_ternary_conditional	= _OPERATOR_DEF(16, 1),			// a?b:c
	ot_throw				= _OPERATOR_DEF(16, 2),			// throw
	ot_co_yield				= _OPERATOR_DEF(16, 3),			// yield
	ot_assign				= _OPERATOR_DEF(16, 4),			// =
	ot_assign_sum			= _OPERATOR_DEF(16, 5),			// +=
	ot_assign_difference	= _OPERATOR_DEF(16, 6),			// -=
	ot_assign_product		= _OPERATOR_DEF(16, 7),			// *=
	ot_assign_quotient		= _OPERATOR_DEF(16, 8),			// /=
	ot_assign_remainder		= _OPERATOR_DEF(16, 9),			// %=
	ot_assign_left_shift	= _OPERATOR_DEF(16, 10),		// <<=
	ot_assign_right_shift	= _OPERATOR_DEF(16, 11),		// >>=
	ot_assign_and			= _OPERATOR_DEF(16, 12),		// &=
	ot_assign_xor			= _OPERATOR_DEF(16, 13),		// ^=
	ot_assign_or			= _OPERATOR_DEF(16, 14),		// |=
	// Precedence 16
	ot_comma				= _OPERATOR_DEF(17, 1),			// ,

	ot_unary_right_fold		= _OPERATOR_DEF(20, 1),			// expr...
	ot_unary_left_fold		= _OPERATOR_DEF(20, 2),			// ...expr

	//
	ot_none					= 99999,						// operator is none. This is single value.
};


struct	srcdata
{
	text	code;				// マッチした全体
	text	next;				// 次に読み込むべき位置
};

template<class T>
struct	readresult : result<T>
{
	using	super_type	= result<T>;
	text	next;				// 次に読み込むべき位置

	readresult(errorval e) : super_type(std::move(e)) {}
	readresult(errorval e, T val) : super_type(std::move(e), std::move(val)) {}

	template<class... Args>
	readresult(Args&&... _args) : super_type(std::forward<Args>(_args)...) {}
};

/*
 * 数値リテラル
 */
struct	numeric_literal : srcdata
{
	enum numeric_type
	{
		nt_decimal	= 10,		// base 10 - [1-9][0-9]*
		nt_hex		= 16,		// base 16 - 0[xX][0-9a-fA-F]+
		nt_oct		= 8,		// base 8  - 0[0-7]+
		nt_bin		= 2,		// base 2  - 0[bB][01]+
	};

	numeric_type	type;			// 種類
	text			code_int;		// 整数部
	text			code_fraction;	// 小数部
	text			code_exponent;	// 指数部
	bool empty() const noexcept { return code.empty(); }
};



/*
 * 文字列リテラル
 */
struct	string_literal : srcdata
{
	enum string_type
	{
		st_basic		= 0,
		st_rawstring	= 1,
		st_hiredoc		= 2,
	};

	string_type		type;
	text			str;				// ダブルコーテーションなどの記号を除いた部分
	char			indent_char	= 0;	// インデントがある場合のインデント文字
	size_t			indent_size	= 0;	// インデントがある場合のインデントの数
	bool empty() const noexcept { return code.empty(); }

	// エスケープコードなどをデコードし文字列データにします
	errorval decode(amtrs_bufferif_one_init _destinate) const noexcept;

	template<class T>
	result<T> decode() const noexcept
	{
		T		val;
		auto	r	= decode(amtrs_bufferif_one_init
		{
			&val,
			[](void* _object, size_t _size) -> void*
			{
				T*	dest	= reinterpret_cast<T*>(_object);
				bufferif_one_init_traits<T>::allocate(dest, _size);
				return	dest->data();
			}
		});
		return	result<T>(std::move(r), std::move(val));
	}
};




struct	sourcelocation
{
	text		filename;
	char const*	linetop;
	int			line;
	int			cols;
};



/*
 * 演算子
 */
struct	operatordata : text
{
	using	text::text;
	using	text::operator =;
	using	text::operator ==;
	using	text::operator !=;

	constexpr operatordata() = default;

	operator_type	type	= operator_type::ot_none;
};


template<class T>
struct	readdata
{
	text	next	= nullptr;
	T		val;

	readdata() = default;
	readdata(readdata const&) = default;
	readdata(readdata&&) = default;
	readdata& operator = (readdata const&) = default;
	readdata& operator = (readdata&&) = default;
	bool good() const noexcept { return next.good(); }
	bool bad() const noexcept { return next.bad(); }
	operator bool () const noexcept { return good(); }

	readdata(text _next, T _val)
		: next(_next)
		, val(std::move(_val))
	{}
};

/*
 * Source code engine を初期化します。
 * (エラーコードをを登録します)
 */
AMTRS_API void sourcecode_parser_initialize();

/*
 * 数値を読み取ります。
 */
AMTRS_API result<numeric_literal> read_numeric(text _code, unsigned int _flags = 0U);



AMTRS_API char const* read_numeric_literal(numeric_literal& _result, text _code);
AMTRS_API numeric_literal read_numeric_literal(text _code);

/*
 * 文字列を読み取ります。
 */
AMTRS_API result<string_literal> read_string(text _code, unsigned int _flags = 0U);

/*
 * 括弧を読み取ります。
 * () [] {} のどれかがあれば抜き出します。
 */
AMTRS_API result<srcdata> read_brackets(text _code, unsigned int _flags = 0U);


/*
 * 指定した文字で分割します。
 * split_chars() などと異なり、文字列や括弧を考慮します。
 */
AMTRS_API result<simplearray<text>> parsed_split(text _code, char _delm);


/*
 * _start から計算して _location_default が示す位置の行カウント、カラム位置を計算する。
 * カウントアップした数を示すため、改行が含まれない場合は line=0 になるので注意。
 * コード中に #line が見つかった場合はそれに従う。また、filename は #line が
 * 見つからない場合は nullptr をセットする。
 */
AMTRS_API sourcelocation compute_location(text _code, char const* _location_default, int _line = 1);




static constexpr unsigned int	tonum_flag_fraction_round_off	= 1<<0;	//!< 整数変換時に小数部を切り捨てる
static constexpr unsigned int	tonum_flag_fraction_rounding	= 1<<1;	//!< 整数変換時に小数部を四捨五入する

/*
 * intmax_t に変換します。
 * tonum_flag の組み合わせを指定できます。
 * tonum_flag_fraction_round_off/tonum_flag_fraction_rounding が指定されていないときは
 * 小数部が現れるとエラー扱いになります。
 * また、数値が intmax_t で表現できない場合もエラー扱いになります。	
 *
 * 0xで始まる文字列は１６進数、0で始まる文字列は８進数、0bは２進数として扱います。
 *
 * エラーになるケース
 *		数値として認識できない文字が現れた時
 *		桁が溢れてしまった時
 */
AMTRS_API intmax_t to_intmax(bool& _success, text _code, unsigned int _flags = 0);


/*
 * 識別子を抜き出す
 * この関数では ([A-Za-z_][A-Za-z0-9_]*) のみであり、名前空間やスペースなどは考慮しない。
 */
AMTRS_API char const* read_identify_token(text& _result, text _code);
AMTRS_API text read_identify_token(text _code);

/*
 * エスケープをデコードします。
 */
AMTRS_API sstring decode_escape_string(text _literal);

/*
 * 改行コードなどをエスケープします
 */
AMTRS_API sstring encode_escape_string(text _string);



static inline int operator_priority(operator_type _ot) noexcept { return (int)_ot & 0xff; }

AMTRS_API char const* get_operator_string(operator_type _ot);

AMTRS_API operatordata read_operator(text _code);

/*
 * 2項演算子を読み取る。
 */
AMTRS_API char const* read_binary_operator(operatordata& exp, text _code);
AMTRS_API operatordata read_binary_operator(text _code);

/*
 * 前置型の演算子を読み取る。
 */
AMTRS_API operatordata read_prefix_operator(text _code);

// 改行コードを空白に置き換えたものを生成して返します
AMTRS_API sstring replace_return_to_space(text _text);

bool is_assign_operator(operator_type _ope);
bool is_bin_operator(operator_type _ope);


AMTRS_SOURCECODE_NAMESPACE_END
#endif
