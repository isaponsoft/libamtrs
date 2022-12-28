/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__node__hxx
#define	__libamtrs__lang__passion__node__hxx
#include "types.hxx"
PSNVM_LANG_NAMESPACE_BEGIN
class	node;


enum class	node_types
{
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
	operator_member_access_arrow,
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


class	nodebase : public ref_object
{
public:
	using	types	= node_types;

	virtual ~nodebase() {}

	auto location() const noexcept { return mLocation; }
	auto type() const noexcept { return mType; }

protected:
	char const*	mLocation	= nullptr;
	types		mType		= types::none;

	friend	node;
};


class	node
{
public:
	using	types	= node_types;

	node() = default;
	node(node const&) = default;
	node(node&&) = default;
	node(ref<nodebase> _r) : mNode(std::move(_r)) {}
	node& operator = (node const&) = default;
	node& operator = (node&&) = default;

	auto type() const noexcept -> types { return mNode.empty() ? types::none : mNode->type(); }
	auto empty() const noexcept -> bool { return type() == types::none; }
	auto location() const noexcept -> char const* { return empty() ? nullptr : mNode->location(); }
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
	static auto make(char const* _location, Args&&... _args) -> ref<T>
	{
		ref<T>	r	= new T(std::forward<Args>(_args)...);
		r->mLocation	= _location;
		r->mType		= T::type;
		return	r;
	}

	template<class T, class... Args>
	static auto make(text _location, Args&&... _args) -> ref<T>
	{
		return	make<T>(_location.s, std::forward<Args>(_args)...);
	}


protected:
	ref<nodebase>	mNode;
};


auto is_destractor_symbol(node& _n) -> bool;



struct	pn_parameter_list : nodebase
{
	static constexpr types	type	= types::parameter_list;

	struct	parameter
	{
		node	first;
		node	second;
		node	init;
	};

	simplearray<parameter>	parameters;

	pn_parameter_list(simplearray<parameter> _params)
		: parameters(std::move(_params))
	{}
};


struct	pn_declare_class : nodebase
{
	static constexpr types	type	= types::declare_class;

	static constexpr unsigned int	ac_public		= 1<<0;
	static constexpr unsigned int	ac_protected	= 1<<1;
	static constexpr unsigned int	ac_private		= 1<<2;
	static constexpr unsigned int	ac_virtual		= 1<<3;

	struct	base
	{
		node			name;
		unsigned int	access	= 0;
	};

	node				name;
	simplearray<base>	extends;
	simplearray<node>	members;

	pn_declare_class(node _name, simplearray<node> _members)
		: name(std::move(_name))
		, members(std::move(_members))
	{}

	pn_declare_class(node _name, simplearray<base> _extends, simplearray<node> _members)
		: name(std::move(_name))
		, extends(std::move(_extends))
		, members(std::move(_members))
	{}
};


struct	pn_declare_concept : nodebase
{
	static constexpr types	type	= types::declare_concept;

	node				name;
	node				expr;

	pn_declare_concept(node _name, node _expr)
		: name(std::move(_name))
		, expr(std::move(_expr))
	{}
};


struct	pn_declare_enum : nodebase
{
	static constexpr types	type	= types::declare_enum;

	struct	item
	{
		text			name;
		node			value;
	};

	node				name;
	node				base;
	simplearray<item>	values;


	pn_declare_enum(node _name, node _base, simplearray<item> _values)
		: name(std::move(_name))
		, base(std::move(_base))
		, values(std::move(_values))
	{}
};


struct	pn_declare_function : nodebase
{
	using	flags_type		= uint32_t;	

	static constexpr types	type	= types::declare_function;


	static constexpr flags_type		spec_const				= 1<<(0+0);
	static constexpr flags_type		spec_volatile			= 1<<(0+1);
	static constexpr flags_type		spec_noexcept			= 1<<(0+2);

	static constexpr flags_type		spec_virt_override		= 1<<(8+0);
	static constexpr flags_type		spec_virt_final			= 1<<(8+1);

	static constexpr flags_type		spec_default			= 1<<(12+0);
	static constexpr flags_type		spec_delete				= 1<<(12+1);


	node		specifier;
	node		name;
	node		rettype;
	node		parameters;
	node		codes;
	flags_type	flags	= 0;

	// except

