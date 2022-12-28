/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__validator__hxx
#define	__libamtrs__lang__passion__validator__hxx
#include "pvlnode.hxx"
PSNVM_LANG_NAMESPACE_BEGIN



// ノード間の整合性や型チェックなどを行います。
//	また静的に演算可能なリテラルの計算とコンパイル時に役立つ情報を生成します。
//	validator で生成された pvlnode は基本的に即実行可能です。
struct	validator
{
	using	varstate	= vn_local_allocate::varstate;

	enum blocktype
	{
		_code		= 0,
		_loop		= 1,
		_switch		= 2,
	};



	struct	blockstate
	{
		blocktype		type		= _code;
		vdecl_function*	func		= nullptr;
	};

	struct	codestate
	{
		bool	is_break	= false;
		bool	is_continue	= false;
		bool	is_return	= false;

		bool disabled() const noexcept { return is_break || is_continue || is_return; }
	};

	validator();
	~validator();
	void reset();

	// エラー状態
	auto good() const noexcept { return errors.size() == 0; }
	auto bad() const noexcept { return !good(); }
	auto error(errorinfo e) -> pvlnode { errors.push_back(std::move(e)); return pvlnode(); }
	void error_clear() { errors.clear(); }


	// validate された結果を返します。
	//	declare系のノードは empty() を返します。
	auto validate(pflnode& _n) -> pvlnode;
	auto validate(pflnode* _nodes, size_t _size) -> pvlnode;
	auto validate(text _src, sourcemode _setting = sourcemode{}) -> pvlnode;



	auto get_class(text _classname) -> vdatatype;
	auto add(ref<pn_declare_function> _func) -> vdecl_function*;
	auto add(ref<pn_declare_function_constractor> _func) -> vdecl_function*;
	auto add(ref<pn_declare_function_destractor> _func) -> vdecl_function*;
	auto add(ref<pn_declare_function_operator> _func) -> vdecl_function*;
	auto add(text _name, text _rettype, std::initializer_list<text> _parameters, vdecl_function::nativehook* _native) -> vdecl_function*;
	auto add(ref<pn_declare_class> _cls) -> vdecl_class*;
	auto make_symbol(pflnode& _n) -> shared_string;

	// アクセス範囲の確認
	bool can_access(vdecl_function* _func);
	bool can_access(vdecl_class::member const& _member);

	// declare系のノードを登録します。
	//	ノードを登録した場合は true, 式などの登録不要な場合は false
	auto declare(pflnode& _n) -> bool;

	// 関数グループを検索します
	auto find_function(text _scope, text _name) -> simplearray<ref<vdecl_function>>*;

	// 関数を検索します
	auto find_function(text _name, view<vdatatype> _args) -> vdecl_function*;

	// 型変換するノードを生成する
	auto cast_to(vdatatype const& _to, pvlnode _from) -> pvlnode;

	// 型変換可能か調べる
	auto can_cast(vdatatype const& _to, vdatatype const& _from, int _nest = 0) -> bool;

	// currentClass から関数を探して呼び出しを生成します。
	//	_thiz == nullptr 以外の場合は指定されたクラスから thiscall を検索します。
	auto make_function_call(pvlnode _thiz, text _name, view<pvlnode> _args) -> pvlnode;

	simplearray<errorinfo>											errors;
	hash_map<shared_string, vdatatype>								classes;		// functions[name]
	hash_map<shared_string, simplearray<ref<vdecl_function>>>		functions;		// functions[name]

protected:
	pvlnode	expr_assign(pvlnode& _dst, pvlnode& _src);
	void free_locals(simplearray<pvlnode>& _statements, text _location, size_t _size);
	void validate_class(vdecl_class* _cls);
	void validate_function(vdecl_function* _func);
	auto validate_function_constractor(vdecl_function* _func) -> simplearray<pvlnode>;

	auto state() -> blockstate& { return mBlockState.back(); }
	void block_in(blocktype _type = _code) { mBlockState.push_back({_type, state().func }); }
	auto block_out() -> blockstate { blockstate r(mBlockState.back()); mBlockState.pop_back(); return r; }

	vdecl_class*				mCurrentClass	= nullptr;
	shared_string				mExprScope;		// 解析中の式のスコープ
	simplearray<blockstate>		mBlockState;
	codestate					mState;
	simplearray<ref<varstate>>	localvars;		// ローカル変数。あくまでも解析のために使用する情報。
};



//auto is_scalar(text _s) -> bool;



PSNVM_LANG_NAMESPACE_END
#endif
