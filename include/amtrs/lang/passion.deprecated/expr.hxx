/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__expr__hpp
#define	__libamtrs__passion__expr__hpp
#include "types.hxx"
#include "vm.hpp"
PSNVM_LANG_NAMESPACE_BEGIN
class	compiler;
struct	codefunction;
struct	exprstatus;


template<class T> struct __is_type { static bool is(decltype(T::type) t) noexcept { return t == T::type; } };
sstring const& void_str();
sstring const& empty_str();


struct	exprbase
{
	using	flag_type	= expr_type;
	virtual ~exprbase() {}

	flag_type	tflg		= static_cast<flag_type>(0);	// ノードの型
	sstring		datatype;									// 式の結果を示す型
	text		code;										// 対応しているコード
	char const*	location	= nullptr;
	sstring		message;
};

struct	exprnode
{
	using	flag_type	= expr_type;
	using	node_type	= exprbase;
	using	this_type	= exprnode;

	// Factory(expr)
	template<class T, class... Args>
	static this_type make(sstring _datatype, char const* _location, Args&&... _args)
	{
		node_type*	n	= new T(std::forward<Args>(_args)...);
		n->tflg		= T::type;
		n->datatype	= std::move(_datatype);
		n->location	= std::move(_location);
		return	this_type(n);
	}

	// Factory(stm)
	template<class T>
	static this_type make()
	{
		node_type*	n	= new T();
		n->tflg		= T::type;
		return	this_type(n);
	}


	exprnode() = default;
	exprnode(exprnode&& r) { swap(r); }
	exprnode(exprnode const&) = delete;
	~exprnode() { clear(); }
	exprnode& operator = (exprnode const&) = delete;
	exprnode& operator = (exprnode&& r) { clear(); swap(r); return *this; }
	void swap(exprnode& r) noexcept { std::swap(mNode, r.mNode); }
	bool empty() const noexcept { return mNode == nullptr; }
	flag_type type() const noexcept { return mNode ? mNode->tflg : static_cast<flag_type>(0); }
	text code() const noexcept { return empty() ? text(nullptr) : mNode->code; }
	char const* location() const noexcept { return mNode->location; }
	void location(char const* _locate) const noexcept { if (!empty()) { mNode->location = _locate; } }
	void clear() { if (mNode) { delete mNode; mNode = nullptr; } }
	node_type* get() const noexcept { return mNode; }
	sstring const& datatype() const noexcept { return empty() ? void_str() : mNode->datatype; };

	sstring const& message() const noexcept { return empty() ? empty_str() : get()->message; }
	void message(sstring _msg) const noexcept { if (!empty()) { get()->message = std::move(_msg); } }

	template<class T> T* as() const noexcept { return __is_type<T>::is(type()) ? static_cast<T*>(mNode) : nullptr; }
	template<class T> bool is() const noexcept { return __is_type<T>::is(type()); }

protected:
	exprnode(node_type* _node)
		: mNode(_node)
	{}
	node_type*	mNode	= nullptr;
};




using namespace AMTRS_SOURCECODE_NAMESPACE;

namespace expr {


// 複数の式をまとめます。主に自動変換処理などの式に使われます。
struct	subexpression : exprbase
{
	static constexpr expr_type	type	= et_subexpression;
	subexpression(simplearray<exprnode> _subs)
		: children(std::move(_subs))
	{}
	simplearray<exprnode>	children;

	static exprnode make(sstring _datatype, char const* _location, simplearray<exprnode> _list)
	{
		return	exprnode::make<subexpression>(std::move(_datatype), _location, std::move(_list));
	}
};



// 静的演算済み数値
struct	static_number : exprbase
{
	static constexpr expr_type	type	= et_static_number;
	intmax_t	num;
	static_number(intmax_t n);
	~static_number();

	static exprnode make(char const* _locate, intmax_t _n)
	{
		return	exprnode::make<static_number>("int", _locate, _n);
	}
};

// 静的演算済み文字列
struct	static_string : exprbase
{
	static constexpr expr_type	type	= et_static_string;
	sstring		str;
	static_string(sstring v) : str(std::move(v)) {}
};


// 数値リテラル
struct	literal_number : static_number, numeric_literal
{
	static constexpr expr_type	type	= et_literal_number;
	literal_number(intmax_t n, numeric_literal _r)
		: static_number(n)
		, numeric_literal(std::move(_r))
	{}