	pn_declare_function(node _specifier, node _name, node _ret, node _params, node _codes, flags_type _flags)
		: specifier(std::move(_specifier))
		, name(std::move(_name))
		, rettype(std::move(_ret))
		, parameters(std::move(_params))
		, codes(std::move(_codes))
		, flags(_flags)
	{}
};


struct	pn_declare_function_constractor : pn_declare_function
{
	static constexpr types	type	= types::declare_function_constractor;

	struct	initinfo
	{
		node	name;
		node	init;
	};
	simplearray<initinfo>	initializers;


	pn_declare_function_constractor(node _specifier, node _name, node _ret, simplearray<initinfo> _inits, node _params, node _codes, flags_type _flags)
		: pn_declare_function(std::move(_specifier), std::move(_name), std::move(_ret), std::move(_params), std::move(_codes), _flags)
		, initializers(std::move(_inits))
	{}
};


struct	pn_declare_function_destractor : pn_declare_function
{
	static constexpr types	type	= types::declare_function_destractor;

	pn_declare_function_destractor(node _specifier, node _name, node _ret, node _params, node _codes, flags_type _flags)
		: pn_declare_function(std::move(_specifier), std::move(_name), std::move(_ret), std::move(_params), std::move(_codes), _flags)
	{}
};



struct	pn_declare_function_conversion : pn_declare_function
{
	static constexpr types	type	= types::declare_function_conversion;

	pn_declare_function_conversion(node _specifier, node _ret, node _params, node _codes, flags_type _flags)
		: pn_declare_function(std::move(_specifier), node(), std::move(_ret), std::move(_params), std::move(_codes), _flags)
	{}
};


struct	pn_declare_function_operator : pn_declare_function
{
	static constexpr types	type	= types::declare_function_operator;

	operator_type	ope;

	pn_declare_function_operator(node _specifier, operator_type _ope, node _ret, node _params, node _codes, flags_type _flags)
		: pn_declare_function(std::move(_specifier), node(), std::move(_ret), std::move(_params), std::move(_codes), _flags)
		, ope(_ope)
	{}
};


struct	pn_declare_namespace : nodebase
{
	static constexpr types	type	= types::declare_namespace;

	node				name;
	simplearray<node>	statements;

	pn_declare_namespace(node _name, simplearray<node> _statements)
		: name(std::move(_name))
		, statements(std::move(_statements))
	{}
};


struct	pn_declare_specifier : nodebase
{
	static constexpr unsigned int	declspec_static		= 1<<0;
	static constexpr unsigned int	declspec_virtual	= 1<<1;
	static constexpr unsigned int	declspec_inline		= 1<<2;
	static constexpr unsigned int	declspec_volatile	= 1<<3;
	static constexpr unsigned int	declspec_constexpr	= 1<<4;

	static constexpr types	type	= types::declare_specifier;

	unsigned int	flags;


	pn_declare_specifier(unsigned int _flags)
		: flags(_flags)
	{}
};


struct	pn_declare_template : pn_parameter_list
{
	static constexpr types	type	= types::declare_template;

	node	declare;	// declare_class or declare_function

	pn_declare_template(pn_parameter_list* paramlist, node _declare)
		: pn_parameter_list(paramlist ? std::move(paramlist->parameters) : simplearray<parameter>())
		, declare(std::move(_declare))
	{}
};



struct	pn_declare_variable : nodebase
{
	static constexpr types	type	= types::declare_variable;

	struct	var
	{
		node		name;
		node		init;
	};

	node				specifier;
	node				typesym;
	simplearray<var>	values;

	pn_declare_variable(node _specifier, node _t, simplearray<var> _values)
		: specifier(std::move(_specifier))
		, typesym(std::move(_t))
		, values(std::move(_values))
	{}
};



struct	pn_expr_cast : nodebase
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
	node		totype;
	node		value;

	pn_expr_cast(cast_types _t, node _to, node _value)
		: casttype(std::move(_t))
		, totype(std::move(_to))
		, value(std::move(_value))
	{}
};


struct	pn_expr_lambda : nodebase
{
	static constexpr types	type	= types::expr_lambda;


	node			rettype;
	node			captures;
	node			parameters;
	node			statements;

	pn_expr_lambda(node _rettype, node _captures, node _parameters, node _statements)
		: rettype(std::move(_rettype))
		, captures(std::move(_captures))
		, parameters(std::move(_parameters))
		, statements(std::move(_statements))
	{}
};


