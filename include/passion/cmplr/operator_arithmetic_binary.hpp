#ifndef	__passion__compiler__operator_binary__hpp
#define	__passion__compiler__operator_binary__hpp
PASSION_NAMESPACE_BEGIN

// ********************************************************************
//! ２項演算子の特性
// --------------------------------------------------------------------
// --------------------------------------------------------------------
template<class Compiler, opecode_id Operator>
struct	binoperator_trits;

inline vmstring binoperator_trits__get_vmstring(const vmvariant& _var)
{
	return	vmstring(std::get<ref<vmobj>>(_var)->base()->native<vmstrobj>()->c_str());
}


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::ADD>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::add_sub;
	static constexpr std::string_view	operation	= std::string_view("+");
	static constexpr std::string_view	nimoc		= std::string_view("add");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a + b);
		}
		if (_a->get_compute_type() == compute_type::static_string && _b->get_compute_type() == compute_type::static_string)
		{
			auto		a	= removeQuotation(binoperator_trits__get_vmstring(_a->get_static_variable()));
			auto		b	= removeQuotation(binoperator_trits__get_vmstring(_b->get_static_variable()));
			return	expr<primitive_string<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), "\"" + a + b + "\"");
		}
		if (_a->get_compute_type() == compute_type::static_string && _b->get_compute_type() == compute_type::static_integer)
		{
			auto		a	= removeQuotation(binoperator_trits__get_vmstring(_a->get_static_variable()));
			auto		b	= std::to_string(std::get<vmvalue>(_b->get_static_variable()).i);
			return	expr<primitive_string<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), "\"" + a + b + "\"");
		}
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_string)
		{
			auto		a	= std::to_string(std::get<vmvalue>(_a->get_static_variable()).i);
			auto		b	= removeQuotation(binoperator_trits__get_vmstring(_b->get_static_variable()));
			return	expr<primitive_string<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), "\"" + a + b + "\"");
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_add();
	}

private:
	template<class E>
	bool is_integer_type(E* _expr) const noexcept
	{
		return	_expr->type() == "int";
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::SUB>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::add_sub;
	static constexpr std::string_view	operation	= std::string_view("-");
	static constexpr std::string_view	nimoc		= std::string_view("sub");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a - b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_sub();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::MUL>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::mul_div;
	static constexpr std::string_view	operation	= std::string_view("*");
	static constexpr std::string_view	nimoc		= std::string_view("mul");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a * b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_mul();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::DIV>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::mul_div;
	static constexpr std::string_view	operation	= std::string_view("/");
	static constexpr std::string_view	nimoc		= std::string_view("div");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a / b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_div();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::MOD>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::mul_div;
	static constexpr std::string_view	operation	= std::string_view("%");
	static constexpr std::string_view	nimoc		= std::string_view("mod");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a % b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_mod();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::CMPS>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::compare;
	static constexpr std::string_view	operation	= std::string_view("<");
	static constexpr std::string_view	nimoc		= std::string_view("cmps");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a < b ? 1 : 0);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_cmps();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::CMPSE>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::compare;
	static constexpr std::string_view	operation	= std::string_view("<=");
	static constexpr std::string_view	nimoc		= std::string_view("cmpse");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a <= b ? 1 : 0);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_cmpse();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::CMPL>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::compare;
	static constexpr std::string_view	operation	= std::string_view(">");
	static constexpr std::string_view	nimoc		= std::string_view("cmpl");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a > b ? 1 : 0);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_cmpl();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::CMPLE>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::compare;
	static constexpr std::string_view	operation	= std::string_view(">=");
	static constexpr std::string_view	nimoc		= std::string_view("cmple");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a >= b ? 1 : 0);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_cmple();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::CMPE>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::compare;
	static constexpr std::string_view	operation	= std::string_view("==");
	static constexpr std::string_view	nimoc		= std::string_view("cmpe");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a == b ? 1 : 0);
		}
#if	0
		if (_a->get_compute_type() == compute_type::static_string && _b->get_compute_type() == compute_type::static_string)
		{
			auto		a	= removeQuotation(binoperator_trits__get_vmstring(_a->get_static_variable()));
			auto		b	= removeQuotation(binoperator_trits__get_vmstring(_b->get_static_variable()));
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a == b ? 1 : 0);
		}
