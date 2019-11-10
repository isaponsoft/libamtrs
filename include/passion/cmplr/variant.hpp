#ifndef	__passion__compiler__variant__hpp
#define	__passion__compiler__variant__hpp
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! プリミティブ、変数の基本クラス
// ----------------------------------------------------------------------------
//! Base of primitive value and variable.
// ----------------------------------------------------------------------------
template<class Compiler>
struct	variant
{
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		value,
		opecode,
		member,
		arrow,
		end,
		pass,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token == Token::COMMAND || _token == "(";
	}


	compute_type get_compute_type() const
	{
		return	mValue->get_compute_type();
	}

	vmvariant variable() const
	{
		return	mValue->get_static_variable();
	}

	vmtypename type() const
	{
		return	mValue->type();
	}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (mValue.empty())
		{
			THROW_COMPILER_ERROR();
		}
		mValue->compile(_cmplr, _asmblr);
	}

	template<class Expr, class C, class T>
	lex_result<variant> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::value :
			{
				                      mValue = _expr.template create_if<operator_arithmetic_parenthesis<Compiler>>(value_mode::rvalue, _cmplr, _token);
				if (mValue.empty())	{ mValue = _expr.template create_if<variant_bool<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<variant_this<Compiler>>(_cmplr, _token, vmtypename(_cmplr.clazz)); }
				if (mValue.empty()) { mValue = _expr.template create_if<expression_sizeof<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<primitive_integer<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<primitive_string<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<variable_argument<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<variable_local<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<expression_new<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<operator_logical_not<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<operator_pointer<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<operator_to_address<Compiler>>(_cmplr, _token); }
				if (mValue.empty()) { mValue = _expr.template create_if<invoke_member<Compiler>>(_cmplr, _token, _expr.template create<variant_this<Compiler>>(value_mode::rvalue, vmtypename(_cmplr.clazz)), this); }
				if (mValue.empty())	{ mValue = _expr.template create_if<variable_member<Compiler>>(_cmplr, _token, nullptr, this); }
				//if (mValue.empty())	{ mValue = _expr.template create_if<variable_member<Compiler>>(_cmplr, _token, _expr.template create<variant_this<Compiler>>(value_mode::rvalue, vmtypename(_cmplr.clazz)), this); }
				if (mValue.empty())	{ mValue = _expr.template create_if<incremental_operator<Compiler>>(_cmplr, _token); }
				if (mValue.empty())	{ mValue = _expr.template create_if<variant_nullptr<Compiler>>(_cmplr, _token); }
				if (mValue.empty())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, _token.str().c_str());
				}
				return	{ phase_type::opecode, mValue->lex(_cmplr, _token) };
			}

			case phase_type::opecode :
			{
				if (_token == ".")
				{
					return	phase_type::member;
				}

				if (_token == "->")
				{
					return	phase_type::arrow;
				}

				#define	OPERATOR_PROCESS(_nimoc)	\
					if (_token == compute_##_nimoc##_operation<Compiler>::operation)\
					{\
						if (_expr.parent() && static_cast<expression_node<Compiler>*>(_expr.parent())->get_priority() >= compute_##_nimoc##_operation<Compiler>::priority)\
						{\
							return	phase_type::pass;\
						}\
						mValue	= _expr.template create<compute_##_nimoc##_operation<Compiler>>(mValue);\
						_expr.set_priority(mValue->get_priority());\
						return	{ phase_type::opecode, mValue->lex(_cmplr, _token) };\
					}\

				OPERATOR_PROCESS(add   )
				OPERATOR_PROCESS(sub   )
				OPERATOR_PROCESS(mul   )
				OPERATOR_PROCESS(div   )
				OPERATOR_PROCESS(mod   )
				OPERATOR_PROCESS(cmps  )
				OPERATOR_PROCESS(cmpse )
				OPERATOR_PROCESS(cmpl  )
				OPERATOR_PROCESS(cmple )
				OPERATOR_PROCESS(cmpe  )
				OPERATOR_PROCESS(cmpn  )
				OPERATOR_PROCESS(land  )
				OPERATOR_PROCESS(lor   )
				OPERATOR_PROCESS(and   )
				OPERATOR_PROCESS(or    )
				OPERATOR_PROCESS(xor   )
				OPERATOR_PROCESS(shiftl)
				OPERATOR_PROCESS(shiftr)

				#undef	OPERATOR_PROCESS

				if (_token == "++" || _token == "--")
				{
					mValue	= _expr.template create<incremental_after_operator<Compiler>>(mValue);
					return	{ phase_type::opecode, mValue->lex(_cmplr, _token) };
				}


				if (_token == "?")
				{
					if (_expr.parent() && static_cast<expression_node<Compiler>*>(_expr.parent())->get_priority() >= expr<operator_arithmetic_conditional<Compiler>>::expression::priority)
					{
						return	phase_type::pass;
					}
					mValue	= _expr.template create<operator_arithmetic_conditional<Compiler>>(mValue);
					_expr.set_priority(expr<operator_arithmetic_conditional<Compiler>>::expression::priority);
					return	{ phase_type::opecode, mValue->lex(_cmplr, _token) };
				}


				if (_token == "[")
				{
					auto	next	= _expr.template create<variable_array<Compiler>>(mValue);
					if (!next)
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, _token.str().c_str());
					}
					mValue	= next;
					return	{ phase_type::opecode, mValue->lex(_cmplr, _token) };
				}


				if (_token == "(")
				{
					//vmtypename	ft	= vmtypename(passion_trim_reference(mValue->type()));
					mValue	= _expr.template create<function_call<Compiler>>(mValue);
					_expr.set_priority(expr<operator_arithmetic_conditional<Compiler>>::expression::priority);
					return	{ phase_type::opecode, mValue->lex(_cmplr, _token) };
				}

				return	phase_type::pass;
			}

			case phase_type::member :
			{
				if (passion_is_pointer(mValue->type()))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), member_is_pointer, mValue->type().c_str());
				}

				ref<expression_type>	next;
				if (next.empty()) { next = _expr.template create_if<invoke_member<Compiler>>(_cmplr, _token, mValue, passion_is_gcpointer(passion_remove_reference(mValue->type()))); }
				if (next.empty()) { next = _expr.template create_if<variable_member<Compiler>>(_cmplr, _token, mValue, passion_is_gcpointer(passion_remove_reference(mValue->type()))); }
				if (next.empty())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), member_not_found, _token.str().c_str(), mValue->type().c_str());
				}
				mValue->set_value_type(value_mode::rvalue);
				mValue	= next;
				return	{ phase_type::opecode, mValue->lex(_cmplr, _token) };
			}

			case phase_type::arrow :
			{
				if (mValue->type().remove_pointer().empty())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), member_is_not_pointer, mValue->type().c_str());
				}


				ref<expression_type>	next;
				if (next.empty()) { next = _expr.template create_if<invoke_member<Compiler>>(_cmplr, _token, mValue, true); }
				if (next.empty()) { next = _expr.template create_if<variable_member<Compiler>>(_cmplr, _token, mValue, true); }
				if (next.empty())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), member_not_found, _token.str().c_str(), mValue->type().c_str());
				}
				mValue->set_value_type(value_mode::rvalue);
				mValue	= next;
				return	{ phase_type::opecode, mValue->lex(_cmplr, _token) };
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}




private:
	ref<expression_type>		mValue;
};



PASSION_NAMESPACE_END
#endif