struct	pn_expr_requires : nodebase
{
	static constexpr types	type	= types::expr_requires;

	struct	condinfo
	{
		node	expr;
		node	symbol;						// empty() || typeid
	};

	node					parameters;		// empty() || parameter_list
	simplearray<condinfo>	conditions;

	pn_expr_requires(node _params, simplearray<condinfo> _conditions)
		: parameters(std::move(_params))
		, conditions(std::move(_conditions))
	{}
};


struct	pn_literal_sizeof : nodebase
{
	static constexpr types	type	= types::literal_sizeof;

	node				value;

	pn_literal_sizeof(node _value)
		: value(std::move(_value))
	{}

};


struct	pn_expr_static_integer : nodebase
{
	static constexpr types	type	= types::expr_static_integer;

	intmax_t		integer;

	pn_expr_static_integer(intmax_t n)
		: integer(n)
	{}
};



struct	pn_expr_static_string : nodebase
{
	static constexpr types	type	= types::expr_static_string;

	text			str;

	pn_expr_static_string(text l)
		: str(std::move(l))
	{}
};


struct	pn_literal_bool : nodebase
{
	static constexpr types	type	= types::literal_bool;

	bool	value;

	pn_literal_bool(bool _v)
		: value(_v)
	{}
};


struct	pn_literal_char : nodebase
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



struct	pn_literal_nullptr : nodebase
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


struct	pn_literal_this : nodebase
{
	static constexpr types	type	= types::literal_this;

	pn_literal_this()
	{}
};



struct	pn_operator_address_of : nodebase
{
	static constexpr types	type	= types::operator_address_of;

	node	value;

	pn_operator_address_of(node _value)
		: value(std::move(_value))
	{}
};


struct	pn_operator_bin: nodebase
{
	static constexpr types	type	= types::operator_bin;

	operator_type	ope;
	node			left;
	node			right;

	pn_operator_bin(operator_type _ope, node _left, node _right)
		: ope(_ope)
		, left(std::move(_left))
		, right(std::move(_right))
	{}
};


struct	pn_operator_call : nodebase
{
	static constexpr types	type	= types::operator_call;

	node				function;
	simplearray<node>	arguments;

	pn_operator_call(node _func, simplearray<node> _args)
		: function(std::move(_func))
		, arguments(std::move(_args))
	{}
};



struct	pn_operator_conditions : nodebase
{
	static constexpr types	type	= types::operator_conditions;

	node	condition;
	node	v1;
	node	v2;

	pn_operator_conditions(node _cond, node _v1, node _v2)
		: condition(std::move(_cond))
		, v1(std::move(_v1))
		, v2(std::move(_v2))
	{}
};


struct	pn_operator_delete : nodebase
{
	static constexpr types	type	= types::operator_delete;

	bool				is_array;
	node				value;

	pn_operator_delete(bool _array, node _value)
		: is_array(_array)
		, value(std::move(_value))
	{}

};


struct	pn_operator_incdec : nodebase
{
	static constexpr types	type	= types::operator_incdec;


	node			value;
	operator_type	ope;

	pn_operator_incdec(operator_type _ope, node _value)
		: ope(std::move(_ope))
		, value(std::move(_value))
	{}
};



struct	pn_operator_indirection : nodebase
{
	static constexpr types	type	= types::operator_indirection;

	node	value;

	pn_operator_indirection(node _value)
		: value(std::move(_value))
	{}
};



struct	pn_operator_member_access : nodebase
{
	static constexpr types	type	= types::operator_member_access;

	node	value;
	node	member;

	pn_operator_member_access(node _value, node _member)
		: value(std::move(_value))
		, member(std::move(_member))
	{}
};


struct	pn_operator_member_access_arrow : nodebase
{
	static constexpr types	type	= types::operator_member_access_arrow;

	node	value;
	node	member;

	pn_operator_member_access_arrow(node _value, node _member)
		: value(std::move(_value))
		, member(std::move(_member))
	{}
};


struct	pn_operator_new : nodebase
{
	static constexpr types	type	= types::operator_new;

	bool				is_array;
	node				totype;
	simplearray<node>	values;
	simplearray<node>	arguments;

