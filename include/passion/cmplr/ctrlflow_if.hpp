#ifndef	__passion__compiler__ctrlflow_if__hpp
#define	__passion__compiler__ctrlflow_if__hpp
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	ctrlflow_if
{
	using	compiler_type	= Compiler;
	using	node_type		= expression_node<compiler_type>;

	enum class phase_type
	{
		start,
		keyword,
		open_bracket,
		init_or_conditions,
		init_end,
		condition,
		close_bracket,
		true_block,
		false_keyword,
		false_block,
		end,
		pass,
	};

	struct	delete_queue
	{
		vmtypename	type;
		std::size_t	lp;
	};

	static constexpr std::string_view	start_token	= std::string_view("if");


	compute_type get_compute_type() const
	{
		return	compute_type::dynamics;//mAnswer ? mAnswer->get_compute_type() : m_true_block->get_compute_type();
	}

	vmvariant variable() const
	{
		return	{};//mAnswer ? mAnswer->get_static_variable() : m_true_block->get_static_variable();
	}

	vmtypename type() const
	{
		return	{};//mAnswer ? mAnswer->type() : m_true_block->type();
	}


	template<class Expr, class C, class T>
	lex_result<ctrlflow_if> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::keyword :
			{
				return	phase_type::open_bracket;
			}

			case phase_type::open_bracket :
			{
				if (_token != "(")
				{
					THROW_ERROR(unkown_token, _token);
				}
				_cmplr.block_begin();
				return	phase_type::init_or_conditions;
			}

			case phase_type::init_or_conditions :
			{
				if (mInit = _expr.template create_if<variable_local_allocate<Compiler>>(value_mode::void_mode, _cmplr, _token); !mInit.empty())
				{
					return	{
						phase_type::init_end,
						mInit->lex(_cmplr, _token)
					};
				}
				m_conditions	= _expr.template create<variant<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::init_end,
					m_conditions->lex(_cmplr, _token)
				};
			}

			case phase_type::init_end :
			{
				if (!m_conditions.empty())
				{
					if (_token == ")")
					{
						// 比較式かつ ) で終わっている場合
						return	phase_type::true_block;
					}
					mInit	= std::move(m_conditions);
				}
				if (_token != ";") 
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::condition;
			}

			case phase_type::condition :
			{
				if (m_conditions = _expr.template create_if<variant<Compiler>>(value_mode::rvalue, _cmplr, _token); !m_conditions.empty())
				{
					return	{
						phase_type::close_bracket,
						m_conditions->lex(_cmplr, _token)
					};
				}
				THROW_ERROR(unkown_token, _token);
			}

			case phase_type::close_bracket :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::true_block;
			}

			case phase_type::true_block :
			{
/*				if (m_conditions->type() != "bool")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), value_is_not_convertible, m_conditions->type().c_str(), "bool");
				}
*/
				m_true_block	= _expr.template create<codeblock<Compiler>>();
				return	{
					phase_type::false_keyword,
					m_true_block->lex(_cmplr, _token)
				};
			}

			case phase_type::false_keyword :
			{
				if (_token != "else")
				{
					constexpression(_cmplr);
					return	phase_type::pass;
				}
				return	phase_type::false_block;
			}

			case phase_type::false_block :
			{
				if (_token == "if")
				{
					m_false_block	= _expr.template create<ctrlflow_if<Compiler>>();
				}
				else if (_token == "{")
				{
					m_false_block	= _expr.template create<codeblock<Compiler>>();
				}
				else
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	{
					phase_type::end,
					m_false_block->lex(_cmplr, _token)
				};
			}

			case phase_type::end :
			default :
			{
				constexpression(_cmplr);
				return	phase_type::pass;
			}
		}
	}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (!mInit.empty())
		{
			mInit->compile(_cmplr, _asmblr);
		}

		if (m_conditions.empty())
		{
			if (!mAnswer.empty())
			{
				mAnswer->compile(_cmplr, _asmblr);
			}
		}
		else
		{
			auto	labelFalse	= make_ref<_brunch_point>();
			m_conditions->compile(_cmplr, _asmblr);
			if (passion_is_gcpointer(m_conditions->type()))
			{
				_asmblr.add_ref2int();
			}
			if (passion_is_reference(m_conditions->type()))
			{
				auto		st		= passion_remove_reference(m_conditions->type());
				if (passion_is_gcpointer(st))
				{
					_asmblr.add_load_adr();
				}
				else
				{
					auto		clz		= _cmplr.find_class(passion_remove_reference(m_conditions->type()));
					_asmblr.add_load(clz);
				}
			}
			_asmblr.add_jfalse(labelFalse);
			m_true_block->compile(_cmplr, _asmblr);
			if (m_false_block.empty())
			{
				_asmblr.insert_label(labelFalse);
			}
			else
			{
				auto	labelEnd	= make_ref<_brunch_point>();
				_asmblr.add_jmp(labelEnd);
				_asmblr.insert_label(labelFalse);
				m_false_block->compile(_cmplr, _asmblr);
				_asmblr.insert_label(labelEnd);
			}
		}

		for (auto& d : mDelete)
		{
			// gcpointer のデストラクタを呼び出し
			_asmblr.add_lea_local(d.lp);
			_asmblr.add_delete_ref();
		}
		if (mLocalAllocate > 0)
		{
			_asmblr.add_free((int)mLocalAllocate);
		}
	}

private:
	void constexpression(compiler_type& _cmplr)
	{
		mLocalAllocate	= _cmplr.local_variable_size();
		for (std::size_t i = 0; i < mLocalAllocate; ++i)
		{
			auto[pos, v]	= _cmplr.get_local_from_back(i);
			if (v && passion_is_gcpointer(v->type))
			{
				mDelete.push_back({v->type, pos});
			}
		}
		_cmplr.block_end();
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
#if	0
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
	}

	vmtypename					m_type;
	std::size_t					mLocalAllocate;
	std::vector<delete_queue>	mDelete;
	ref<node_type>				mInit;
	ref<node_type>				m_conditions;
	ref<node_type>				m_true_block;
	ref<node_type>				m_false_block;
	ref<node_type>				mAnswer;
};

PASSION_NAMESPACE_END
#endif
