#ifndef	__passion__compiler__variant_nullptr__hpp
#define	__passion__compiler__variant_nullptr__hpp
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! nullptr型
// ----------------------------------------------------------------------------
template<class Compiler>
struct	variant_nullptr
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
		return _token == "nullptr";
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
		return	"nullptr_t";
	}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		_asmblr.add_load_int(0);
	}

	template<class Expr, class C, class T>
	lex_result<variant_nullptr> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}
};



PASSION_NAMESPACE_END
#endif
