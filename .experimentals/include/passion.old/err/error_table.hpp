/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
PSNEC_DEFINE(9999, compiler_error,		"compiler error. error token '%s'")				// プログラムのバグ(アサート的なもの)

// 実行時
PSNEC_DEFINE(1, err_operation_bad_type_error,		"")	// オブジェクト同士の足し算など対応していない演算が行われた

// 語句解析
PSNEC_DEFINE(100, err_not_close_string_expression,		"")	// 文字列への式埋め込みが閉じられていない


// 式の基本
PSNEC_DEFINE(1000, err_not_close_parenthesis,		"")	// ) が無い。
PSNEC_DEFINE(1001, err_can_not_assign_type,						"Can't assign. No variable convation '%s' <- '%s'.")		// 代入しようとしたが型変換できない。
PSNEC_DEFINE(1002, excepted,									"expected '%s'")						// 後ろにキーワードが必要
PSNEC_DEFINE(1003, excepted_after,								"expected '%s' after '%s'")				// キーワードの後ろにキーワードが必要
PSNEC_DEFINE(1004, excepted_expression,							"expected expression '%s'")				// 後ろにキーワードが必要


PSNEC_DEFINE(1005, declaration_of_reference_variable,				"declaration of reference variable '%s' requires an initializer")				// 後ろにキーワードが必要
PSNEC_DEFINE(1006, declaration_of_reference_variable_no_variable,	"declaration of reference variable '%s' requires an initializer")				// 後ろにキーワードが必要
PSNEC_DEFINE(1007, extraneous_before,								"extraneous ')' before ';'")


// 配列
PSNEC_DEFINE(2000, err_array_size_not_integral,		"")	// 配列のサイズ式にint以外が渡されていません。
PSNEC_DEFINE(2001, err_array_size_not_closed,		"")	// 配列の [] が閉じられていません。

// メンバ
PSNEC_DEFINE(3001, err_member_nothing,							"")			// 指定されたメンバがない。
PSNEC_DEFINE(3002, member_is_not_pointer,						"member reference type '%s' is not a pointer; did you mean to use '.'?")	// 指定されたメンバがない。
PSNEC_DEFINE(3003, member_is_pointer,							"mwmber reference type '%s' is a pointer; did you mean to use '->'")									// ポンターに対して . でアクセスしようとした
PSNEC_DEFINE(3004, member_access_denay,							"'%s' is a %s member of '%s'")												// アクセス権が無い


// 演算
PSNEC_DEFINE(4001, condition_type_unmatch,						"unmatch conditional operator type. '%s' and '%s'")	// ?: で返される値の型が異なる
PSNEC_DEFINE(4002, not_found_operator,							"'%s::operator %s (%s)' not found")						//演算子がない
PSNEC_DEFINE(4003, value_is_not_convertible,					"value of type '%s' is not contextually convertible to '%s'")

// ローカル変数
PSNEC_DEFINE(5001, local_var_declare_after_unkown_token,		"Unexpected code after variable definition '%s'.")	// ?: で返される値の型が異なる
PSNEC_DEFINE(5002, expected_unqualified_id,						"expected unqualified-id. token '%s'.")				// ?: で返される値の型が異なる


// クラス
PSNEC_DEFINE(5100, redeclare_class,								"redeclaration class '%s'")		//!< クラスを再定義しようとした。
PSNEC_DEFINE(5101, declaration_class_unkown_token,				"class '%s' has unknown token '%s'. require ';' or ':' or '{'.")		//!< ; : { のどれかが必要
PSNEC_DEFINE(5102, declaration_class_has_not_open,				"class '%s' has unknown token '%s'. require '{'.")						//!< { が必要
PSNEC_DEFINE(5103, unkown_token_in_class_block,					"class '%s' declare block has unknown token '%s'.")									//!< クラスブロックに不明なトークン。
PSNEC_DEFINE(5104, expected_member_name_or,						"expected member name or '%s' after declaration specifiers")



// 関数呼び出し
PSNEC_DEFINE(5200, function_not_found,							"function '%s' not found.")										//!< 関数が見つからない
PSNEC_DEFINE(5201, function_argument_unmatch,					"function '%s' is unmatch argument. '%s'")						//!< 関数の引数が一致しない
PSNEC_DEFINE(5202, function_did_arguments,						"reference to overloaded function '%s' could not be resolved; did you mean to call it with no arguments?")

// 名前解決
PSNEC_DEFINE(5300, unkown_type_name,							"unkown type name '%s'")										//!< 不明な型名
PSNEC_DEFINE(5301, member_not_found,							"member not found name '%s' class '%s'")

// 文
PSNEC_DEFINE(5400, break_not_in,								"'%s' statement not in loop or switch statement")
PSNEC_DEFINE(5401, continue_not_in,								"'%s' statement not in loop statement")
PSNEC_DEFINE(5402, code_will_never_be_executed,					"code will never be executed")									//!< 永久に実行されないコード
PSNEC_DEFINE(5403, expression_is_not_assignable,				"expression is not assignable")


// プリプロセッサ
PSNEC_DEFINE(7001, macro_not_identifier,						"macro name must be an identifier '%s'")
PSNEC_DEFINE(7002, macro_name_missing,							"macro name missing")
PSNEC_DEFINE(7003, expected_comma_in_macro_parameter_list,		"expected comma in macro parameter list before '%s'")
PSNEC_DEFINE(7004, invalid_preprocessing_directive,				"invalid preprocessing directive '%s'")
PSNEC_DEFINE(7005, token_is_not_a_valid_binary_operator_in_a_preprocessor_subexpression,	"token is not a valid binary operator in a preprocessor subexpression. '%s'")




// 廃止予定
PSNEC_DEFINE(9998, _dummy_,		"")						// まだエラーコードを振っていないダミーのエラーコード
PSNEC_DEFINE(9997, type_name_nothing,		"")			// 名前があるべき場所に名前がない
PSNEC_DEFINE(9996, value_name_nothing,		"")			// 名前があるべき場所に名前がない
PSNEC_DEFINE(9995, function_name_nothing,		"")		// 関数名もしくはメソッド名が無い
PSNEC_DEFINE(9994, nothing_open_bracket,		"")		// {) () [ が無い
PSNEC_DEFINE(9993, nothing_close_bracket,		"")		// }) )) ] が無い
PSNEC_DEFINE(9992, nothing_open_circle,		"")			// ( が無い
PSNEC_DEFINE(9991, nothing_close_circle,		"")		// ) が無い
PSNEC_DEFINE(9990, class_name_nothing,		"")			// クラス名が必要な場所にクラス名が見つからなかった
PSNEC_DEFINE(9989, comma_or_name_nothing,		"")		// ) もしくは名前が必要な場所にカンマがない
PSNEC_DEFINE(9988, unkown_token,		"")				// 予期せぬトークン(キーワードによって処理が変わるような文の中で想定しない語句が現れた)
