/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__node__hxx
#define	__libamtrs__lang__passion__node__hxx
PSNVM_LANG_NAMESPACE_BEGIN
using namespace amtrs;
using namespace amtrs::sourcecode;
enum class	pflnode_types {
	none			= 0,
	parameter_list,
	declare_class,
	declare_concept,
	declare_enum,
	declare_function,
	declare_function_constractor,
	declare_function_destractor,
	declare_function_conversion,
	declare_function_operator,
	declare_namespace,
	declare_template,
	declare_variable,
	declare_specifier,
	expr_cast,
	expr_lambda,
	expr_requires,
	expr_static_integer,
	expr_static_string,
	literal_bool,
	literal_char,
	literal_integer,
	literal_string,
	literal_nullptr,
	literal_this,
	literal_sizeof,
	operator_address_of,
	operator_bin,
	operator_call,
	operator_conditions,
	operator_delete,
	operator_incdec,
	operator_indirection,
	operator_member_access,
	operator_member_call,
	operator_member_access_arrow,
	operator_member_call_arrow,
	operator_new,
	operator_single,
	operator_subscriptl,
	spec_member_friend,
	sepc_symbol_array,
	spec_symbol_const,
	spec_symbol_reference,
	spec_symbol_move,
	spec_symbol_parampack,
	sepc_symbol_pointer,
	stmt_branch_break,
	stmt_branch_continue,
	stmt_compound,
	stmt_for,
	stmt_for_rangedbase,
	stmt_if,
	stmt_do_while,
	stmt_return,
	stmt_switch,
	stmt_while,
	symbol_decltye,
	symbol_name,
	symbol_nameabs,
	symbol_functype,
	symbol_scope,
	symbol_operator,
	symbol_template,
	using_alias,
	using_namespace,
	echo,
	initializer_list,
	struct_initializer,
	list,
	label,
};


class	pflnode;


enum class	pn_declare_specifier : uint32_t
{
	spec_static				= 1<<0,
	spec_virtual			= 1<<1,
	spec_inline				= 1<<2,
	spec_volatile			= 1<<3,
	spec_constexpr			= 1<<4,
	spec_const				= 1<<5,
	spec_noexcept			= 1<<7,
	spec_virt_override		= 1<<8,
	spec_virt_final			= 1<<9,
	spec_default			= 1<<10,
	spec_delete				= 1<<11,
	spec_public				= 1<<12,		// 省略時は public 扱いになる
	spec_protected			= 1<<13,
	spec_private			= 1<<14,
};


class	pflnodebase : public ref_object
{
public:
	using	types	= pflnode_types;

	virtual ~pflnodebase() {}

	auto location() const noexcept { return mLocation; }
	auto type() const noexcept { return mType; }

protected:
	text		mLocation	= nullptr;
	types		mType		= types::none;

	friend	pflnode;
};


class	pflnode
{
public:
	using	types	= pflnode_types;

	pflnode() = default;
	pflnode(pflnode const&) = default;
	pflnode(pflnode&&) = default;
	pflnode(ref<pflnodebase> _r) : mNode(std::move(_r)) {}
	pflnode& operator = (pflnode const&) = default;
	pflnode& operator = (pflnode&&) = default;

	auto type() const noexcept -> types { return mNode.empty() ? types::none : mNode->type(); }
	auto empty() const noexcept -> bool { return type() == types::none; }
	auto location() const noexcept -> text { return empty() ? nullptr : mNode->location(); }
	auto is_symbol() const noexcept -> bool;
	operator bool () const noexcept { return !empty(); }

	template<class T>
	auto at() -> ref<T>
	{
		if (type() == T::type)
		{
			return	static_cast<T*>(mNode.get());
		}
		return	{};
	}

	template<class T, class... Args>
	static auto make(text _location, Args&&... _args) -> ref<T>
	{
		ref<T>	r	= new T(std::forward<Args>(_args)...);
		r->mLocation	= _location;
		r->mType		= T::type;
		return	r;
	}



protected:
	ref<pflnodebase>	mNode;
};


auto is_destractor_symbol(pflnode& _n) -> bool;



struct	pn_parameter_list : pflnodebase
{
	static constexpr types	type	= types::parameter_list;

	struct	parameter
	{
		pflnode	first;
		pflnode	second;
		pflnode	init;
	};

