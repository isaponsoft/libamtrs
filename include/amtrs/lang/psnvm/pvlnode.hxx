/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__validatednode__hxx
#define	__libamtrs__lang__passion__validatednode__hxx
PSNVM_LANG_NAMESPACE_BEGIN
class	pvlnode;
enum class vnodetypes
{
	_none,
	_tmpnode,								// validator が一時的に使用するノード
	_imm_integer,
	_imm_offset,
	_imm_sizeof,
	_imm_string,
	_cast,
	_compound,
	_return,
	_returnval,
	_bin_operation_int,
	_address_argvref,
	_address_localref,
	_address_cast,
	_address_memberref,
	_assign,
	_declare_function,
	_expr_call,
	_expr_new,
	_expr_vcall,
	_list,
	_load_effective_address,	
	_load_integral,	
	_local_allocate,
	_local_free,
	_operation_single_int,
	_stmt_branch_break,
	_stmt_delete,
	_stmt_do_while,
	_stmt_for,
	_stmt_if,
	_stmt_switch,
	_stmt_while,
	_symbol,
	_this,
	_typename,
	_value_initializerlist,
	_vtable,
};


struct	vdecl_type;
struct	vdecl_class;
struct	vdecl_function;
struct	validator;

class	vdatatype
{
public:
	enum	types
	{
		_none		= 0,
		_void		= 1,
		_scalar		= 2,
		_pointer	= 3,
		_reference	= 4,
		_class		= 5,
		_packs		= 6,
		_array		= 7,
		_string		= 8,
		_const		= 9,
	};

	enum	casttypes
	{
		_nocast		= 0,	// 変換不可能
		_same		= 1,	// 型が一致する（参照も含む）
		_cast		= 2,	// 暗黙なキャストが可能
		_explicit	= 3,	// 明示的なキャストが可能
	};

	vdatatype() = default;
	vdatatype(vdatatype const&) = default;
	vdatatype(vdatatype&&) = default;
	vdatatype& operator = (vdatatype const&) = default;
	vdatatype& operator = (vdatatype&&) = default;
	vdatatype(vdecl_type* _decl);
	auto name() const noexcept -> shared_string;
	auto empty() const noexcept -> bool;
	auto type() const noexcept -> types;
	bool operator == (vdatatype const& r) const noexcept { return mDecl == r.mDecl; }
	bool operator != (vdatatype const& r) const noexcept { return mDecl != r.mDecl; }
	vdecl_type* cls() { return mDecl.get(); }

	bool has_constructor() const noexcept;
	auto is_constructible(view<text> _arguments = nullptr) const noexcept -> vdecl_function*;
	auto is_destructible() const noexcept -> vdecl_function*;

	// 現プラットフォームでのデータサイズを計算します
	//	この値はメモリ上に確保されるサイズであり、 sizeof() 演算子の返す値とは一致しないことがあります。
	auto size_of(bool _extract_ref = false) const noexcept -> size_t;
	auto align_of() const noexcept -> size_t;


	template<class T> T* at() noexcept { return type() == T::type ? static_cast<T*>(mDecl.get()) : nullptr; }
	template<class T> T const* at() const noexcept { return type() == T::type ? static_cast<T const*>(mDecl.get()) : nullptr; }

	auto add_extents(size_t _size) -> vdatatype;

	bool is_const() const noexcept;
	bool is_scalar() const noexcept;
	bool is_pointer() const noexcept;
	bool is_reference() const noexcept;

	vdatatype add_const() const;
	vdatatype add_pointer() const;
	vdatatype add_reference() const;

	vdatatype remove_const() const noexcept;
	vdatatype remove_pointer() const noexcept;
	vdatatype remove_reference() const noexcept;


	auto can_cast(vdatatype const& _src) const noexcept -> casttypes;

protected:
	ref<vdecl_type>	mDecl;
};



class	vnodebase : public ref_object
{
public:
	using	vntype	= vnodetypes;

	vntype			type;
	text			location;				// ソースコードの位置(プロファイル情報に使用する)
	vdatatype		datatype;				// ノードが示すデータの型
};


// validate 済みのノード
class	pvlnode
{
public:
	using	vntype	= vnodetypes;

