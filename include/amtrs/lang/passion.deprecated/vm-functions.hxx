/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__vm_functions__hpp
#define	__libamtrs__lang__passion__vm_functions__hpp
PSNVM_NAMESPACE_BEGIN
struct	vmstate;
struct	function;
class	vmclassmgr;


enum class	function_result
{
	fr_return			= 0,				// 関数から戻る。
	fr_susspend			= 1,				// 関数は一時停止している。主にコルーチンで使用される。
	fr_exception		= 2,				// 例外が発生した。
};

enum class	function_mode
{
	fm_basic			= 0,				// 一般関数
	fm_constract		= 1,				// コンストラクタ
	fm_destract			= 2,				// デストラクタ
};

using	function_type	= function_result(vmstate*, function*);


// 関数定義のベース
struct	function
{

	using	result_type	= function_result;

	function(sstring _name, sstring _signature);
	virtual ~function();

	// 関数の呼び出しが行われた
	result_type invoke(vmstate* _vm) { return func(_vm, this); }

	sstring			name;				// symbol name.
	sstring			signature;			// signature. example) void(int;string;float)
	function_type*	func	= nullptr;

	PSNVM_API sstring fullname() const;

	/*
	 * スコープ名を取得します。
	 */
	PSNVM_API static text scope(text _functionname) noexcept; 
	text scope() const noexcept { return scope(name); }

	/*
	 * 関数の種類を調べます。
	 * コンストラクタなのか調べるためにクラスマネージャが必要です。
	 */
	PSNVM_API static function_mode mode(vmclassmgr& _classes, text _functionname) noexcept;

	function_mode mode(vmclassmgr& _classes) const noexcept { return mode(_classes, name); }
};


// bytecode を持った関数(非ネイティブ関数)
struct	function_bc : function
{
	simplearray<bytecode>	codes;

	function_bc(sstring _name, sstring _sig, simplearray<bytecode> _bytecode)
		: function(std::move(_name), std::move(_sig))
		, codes(std::move(_bytecode))
	{}
};


class	functionlist
{
public:
	functionlist() = default;
	functionlist(functionlist const&) = delete;
	functionlist(functionlist&&) = default;
	functionlist& operator = (functionlist const&) = delete;
	functionlist& operator = (functionlist&&) = default;
	functionlist(functionlist* _parent)
		: mParent(_parent)
	{}

	// 登録する
	void add(function* _f)
	{
		mList.push_back(functionnode(_f));
	}

	/*
	 * 破棄する
	 */
	void clear();

	// 名前とシグネチャを比較して完全に一致するものを返す。
	function* find(text _name, text _signature) const noexcept;

	// 名前を比較して致するものの一覧を返す。
	simplearray<function*> listup(text _name) const;

	size_t size() const noexcept
	{
		return	mList.size();
	}

	function* find_if(typeutil::callback<bool(function*)> const& _compare);

private:
	struct	functionnode
	{
		function*	func	= nullptr;
		functionnode() = default;
		functionnode(functionnode const&) = delete;
		functionnode(functionnode&& r) : func(r.func) { r.func = nullptr; }
		functionnode(function* f) : func(f) {}
		functionnode& operator = (functionnode const&) = delete;
		functionnode& operator = (functionnode&& r) { if (func) { delete func; } func = r.func; r.func = nullptr; return *this; }
		~functionnode() { if (func) { delete func; } }
	};

	simplearray<functionnode>	mList;
	functionlist*				mParent	= nullptr;
};


PSNVM_NAMESPACE_END
#endif