	simplearray<parameter>	parameters;

	pn_parameter_list(simplearray<parameter> _params)
		: parameters(std::move(_params))
	{}
};


struct	pn_declare_class : pflnodebase
{
	static constexpr types	type	= types::declare_class;

	struct	base
	{
		pflnode			name;
		unsigned int	access	= 0;
	};

	pflnode					name;
	simplearray<base>		extends;
	simplearray<pflnode>	members;

	pn_declare_class(pflnode _name, simplearray<pflnode> _members)
		: name(std::move(_name))
		, members(std::move(_members))
	{}

	pn_declare_class(pflnode _name, simplearray<base> _extends, simplearray<pflnode> _members)
		: name(std::move(_name))
		, extends(std::move(_extends))
		, members(std::move(_members))
	{}
};


struct	pn_declare_concept : pflnodebase
{
	static constexpr types	type	= types::declare_concept;

	pflnode				name;
	pflnode				expr;

	pn_declare_concept(pflnode _name, pflnode _expr)
		: name(std::move(_name))
		, expr(std::move(_expr))
	{}
};


struct	pn_declare_enum : pflnodebase
{
	static constexpr types	type	= types::declare_enum;

	struct	item
	{
		text			name;
		pflnode			value;
	};

	pflnode				name;
	pflnode				base;
	simplearray<item>	values;


	pn_declare_enum(pflnode _name, pflnode _base, simplearray<item> _values)
		: name(std::move(_name))
		, base(std::move(_base))
		, values(std::move(_values))
	{}
};


struct	pn_declare_function : pflnodebase
{
	static constexpr types	type	= types::declare_function;

	pflnode		name;
	pflnode		rettype;
	pflnode		parameters;
	pflnode		codes;
	uint32_t	specifier	= 0;

	// except

	pn_declare_function(pflnode _name, pflnode _ret, pflnode _params, pflnode _codes, uint32_t _specifer)
		: name(std::move(_name))
		, rettype(std::move(_ret))
		, parameters(std::move(_params))
		, codes(std::move(_codes))
		, specifier(_specifer)
	{}
};


struct	pn_declare_function_constractor : pn_declare_function
{
	static constexpr types	type	= types::declare_function_constractor;

	struct	initinfo
	{
		pflnode					name;
		simplearray<pflnode>	init;
	};
	simplearray<initinfo>	initializers;


	pn_declare_function_constractor(pflnode _name, pflnode _ret, simplearray<initinfo> _inits, pflnode _params, pflnode _codes, uint32_t _flags)
		: pn_declare_function(std::move(_name), std::move(_ret), std::move(_params), std::move(_codes), _flags)
		, initializers(std::move(_inits))
	{}
};


struct	pn_declare_function_destractor : pn_declare_function
{
	static constexpr types	type	= types::declare_function_destractor;

	pn_declare_function_destractor(pflnode _name, pflnode _ret, pflnode _params, pflnode _codes, uint32_t _flags)
		: pn_declare_function(std::move(_name), std::move(_ret), std::move(_params), std::move(_codes), _flags)
	{}
};



struct	pn_declare_function_conversion : pn_declare_function
{
	static constexpr types	type	= types::declare_function_conversion;

	pn_declare_function_conversion(pflnode _ret, pflnode _params, pflnode _codes, uint32_t _flags)
		: pn_declare_function(pflnode(), std::move(_ret), std::move(_params), std::move(_codes), _flags)
	{}
};


struct	pn_declare_function_operator : pn_declare_function
{
	static constexpr types	type	= types::declare_function_operator;

	operator_type	ope;

	pn_declare_function_operator(operator_type _ope, pflnode _ret, pflnode _params, pflnode _codes, uint32_t _flags)
		: pn_declare_function(pflnode(), std::move(_ret), std::move(_params), std::move(_codes), _flags)
		, ope(_ope)
	{}
};


struct	pn_declare_namespace : pflnodebase
{
	static constexpr types	type	= types::declare_namespace;

	pflnode				name;
	simplearray<pflnode>	statements;

	pn_declare_namespace(pflnode _name, simplearray<pflnode> _statements)
		: name(std::move(_name))
		, statements(std::move(_statements))
	{}
};



struct	pn_declare_template : pn_parameter_list
{
	static constexpr types	type	= types::declare_template;