	pn_operator_new(bool _array, node _totype, simplearray<node> _values, simplearray<node> _args)
		: is_array(_array)
		, totype(std::move(_totype))
		, values(std::move(_values))
		, arguments(std::move(_args))
	{}
};


struct	pn_operator_single: nodebase
{
	static constexpr types	type	= types::operator_single;

	operator_type	ope;
	node			value;

	pn_operator_single(operator_type _ope, node _value)
		: ope(_ope)
		, value(std::move(_value))
	{}
};


struct	pn_operator_subscriptl : nodebase
{
	static constexpr types	type	= types::operator_subscriptl;

	node	value;
	node	idx;

	pn_operator_subscriptl(node _value, node _idx)
		: value(std::move(_value))
		, idx(std::move(_idx))
	{}
};


struct	pn_sepc_symbol_array : nodebase
{
	static constexpr types	type	= types::sepc_symbol_array;

	node					name;
	simplearray<node>		sizes;

	pn_sepc_symbol_array(node _sym, simplearray<node> _sizes)
		: name(std::move(_sym))
		, sizes(std::move(_sizes))
	{}
};



struct	pn_spec_symbol_const : nodebase
{
	static constexpr types	type	= types::spec_symbol_const;

	node			name;

	pn_spec_symbol_const(node l)
		: name(std::move(l))
	{}
};


struct	pn_symbol_decltye : nodebase
{
	static constexpr types	type	= types::symbol_decltye;

	node	value;

	pn_symbol_decltye(node _value)
		: value(std::move(_value))
	{}
};


struct	pn_spec_member_friend : nodebase
{
	static constexpr types	type	= types::spec_member_friend;

	node			decl;

	pn_spec_member_friend(node _decl)
		: decl(std::move(_decl))
	{}
};


struct	pn_symbol_functype : nodebase
{
	static constexpr types	type	= types::symbol_functype;

	node	rettype;
	node	parameters;

	pn_symbol_functype(node r, node p)
		: rettype(std::move(r))
		, parameters(std::move(p))
	{}
};

struct	pn_spec_symbol_move : nodebase
{
	static constexpr types	type	= types::spec_symbol_move;

	node			name;

	pn_spec_symbol_move(node l)
		: name(std::move(l))
	{}
};

struct	pn_spec_symbol_parampack : nodebase
{
	static constexpr types	type	= types::spec_symbol_parampack;

	node			name;

	pn_spec_symbol_parampack(node l)
		: name(std::move(l))
	{}
};


struct	pn_sepc_symbol_pointer : nodebase
{
	static constexpr types	type	= types::sepc_symbol_pointer;

	node			name;

	pn_sepc_symbol_pointer(node _type_id)
		: name(std::move(_type_id))
	{}
};

struct	pn_spec_symbol_reference : nodebase
{
	static constexpr types	type	= types::spec_symbol_reference;

	node			name;

	pn_spec_symbol_reference(node l)
		: name(std::move(l))
	{}
};


struct	pn_symbol_name : nodebase
{
	static constexpr types	type	= types::symbol_name;

	text	name;

	pn_symbol_name(text l)
		: name(std::move(l))
	{}
};


struct	pn_symbol_nameabs : nodebase
{
	static constexpr types	type	= types::symbol_nameabs;

	node	scope;

	pn_symbol_nameabs(node _s)
		: scope(std::move(_s))
	{}
};



struct	pn_symbol_operator : nodebase
{
	static constexpr types	type	= types::symbol_operator;

	operator_type	ope;

	pn_symbol_operator(operator_type _o)
		: ope(std::move(_o))
	{}
};


struct	pn_symbol_scope : nodebase
{
	static constexpr types	type	= types::symbol_scope;

	simplearray<node>	scopes;

	pn_symbol_scope(simplearray<node> _scopes)
		: scopes(std::move(_scopes))
	{}
};



struct	pn_symbol_template : pn_symbol_name
{
	static constexpr types	type	= types::symbol_template;

	simplearray<node>	templateParams;

	pn_symbol_template(text s, simplearray<node> _params)
		: pn_symbol_name(std::move(s))
		, templateParams(std::move(_params))
	{}
};


struct	pn_stmt_branch_break : nodebase
{
	static constexpr types	type	= types::stmt_branch_break;
	pn_stmt_branch_break()
	{}
};


