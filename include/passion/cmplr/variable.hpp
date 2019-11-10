#ifndef	__passion__compiler__variable__hpp
#define	__passion__compiler__variable__hpp
PASSION_NAMESPACE_BEGIN




// ********************************************************************
//!	変数のベースクラス
// --------------------------------------------------------------------
//! 値の代入が可能なノードです。
//!	代入演算子の処理を行います。
// --------------------------------------------------------------------
template<class Compiler>
class	variable_base
{
public:
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		name,
		next,
		end,
		pass,
	};


	virtual vmtypename type() const noexcept
	{
		return	m_type;
	}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		std::size_t		prmsize	= _expr.type().primitive_size();
		const char*		cmd		= nullptr;
		if (!m_assign.empty())
		{
			m_assign->compile(_cmplr, _asmblr);
			if (passion_is_reference(m_assign->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(m_assign->type()).remove_pointer()), _cmplr.find_class(passion_remove_reference(m_type).remove_pointer()));
			}
			lea(_cmplr, _asmblr);
			if (_expr == value_mode::rvalue)
			{
				auto	clz		= _cmplr.find_class(passion_remove_reference(m_type).remove_pointer());
				_asmblr.add_storenp(clz);
			}
			else
			{
				auto	clz		= _cmplr.find_class(passion_remove_reference(m_type).remove_pointer());
				_asmblr.add_store(clz);
			}
		}
		else
		{
			lea(_cmplr, _asmblr);
		}
	}


	template<class Expr, class C, class T>
	lex_result<variable_base> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::name :
			{
				if (_token != Token::COMMAND)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, _token.str().c_str());
				}
				m_type	= set_variable_name(_cmplr, _token);
				if (m_type.empty())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, _token.str().c_str());
				}
				if (_expr == value_mode::lvalue)
				{
					m_type	= passion_add_reference(m_type);
				}
				return	phase_type::next;
			}

			case phase_type::next :
			{
				if (_token == "=")
				{
					m_assign	= _expr.template create<variant<Compiler>>(value_mode::rvalue);
					return	{ phase_type::end, m_assign };
				}
				return	phase_type::pass;
			}

			default :
			{
				if (!m_assign.empty())
				{
					if (!is_assign_type(_cmplr, m_type, m_assign->type()))
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _token.location(), err_can_not_assign_type, m_type.c_str(), m_assign->type().c_str());
					}
				}
				return	phase_type::pass;
			}
		}
	}

protected:
	//! 変数名が確定した
	virtual vmtypename set_variable_name(compiler_type& _cmplr, const Token& _token) = 0;


	virtual void lea(compiler_type& _cmplr, assembler& _asmblr) = 0;


	ref<expression_type>		m_assign;
	vmtypename					m_type;
};



PASSION_NAMESPACE_END
#endif