#endif
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_cmpe();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::CMPN>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::compare;
	static constexpr std::string_view	operation	= std::string_view("!=");
	static constexpr std::string_view	nimoc		= std::string_view("cmpn");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a != b ? 1 : 0);
		}
#if	0
		if (_a->get_compute_type() == compute_type::static_string && _b->get_compute_type() == compute_type::static_string)
		{
			auto		a	= removeQuotation(binoperator_trits__get_vmstring(_a->get_static_variable()));
			auto		b	= removeQuotation(binoperator_trits__get_vmstring(_b->get_static_variable()));
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a != b ? 1 : 0);
		}
#endif
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_cmpn();
	}
};

template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::LAND>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::logic_and;
	static constexpr std::string_view	operation	= std::string_view("&&");
	static constexpr std::string_view	nimoc		= std::string_view("land");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a && b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_land();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::LOR>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::logic_and;
	static constexpr std::string_view	operation	= std::string_view("||");
	static constexpr std::string_view	nimoc		= std::string_view("lor");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a || b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_lor();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::XOR>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::bitwise;
	static constexpr std::string_view	operation	= std::string_view("^");
	static constexpr std::string_view	nimoc		= std::string_view("xor");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a ^ b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_xor();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::AND>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::bitwise;
	static constexpr std::string_view	operation	= std::string_view("&");
	static constexpr std::string_view	nimoc		= std::string_view("and");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a & b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_and();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::OR>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::bitwise;
	static constexpr std::string_view	operation	= std::string_view("|");
	static constexpr std::string_view	nimoc		= std::string_view("or");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a | b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_or();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::SHIFTL>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::shift;
	static constexpr std::string_view	operation	= std::string_view("<<");
	static constexpr std::string_view	nimoc		= std::string_view("shiftl");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a << b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_shiftl();
	}
};


template<class Compiler>
struct	binoperator_trits<Compiler, opecode_id::SHIFTR>
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<compiler_type>;
	static constexpr opecode_priority	priority	= opecode_priority::shift;
	static constexpr std::string_view	operation	= std::string_view(">>");
	static constexpr std::string_view	nimoc		= std::string_view("shiftr");

	ref<expression_type> compute(expression_type* _parent, compiler_type& _state, expression_type* _a, expression_type* _b)
	{
		if (_a->get_compute_type() == compute_type::static_integer && _b->get_compute_type() == compute_type::static_integer)
		{
			auto	a	= std::get<vmvalue>(_a->get_static_variable()).i;
			auto	b	= std::get<vmvalue>(_b->get_static_variable()).i;
			return	expr<primitive_integer<Compiler>>::create(_parent, value_mode::rvalue, _a->location(), a >> b);
		}
		return	{};
	}

	void compile(compiler_type& _cmplr, assembler& _asmblr, expression_type* _a, expression_type* _b)
	{
		_asmblr.add_shiftr();
	}
};




// ********************************************************************
//!	@brief ２項演算子
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler, class Trits>
struct	basic_compute_binary_operation : public Trits
{
	using	this_type		= basic_compute_binary_operation<Compiler, Trits>;
	using	trits_type		= Trits;

	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<Compiler>;