	pflnode	declare;	// declare_class or declare_function

	pn_declare_template(pn_parameter_list* paramlist, pflnode _declare)
		: pn_parameter_list(paramlist ? std::move(paramlist->parameters) : simplearray<parameter>())
		, declare(std::move(_declare))
	{}
};



struct	pn_declare_variable : pflnodebase
{
	static constexpr types	type	= types::declare_variable;

	struct	var
	{
		pflnode		name;
		pflnode		init;
	};

	uint32_t			specifier;
	pflnode				typesym;
	simplearray<var>	values;

	pn_declare_variable(uint32_t _specifier, pflnode _t, simplearray<var> _values)
		: specifier(std::move(_specifier))
		, typesym(std::move(_t))
		, values(std::move(_values))
	{}
};



struct	pn_expr_cast : pflnodebase
{
	static constexpr types	type	= types::expr_cast;
	enum class cast_types
	{
		cast_ctype			= 0,
		cast_const			= 1,
		cast_reinterpret	= 2,
		cast_static			= 3,
	};

	cast_types	casttype;
	pflnode		totype;
	pflnode		value;

	pn_expr_cast(cast_types _t, pflnode _to, pflnode _value)
		: casttype(std::move(_t))
		, totype(std::move(_to))
		, value(std::move(_value))
	{}
};


struct	pn_expr_lambda : pflnodebase
{
	static constexpr types	type	= types::expr_lambda;


	pflnode			rettype;
	pflnode			captures;
	pflnode			parameters;
	pflnode			statements;

	pn_expr_lambda(pflnode _rettype, pflnode _captures, pflnode _parameters, pflnode _statements)
		: rettype(std::move(_rettype))
		, captures(std::move(_captures))
		, parameters(std::move(_parameters))
		, statements(std::move(_statements))
	{}
};


struct	pn_expr_requires : pflnodebase
{
	static constexpr types	type	= types::expr_requires;

	struct	condinfo
	{
		pflnode	expr;
		pflnode	symbol;						// empty() || typeid
	};

	pflnode					parameters;		// empty() || parameter_list
	simplearray<condinfo>	conditions;

	pn_expr_requires(pflnode _params, simplearray<condinfo> _conditions)
		: parameters(std::move(_params))
		, conditions(std::move(_conditions))
	{}
};


struct	pn_literal_sizeof : pflnodebase
{
	static constexpr types	type	= types::literal_sizeof;

	pflnode				value;

	pn_literal_sizeof(pflnode _value)
		: value(std::move(_value))
	{}

};


struct	pn_expr_static_integer : pflnodebase
{
	static constexpr types	type	= types::expr_static_integer;

	intmax_t		integer;

	pn_expr_static_integer(intmax_t n)
		: integer(n)
	{}
};



struct	pn_expr_static_string : pflnodebase
{
	static constexpr types	type	= types::expr_static_string;

	text			str;

	pn_expr_static_string(text l)
		: str(std::move(l))
	{}
};


struct	pn_literal_bool : pflnodebase
{
	static constexpr types	type	= types::literal_bool;

	bool	value;

	pn_literal_bool(bool _v)
		: value(_v)
	{}
};


struct	pn_literal_char : pflnodebase
{
	static constexpr types	type	= types::literal_char;

	int		c;

	pn_literal_char(int _c)
		: c(_c)
	{}
};



struct	pn_literal_integer : pn_expr_static_integer
{
	static constexpr types	type	= types::literal_integer;

	numeric_literal	literal;

	pn_literal_integer(intmax_t n, numeric_literal l)
		: pn_expr_static_integer(n)
		, literal(std::move(l))
	{}
};



struct	pn_literal_nullptr : pflnodebase
{
	static constexpr types	type	= types::literal_nullptr;

	pn_literal_nullptr()
	{}
};



struct	pn_literal_string : pn_expr_static_string
{
	static constexpr types	type	= types::literal_string;

	pn_literal_string(text l)
		: pn_expr_static_string(std::move(l))
	{}
};


struct	pn_literal_this : pflnodebase
{
	static constexpr types	type	= types::literal_this;

	pn_literal_this()
	{}
};



struct	pn_operator_address_of : pflnodebase
{
	static constexpr types	type	= types::operator_address_of;

	pflnode	value;

