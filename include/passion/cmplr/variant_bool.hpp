#ifndef	__passion__compiler__variant_bool__hpp
#define	__passion__compiler__variant_bool__hpp
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! true, false
// ----------------------------------------------------------------------------
template<class Compiler>
struct	variant_bool
{
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		end,
		pass,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token == "true" || _token == "false";
	}


	compute_type get_compute_type() const
	{
		return	compute_type::dynamics;
	}

	vmvariant variable() const
	{
		return	{};
	}

	vmtypename type() const
	{
		return	"bool";
	}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		_asmblr.add_load_int(mValue ? 1 : 0);
	}

	template<class Expr, class C, class T>
	lex_result<variant_bool> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				mValue	= _token == "true" ;
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	bool		mValue;
};



PASSION_NAMESPACE_END
#endif