	static exprnode make(char const* _location, intmax_t n, numeric_literal _r)
	{
		return	exprnode::make<literal_number>("int", _location, n, std::move(_r));
	}

	static exprnode make(char const* _location, intmax_t n)
	{
		numeric_literal	tmp;
		tmp.type	= (numeric_literal::numeric_type)0;
		return	make(_location, n, tmp);
	}
};

// 文字列リテラル
struct	literal_string : static_string, string_literal
{
	static constexpr expr_type	type	= et_literal_string;
	literal_string(sstring v, string_literal l)
		: static_string(std::move(v))
		, string_literal(std::move(l))
	{}
};

// sizeofリテラル。データサイズは実行環境によって変わるので中間コードでは名前で保持する
struct	literal_sizeof : exprbase
{
	static constexpr expr_type	type	= et_literal_sizeof;
	literal_sizeof(sstring _sizetype)
		: sizetype(std::move(_sizetype))
	{}
	sstring	sizetype;

	static exprnode make(char const* _locate, sstring _target_type)
	{
		return	exprnode::make<literal_sizeof>("int", _locate, std::move(_target_type));
	}
};




// ローカル領域の確保
struct	alloc_local : exprbase
{
	static constexpr expr_type	type	= et_alloc_local;
	size_t		size;
	sstring		name;
	alloc_local(sstring _name, size_t _n) : size(_n), name(std::move(_name)) {}
};

// スタックトップのローカル変数をひとつ開放
struct	free_local : exprbase
{
	static constexpr expr_type	type	= et_free_local;

	free_local() {}
	free_local(simplearray<sstring> _names) : names(std::move(_names)) {}
	simplearray<sstring>		names;


	static exprnode make(sstring _datatype, char const* _location, simplearray<sstring> _list)
	{
		return	exprnode::make<free_local>(std::move(_datatype), _location, std::move(_list));
	}
};

// スカラ値のロード
struct	load_scalar : exprbase
{
	static constexpr expr_type	type	= et_load_scalar;
	exprnode	srcadr;
	load_scalar(exprnode _src) : srcadr(std::move(_src)) {}

	static exprnode make(sstring _datatype, char const* _locate, exprnode _operand)
	{
		return	exprnode::make<load_scalar>(std::move(_datatype), _locate, std::move(_operand));
	}
};

// スカラ値のストア
struct	store_scalar : exprbase
{
	static constexpr expr_type	type	= et_store_scalar;
	store_scalar(sstring _storetype, exprnode _dest, exprnode _src) : storetype(std::move(_storetype)), destadr(std::move(_dest)), src(std::move(_src)) {}
	sstring			storetype;
	exprnode	destadr;
	exprnode	src;
	static exprnode make(sstring _datatype, char const* _locate, sstring _storetype, exprnode _dest, exprnode _src)
	{
		return	exprnode::make<store_scalar>(std::move(_datatype), _locate, std::move(_storetype), std::move(_dest), std::move(_src));
	}
};

// 任意の構造体もしくは値
struct	variable : exprbase
{
	static constexpr expr_type	type	= et_variable;
	variable()
	{}
};

// 任意の構造体もしくは値への参照
// この式で示す値は読み書き書き可能である
struct	variableref : variable
{
	static constexpr expr_type	type	= et_variableref;
	enum reftype
	{
		ref_global		= 0,		// 大域空間
		ref_stack		= 1,		// スタック（スタックトップを基準としたオフセット）
		ref_args		= 2,		// 関数の引数(≒ref_local)
		ref_local		= 3,		// 関数のローカルエリア
		ref_member		= 4,		// メンバ変数
	};

	variableref(reftype _r, sstring _name)
		: rtype(_r)
		, name(std::move(_name))
	{}

