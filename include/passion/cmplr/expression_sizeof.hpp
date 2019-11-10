#ifndef	__passion__compiler__node_operator_sizeof__hpp
#define	__passion__compiler__node_operator_sizeof__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	sizeof()
// --------------------------------------------------------------------
template<class Compiler>
struct	expression_sizeof
{
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		keyword,
		variadic,
		open,
		name,
		close,
		end,
		pass,
	};

	static constexpr std::string_view	start_token	= std::string_view("sizeof");

	vmtypename type() const noexcept { return mTypename; }


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
	}

	template<class Expr, class C, class T>
	lex_result<expression_sizeof> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::keyword :
			{
				return	phase_type::variadic;
			}

			case phase_type::variadic :
			{
				if (_token != "...")
				{
					// 現状では可変引数以外には利用できない
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::open;
			}

			case phase_type::open :
			{
				if (_token != "(")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::name;
			}

			case phase_type::name :
			{
				if (_token != "...")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::close;
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					THROW_ERROR(err_not_close_parenthesis, _token);
				}
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::end;
			}
		}
	}

private:
	vmtypename		mTypename;
};


PASSION_NAMESPACE_END
#endif
