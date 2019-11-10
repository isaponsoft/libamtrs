#ifndef	__passion__compiler__operator_logical_not__hpp
#define	__passion__compiler__operator_logical_not__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	@brief !expression
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler>
struct	operator_logical_not
{
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		keyword,
		value,
		end,
		pass
	};

	static constexpr std::string_view	start_token	= std::string_view("!");


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
		return	"bool";
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		mRValue->compile(_cmplr, _asmblr);
		if (passion_is_gcpointer(mRValue->type()))
		{
			_asmblr.add_ref2int();
		}
		_asmblr.add_not();
	}


	template<class Expr, class C, class T>
	lex_result<operator_logical_not> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::keyword :
			{
				return	phase_type::value;
			}

			case phase_type::value :
			{
				mRValue	= _expr.template create<variant<Compiler>>(value_mode::rvalue);
				return	{ phase_type::end, mRValue->lex(_cmplr, _token) };
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<expression_type>		mRValue;
};



PASSION_NAMESPACE_END
#endif