	reftype	rtype;
	sstring	name;
};


// スタックの参照
struct	stackref : variableref
{
	static constexpr expr_type	type	= et_stackref;
	intmax_t		offset;
	stackref(intmax_t _off, sstring _identify)
		: variableref(ref_stack, std::move(_identify))
		, offset(_off)
	{}

	static exprnode make(sstring _datatype, char const* _locate, intmax_t _off, sstring _identify)
	{
		return	exprnode::make<stackref>(std::move(_datatype), _locate, _off, _identify);
	}
};

// 引数への参照
struct	argumentref : variableref
{
	static constexpr expr_type	type	= et_argumentref;

	argumentref(int _argindex, sstring _id)
		: variableref(ref_args, std::move(_id))
		, index(_argindex)
	{}

	// ap = 0 は戻り値の格納先を指す
	// 第１引数は -1, 第２引数は -2 を指すことになる
	int		index;


	static exprnode make(sstring _datatype, char const* _locate, int _argindex, sstring _id)
	{
		return	exprnode::make<argumentref>(std::move(_datatype), _locate, _argindex, std::move(_id));
	}};

// ローカル変数への参照
struct	localref : variableref
{
	static constexpr expr_type	type	= et_localref;

	localref(sstring _identify)
		: variableref(ref_local, std::move(_identify))
	{}

	static exprnode make(sstring _datatype, char const* _locate, sstring _name)
	{
		return	exprnode::make<localref>(std::move(_datatype), _locate, std::move(_name));
	}
};

// メンバ変数のオフセット値
struct	memberoff : variableref
{
	static constexpr expr_type	type	= et_memberoff;

	memberoff(sstring _name)
		: variableref(ref_member, std::move(_name))
	{}

	static exprnode make(sstring _datatype, char const* _locate, sstring _name)
	{
		return	exprnode::make<memberoff>(std::move(_datatype), _locate, std::move(_name));
	}
	
};

// 参照をレジスタに格納する
struct	lea_r : exprbase
{
	static constexpr expr_type	type	= et_lea_r;
	vmregmap::reg_type	reg;
	exprnode			ref;

	// _ref
	//		stackref, argumentref, localref
	lea_r(vmregmap::reg_type _reg, exprnode _ref)
		: reg(_reg)
		, ref(std::move(_ref))
	{}

	static exprnode make(sstring _datatype, char const* _locate, vmregmap::reg_type _reg, exprnode _ref)
	{
		return	exprnode::make<lea_r>(std::move(_datatype), _locate, _reg, std::move(_ref));
	}

	static exprnode make_localref(sstring _datatype, char const* _locate, vmregmap::reg_type _reg, sstring _name)
	{
		return	exprnode::make<lea_r>(std::move(_datatype), _locate, _reg, localref::make(_datatype, _locate, _name));
	}

	static exprnode make_argumentref(sstring _datatype, char const* _locate, vmregmap::reg_type _reg, int _argindex, sstring _id)
	{
		return	exprnode::make<lea_r>(std::move(_datatype), _locate, _reg, argumentref::make(_datatype, _locate, _argindex, std::move(_id)));
	}

	static exprnode make_stackref(sstring _datatype, char const* _locate, vmregmap::reg_type _reg, intmax_t _off, sstring _identify)
	{
		return	exprnode::make<lea_r>(std::move(_datatype), _locate, _reg, stackref::make(_datatype, _locate, _off, std::move(_identify)));
	}
};

// sp+off の値をレジスタへロードする
struct	load_r : exprbase
{
	static constexpr expr_type	type	= et_load_r;
	vmregmap::reg_type	reg;
	exprnode			ref;

	// _ref
	//		stackref, argumentref, localref
	load_r(vmregmap::reg_type _reg, exprnode _ref)
		: reg(_reg)
		, ref(std::move(_ref))
	{}

	static exprnode make(sstring _datatype, char const* _locate, vmregmap::reg_type _reg, exprnode _ref)
	{
		return	exprnode::make<load_r>(std::move(_datatype), _locate, _reg, std::move(_ref));
	}

	static exprnode make_localref(sstring _datatype, char const* _locate, vmregmap::reg_type _reg, sstring _name)
	{
		return	exprnode::make<load_r>(std::move(_datatype), _locate, _reg, localref::make(_datatype, _locate, _name));
	}

