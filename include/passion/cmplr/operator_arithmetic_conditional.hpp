#ifndef	__passion__compiler__operator_arithmetic_conditional__hpp
#define	__passion__compiler__operator_arithmetic_conditional__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	条件演算子  conditions ? true : false
// --------------------------------------------------------------------
template<class Compiler>
struct	operator_arithmetic_conditional
{
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;

	//! 式の解析フェーズ
	enum class phase_type
	{
		start,
		opecode,
		true_value,
		separator,
		false_value,
		end,
		pass,
	};

	//! 演算子の優先順位を指定
	static constexpr opecode_priority	priority	= opecode_priority::conditions;


	//! 解析開始に使用するトークン
	static constexpr std::string_view	start_token	= std::string_view("?");


	// ========================================================================
	//! コンストラクタ
	// ------------------------------------------------------------------------
	//! 先行して解析した条件式を示すノードを渡してください。
	// ------------------------------------------------------------------------
	operator_arithmetic_conditional(expression_type* _conditions)
		: m_conditions(_conditions)
	{}



	compute_type get_compute_type() const
	{
		return	mAnswer ? mAnswer->get_compute_type() : m_true_block->get_compute_type();
	}

	vmvariant variable() const
	{
		return	mAnswer ? mAnswer->get_static_variable() : m_true_block->get_static_variable();
	}

	vmtypename type() const
	{
		return	mAnswer ? mAnswer->type() : m_true_block->type();
	}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (mAnswer)
		{
			mAnswer->compile(_cmplr, _asmblr);
		}
		else
		{
			auto	labelFalse	= make_ref<_brunch_point>();
			auto	labelEnd	= make_ref<_brunch_point>();
			m_conditions->compile(_cmplr, _asmblr);
			if (passion_is_reference(m_conditions->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(m_conditions->type())));
			}
			_asmblr.add_jfalse(labelFalse);
			m_true_block->compile(_cmplr, _asmblr);
			if (passion_is_reference(m_true_block->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(m_true_block->type())));
			}
			_asmblr.add_jmp(labelEnd);
			_asmblr.insert_label(labelFalse);
			m_false_block->compile(_cmplr, _asmblr);
			if (passion_is_reference(m_false_block->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(m_false_block->type())));
			}
			_asmblr.insert_label(labelEnd);
		}
	}


	template<class Expr, class C, class T>
	lex_result<operator_arithmetic_conditional> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::opecode :
			{
				return	phase_type::true_value;
			}

			case phase_type::true_value :
			{
				m_true_block	= _expr.template create<variant<Compiler>>(value_mode::rvalue);
				return	{ phase_type::separator, m_true_block->lex(_cmplr, _token) };
			}

			case phase_type::separator :
			{
				if (_token != ":")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::false_value;
			}

			case phase_type::false_value :
			{
				m_false_block	= _expr.template create<variant<Compiler>>(value_mode::rvalue);
				return	{ phase_type::end, m_false_block->lex(_cmplr, _token) };
			}

			case phase_type::end :
			{
				if (m_true_block->type() != m_false_block->type())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), condition_type_unmatch, m_true_block->type().c_str(), m_false_block->type().c_str())
				}
#if	0
				if (m_conditions->get_compute_type() == compute_type::static_integer)
				{
					// 静的解決
					mAnswer	= std::get<vmvalue>(m_conditions->get_static_variable()).i == 0
							? m_false_block
							: m_true_block;
					m_conditions	= nullptr;
					m_true_block	= nullptr;
					m_false_block	= nullptr;
				}
				else if (m_conditions->get_compute_type() == compute_type::static_string)
				{
					// 静的解決
					auto	tx	= removeQuotation(std::get<vmstring>(m_conditions->get_static_variable()));
					mAnswer	= tx.empty()
							? m_false_block
							: m_true_block;
					m_conditions	= nullptr;
					m_true_block	= nullptr;
					m_false_block	= nullptr;
				}
#endif
				return	phase_type::pass;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<expression_type>		m_conditions;
	ref<expression_type>		m_true_block;
	ref<expression_type>		m_false_block;
	ref<expression_type>		mAnswer;
};


PASSION_NAMESPACE_END
#endif