struct	pn_stmt_branch_continue : nodebase
{
	static constexpr types	type	= types::stmt_branch_continue;
	pn_stmt_branch_continue()
	{}
};

struct	pn_stmt_compound : nodebase
{
	static constexpr types	type	= types::stmt_compound;

	simplearray<node>	statements;

	pn_stmt_compound(simplearray<node> _value)
		: statements(std::move(_value))
	{}
};


struct	pn_stmt_do_while : nodebase
{
	static constexpr types	type	= types::stmt_do_while;

	node	conditions;
	node	statements;

	pn_stmt_do_while(node _cond, node _stmt)
		: conditions(std::move(_cond))
		, statements(std::move(_stmt))
	{}
};


struct	pn_stmt_for : nodebase
{
	static constexpr types	type	= types::stmt_for;

	node	init;
	node	conditions;
	node	increments;
	node	statements;

	pn_stmt_for(node _init, node _cond, node _inc, node _stmt)
		: init(std::move(_init))
		, conditions(std::move(_cond))
		, increments(std::move(_inc))
		, statements(std::move(_stmt))
	{}
};


struct	pn_stmt_for_rangedbase : nodebase
{
	static constexpr types	type	= types::stmt_for_rangedbase;

	node	init;
	node	value;
	node	statements;

	pn_stmt_for_rangedbase(node _init, node _value, node _stmt)
		: init(std::move(_init))
		, value(std::move(_value))
		, statements(std::move(_stmt))
	{}
};


struct	pn_stmt_if : nodebase
{
	static constexpr types	type	= types::stmt_if;

	node	init;
	node	conditions;
	node	statements1;
	node	statements2;

	pn_stmt_if(node _init, node _cond, node _stmt1, node _stmt2)
		: init(std::move(_init))
		, conditions(std::move(_cond))
		, statements1(std::move(_stmt1))
		, statements2(std::move(_stmt2))
	{}
};


struct	pn_stmt_return : nodebase
{
	static constexpr types	type	= types::stmt_return;

	node	value;

	pn_stmt_return(node _value)
		: value(std::move(_value))
	{}
};


struct	pn_stmt_switch : nodebase
{
	static constexpr types	type	= types::stmt_switch;

	struct	casedata
	{
		node	value;
		node	statements;
	};

	node					conditions;
	simplearray<casedata>	cases;

	pn_stmt_switch(node _cond, simplearray<casedata> _cases)
		: conditions(std::move(_cond))
		, cases(std::move(_cases))
	{}
};



struct	pn_stmt_while : nodebase
{
	static constexpr types	type	= types::stmt_while;

	node	conditions;
	node	statements;

	pn_stmt_while(node _cond, node _stmt)
		: conditions(std::move(_cond))
		, statements(std::move(_stmt))
	{}
};



struct	pn_using_alias : nodebase
{
	static constexpr types	type	= types::using_alias;

	node	name;
	node	target;

	pn_using_alias(node l, node r)
		: name(std::move(l))
		, target(std::move(r))
	{}
};


struct	pn_using_namespace : nodebase
{
	static constexpr types	type	= types::using_namespace;

	node	name;

	pn_using_namespace(node l)
		: name(std::move(l))
	{}
};


struct	pn_echo : nodebase
{
	static constexpr types	type	= types::echo;

	text	doc;

	pn_echo(text _doc)
		: doc(std::move(_doc))
	{}
};


struct	pn_label : nodebase
{
	static constexpr types	type	= types::label;

	text			name;

	pn_label(text _label)
		: name(std::move(_label))
	{}
};


struct	pn_list : nodebase
{
	static constexpr types	type	= types::list;

	simplearray<node>	values;

	pn_list(simplearray<node> _values)
		: values(std::move(_values))
	{}
};




struct	pn_initializer_list : nodebase
{
	static constexpr types	type	= types::initializer_list;

	struct	initinfo
	{
		text	name;
		node	value;
	};

	simplearray<initinfo>		values;

	pn_initializer_list(simplearray<initinfo> _values)
		: values(std::move(_values))
	{}

};

struct	pn_struct_initializer : nodebase
{
	static constexpr types	type	= types::struct_initializer;

	node	name;
	node	initlist;

	pn_struct_initializer(node _sym, node _inits)
		: name(std::move(_sym))
		, initlist(std::move(_inits))
	{}
};




PSNVM_LANG_NAMESPACE_END
#endif