	static exprnode make_argumentref(sstring _datatype, char const* _locate, vmregmap::reg_type _reg, int _argindex, sstring _id)
	{
		return	exprnode::make<load_r>(std::move(_datatype), _locate, _reg, argumentref::make(_datatype, _locate, _argindex, std::move(_id)));
	}

	static exprnode make_stackref(sstring _datatype, char const* _locate, vmregmap::reg_type _reg, intmax_t _off, sstring _identify)
	{
		return	exprnode::make<load_r>(std::move(_datatype), _locate, _reg, stackref::make(_datatype, _locate, _off, std::move(_identify)));
	}
};

// 関数名の指定
struct	function_name : exprbase
{
	static constexpr expr_type	type	= et_function_name;

	sstring					name;		// 関数名
	exprnode				thiz;		// メンバ関数の場合の this

	function_name(sstring _name, exprnode _thiz)
		: name(std::move(_name))
		, thiz(std::move(_thiz))
	{}

	static exprnode make(char const* _location, sstring _name, exprnode _thiz)
	{
		return	exprnode::make<function_name>("&", _location, std::move(_name), std::move(_thiz));
	}
};



// 関数呼び出し
struct	invoke_function : exprbase
{
	static constexpr expr_type	type	= et_invoke_function;
	function*					func;
	simplearray<exprnode>	arguments;		// 引数の式
	simplearray<exprnode>	argsfree;		// 引数の開放式

	invoke_function(function* _func, simplearray<exprnode> _args, simplearray<exprnode> _free)
		: func(_func)
		, arguments(std::move(_args))
		, argsfree(std::move(_free))
	{}

	// 戻り値が tmp を差している場合は tmp を削除する
	bool remove_return_tmpif(exprstatus& _state);

	localref* returnref() const
	{
		if (arguments.empty())
		{
			return	nullptr;
		}
		return	arguments[0].as<localref>();
	}

	void returnref(exprnode _expr)
	{
		arguments[0]	= std::move(_expr);
	}
};

// スカラ型のコピー
struct	scalar_copy : exprbase
{
	static constexpr expr_type	type	= et_scalar_copy;
	scalar_copy(exprnode _dest, exprnode _src)
		: dest(std::move(_dest))
		, src(std::move(_src))
	{}
	exprnode				dest;
	exprnode				src;
};

// new 演算子
struct	operator_new : exprbase
{
	static constexpr expr_type	type	= et_operator_new;
	exprnode	alloc;
	exprnode	ctor;
	operator_new(exprnode&& _alloc, exprnode&& _ctor)
		: alloc(std::move(_alloc))
		, ctor(std::move(_ctor))
	{}
	static exprnode make(sstring _datatype, char const* _location, exprnode&& _alloc, exprnode&& _ctor)
	{
		return	exprnode::make<operator_new>(std::move(_datatype), _location, std::move(_alloc), std::move(_ctor));
	}
};


// delete 演算子
struct	operator_delete : exprbase
{
	static constexpr expr_type	type	= et_operator_delete;
	exprnode	dealloc;
	exprnode	dtor;
	operator_delete(exprnode&& _dealloc, exprnode&& _dtor)
		: dealloc(std::move(_dealloc))
		, dtor(std::move(_dtor))
	{}
	static exprnode make(sstring _datatype, char const* _location, exprnode&& _dealloc, exprnode&& _dtor)
	{
		return	exprnode::make<operator_delete>(std::move(_datatype), _location, std::move(_dealloc), std::move(_dtor));
	}
};


// 単項演算子
struct	operator_single : exprbase
{
	static constexpr expr_type	type	= et_operator_single;
	operator_type				opecode;
	exprnode				operand1;
	operator_single(operator_type _opecode, exprnode&& _operand)
		: opecode(std::move(_opecode))
		, operand1(std::move(_operand))
	{}
	static exprnode make(sstring _datatype, char const* _location, operator_type _ope, exprnode _operand1)
	{
		return	exprnode::make<operator_single>(std::move(_datatype), _location, std::move(_ope), std::move(_operand1));
	}
};

// ２項演算子
struct	operator_bin : exprbase
{
	static constexpr expr_type	type	= et_operator_bin;
	operator_type				opecode;
	exprnode					operand1;
	exprnode					operand2;