	pn_operator_address_of(pflnode _value)
		: value(std::move(_value))
	{}
};


struct	pn_operator_bin: pflnodebase
{
	static constexpr types	type	= types::operator_bin;

	operator_type	ope;
	pflnode			left;
	pflnode			right;

	pn_operator_bin(operator_type _ope, pflnode _left, pflnode _right)
		: ope(_ope)
		, left(std::move(_left))
		, right(std::move(_right))
	{}
};


struct	pn_operator_call : pflnodebase
{
	static constexpr types	type	= types::operator_call;

	pflnode				function;
	simplearray<pflnode>	arguments;

	pn_operator_call(pflnode _func, simplearray<pflnode> _args)
		: function(std::move(_func))
		, arguments(std::move(_args))
	{}
};



struct	pn_operator_conditions : pflnodebase
{
	static constexpr types	type	= types::operator_conditions;

	pflnode	condition;
	pflnode	v1;
	pflnode	v2;

	pn_operator_conditions(pflnode _cond, pflnode _v1, pflnode _v2)
		: condition(std::move(_cond))
		, v1(std::move(_v1))
		, v2(std::move(_v2))
	{}
};


struct	pn_operator_delete : pflnodebase
{
	static constexpr types	type	= types::operator_delete;

	bool				is_array;
	pflnode				value;

	pn_operator_delete(bool _array, pflnode _value)
		: is_array(_array)
		, value(std::move(_value))
	{}

};


struct	pn_operator_incdec : pflnodebase
{
	static constexpr types	type	= types::operator_incdec;


	operator_type	ope;
	pflnode			value;

	pn_operator_incdec(operator_type _ope, pflnode _value)
		: ope(std::move(_ope))
		, value(std::move(_value))
	{}
};



struct	pn_operator_indirection : pflnodebase
{
	static constexpr types	type	= types::operator_indirection;

	pflnode	value;

	pn_operator_indirection(pflnode _value)
		: value(std::move(_value))
	{}
};


struct	pn_operator_member_access : pflnodebase
{
	static constexpr types	type	= types::operator_member_access;

	pflnode	value;
	pflnode	member;

	pn_operator_member_access(pflnode _value, pflnode _member)
		: value(std::move(_value))
		, member(std::move(_member))
	{}
};


struct	pn_operator_member_access_arrow : pn_operator_member_access
{
	static constexpr types	type	= types::operator_member_access_arrow;
	using	pn_operator_member_access::pn_operator_member_access;
};


struct	pn_operator_member_call : pflnodebase
{
	static constexpr types	type	= types::operator_member_call;

	pflnode					function;
	pflnode					thiz;
	simplearray<pflnode>	arguments;

	pn_operator_member_call(pflnode _func, pflnode _thiz, simplearray<pflnode> _args)
		: function(std::move(_func))
		, thiz(std::move(_thiz))
		, arguments(std::move(_args))
	{}
};


struct	pn_operator_member_call_arrow : pn_operator_member_call
{
	static constexpr types	type	= types::operator_member_call_arrow;
	using	pn_operator_member_call::pn_operator_member_call;
};


struct	pn_operator_new : pflnodebase
{
	static constexpr types	type	= types::operator_new;

	bool					is_array;
	pflnode					totype;
	simplearray<pflnode>	values;
	simplearray<pflnode>	arguments;

	pn_operator_new(bool _array, pflnode _totype, simplearray<pflnode> _values, simplearray<pflnode> _args)
		: is_array(_array)
		, totype(std::move(_totype))
		, values(std::move(_values))
		, arguments(std::move(_args))
	{}
};


struct	pn_operator_single: pflnodebase
{
	static constexpr types	type	= types::operator_single;

	operator_type	ope;
	pflnode			value;

	pn_operator_single(operator_type _ope, pflnode _value)
		: ope(_ope)
		, value(std::move(_value))
	{}
};


struct	pn_operator_subscriptl : pflnodebase
{
	static constexpr types	type	= types::operator_subscriptl;

	pflnode	value;
	pflnode	idx;

	pn_operator_subscriptl(pflnode _value, pflnode _idx)
		: value(std::move(_value))
		, idx(std::move(_idx))
	{}
};


struct	pn_spec_symbol_array : pflnodebase
{
	static constexpr types	type	= types::sepc_symbol_array;

