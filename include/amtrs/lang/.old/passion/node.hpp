/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__node__hpp
#define	__libamtrs__lang__passion__node__hpp
#include "../system/log.hpp"
#include "../string/shared_string.hpp"
#include "../util/callback.hpp"
#include "types.hpp"
PASSION_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(node)
template<class, class...>
struct	_eacher;
AMTRS_IMPLEMENTS_END(node)
class	node;
struct	symbol;
class	symbolmap;
class	nodestatus;





struct	expr : ref_object
{
	enum class	each_result : int
	{
		r_continue	=  0,
		r_break		= -1,
		r_skip		=  1,
	};
	static constexpr each_result	r_continue	= each_result::r_continue;
	static constexpr each_result	r_break		= each_result::r_break;
	static constexpr each_result	r_skip		= each_result::r_skip;

	using	types				= expr_types;
	using	each_callback_type	= callback<each_result(expr* _e, int _depth, bool _in)>;		// 0 == continue, -1 == break, 1 == skip

	types			type		= types::none;
	char const*		location	= nullptr;


	expr(types _type, char const* _locate)
		: type(_type)
		, location(_locate)
	{}

	virtual auto clone() -> ref<expr> = 0;
	auto each(each_callback_type const& _callback) -> expr* { return each(_callback, 0); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* = 0;
	auto vartype(symbolmap& _map) -> sstring const& { if (mVartype.empty()) { on_type(_map); } return mVartype; }

protected:
	template<class... Args>	auto _each(each_callback_type const& _callback, int _depth, Args&&... _args) -> expr*;
	virtual void on_type(symbolmap& _map){}

	sstring	mVartype;
};


class	node
{
public:
	using	each_result	= expr::each_result;
	static constexpr each_result	r_continue	= each_result::r_continue;
	static constexpr each_result	r_break		= each_result::r_break;
	static constexpr each_result	r_skip		= each_result::r_skip;

	using	types				= expr::types;
	using	each_callback_type	= expr::each_callback_type;