	operator_bin(operator_type _opecode, exprnode _v1, exprnode _v2)
		: opecode(_opecode)
		, operand1(std::move(_v1))
		, operand2(std::move(_v2))
	{}

	static exprnode make(sstring _datatype, char const* _locate, operator_type _opecode, exprnode _v1, exprnode _v2)
	{
		return	exprnode::make<operator_bin>(std::move(_datatype), _locate, _opecode, std::move(_v1), std::move(_v2));
	}
};

// ３項演算子
struct	operator_three : exprbase
{
	static constexpr expr_type	type	= et_operator_bin;
	operator_type				opecode;
	exprnode				operand1;
	exprnode				operand2;
	exprnode				operand3;
};

// 関数を戻る
struct	function_return : exprbase
{
	static constexpr expr_type	type	= et_function_return;
	function_return() {}
};

// 型名指定が見つかった
struct	typename_node : exprbase
{
	static constexpr expr_type	type	= et_typename_node;

	static exprnode make(sstring _datatype, char const* _locate)
	{
		return	exprnode::make<typename_node>(std::move(_datatype), _locate);
	}
};


// 値を重複します
struct	duplicate : exprbase
{
	static constexpr expr_type	type	= et_duplicate;

	static exprnode make(sstring _datatype, char const* _locate)
	{
		return	exprnode::make<duplicate>(std::move(_datatype), _locate);
	}
};

// レジスタをPUSHします
struct	reg_push : exprbase
{
	static constexpr expr_type	type	= et_reg_push;
	int		reg;
	sstring	name;
	reg_push(int _r, sstring _name) : reg(_r), name(std::move(_name)) {}
	static exprnode make(char const* _locate, int _reg, sstring _name)
	{
		return	exprnode::make<reg_push>("int", _locate, _reg, std::move(_name));
	}
	static exprnode make(sstring _datatype, char const* _locate, int _reg, sstring _name)
	{
		return	exprnode::make<reg_push>(std::move(_datatype), _locate, _reg, std::move(_name));
	}
};

// レジスタをPOPします
struct	reg_pop : exprbase
{
	static constexpr expr_type	type	= et_reg_pop;
	int		reg;
	reg_pop(int _r) : reg(_r) {}
	static exprnode make(char const* _locate, int _reg)
	{
		return	exprnode::make<reg_pop>("int", _locate, _reg);
	}
};

// if分岐
struct	statement_if : exprbase
{
	static constexpr expr_type	type	= et_statement_if;
	statement_if(exprnode _cond, exprnode _true, exprnode _false) : cond(std::move(_cond)), true_if(std::move(_true)), false_if(std::move(_false)) {}
	exprnode				cond;
	exprnode				true_if;
	exprnode				false_if;

	static exprnode make(char const* _location, exprnode _cond, exprnode true_if, exprnode false_if)
	{
		return	exprnode::make<statement_if>("void", _location, std::move(_cond), std::move(true_if), std::move(false_if));
	}
};

// for分岐
struct	statement_for : exprbase
{
	static constexpr expr_type	type	= et_statement_for;
	statement_for(exprnode _init, exprnode _cond, exprnode _next, exprnode _stm)
		: init(std::move(_init)), cond(std::move(_cond)), next(std::move(_next)), stm(std::move(_stm))
	{}
	exprnode				init;
	exprnode				cond;
	exprnode				next;
	exprnode				stm;