	pflnode					name;
	simplearray<pflnode>		sizes;

	pn_spec_symbol_array(pflnode _sym, simplearray<pflnode> _sizes)
		: name(std::move(_sym))
		, sizes(std::move(_sizes))
	{}
};



struct	pn_spec_symbol_const : pflnodebase
{
	static constexpr types	type	= types::spec_symbol_const;

	pflnode			name;

	pn_spec_symbol_const(pflnode l)
		: name(std::move(l))
	{}
};

struct	pn_spec_member_friend : pflnodebase
{
	static constexpr types	type	= types::spec_member_friend;

	pflnode			decl;

	pn_spec_member_friend(pflnode _decl)
		: decl(std::move(_decl))
	{}
};


struct	pn_symbol_functype : pflnodebase
{
	static constexpr types	type	= types::symbol_functype;

	pflnode	rettype;
	pflnode	parameters;

	pn_symbol_functype(pflnode r, pflnode p)
		: rettype(std::move(r))
		, parameters(std::move(p))
	{}
};

struct	pn_spec_symbol_move : pflnodebase
{
	static constexpr types	type	= types::spec_symbol_move;

	pflnode			name;

	pn_spec_symbol_move(pflnode l)
		: name(std::move(l))
	{}
};

struct	pn_spec_symbol_parampack : pflnodebase
{
	static constexpr types	type	= types::spec_symbol_parampack;

	pflnode			name;

	pn_spec_symbol_parampack(pflnode l)
		: name(std::move(l))
	{}
};


struct	pn_spec_symbol_pointer : pflnodebase
{
	static constexpr types	type	= types::sepc_symbol_pointer;

	pflnode			name;

	pn_spec_symbol_pointer(pflnode _type_id)
		: name(std::move(_type_id))
	{}
};

struct	pn_spec_symbol_reference : pflnodebase
{
	static constexpr types	type	= types::spec_symbol_reference;

	pflnode			name;

	pn_spec_symbol_reference(pflnode l)
		: name(std::move(l))
	{}
};


struct	pn_symbol_decltye : pflnodebase
{
	static constexpr types	type	= types::symbol_decltye;

	pflnode	value;

	pn_symbol_decltye(pflnode _value)
		: value(std::move(_value))
	{}
};



struct	pn_symbol_name : pflnodebase
{
	static constexpr types	type	= types::symbol_name;

	text	name;

	pn_symbol_name(text l)
		: name(std::move(l))
	{}
};


struct	pn_symbol_nameabs : pflnodebase
{
	static constexpr types	type	= types::symbol_nameabs;

	pflnode	scope;

	pn_symbol_nameabs(pflnode _s)
		: scope(std::move(_s))
	{}
};



struct	pn_symbol_operator : pflnodebase
{
	static constexpr types	type	= types::symbol_operator;

	operator_type	ope;

	pn_symbol_operator(operator_type _o)
		: ope(std::move(_o))
	{}
};


struct	pn_symbol_scope : pflnodebase
{
	static constexpr types	type	= types::symbol_scope;

	simplearray<pflnode>	scopes;

	pn_symbol_scope(simplearray<pflnode> _scopes)
		: scopes(std::move(_scopes))
	{}
};



struct	pn_symbol_template : pn_symbol_name
{
	static constexpr types	type	= types::symbol_template;

	simplearray<pflnode>	templateParams;

	pn_symbol_template(text s, simplearray<pflnode> _params)
		: pn_symbol_name(std::move(s))
		, templateParams(std::move(_params))
	{}
};


struct	pn_stmt_branch_break : pflnodebase
{
	static constexpr types	type	= types::stmt_branch_break;
	pn_stmt_branch_break()
	{}
};


struct	pn_stmt_branch_continue : pflnodebase
{
	static constexpr types	type	= types::stmt_branch_continue;
	pn_stmt_branch_continue()
	{}
};

struct	pn_stmt_compound : pflnodebase
{
	static constexpr types	type	= types::stmt_compound;

	simplearray<pflnode>	statements;

	pn_stmt_compound(simplearray<pflnode> _value)
		: statements(std::move(_value))
	{}
};


struct	pn_stmt_do_while : pflnodebase
{
	static constexpr types	type	= types::stmt_do_while;

	pflnode	conditions;
	pflnode	statements;

