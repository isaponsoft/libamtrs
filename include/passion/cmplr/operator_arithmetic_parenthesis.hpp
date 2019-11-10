#ifndef	__passion__compiler__operator_arithmetic_parenthesis__hpp
#define	__passion__compiler__operator_arithmetic_parenthesis__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	() を処理する。
// --------------------------------------------------------------------
template<class Compiler>
struct	operator_arithmetic_parenthesis
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<Compiler>;

	enum class phase_type
	{
		start,
		close,
		end,
		pass
	};

	static constexpr std::string_view	start_token	= std::string_view("(");

	compute_type get_compute_type() const
	{
		return	m_value->get_compute_type();
	}

	vmvariant variable() const
	{
		return	m_value->get_static_variable();
	}

	vmtypename type() const
	{
		return	m_value->type();
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (m_value.empty())
		{
			THROW_COMPILER_ERROR();
		}
		m_value->compile(_cmplr, _asmblr);
	}

	template<class Expr, class C, class T>
	lex_result<operator_arithmetic_parenthesis> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				if (_token != "(")
				{
					// create_if() で正しく呼び出しているならここに来ることはないはず
					THROW_ERROR(unkown_token, _token);
				}
				m_value	= _expr.template create<variant<Compiler>>();
				return	{ phase_type::close, m_value };
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
				return	phase_type::pass;
			}
		}
	}

private:
	ref<expression_type>		m_value;
};




PASSION_NAMESPACE_END
#endif