	template<class T, class... Argv>
	static pvlnode make(text _code, vdatatype _datatype, Argv&&... _argv)
	{
		ref<vnodebase>	r	= new T(std::forward<Argv>(_argv)...);
		r->type		= T::type;
		r->location	= _code;
		r->datatype	= std::move(_datatype);
		return	(pvlnode)r;
	}


	pvlnode() = default;
	pvlnode(pvlnode const&) = default;
	pvlnode(pvlnode&&) = default;
	pvlnode(ref<vnodebase> _r) : mNode(std::move(_r)) {}
	pvlnode& operator = (pvlnode const&) = default;
	pvlnode& operator = (pvlnode&&) = default;
	bool empty() const noexcept { return mNode.empty(); }
	operator bool () const noexcept { return !mNode.empty(); }

	auto datatype() -> vdatatype;
	void datatype(vdatatype _type);
	auto location() -> text;
	auto type() -> vntype;
	template<class T> T* at();

protected:
	ref<vnodebase>	mNode;
};




// 即値整数
struct	vn_imm_integer : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_imm_integer;

	intmax_t	value;

	vn_imm_integer(intmax_t i) : value(i) {}
};


// 即値アドレス
struct	vn_imm_offset : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_imm_offset;

	char const*	label;
};


// 即値サイズ
struct	vn_imm_sizeof : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_imm_sizeof;

	vdatatype	t;

	vn_imm_sizeof(vdatatype _t) : t(std::move(_t)) {}
};


// 即値文字列
struct	vn_imm_string : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_imm_string;

	shared_string	str;

	vn_imm_string(shared_string _s) : str(std::move(_s)) {}
};


// 型変換
struct	vn_cast : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_cast;

	pvlnode	value;

	vn_cast(pvlnode _value) : value(std::move(_value)) {}
};


// 複合式
struct	vn_compound : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_compound;

	simplearray<pvlnode>	statements;

	vn_compound(simplearray<pvlnode> _statements) : statements(std::move(_statements)) {}
};


// 整数演算
struct	vn_bin_operation_int : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_bin_operation_int;

	operator_type	operation;
	pvlnode			v1;
	pvlnode			v2;

	vn_bin_operation_int(operator_type _o, pvlnode _v1, pvlnode _v2) : operation(_o), v1(std::move(_v1)), v2(std::move(_v2)) {}
};


// ローカル変数の確保
struct	vn_local_allocate : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_local_allocate;

	struct	varstate : ref_object
	{
		vdatatype		datatype;			// 変数名
		shared_string	name;				// 変数名
		pvlnode			init;				// 書き込み先の代入もしくはコンストラクタが含まれる
		pvlnode			value;				// 静的演算が可能な場合は静的な結果を保持する
		bool			read	= false;	// 読み取りがあった場合 true
		bool			write	= false;	// 書き込みがあった場合 true
	};


	simplearray<ref<varstate>>	variables;


	vn_local_allocate(simplearray<ref<varstate>> _vars) : variables(std::move(_vars)) {}
};


// ローカル変数の破棄
struct	vn_local_free : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_local_free;

	struct	varstate : ref_object
	{
		vdatatype		datatype;			// 変数名
		shared_string	name;				// 変数名
		pvlnode			destroy;			// デストラクタ呼び出しが必要な場合デストラクタ式が設定される
	};


	// 開放する値は最後尾からスタートします
	simplearray<ref<varstate>>	variables;


	vn_local_free(simplearray<ref<varstate>> _vars) : variables(std::move(_vars)) {}
};


// 引数変数への参照アドレス
struct	vn_address_argvref : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_address_argvref;

	intmax_t	index;						// 引数変数のインデックス

	vn_address_argvref(intmax_t _index) : index(_index) {}
};



// ローカル変数の参照アドレス
struct	vn_address_localref : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_address_localref;

	intmax_t	index;						// ローカル変数のインデックス

	vn_address_localref(intmax_t _index) : index(_index) {}
};


// オブジェクト変数のキャスト
struct	vn_address_cast : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_address_cast;

	vdecl_class*	to;			// 変換先のクラス
	vdecl_class*	from;		// 変換元のクラス
	pvlnode			value;

	vn_address_cast(vdecl_class* _to, vdecl_class* _from, pvlnode _val) : to(_to), from(_from), value(std::move(_val)) {}

	auto offset() const noexcept -> size_t;
};


