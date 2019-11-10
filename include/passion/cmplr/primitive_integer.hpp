#ifndef	__passion__compiler__primitive_integer__hpp
#define	__passion__compiler__primitive_integer__hpp
#include "variant.hpp"
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	@brief 整数値
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler>
struct	primitive_integer
{
	using	compiler_type	= Compiler;

	enum class	prefix_type
	{
		none,
		minus,
		plus
	};

	enum class	phase_type
	{
		start,
		integer,
		end,
		pass
	};

	primitive_integer(){}
	primitive_integer(vmint_t _n) : mValue(_n) {}

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		if (_token != Token::NUMBER && _token != "-" && _token != "+")
		{
			return	false;
		}
		return	true;
	}


	compute_type get_compute_type() const noexcept
	{
		return	compute_type::static_integer;
	}

	vmvariant variable() const noexcept
	{
		return	vmvariant(mValue);
	}

	vmtypename type() const
	{
		return	vmtypename("int");
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		_asmblr.add_load_int(mValue);
	}


	template<class Expr, class C, class T>
	lex_result<primitive_integer> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				if (_token == "-")
				{
					mPrefix	= prefix_type::minus;
					return	phase_type::integer;
				}
				else if (_token == "+")
				{
					mPrefix	= prefix_type::plus;
					return	phase_type::integer;
				}
			}

			case phase_type::integer :
			{
				if (_token != Token::NUMBER)
				{
					THROW_ERROR(_dummy_, _token);
				}
				mValue		= _token.number();
				if (mPrefix == prefix_type::minus)
				{
					mValue	= -mValue;
				}
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmint_t			mValue	= 0;
	prefix_type		mPrefix	= prefix_type::none;

};


PASSION_NAMESPACE_END
#endif
