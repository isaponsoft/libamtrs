#ifndef	__passion__compiler__expression__hpp
#define	__passion__compiler__expression__hpp
#include "node.hpp"
PASSION_NAMESPACE_BEGIN

// ********************************************************************
//!	@brief 式ノード
// ********************************************************************
template<class Compiler>
class	expression_node		// expression_nodeを削除し、exprに統合予定
		: public node<Compiler>
{
	using	_base_type		= node<Compiler>;
public:
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;
	using	node_type		= typename _base_type::node_type;
	using	input_type		= typename _base_type::input_type;
	using	location_type	= source_location::location_type;

	expression_node(node_type* _parent, const location_type& _location, value_mode _mode)
		: _base_type(_parent, _location)
		, m_return_mode(_mode)
	{}

	virtual compute_type get_compute_type() const noexcept = 0;

	virtual vmvariant get_static_variable() const noexcept = 0;




	value_mode get_value_type() const noexcept { return m_return_mode; }

	void set_value_type(value_mode _type)
	{
		m_return_mode = _type;
	}

	value_mode			m_return_mode;
};




// ********************************************************************
//!	汎用的な式ノードのテンプレート
// --------------------------------------------------------------------
template<class Expression>
class	expr
		: public expression_node<typename Expression::compiler_type>		// expression_nodeを削除し、こちらに統合予定
{
	using	_base_type	= expression_node<typename Expression::compiler_type>;
public:
	using	node_type		= typename _base_type::node_type;
	using	input_type		= typename _base_type::input_type;

	using	compiler_type	= typename Expression::compiler_type;
	using	expression_type	= expression_node<typename Expression::compiler_type>;
	using	this_type		= expr<Expression>;

	using	expression		= Expression;
	using	phase_type		= typename expression::phase_type;

	using	location_type	= typename _base_type::location_type;


	template<class... Args>
	expr(node_type* _parent, value_mode _vtype, location_type _location, Args... _args)
		: _base_type(_parent, _location, _vtype)
		, mExpression(std::forward<Args>(_args)...)
	{
		_base_type::mLocation	= _location;
	}


	bool operator == (value_mode _mode) const noexcept { return this->get_value_type() == _mode; }
	bool operator != (value_mode _mode) const noexcept { return !(*this == _mode); }

	template<class... Opts>
	static ref<this_type> create(node_type* _parent, value_mode _vtype, location_type _location, Opts... _opts)
	{
		return	new this_type(_parent, _vtype, _location, std::forward<Opts>(_opts)...);
	}

	static ref<this_type> create_if(node_type* _parent, value_mode _vtype, compiler_type& _cmplr, const Token& _token)
	{
		bool	match;
		if constexpr (has_function<expression, has_is_start<>>::value)
		{
			match	= expression::is_start(_cmplr, _token);
		}
		else
		{
			match	= _token == expression::start_token;
		}
		return	match ? create(_parent, _vtype, _token) : ref<this_type>();
	}


	template<class Expr>
	ref<expr<Expr>> create()
	{
		return	create<Expr>(this->get_value_type());
	}


	template<class Expr>
	ref<expr<Expr>> create(value_mode _vtype)
	{
		using	target_type	= expr<Expr>;
		return	new target_type(this, _vtype, _base_type::mLocation);
	}


	template<class Expr, class... Opts>
	ref<expr<Expr>> create(Opts... _opts)
	{
		return	create<Expr>(this->get_value_type(), std::forward<Opts>(_opts)...);
	}

	template<class Expr, class... Opts>
	ref<expr<Expr>> create(value_mode _vtype, Opts... _opts)
	{
		using	target_type	= expr<Expr>;
		return	new target_type(this, _vtype, _base_type::mLocation, std::forward<Opts>(_opts)...);
	}



	template<class Expr, class... Opts>
	ref<expr<Expr>> create_if(value_mode _vtype, compiler_type& _cmplr, const Token& _token, Opts... _opts)
	{
		using	target_type	= expr<Expr>;
		bool	match;
		if constexpr (has_function<Expr, has_is_start<Opts...>>::value)
		{
			match	= Expr::is_start(_cmplr, _token, std::forward<Opts>(_opts)...);
		}
		else
		{
			match	= (_token == Expr::start_token);
		}
		return	match ? ref<target_type>(new target_type(this, _vtype, _token, std::forward<Opts>(_opts)...)) : ref<target_type>();
	}

	template<class Expr, class... Opts>
	ref<expr<Expr>> create_if(compiler_type& _cmplr, const Token& _token, Opts... _opts)
	{
		return	create_if<Expr>(this->get_value_type(), _cmplr, _token, std::forward<Opts>(_opts)...);
	}



	virtual compute_type get_compute_type() const noexcept override
	{
		if constexpr (has_function<const expression, has_get_compute_type>::value)
		{
			return	mExpression.get_compute_type();
		}
		else
		{
			return	compute_type::dynamics;
		}
	}

	virtual vmvariant get_static_variable() const noexcept override
	{
		if constexpr (has_function<const expression, has_variable>::value)
		{
			return	mExpression.variable();
		}
		else
		{
			return	{};
		}
	}

	virtual vmtypename type() const noexcept override
	{
		if constexpr (has_function<const expression, has_type>::value)
		{
			return	mExpression.type();
		}
		else
		{
			return	{};
		}
	}


	virtual void compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		if constexpr (has_function<expression, has_compile>::value)
		{
			return	mExpression.compile(static_cast<this_type&>(*this), _cmplr, _asmblr);
		}
	}


	virtual ref<node_type> lex(compiler_type& _cmplr, input_type _token) override
	{
		PSNCMPLR_TRACE_LOG("%s  :  %s", _token.str().c_str(), typeid(Expression).name());
		if (_token.line() > 0)
		{
			_base_type::mLocation	= _token;
		}
		auto	next	= mExpression.lex(static_cast<this_type&>(*this), mPhase, _cmplr, _token);
		mPhase	= (phase_type)next.next_phase;
		if (next.next_node)
		{
			return	next.next_node;
		}
		if (mPhase == phase_type::end)
		{
			return	this->finish();
		}
		if (mPhase == phase_type::pass)
		{
			return	this->finish(_cmplr, _token);
		}
		return	this;
	}


private:
	phase_type		mPhase	= phase_type::start;
	expression		mExpression;

	struct	has_get_compute_type
	{
		template<class T>
		auto operator () (T* e)
			-> decltype(e->get_compute_type(), std::true_type{});
	};

	struct	has_variable
	{
		template<class T>
		auto operator () (T* e)
			-> decltype(e->variable(), std::true_type{});
	};

	struct	has_type
	{
		template<class T>
		auto operator () (T* e)
			-> decltype(e->type(), std::true_type{});
	};

	struct	has_compile
	{
		template<class T>
		auto operator () (T* e)
			-> decltype(e->compile(std::declval<this_type&>(), std::declval<compiler_type&>(), std::declval<assembler&>()), std::true_type{});
	};


	template<class... Opts>
	struct	has_is_start
	{
		template<class T>
		auto operator () (T* e)
			-> decltype(e->is_start(std::declval<compiler_type&>(), std::declval<const Token&>(), std::declval<Opts>()...), std::true_type{});
	};

	struct	has_priority
	{
		template<class T>
		auto operator () (T* e)
			-> decltype(e->priority, std::true_type{});
	};


	template<class>
	friend	struct	lex_result;
};


PASSION_NAMESPACE_END
#endif
