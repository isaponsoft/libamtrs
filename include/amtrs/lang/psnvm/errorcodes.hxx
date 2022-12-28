PSNVMPARSER_ERRORCODE(ec_success,												0)

PSNVMPARSER_UNTERMERROR(ec_unkown_error,										1)			// 未定義や作業中のエラー
PSNVMPARSER_UNTERMERROR(ec_unkown_token,										2)
PSNVMPARSER_UNTERMERROR(ec_codeblock_unclosed,									3)
PSNVMPARSER_UNTERMERROR(ec_nothing_token,										4)
PSNVMPARSER_UNTERMERROR(ec_circlebracket_unclosed,								5)
PSNVMPARSER_UNTERMERROR(ec_qauarebracket_unclosed,								6)
PSNVMPARSER_UNTERMERROR(ec_unkown_symbol,										7)


// symbol
PSNVMPARSER_UNTERMERROR(ec_nothing_scope_right,									40)
PSNVMPARSER_UNTERMERROR(ec_template_has_not_params,								41)
PSNVMPARSER_UNTERMERROR(ec_decltype_has_not_expr,								42)


// declare class
PSNVMPARSER_UNTERMERROR(ec_class_classname_nothing,								101)
PSNVMPARSER_UNTERMERROR(ec_class_terminate_nothing,								102)
PSNVMPARSER_UNTERMERROR(ec_class_baseclass_nothing,								103)

// declare using
PSNVMPARSER_UNTERMERROR(ec_using_aliasname_nothing,								200)
PSNVMPARSER_UNTERMERROR(ec_using_alias_separator_nothing,						201)
PSNVMPARSER_UNTERMERROR(ec_using_alias_typeid_nothing,							202)
PSNVMPARSER_UNTERMERROR(ec_using_terminate_nothing,								203)
PSNVMPARSER_UNTERMERROR(ec_using_namespace_scope_nothing,						204)
PSNVMPARSER_UNTERMERROR(ec_using_namespace_terminate_nothing,					205)


// declare variable or function
PSNVMPARSER_UNTERMERROR(ec_declare_terminate_nothing,							300)
PSNVMPARSER_UNTERMERROR(ec_declare_varname_nothing,								301)		// 変数宣言の , の後ろに変数名が無い
PSNVMPARSER_UNTERMERROR(ec_declare_function_parameter_notclosed,				302)		// 関数パラメータの () が閉じられていない
PSNVMPARSER_UNTERMERROR(ec_declare_function_have_intializers,					303)		// 非コンストラクタに初期化リストがある
PSNVMPARSER_UNTERMERROR(ec_declare_variable_initexpr_nothing,					304)		// 変数宣言に = があるのに値が無い
PSNVMPARSER_UNTERMERROR(ec_declare_constractor_intializers_varname_nothing,		305)		// コンストラクタの初期化リストに名前が無い
PSNVMPARSER_UNTERMERROR(ec_declare_constractor_intializers_value_nothing,		306)		// コンストラクタの初期化リストに値が無い
PSNVMPARSER_UNTERMERROR(ec_declare_constractor_intializers_value_unclosed,		307)
PSNVMPARSER_UNTERMERROR(ec_declare_constractor_codeblock_nothing,				308)
PSNVMPARSER_UNTERMERROR(ec_declare_function_after_returntype_not_auto,			309)		// auto f() -> ret が指定されたが auto ではない 
PSNVMPARSER_UNTERMERROR(ec_declare_function_after_returntype_nothing,			319)		// auto f() -> ret が指定されたが ret が無い
PSNVMPARSER_UNTERMERROR(ec_declare_functin_codeblock_unclosed,					320)
PSNVMPARSER_UNTERMERROR(ec_declare_functin_terminate_nothing,					321)
PSNVMPARSER_UNTERMERROR(ec_declare_constractor_intializers_unkown_symbol,		322)		// 不明なシンボル
PSNVMPARSER_UNTERMERROR(ec_declare_function_duplicate,							323)		// 関数の２重定義
PSNVMPARSER_UNTERMERROR(ec_declare_function_nothing_return,						324)		// returnが無い
PSNVMPARSER_UNTERMERROR(ec_declare_function_nothing_override_base,				325)		// override すべき仮想関数がない
PSNVMPARSER_UNTERMERROR(ec_declare_function_ambitious_override_base,			326)		// override すべき仮想関数が複数見つかった
PSNVMPARSER_UNTERMERROR(ec_declare_function_final_override,						327)		// final を override しようとした


// declare template (class member)
PSNVMPARSER_UNTERMERROR(ec_declare_template_not_paramlist,						400)		// クラスメンバ内でtemplateの後ろにパラメータリストが無い
PSNVMPARSER_UNTERMERROR(ec_declare_template_paramlist_error,					401)
PSNVMPARSER_UNTERMERROR(ec_declare_template_unkown_statement_error,				402)


// declare enum
PSNVMPARSER_UNTERMERROR(ec_enum_typename_nothing,								500)
PSNVMPARSER_UNTERMERROR(ec_enum_terminate_nothing,								501)
PSNVMPARSER_UNTERMERROR(ec_enum_varname_nothing,								502)
PSNVMPARSER_UNTERMERROR(ec_enum_value_nothing,									503)
PSNVMPARSER_UNTERMERROR(ec_enum_unkown_token,									504)
PSNVMPARSER_UNTERMERROR(ec_enum_base_nothing,									505)

// expression
PSNVMPARSER_UNTERMERROR(ec_expr_cant_invoke_scalar,								600)
PSNVMPARSER_UNTERMERROR(ec_expr_nothing_invoke_operator,						601)
PSNVMPARSER_UNTERMERROR(ec_expr_unmatch_function_paramtypes,					602)
PSNVMPARSER_UNTERMERROR(ec_expr_cant_call_thiscall,								603)
PSNVMPARSER_UNTERMERROR(ec_expr_cant_access_controll,							604)
PSNVMPARSER_UNTERMERROR(ec_expr_cant_access_static_member,						605)


// statment
PSNVMPARSER_UNTERMERROR(ec_for_circle_unopened,									1000)
PSNVMPARSER_UNTERMERROR(ec_return_unmatch_type,									1001)
PSNVMPARSER_UNTERMERROR(ec_return_not_void,										1002)
PSNVMPARSER_UNTERMERROR(ec_return_nothing,										1003)


// prepro
PSNVMPARSER_UNTERMERROR(ec_prepro_has_not_macroname,							2000)
PSNVMPARSER_UNTERMERROR(ec_prepro_not_defined_undef,							2001)
PSNVMPARSER_UNTERMERROR(ec_prepro_error,										2002)
PSNVMPARSER_UNTERMERROR(ec_prepro_message,										2003)
PSNVMPARSER_UNTERMERROR(ec_prepro_include_syntax,								2004)
PSNVMPARSER_UNTERMERROR(ec_prepro_file_not_found,								2005)
PSNVMPARSER_UNTERMERROR(ec_prepro_else_duplicated,								2006)
PSNVMPARSER_UNTERMERROR(ec_prepro_defind_has_not_name,							2007)
PSNVMPARSER_UNTERMERROR(ec_prepro_nothing_closing_circle_brackets,				2008)
PSNVMPARSER_UNTERMERROR(ec_prepro_nothing_closing_square_brackets,				2009)
PSNVMPARSER_UNTERMERROR(ec_prepro_nothing_closing_curry_brackets,				2010)