	static exprnode make(char const* _location, exprnode _init, exprnode _cond, exprnode _next, exprnode _stm)
	{
		return	exprnode::make<statement_for>("void", _location, std::move(_init), std::move(_cond), std::move(_next), std::move(_stm));
	}
};


struct	unparsed_expr : exprbase
{
	static constexpr expr_type	type	= expr_type::st_unparsed_expr;
	sstring	stm;				// 式のコンパイルは関数名の解決が必要なので文解析中には式展開は行わない
};


struct	substatements : exprbase
{
	static constexpr expr_type	type	= expr_type::st_substatements;
	simplearray<exprnode>	stms;
};

struct	compound : exprbase
{
	static constexpr expr_type	type	= expr_type::st_compound;
	simplearray<exprnode>	stms;
};

struct	stm_if : exprbase
{
	static constexpr expr_type	type	= expr_type::st_if;
	exprnode	init;		// 初期化式
	text		cond;		// 条件式
	exprnode	stm;		// 文
	exprnode	stm_else;	// else 文
};


struct	stm_for : exprbase
{
	static constexpr expr_type	type	= expr_type::st_for;
	stm_for() {}
	~stm_for() {}

	exprnode	init;		// 初期化式
	text		cond;		// 条件式
	text		inc;		// インクリメント式
	exprnode	stm;		// ループ部
};


struct	stm_foreach : exprbase
{
	static constexpr expr_type	type	= expr_type::st_foreach;

	exprnode	init;			// 初期化式
	text		iterator_type;	// 範囲宣言(ranged base)
	text		iterator;		// 範囲宣言(ranged base)
	text		value;			// 範囲式(ranged base)
	exprnode	stm;			// ループ部
};


struct	stm_while : exprbase
{
	static constexpr expr_type	type	= expr_type::st_while;
	text	cond;		// 条件式
	text	stm;		// ループ部
};

struct	stm_switch : exprbase
{
	static constexpr expr_type	type	= expr_type::st_switch;
	text					val;		// 値
};

// return statement
struct	stm_return : exprbase
{
	static constexpr expr_type	type	= expr_type::st_return;
	text			val;		// 値
};


struct	decl_class : exprbase
{
	static constexpr expr_type	type	= expr_type::st_class_declare;

	/*
	 * 継承元情報
	 */
	struct	baseclass
	{
		accesstype	access;
		text		name;
	};

	text					declkeyword;	// "struct" or "class".
	text					name;			// class name
	simplearray<baseclass>	bases;
};


struct	decl_function : exprbase
{
	struct	decl_template
	{
		text	code;				// source code 全体を表す
		text	parameters;			// template<PARAMETERS> declare
	};

	static constexpr expr_type	type	= expr_type::st_function;
	text			code;				// source code 全体を表す
	decl_template	template_specifer;	// template parameters.

	text			decl_specifer;		// static inline virtual explicit friend constexpr constval constinit
	text			return_type;		// autoの場合は -> return_type の値が設定されます
	text			name;
	parameterlist	parameters;
	text			cv;					// const volatile
	text			ref;				// reference
	text			except;				// except
	text			virt_specifer;		// override final
	text			ctor_intiializer;	// constractor initializer
	text			body;				// compunds statement, "= delete", "= default"
};


struct	decl_variable : exprbase
{
	struct	decldata
	{
		text	identify;
		text	init;
	};

	static constexpr expr_type	type	= expr_type::st_variable;
	text					datatype;
	simplearray<decldata>	decls;
};


} // namespace expr


template<>
struct	__is_type<expr::static_number>
{
	static bool is(expr_type t) noexcept
	{
		return	t == expr::static_number::type || t == expr::literal_number::type;;
	}
};

template<>
struct	__is_type<expr::static_string>
{
	static bool is(expr_type t) noexcept
	{
		return	t == expr::static_string::type || t == expr::literal_string::type;
	}
};

template<>
struct	__is_type<expr::variableref>
{
	static bool is(expr_type t) noexcept
	{
		return	t == expr::variableref::type || t == expr::argumentref::type || t == expr::localref::type;
	}
};




struct	exprstatus
{
	struct	localvar
	{
		sstring		datatype;
		sstring		name;
		bool		enable	= true;
		char const*	locate	= nullptr;
		char const* location() const noexcept { return locate; }
		void location(char const* _locate) noexcept { locate = _locate; }

		#ifdef	DEBUG
		sstring const& message() const noexcept { static sstring d; return msg; }
		void message(sstring _msg) noexcept { msg = std::move(_msg); }
		sstring		msg;
		#endif
	};


