/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__nodevm__hpp
#define	__libamtrs__lang__passion__nodevm__hpp
#include <stdexcept>
#include "../../memory/hash_map.hpp"
#include "../../memory/stackbuff.hpp"
#include "../../string/format.hpp"
#include "../../string/shared_string.hpp"
#include "../../system/log.hpp"
#include "psnfl.hpp"
PASSION_NAMESPACE_BEGIN

enum class	symbol_types
{
	s_function		= 0,		// 関数
	s_variable		= 1,		// グローバル変数
	s_class			= 2,		// クラス。このデータは静的データとしては配置されない。
	s_namespace		= 3,		// 名前空間。このデータは静的データとしては配置されない。
};


struct	lang_exception : std::runtime_error
{
	using	super_type	= std::runtime_error;
	template<class... Args>
	lang_exception(char const* _location, Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
		, location(_location)
	{}
	char const*	location;
};


// シンボルが見つからないときに投げられる例外です
struct	symbol_not_found : lang_exception
{
	using	super_type	= lang_exception;
	using	super_type::super_type;
};


struct	symbol
{
	using	string_type		= shared_string;
	using	types			= symbol_types;
	static constexpr char	separator[]	= "/";

	types			type;
	string_type		identify;
	node			target;
};


// symbol は完全に一意な名前を持ちます。
// 関数の場合は引数を含めた名前になります。
//	example)
//		int ns1::ns2::funcname(string x, char** y);
//		=> ns1/ns2/funcname:int(string:char**)
//
//		class ns1::ns2::ns3::myclass
//		=> ns1/ns2/ns3/myclass
class	symbolmap
{
public:
	using	string_type			= shared_string;
	using	storate_type		= hash_map<string_type, symbol>;
	using	storate_iterator	= typename storate_type::iterator;

	static constexpr symbol_types	s_function	= symbol_types::s_function;
	static constexpr symbol_types	s_class		= symbol_types::s_class;
	static constexpr symbol_types	s_variable	= symbol_types::s_variable;

	symbolmap(symbolmap* _parent = nullptr);
	symbolmap(symbolmap const&) = delete;
	symbolmap(symbolmap&&) = default;
	symbolmap& operator = (symbolmap const&) = delete;
	symbolmap& operator = (symbolmap&&) = default;

	// add()
	void add(symbol _sym);

	// _nameで指定するシンボルを取得します。
	auto find(text _name) -> view<symbol*>;

	// _nameで指定するシンボルを取得します。
	auto find(text _name, text _scope) -> view<symbol*>;

	auto begin() const noexcept { return mTank.begin(); }
	auto end() const noexcept { return mTank.end(); }

protected:
	symbolmap*									mParent	= nullptr;
	hash_map<string_type, symbol>				mTank;
	hash_map<string_type, simplearray<symbol*>>	mFunctions;
	hash_map<string_type, symbol*>				mClasses;
	hash_map<string_type, symbol*>				mVariables;
	hash_map<string_type, symbol*>				mNamespaces;
};


struct	vmint
{
	static vmint mkint(intmax_t n)
	{
		vmint	r;
		r.i		= n;
		return	r;
	}

	static vmint mkuint(uintmax_t n)
	{
		vmint	r;
		r.u		= n;
		return	r;
	}

	static vmint mkptr(void* v)
	{
		vmint	r;
		r.p		= v;
		return	r;
	}

	union
	{
		intmax_t	i;
		uintmax_t	u;
		void*		p;
	};
};




// ノードを直接実行するための仮想マシンです。
//	主に静的解析、静的実行のために使用されます。
class	nodevm
{
public:
	using	destractor	= void(void*);

	struct	stkdat
	{
		char const*		type;
		vmint			val;
		destractor*		dest	= nullptr;
		~stkdat() { if (dest) { dest(val.p); } }
	};

	stackbuf<stkdat>	stack;
	symbolmap			symbols;

	nodevm()
		: stack(8192)
	{}


	// ****************************************************
	// 解析系

	// シンボルを追加します。
	void add(symbol _sym) { symbols.add(std::move(_sym)); }

	// ノードを接続します。
	//	関数やクラスなどがあればシンボルマップに登録されます。
	void attach(simplearray<node>& _nodes);

	// _nameで指定するシンボルを取得します。
	auto find(text _name) -> view<symbol*> { symbols.find(_name); }

	// _nameで指定するシンボルを取得します。
	auto find(text _name, text _scope) -> view<symbol*> { symbols.find(_name, _scope); }

	// _identify で完全一致する関数を返します。
	//	複数にマッチする場合はエラーです。
	auto find_function(text _identify) -> symbol*;

	// 型名を取得します。
	//	事前に validate() が必要です。
	auto type(node& _node) -> sstring;

	// 指定したノードのシンボルの検証を行います。
	//	(signature) や (symbol) は可能な限りアンロールされます。
	//	テンプレートなどがインスタンス化され場合はsymbolsに登録されます。
	//	戻り値は正規化されたノードです。
	node validate(node& _node);

	// ****************************************************
	// 実行系

	stkdat call(text _identify, std::initializer_list<stkdat> _argv);
	stkdat call(node& _function, std::initializer_list<stkdat> _argv);

	stkdat exec(node& _node);
};


PASSION_NAMESPACE_END
#endif