// メンバ変数のアドレス
struct	vn_address_memberref : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_address_memberref;

	pvlnode			obj;			// メンバを持つオブジェクトのアドレスまたは参照
	vdecl_class*	thiscls;		// obj が指すオブジェクトのクラス
	vdecl_class*	scope;			// 現在のスコープ
	intmax_t		index_base_of;	// メンバのインデックス(ベースクラスからのインデックス)

	vn_address_memberref(pvlnode _obj, vdecl_class* _scope, vdecl_class* _thiscls, intmax_t _idx_base_of) : obj(std::move(_obj)), scope(_scope), thiscls(_thiscls), index_base_of(_idx_base_of) {}

	auto offset() const noexcept -> size_t;
};


// スカラの代入
struct	vn_assign : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_assign;

	pvlnode		dest;			// 書き込み先のスカラ変数の参照
	pvlnode		src;			// 書き込み元のスカラもしくはスカラの参照

	vn_assign(pvlnode _dst, pvlnode _src) : dest(_dst), src(_src) {}
};




// 関数呼び出し
struct	vn_expr_call : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_expr_call;

	vdecl_function*			func;			// 呼び出し対象
	simplearray<pvlnode>	arguments;

	vn_expr_call(vdecl_function* _func, simplearray<pvlnode> _args) : func(_func), arguments(std::move(_args)) {}
};


// 関数呼び出し
struct	vn_expr_new : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_expr_new;

	pvlnode				allocate;
	ref<vn_expr_call>	constractor;	// arguments[0] は常に empty() がセットされる

	vn_expr_new(pvlnode _alloc, ref<vn_expr_call> _const) : allocate(std::move(_alloc)), constractor(std::move(_const)) {}
};


// 関数呼び出し
struct	vn_expr_vcall : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_expr_vcall;

	vdecl_class*			cls;
	vdecl_function*			func;
	int						tblidx;
	simplearray<pvlnode>	arguments;

	vn_expr_vcall(vdecl_class* _cls, vdecl_function* _func, int _tblidx, simplearray<pvlnode> _args)
		: cls(_cls), func(_func), tblidx(_tblidx), arguments(std::move(_args)) {}

	auto offset() const noexcept -> size_t;
};


// リスト
struct	vn_list : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_list;

	simplearray<pvlnode>	values;

	vn_list(simplearray<pvlnode> _values) : values(std::move(_values)) {}
};

// アドレスを計算する
//	address + mul1 * mul2 + addtions を計算する
//	addtions は empty である可能性がある
struct	vn_load_effective_address : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_load_effective_address;

	pvlnode		address;			// ベースアドレス
	pvlnode		mul1;				// 乗算値
	pvlnode		mul2;				// 乗算値
	pvlnode		additions;			// 加算値

	vn_load_effective_address(pvlnode _address, pvlnode _mul1, pvlnode _mul2, pvlnode _add = {})
		: address(std::move(_address)) , mul1(std::move(_mul1)) , mul2(std::move(_mul2)) , additions(std::move(_add)) {}
};


// 参照先からの整数型スカラのロード
struct	vn_load_integral : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_load_integral;

	pvlnode		ref;			// 参照

	vn_load_integral(pvlnode _reference) : ref(_reference) {}
};


// 関数から抜ける
struct	vn_return : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_return;

	vn_return() {}
};


// 関数が返す値を決定する
struct	vn_returnval : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_returnval;

	pvlnode		expr;

	vn_returnval(pvlnode _expr) : expr(std::move(_expr)) {}
};


struct	vn_operation_single_int : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_operation_single_int;

	operator_type	operation;
	pvlnode			val;

	vn_operation_single_int(operator_type _ope, pvlnode _val)
		: operation(std::move(_ope)), val(std::move(_val)) {}
};


struct	vn_stmt_branch_break : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_stmt_branch_break;

	vn_stmt_branch_break() {}
};


struct	vn_stmt_delete : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_stmt_delete;

	pvlnode				value;
	ref<vn_expr_call>	deallocate;	// arguments[0] は常に empty() がセットされる
	ref<vn_expr_call>	destractor;	// arguments[0] は常に empty() がセットされる

	vn_stmt_delete(pvlnode _value, ref<vn_expr_call> _dealloc, ref<vn_expr_call> _destract)
		: value(std::move(_value)), deallocate(std::move(_dealloc)), destractor(std::move(_destract)) {}
};