	enum class phase_type
	{
		start,
		opecode,
		right_value,
		compute,
		end,
		pass
	};



	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token, expression_type* _left_value)
	{
		return _token == trits_type::operation;
	}


	basic_compute_binary_operation(expression_type* _left_value)
		: mLeft(_left_value)
		, mTypename(passion_remove_reference(_left_value->type()))
	{
	}

	compute_type get_compute_type() const noexcept
	{
		return	(mAnswer.empty() || mOperatorFunc.first) ? compute_type::dynamics : mAnswer->get_compute_type();
	}

	vmtypename type() const noexcept
	{
		return	mTypename;
	}

	vmvariant variable() const noexcept
	{
		return	mAnswer.empty() ? vmvariant{} : mAnswer->get_static_variable();
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (mAnswer)
		{
			mAnswer->compile(_cmplr, _asmblr);
		}
		else if (mOperatorFunc.first)
		{
			_asmblr.add_alloc(1);
			mLeft->compile(_cmplr, _asmblr);
			if (passion_is_reference(mLeft->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mLeft->type())));
			}
			mRight->compile(_cmplr, _asmblr);
			if (passion_is_reference(mRight->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mRight->type())));
			}

			_asmblr.add_call(mOperatorFunc.second, 2);
			// 引数の削除
			if (passion_is_gcpointerable(passion_remove_reference(mLeft->type())))
			{
				_asmblr.add_lea_stack(-2);
				_asmblr.add_delete_ref();
			}
			if (passion_is_gcpointerable(passion_remove_reference(mRight->type())))
			{
				_asmblr.add_lea_stack(-1);
				_asmblr.add_delete_ref();
			}
			_asmblr.add_free(2);


			if (_expr != value_mode::rvalue)
			{
				_asmblr.add_free(1);
			}
		}
		else
		{
			mLeft->compile(_cmplr, _asmblr);
			if (passion_is_reference(mLeft->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mLeft->type())));
			}
			mRight->compile(_cmplr, _asmblr);
			if (passion_is_reference(mRight->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mRight->type())));
			}
			trits_type::compile(_cmplr, _asmblr, mLeft, mRight);
		}
	}

	template<class Expr, class C, class T>
	lex_result<basic_compute_binary_operation> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
				mLeft->change_parent(&_expr);
				_expr.set_priority(trits_type::priority);
			case phase_type::opecode :
			{
				return	phase_type::right_value;
			}

			case phase_type::right_value :
			{
				mRight		= expr<variant<Compiler>>::create(&_expr, value_mode::rvalue, {});
				return	{ phase_type::compute, mRight->lex(_cmplr, _token) };
			}

			case phase_type::compute :
			{
				mAnswer	= trits_type::compute(&_expr, _cmplr, mLeft, mRight);
				if (!mAnswer.empty())
				{
					mLeft	= nullptr;
					mRight	= nullptr;
				}
				else if ((passion_remove_reference(mLeft->type()) != "int") || (passion_remove_reference(mRight->type()) != "int"))
				{
					// 演算できないので operator 関数を検索する
					vmstring	func	= vmstring("operator") + vmstring(trits_type::operation);
					vmtypename	left	= passion_remove_reference(mLeft->type());
					vmtypename	right	= passion_remove_reference(mRight->type());
					mOperatorFunc		= _cmplr.find_function({_cmplr.find_class(left)}, func, &right, 1);
					if (!mOperatorFunc.second)
					{
						vmtypename	args[2] = {left, right};
						mOperatorFunc	= _cmplr.find_function({_cmplr.find_class("")}, func, args, 2);
						if (!mOperatorFunc.second)
						{
							PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), not_found_operator, left.c_str(), trits_type::operation.data(), right.c_str());
						}
						mGlobalFunc	= true;
					}
					mTypename		= mOperatorFunc.second->result_type();
				}
				return	phase_type::pass;
			}

			default: return phase_type::pass;
		}
	}

	vmtypename							mTypename;
	ref<expression_type>				mAnswer;		//!< 事前に計算可能な場合は計算結果を保持する
	ref<expression_type>				mLeft;
	ref<expression_type>				mRight;
	std::pair<vmclass*, vmfunction*>	mOperatorFunc;
	bool								mGlobalFunc	= false;
};

template<class Compiler>	using	compute_add_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::ADD>>;
template<class Compiler>	using	compute_sub_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::SUB>>;
template<class Compiler>	using	compute_mul_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::MUL>>;
template<class Compiler>	using	compute_div_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::DIV>>;
template<class Compiler>	using	compute_mod_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::MOD>>;
template<class Compiler>	using	compute_cmps_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::CMPS>>;
template<class Compiler>	using	compute_cmpse_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::CMPSE>>;
template<class Compiler>	using	compute_cmpl_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::CMPL>>;
template<class Compiler>	using	compute_cmple_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::CMPLE>>;
template<class Compiler>	using	compute_cmpe_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::CMPE>>;
template<class Compiler>	using	compute_cmpn_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::CMPN>>;
template<class Compiler>	using	compute_land_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::LAND>>;
template<class Compiler>	using	compute_lor_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::LOR>>;
template<class Compiler>	using	compute_and_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::AND>>;
template<class Compiler>	using	compute_or_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::OR>>;
template<class Compiler>	using	compute_xor_operation				= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::XOR>>;
template<class Compiler>	using	compute_shiftl_operation			= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::SHIFTL>>;
template<class Compiler>	using	compute_shiftr_operation			= basic_compute_binary_operation<Compiler, binoperator_trits<Compiler, opecode_id::SHIFTR>>;


#undef	DECLERE_OPERATOR


PASSION_NAMESPACE_END
#endif