	pn_stmt_do_while(pflnode _cond, pflnode _stmt)
		: conditions(std::move(_cond))
		, statements(std::move(_stmt))
	{}
};


struct	pn_stmt_for : pflnodebase
{
	static constexpr types	type	= types::stmt_for;

	pflnode	init;
	pflnode	conditions;
	pflnode	increments;
	pflnode	statements;

	pn_stmt_for(pflnode _init, pflnode _cond, pflnode _inc, pflnode _stmt)
		: init(std::move(_init))
		, conditions(std::move(_cond))
		, increments(std::move(_inc))
		, statements(std::move(_stmt))
	{}
};


struct	pn_stmt_for_rangedbase : pflnodebase
{
	static constexpr types	type	= types::stmt_for_rangedbase;

	pflnode	init;
	pflnode	value;
	pflnode	statements;

	pn_stmt_for_rangedbase(pflnode _init, pflnode _value, pflnode _stmt)
		: init(std::move(_init))
		, value(std::move(_value))
		, statements(std::move(_stmt))
	{}
};


struct	pn_stmt_if : pflnodebase
{
	static constexpr types	type	= types::stmt_if;

	pflnode	init;
	pflnode	conditions;
	pflnode	statements1;
	pflnode	statements2;

	pn_stmt_if(pflnode _init, pflnode _cond, pflnode _stmt1, pflnode _stmt2)
		: init(std::move(_init))
		, conditions(std::move(_cond))
		, statements1(std::move(_stmt1))
		, statements2(std::move(_stmt2))
	{}
};


struct	pn_stmt_return : pflnodebase
{
	static constexpr types	type	= types::stmt_return;

	pflnode	value;

	pn_stmt_return(pflnode _value)
		: value(std::move(_value))
	{}
};


struct	pn_stmt_switch : pflnodebase
{
	static constexpr types	type	= types::stmt_switch;

	struct	casedata
	{
		pflnode	value;
		pflnode	statements;
	};

	pflnode					conditions;
	simplearray<casedata>	cases;

	pn_stmt_switch(pflnode _cond, simplearray<casedata> _cases)
		: conditions(std::move(_cond))
		, cases(std::move(_cases))
	{}
};


struct	pn_stmt_while : pflnodebase
{
	static constexpr types	type	= types::stmt_while;

	pflnode	conditions;
	pflnode	statements;

	pn_stmt_while(pflnode _cond, pflnode _stmt)
		: conditions(std::move(_cond))
		, statements(std::move(_stmt))
	{}
};



struct	pn_using_alias : pflnodebase
{
	static constexpr types	type	= types::using_alias;

	pflnode	name;
	pflnode	target;

	pn_using_alias(pflnode l, pflnode r)
		: name(std::move(l))
		, target(std::move(r))
	{}
};


struct	pn_using_namespace : pflnodebase
{
	static constexpr types	type	= types::using_namespace;

	pflnode	name;

	pn_using_namespace(pflnode l)
		: name(std::move(l))
	{}
};


struct	pn_echo : pflnodebase
{
	static constexpr types	type	= types::echo;

	text	doc;

	pn_echo(text _doc)
		: doc(std::move(_doc))
	{}
};


struct	pn_label : pflnodebase
{
	static constexpr types	type	= types::label;

	text			name;

	pn_label(text _label)
		: name(std::move(_label))
	{}
};


struct	pn_list : pflnodebase
{
	static constexpr types	type	= types::list;

	simplearray<pflnode>	values;

	pn_list(simplearray<pflnode> _values)
		: values(std::move(_values))
	{}
};




struct	pn_initializer_list : pflnodebase
{
	static constexpr types	type	= types::initializer_list;

	struct	initinfo
	{
		text	name;
		pflnode	value;
	};

	simplearray<initinfo>		values;

	pn_initializer_list(simplearray<initinfo> _values)
		: values(std::move(_values))
	{}

};

struct	pn_struct_initializer : pflnodebase
{
	static constexpr types	type	= types::struct_initializer;

	pflnode	name;
	pflnode	initlist;

	pn_struct_initializer(pflnode _sym, pflnode _inits)
		: name(std::move(_sym))
		, initlist(std::move(_inits))
	{}
};




PSNVM_LANG_NAMESPACE_END
#endif
