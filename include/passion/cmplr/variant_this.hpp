#ifndef	__passion__compiler__variant_this__hpp
#define	__passion__compiler__variant_this__hpp
#include "variant.hpp"
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	@brief this
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler>
struct	variant_this
{
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;

	enum class	phase_type
	{
		start,
		end,
		pass,
	};

	static constexpr std::string_view	start_token	= std::string_view("this");

	variant_this(vmtypename _scope)
		: mType(_scope + "^")
	{
	}

	vmtypename type() const noexcept
	{
		return	mType;
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		_asmblr.add_load_this();
	}

	template<class Expr, class C, class T>
	lex_result<variant_this> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		return	phase_type::end;
	}


private:
	vmtypename	mType;
};


PASSION_NAMESPACE_END
#endif