	node() = default;
	node(node const& _r) = default;
	node(node&&) = default;
	node& operator = (node const& _r) = default;
	node& operator = (node&&) = default;
	node(ref<expr> _e) : mExpr(std::move(_e)) {}
	auto type() const noexcept -> types { return mExpr ? mExpr->type : types::none; }
	auto empty() const noexcept -> bool { return type() == types::none; }
	auto get() const noexcept -> expr* { return mExpr; }  
	auto vartype(symbolmap& _map) const noexcept -> sstring const& { static sstring d; return empty() ? d : mExpr->vartype(_map); }
	template<class T> ref<T> get_at() const noexcept { return type() == T::nodetype ? static_cast<T*>(mExpr.get()) : nullptr; }
	auto location() const noexcept { return mExpr ? mExpr->location : nullptr; }
	auto clone() const -> node { return mExpr ? node(mExpr->clone()) : node(); }
	template<class Callback> auto each(Callback _callback) -> expr*;

	
	// ノードを正規化します。
	//	すべてのノードのシンボルが解決されます。
	bool normalize(nodestatus& _status);


protected:
	ref<expr>	mExpr;
};


struct	expr_identify : expr
{
	static constexpr types	nodetype	= types::identify;
	sstring		name;
	expr_identify(expr_identify const&) = default;
	expr_identify(char const* _locate)
		: expr(nodetype, _locate)
	{}
	virtual auto clone() -> ref<expr> override { return new expr_identify(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
protected:
	virtual void on_type(symbolmap& _map){ mVartype = name; }
};


struct	expr_name_and_type : expr
{
	static constexpr types	nodetype	= types::name_and_type;
	sstring		name;
	node		type;
	expr_name_and_type(expr_name_and_type const&) = default;
	expr_name_and_type(char const* _locate)
		: expr(nodetype, _locate)
	{}
	virtual auto clone() -> ref<expr> override { return new expr_name_and_type(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
protected:
	virtual void on_type(symbolmap& _map){ mVartype = type.vartype(_map); }
};


struct	expr_number : expr
{
	static constexpr types	nodetype	= types::number;

	sstring		number;
	expr_number(expr_number const&) = default;
	expr_number(char const* _locate)
		: expr(nodetype, _locate)
	{}
	virtual auto clone() -> ref<expr> override { return new expr_number(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
protected:
	virtual void on_type(symbolmap& _map) { mVartype = "int"; }
};


struct	expr_command : expr
{
	sstring				identify;
	simplearray<node>	children;
	expr_command(expr_command const&) = default;
	expr_command(char const* _locate)
		: expr(types::command, _locate)
	{}
	virtual auto clone() -> ref<expr> override { return new expr_command(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// (parameters expr [expr...])
struct	expr_parameters : expr
{
	static constexpr types	nodetype	= types::c_parameters;
	simplearray<ref<expr_name_and_type>>	parameters;


	expr_parameters(expr_parameters const&) = default;
	expr_parameters(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_parameters(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};



// (signature expr...)
struct	expr_signature : expr
{
	static constexpr types	nodetype	= types::c_signature;
	simplearray<node>	signatures;

	expr_signature(expr_signature const&) = default;
	expr_signature(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_signature(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;

private:
	virtual void on_type(symbolmap& _map);
};


// (function `name`:`return_type` [(parameters)])
struct	expr_function : expr
{
	static constexpr types	nodetype	= types::c_function;
	bool					thiscall	= false;
	node					symbol;
	ref<expr_parameters>	parameters;
	simplearray<node>		statements;

	// validation 後に使用可能。
	auto validated_symbol() -> shared_string;

	expr_function(expr_function const&) = default;
	expr_function(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_function(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// (decltype expr)
struct	expr_decltype : expr
{
	static constexpr types	nodetype	= types::c_decltype;
	node					exp;

	expr_decltype(expr_decltype const&) = default;
	expr_decltype(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_decltype(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
protected:
	virtual void on_type(symbolmap& _map) { mVartype = exp.vartype(_map); }
};


// (decltype expr)
struct	expr_return : expr
{
	static constexpr types	nodetype	= types::c_return;
	node					value;

	expr_return(expr_return const&) = default;
	expr_return(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_return(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// (extends `base-class-name`:(extendsattr)...)
struct	expr_extendsattr : expr
{
	static constexpr types	nodetype	= types::c_extendsattr;
	simplearray<extends_attr_types>		attributes;

	expr_extendsattr(expr_extendsattr const&) = default;
	expr_extendsattr(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_extendsattr(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// (extends `base-class-name`:(extendsattr)...)
struct	expr_extends : expr
{
	static constexpr types	nodetype	= types::c_extends;
	struct	extend
	{
		sstring					name;
		ref<expr_extendsattr>	attr;
		expr* get() noexcept { return attr.get(); }
	};
	simplearray<extend>		bases;

	expr_extends(expr_extends const&) = default;
	expr_extends(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_extends(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// (class `class-name` (extends) member-expr...)
struct	expr_class : expr
{
	static constexpr types	nodetype	= types::c_class;
	node					symbol;
	ref<expr_extends>		extents;
	simplearray<node>		members;

	expr_class(expr_class const&) = default;
	expr_class(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { auto* p = new expr_class(*this); AMTRS_FORCE_LOG("clone {P} <= {P}", (expr*)p, (expr*)this); return p; }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// (add|sub|... left-expr right-expr)
struct	expr_bin_operator : expr
{
	static constexpr types	nodetype	= types::c_bin_operator;
	bin_operator_types		ope;
	node					left;
	node					right;

	expr_bin_operator(expr_bin_operator const&) = default;
	expr_bin_operator(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_bin_operator(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// (call `symbol` arguments...)
struct	expr_call : expr
{
	static constexpr types	nodetype	= types::c_call;
	node					symbol;
	simplearray<node>		arguments;
	auto validated_symbol() -> shared_string;

	expr_call(expr_call const&) = default;
	expr_call(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_call(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// (callm `symbol` this arguments...)
struct	expr_callm : expr
{
	static constexpr types	nodetype	= types::c_callm;
	node					symbol;
	node					thiz;
	simplearray<node>		arguments;

	expr_callm(expr_callm const&) = default;
	expr_callm(psnfl_status& _status, expr_command const&);
	virtual auto clone() -> ref<expr> override { return new expr_callm(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};


// "string"
struct	expr_string : expr
{
	static constexpr types	nodetype	= types::string;
	sstring					str;

	expr_string(expr_string const&) = default;
	expr_string(char const* _locate)
		: expr(nodetype, _locate)
	{}
	virtual auto clone() -> ref<expr> override { return new expr_string(*this); }
	virtual auto each(each_callback_type const& _callback, int _depth) -> expr* override;
};



template<class Callback>
auto node::each(Callback _callback) -> expr*
{
	if (!mExpr)
	{
		return	nullptr;
	}
	each_callback_type	cb(std::forward<Callback>(_callback));
	return	mExpr->each(cb);
}


void nodedump(node& _node);

PASSION_NAMESPACE_END
#endif
