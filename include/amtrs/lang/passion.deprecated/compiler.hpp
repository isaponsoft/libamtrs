/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__compiler__hpp
#define	__libamtrs__passion__compiler__hpp
#include "../../typeutil/callback.hpp"
#include "core-error.hpp"
#include "assembler.hpp"
#include "expr.hxx"
PSNVM_LANG_NAMESPACE_BEGIN
using namespace AMTRS_SOURCECODE_NAMESPACE;

class	compiler;
struct	codefunction;

enum compile_flags : uint32_t
{
	cf_document_mode		= 1<<0,			//!< enable document mode.
	cf_auto_return			= 1<<1,			//!< function add "return 0".
	cf_no_main				= 1<<2,			//!< no main
};

struct	argdef
{
	text	name;
	sstring	value;
};



// インタプリタ実行型関数
struct	codefunction : function
{
	struct	paramdata
	{
		sstring				datatype;		// 型
		sstring				name;			// 名前
		exprnode			defparam;		// デフォルトパラメータ
	};

	compiler*			mCompiler	= nullptr;
	codefunction(compiler* _compiler, sstring _name, sstring _sig)
		: function(std::move(_name), std::move(_sig))
		, mCompiler(_compiler)
	{}

	//インタプリタ実行
	static constexpr unsigned int		attr_const			= 1<<0;		// メンバ関数用／オブジェクトを変更しない
	static constexpr unsigned int		attr_static			= 1<<1;		// メンバ関数用／静的関数
	static constexpr unsigned int		attr_virtual		= 1<<2;		// メンバ関数用／仮想関数
	static constexpr unsigned int		attr_thiscall		= 1<<3;		// thisを持つメンバ関数
	static constexpr unsigned int		attr_constrxpr		= 1<<4;		// constexpr
	unsigned int					attributes;		// 属性
	simplearray<paramdata>			paramlist;		// パラメータリスト
	exprnode						stms;			// 関数内の文
};


class	compiler
		: public errorhandler
{
public:
	uint32_t									flags		= 0;
	text										scope		= nullptr;	// 現在のスコープ名
	text										code		= nullptr;	// 処理中のコード
	simplearray<sstring>						module_path;			// モジュールの検索パス
	simplearray<sstring>						imports;				// インポートされるモジュール
	simplearray<codefunction*>					functions;				// コンパイル中の関数一覧
	simplearray<argdef>							argdeflist;				// コマンドライン引数で宣言された define
	vmclassmgr									classes;
	typeutil::callback<expression_callback>		cb_expr;				// 不明な式を検出した時のコールバック

	struct	scopeguard
	{
		compiler*	cmp;
		text		name;
		scopeguard(compiler* c) : cmp(c), name(c->scope) {}
		~scopeguard() { cmp->scope = name; }
	};

	compiler(vmstate* _vm)
		: mVm(_vm)
		, classes(&_vm->classes)
	{}

	void reset()
	{
		functions.clear();
	}

	vmstate& vm()
	{
		return	*mVm;
	}

	text change(text _code)
	{
		std::swap(code, _code);
		return	_code;
	}

	// import process.
	bool import();

	bool has_code();


private:
	vmstate*					mVm;
};


// フェーズ１のパースを行います
// 構文解析を行い文ノードに展開します。文ノードではテンプレートや関数名の解析は行われず
// 文法の解析のみ行われます。
auto parse(compiler& _compiler, text _code, unsigned int _flags) -> simplearray<exprnode>;

// フェーズ２のパースを行います。
// 文ノードから式ノードへ変換します。
void validate(compiler& _compiler, simplearray<exprnode>& _node);


// 一致するパラメータが何番目にあるか調べる。
// 見つからない場合は負の値を返す。
int find_parameter(simplearray<codefunction::paramdata> const& _func, text _name);


sstring signature(codefunction* _func);

bool is_reserved_keyword(text _keyword);

// _func で指定する関数に対してソースコードをパースした結果を格納します。
PSNVM_API void parse(compiler& _compiler, codefunction* _func, text _code, bool _docmode = false);

PSNVM_API void parse(compiler& _compiler, codefunction* _func, bool _docmode = false);

// ==================================================================
// パースの１パス目を行い、_function->stms を生成します。
// ------------------------------------------------------------------
// 型名の解決など式には触れず、文法の解析のみを行います。
// ------------------------------------------------------------------
PSNVM_API void parse_pass1(compiler& _compiler, codefunction* _function, bool _docmode = false);
PSNVM_API void parse_pass1(compiler& _compiler, codefunction* _function, text _code, bool _docmode = false);


// ==================================================================
// codefunctionをコンパイルし、バイトコードを持った関数を生成します。
// ------------------------------------------------------------------
PSNVM_API function_bc* parse_pass2(compiler& _compiler, codefunction* _function);


// デフォルトコンストラクタを検索する
function* find_default_ctor(compiler& _compiler, codefunction* _function, text _datatype);

// コンストラクタを列挙する
simplearray<function*> enumrate_ctors(compiler& _compiler, codefunction* _current, text _name);

// 指定する型からのキャストが可能なコンストラクタを列挙する
// この関数は単一の引数のみを扱う
simplearray<function*> enumrate_cast(compiler& _compiler, codefunction* _current, text _name, text _from);

PSNVM_API simplearray<function*> enumrate_functions(compiler& _compiler, codefunction* _current, text _name);


// アセンブリを出力します
PSNVM_API void compile(compiler& _compiler, codefunction* _function);
PSNVM_API function_bc* compile(codefunction* _function, exprnode _e);



simplearray<psnvm::function_bc*> compile(lang::compiler& _compiler, text _code, bool _module);

PSNVM_LANG_NAMESPACE_END
#endif