struct	vn_stmt_do_while : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_stmt_do_while;

	pvlnode	conditions;
	pvlnode	statement;

	vn_stmt_do_while(pvlnode _cond, pvlnode _stms)
		: conditions(std::move(_cond)), statement(std::move(_stms)) {}
};


struct	vn_stmt_for : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_stmt_for;

	pvlnode	conditions;
	pvlnode	increment;
	pvlnode	statement;

	vn_stmt_for(pvlnode _cond, pvlnode _incr, pvlnode _stm)
		: conditions(std::move(_cond)), increment(std::move(_incr)), statement(std::move(_stm)) {}
};


struct	vn_stmt_if : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_stmt_if;

	pvlnode	conditions;
	pvlnode	statement1;
	pvlnode	statement2;

	vn_stmt_if(pvlnode _cond, pvlnode _stm1, pvlnode _stm2)
		: conditions(std::move(_cond)), statement1(std::move(_stm1)), statement2(std::move(_stm2)) {}
};


struct	vn_stmt_switch : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_stmt_switch;

	struct	casedata
	{
		pvlnode	value;
		pvlnode	statements;
	};

	pvlnode					conditions;
	simplearray<casedata>	cases;

	vn_stmt_switch(pvlnode _cond, simplearray<casedata> _cases)
		: conditions(std::move(_cond)), cases(std::move(_cases)) {}
};


struct	vn_stmt_while : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_stmt_while;

	pvlnode	conditions;
	pvlnode	statement;

	vn_stmt_while(pvlnode _cond, pvlnode _stms)
		: conditions(std::move(_cond)), statement(std::move(_stms)) {}
};


struct	vn_symbol : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_symbol;

	shared_string	name;

	vn_symbol(shared_string _name) : name(std::move(_name)) {}
	
};


// this
struct	vn_this : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_this;

	vn_this() {}
};


// 型名
struct	vn_typename : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_typename;

	vn_typename() {}
};


struct	vn_value_initializerlist : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_value_initializerlist;

	struct	initinfo
	{
		text	name;
		pvlnode	value;
	};

	simplearray<initinfo>		values;

	vn_value_initializerlist(simplearray<initinfo> _values)
		: values(std::move(_values))
	{}
};


// vtable のアドレスを返す
struct	vn_vtable : vnodebase
{
	static constexpr vnodetypes	type	= vnodetypes::_vtable;

	vdecl_class*	cls;
	intmax_t		offset;

	vn_vtable(vdecl_class* _cls, intmax_t _offset) : cls(std::move(_cls)), offset(_offset) {}
};




struct	vdecl_type : ref_object
{
	using	types		= vdatatype::types;
	using	casttypes	= vdatatype::casttypes;

	types				type;
	shared_string		name;

	vdecl_type(types _type, shared_string _name) : type(_type), name(std::move(_name)) {}

	virtual size_t align_of() const noexcept = 0;
	virtual size_t size_of() const noexcept = 0;
	virtual casttypes can_cast(vdatatype const& _src) const noexcept { return casttypes::_nocast; }
};


struct	vdecl_array : vdecl_type
{
	static constexpr types	type	= types::_array;

	vdatatype	element;

	vdecl_array(vdatatype _elm, shared_string _name) : vdecl_type(types::_array, std::move(_name)) {}

	virtual size_t align_of() const noexcept override { return element.align_of(); }
	virtual size_t size_of() const noexcept override;
	virtual casttypes can_cast(vdatatype const& _src) const noexcept override;
};


struct	vdecl_scalar : vdecl_type
{
	static constexpr types	type	= types::_scalar;

	size_t		size;

	vdecl_scalar(shared_string _name, size_t _size) : vdecl_type(types::_scalar, std::move(_name)), size(_size) {}
	vdecl_scalar(types _type, shared_string _name, size_t _size) : vdecl_type(_type, std::move(_name)), size(_size) {}

	virtual size_t align_of() const noexcept override { return size; }
	virtual size_t size_of() const noexcept override { return size; }
	virtual casttypes can_cast(vdatatype const& _src) const noexcept override;
};


