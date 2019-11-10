#ifndef	__passion__compiler__primitive_string__hpp
#define	__passion__compiler__primitive_string__hpp
#include "variant.hpp"
PASSION_NAMESPACE_BEGIN




// ********************************************************************
//!	@brief 文字列値
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler>
struct	primitive_string
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<Compiler>;

	enum class	phase_type
	{
		start,
		strvalue,
		end,
		pass
	};

	primitive_string(){}
	primitive_string(const vmstring& _n) : mValue(_n) {}

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		if (_token != Token::STRING)
		{
			return	false;
		}
		return	true;
	}

	compute_type get_compute_type() const noexcept
	{
//		return	compute_type::static_string;
		return	compute_type::dynamics;
	}

	vmvariant variable() const noexcept
	{
//		return	vmvariant(vmstring(mValue));
		return	{};
	}

	vmtypename type() const noexcept
	{
		return	"string";
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		_asmblr.add_load_string(mValue);
	}

	template<class Expr, class C, class T>
	lex_result<primitive_string> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::strvalue :
			{
				if (_token != Token::STRING)
				{
					THROW_ERROR(_dummy_, _token);
				}
				mValue		= _token.str();
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmstring		mValue;
};



PASSION_NAMESPACE_END
#endif
