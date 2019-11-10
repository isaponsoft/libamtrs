#ifndef	__passion__compiler__operatorto_address__hpp
#define	__passion__compiler__operatorto_address__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	@brief !expression
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler>
struct	operator_to_address
{
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		keyword,
		value,
		value_end,
		end,
		pass
	};

	static constexpr std::string_view	start_token	= std::string_view(PASSION_TYPENAME_REFERENCE_SUFFIX);


	compute_type get_compute_type() const noexcept
	{
		return	mRValue->get_compute_type();
	}

	vmvariant variable() const noexcept
	{
		return	{};
	}

	vmtypename type() const noexcept
	{
		return	mTypename;
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		mRValue->compile(_cmplr, _asmblr);
	}


	template<class Expr, class C, class T>
	lex_result<operator_to_address> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::keyword :
			{
				_expr.set_priority(opecode_priority::single_operation);
				return	phase_type::value;
			}

			case phase_type::value :
			{
				mRValue		= _expr.template create<variant<Compiler>>(value_mode::rvalue);
				mRValue->set_priority(opecode_priority::single_operation);
				return	{ phase_type::value_end, mRValue->lex(_cmplr, _token) };
			}

			case phase_type::value_end :
			{
				mTypename	= vmtypename(passion_trim_reference(mRValue->type())) + PASSION_TYPENAME_POINTER_SUFFIX;
				return	phase_type::pass;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<expression_type>		mRValue;
	vmtypename					mTypename;
};



PASSION_NAMESPACE_END
#endif