struct	vdecl_string : vdecl_type
{
	static constexpr types	type	= types::_string;

	vdecl_string(shared_string _name) : vdecl_type(types::_string, std::move(_name)) {}

	virtual size_t align_of() const noexcept override;
	virtual size_t size_of() const noexcept override;
	virtual casttypes can_cast(vdatatype const& _src) const noexcept override;
};


struct	vdecl_pointer : vdecl_scalar
{
	static constexpr types	type	= types::_pointer;

	vdatatype	t;

	vdecl_pointer(vdatatype _t, shared_string _name) : vdecl_scalar(types::_pointer, std::move(_name), sizeof(void*)), t(std::move(_t)) {}

	virtual size_t align_of() const noexcept override;
	virtual size_t size_of() const noexcept override;
	virtual casttypes can_cast(vdatatype const& _src) const noexcept override;
};


struct	vdecl_reference : vdecl_scalar
{
	static constexpr types	type	= types::_reference;

	vdatatype	t;

	vdecl_reference(vdatatype _t, shared_string _name) : vdecl_scalar(types::_reference, std::move(_name), sizeof(void*)), t(std::move(_t)) {}

	virtual size_t align_of() const noexcept override;
	virtual size_t size_of() const noexcept override;
	virtual casttypes can_cast(vdatatype const& _src) const noexcept override;
};


struct	vdecl_const : vdecl_type
{
	static constexpr types	type	= types::_const;

	vdatatype	t;

	vdecl_const(vdatatype _t, shared_string _name) : vdecl_type(type, std::move(_name)), t(std::move(_t)) {}

	virtual size_t align_of() const noexcept override;
	virtual size_t size_of() const noexcept override { return t.size_of(); }
	virtual casttypes can_cast(vdatatype const& _src) const noexcept override;
};


// 関数定義(validate用)
struct	vdecl_class : vdecl_type
{
	static constexpr types	type	= types::_class;

	enum	accesstypes
	{
		_invisivle	= 0,
		_public		= 1,
		_protected	= 2,
		_private	= 3
	};

	enum	variabletypes
	{
		_value			= 0,
		_vtable			= 1,
		_castposition	= 2,
	};

	struct	base
	{
		accesstypes			access	= _public;
		ref<vdecl_class>	cls;
		uintptr_t			offset	= 0;
	};

	struct	variable
	{
		accesstypes			access	= _public;
		variabletypes		type	= variabletypes::_value;
		vdatatype			datatype;
		shared_string		name;
		pvlnode				init;
		vdecl_class*		cls		= nullptr;		// 定義していたクラス
		uintptr_t			offset	= 0;
		uintptr_t			vtblidx	= 0;
	};

	enum	membertypes
	{
		_none		= 0,
		_function	= 1,
		_vfunction	= 2,
		_variable	= 3,
		_type		= 4,
	};

	struct	member
	{
		membertypes		type;
		vdatatype		datatype;
		int				index;			// variable: variable index, function: this cast index, _vfunction vtbl index.
		variable*		var	= nullptr;
		vdecl_class*	cls	= nullptr;
		union
		{
			void*							p;
			variable*						v;
			vdecl_function*					f;
//			type*							t;
		};
		bool empty() const noexcept { return type == _none || p == nullptr; }
		bool is_variable() const noexcept { return type == _variable; }
		accesstypes access() const noexcept;
		shared_string name() const noexcept;
	};

	struct	castpos
	{
		vdecl_class*	cls;
		intptr_t		offset;
	};

	ref<pn_declare_class>				declare;
	simplearray<base>					bases;
	simplearray<variable>				variables;			//  without static var.
	simplearray<ref<vdecl_function>>	constructors;
	simplearray<ref<vdecl_function>>	functions;
	ref<vdecl_function>					destractor;
	simplearray<castpos>				castoffset;			// cast offset
	simplearray<vdecl_function*>		vtbl;				// virtual functions. [0] is destractor.
	bool								validated	= false;


	vdecl_class(shared_string _name);
	~vdecl_class();