	struct	localstack
	{
		simplearray<localvar>		works;
		stackbuffer					stack;				// スタックの状態を計算するための作業データ
	};


	simplearray<localstack>		locstack;
	bool						noreturn	= true;		// return されないパスが存在する場合は true

	exprstatus()
	{
		locstack.push_back({});
	}

	// ローカル変数の名前解決用データをスタックに登録します
	void local_push(localvar _var)
	{
		new (stack().template push<localvar>()) localvar (std::move(_var));
	}

	// ローカル変数の名前解決用データをひとつスタックから除外します
	void local_pop()
	{
		stack().template pop<localvar>()->~localvar();
	}

	// 現在のブロックのローカル変数一覧
	stackbuffer& compound_current_local()
	{
		return	stack();
	}

	// ブロックを開始する
	void compound_begin()
	{
		locstack.push_back({});
	}

	// ブロックを終了
	void compound_end()
	{
		locstack.pop_back();
	}

	localvar* local_current()
	{
		return	reinterpret_cast<localvar*>(stack().sp());
	}

	localvar* local_top()
	{
		return	reinterpret_cast<localvar*>(stack().top());
	}

	void local_reset()
	{
		while (local_top() != local_current())
		{
			local_pop();
		}
	}

	// 指定する名前のローカル変数が定義されているか調べる
	localvar* local_find(text _name)
	{
		for (auto& stk : locstack)
		{
			for (auto sp = (intmax_t)stk.stack.sp(); sp != (intmax_t)stk.stack.top(); sp += sizeof(localvar))
			{
				localvar*	v	= reinterpret_cast<localvar*>(sp);
				if (v->name == _name)
				{
					return	v;
				}
			}
		}
		return	nullptr;
	}

	sstring create_work_name(char const* _prefix)
	{
		char	buff[30];
		char*	p	= buff;
		auto	n	= workCount++;
		do
		{
			*p++ = (n%10) + '0';
			n /= 10;
		} while (n > 0);
		*p	= 0;
		sstring	name("_");
		name.append(buff);
		name.append("_");
		name.append(_prefix);
		return	name;
	}

	// 式の計算中に生成したワーク用変数を生成します。
	localvar* create_tempvar(sstring _type, char const* _name)
	{
		return	create_tempvar2(std::move(_type), create_work_name(_name));
	}

	localvar* create_tempvar2(sstring _type, sstring _name)
	{
		works().push_back({ std::move(_type), std::move(_name) });
		return	&works().back();
	}

	// ワーク変数を破棄します
	localvar pop_tempvar()
	{
		localvar	r	= works().back();
		works().pop_back();
		return	r;
	}

	void remove_tempvar(text _name)
	{
		for (auto& w : works())
		{
			if (w.name == _name)
			{
				w.enable	= false;
			}
		}
	}


	// ワークをリセットします
	void reset_tempvar()
	{
	}

	simplearray<localvar>& works()
	{
		return	locstack.back().works;
	}

	stackbuffer& stack()
	{
		return	locstack.back().stack;
	}

	struct	nestkeep
	{
		exprstatus*					status;

		nestkeep(nestkeep const& _r) = delete;
		nestkeep& operator = (nestkeep const& _r) = delete;

		nestkeep(nestkeep&& _r)
			: status(_r.status)
		{
			_r.status	= nullptr;
		}

		nestkeep& operator = (nestkeep&& _r)
		{
			status		= _r.status;
			_r.status	= nullptr;
			return	*this;
		}


		nestkeep(exprstatus* _s)
			: status(_s)
		{}

		~nestkeep()
		{
			if (status)
			{
				status->locstack.pop_back();
			}
		}
	};

	nestkeep status_nest()
	{
		locstack.push_back(localstack{});
		return	nestkeep(this);
	}

private:
	size_t						workCount	= 0;
};

using	expression_callback	= exprnode(exprstatus& _state, compiler& _compiler, codefunction* _function, text& _code);


PSNVM_API exprnode parse_expression(exprstatus& _state, compiler& _compiler, codefunction* _function, text _code);

PSNVM_LANG_NAMESPACE_END
#endif