	auto find_member(text _name) -> member;
	auto find_function(text _name, text _signature) -> vdecl_function*;
	auto variables_count() -> size_t;			// ベースクラスを含むメンバ変数の数を計算する
	bool is_validated() const noexcept { return validated; }
	bool has_constructor() const noexcept { return constructors.size() > 0; }
	auto layout() const -> simplearray<variable>;
	auto align_size() const -> size_t { return mAlignSize; }
	auto align_size_witout_vtable() const -> size_t { return mAlignSizeNoVtbl; }
	auto size_of_extends() const noexcept { return mSizeExtends; }
	auto size_of_local() const noexcept { return mSizeOfLocal; }
	auto size_of_bases() const noexcept { return mSizeOf - mSizeOfLocal; }
	auto has_vtable_origin() const noexcept { return mHasVtable; }
	size_t size_of_padding() const noexcept { return mPaddingSize; }		// 構造体の最後尾に追加されたパディング領域のサイズ

	virtual size_t align_of() const noexcept override;
	virtual size_t size_of() const noexcept override;
	virtual casttypes can_cast(vdatatype const& _src) const noexcept override;


protected:
	mutable size_t			mSizeOf				= static_cast<size_t>(~0);	// with bases
	mutable size_t			mSizeExtends		= static_cast<size_t>(~0);	// with bases
	mutable size_t			mSizeOfLocal		= static_cast<size_t>(~0);	// without bases
	mutable size_t			mAlignSize			= 0;
	mutable size_t			mAlignSizeNoVtbl	= 0;
	mutable size_t			mPaddingSize		= 0;
	mutable vdecl_class*	mLastVtable			= nullptr;
	mutable	bool			mHasVtable			= false;

	friend	validator;
};



// 関数宣言
struct	vdecl_function : vnodebase
{
	using	accesstypes	= vdecl_class::accesstypes;

	static constexpr vnodetypes		type		= vnodetypes::_declare_function;

	struct	parameter
	{
		vdatatype		datatype;
		shared_string	name;
		pvlnode			defvalue;
	};

	using	nativehook					= intmax_t(intmax_t* _argv, size_t _argc);


	ref<pn_declare_function>		declare;
	unsigned int					spec		= 0;
	accesstypes						access		= accesstypes::_public;
	vdecl_class*					cls			= nullptr;		// declared class
	vdecl_class*					thiscls		= nullptr;		// this pointer class
	shared_string					name;
	shared_string					localname;
	shared_string					display_name;				// name(signature)
	vdatatype						rettype;
	shared_string					signature;
	simplearray<parameter>			parameters;
	pvlnode							statement;
	nativehook*						native		= nullptr;		// Native function の場合、そのポインタを保持する。constexpr などで実行される可能性がある

	vdecl_function(ref<pn_declare_function> _decl, vdecl_class* _cls, shared_string _name, vdatatype _rettype, shared_string _signature, simplearray<parameter> _params, pvlnode _stmt);
	~vdecl_function();

	bool is_prototype() const noexcept { return declare->codes.type() == pflnode_types::none; }
	bool is_virtual() const noexcept { return spec & (uint32_t)pn_declare_specifier::spec_virtual; }
	bool is_thiscall() const noexcept { return thiscls && !(spec & (uint32_t)pn_declare_specifier::spec_static); }
};


inline vdatatype::vdatatype(vdecl_type* _decl)
	: mDecl(_decl)
{
}

inline auto vdatatype::name() const noexcept -> shared_string
{
	return	empty() ? shared_string() : mDecl->name;
}


inline auto vdatatype::empty() const noexcept -> bool { return mDecl.empty(); }
inline auto vdatatype::type() const noexcept -> types{ return empty() ? types::_none : mDecl->type; }

inline auto pvlnode::datatype() -> vdatatype { return mNode ? mNode->datatype : vdatatype(); }
inline void pvlnode::datatype(vdatatype _type) { if (!empty()) {mNode->datatype = std::move(_type); } }
inline auto pvlnode::location() -> text { return mNode ? mNode->location : text(nullptr); }
inline auto pvlnode::type() -> vntype { return mNode ? mNode->type : vnodetypes::_none; }
template<class T> T* pvlnode::at() { return (type() == T::type) ? static_cast<T*>(mNode.get()) : nullptr; }


void serialize(pvlnode& _node, typeutil::callback<void(text)> _callback);


PSNVM_LANG_NAMESPACE_END
#endif
