#ifndef	__passion__compiler__ctrlflow_for__hpp
#define	__passion__compiler__ctrlflow_for__hpp
PASSION_NAMESPACE_BEGIN



template<class Compiler>
struct	ctrlflow_for
{
	using	compiler_type	= Compiler;
	using	node_type		= node<compiler_type>;

	enum class phase_type
	{
		start,
		open,
		init,
		init_end,
		conditions,
		conditions_end,
		next,
		close,
		loop_block,
		end,
		pass,
	};

	static constexpr std::string_view	start_token	= std::string_view("for");

	vmtypename type() const { return mTypename; }
	compute_type get_compute_type() const noexcept { return compute_type::none; }


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		auto	labelLoop		= make_ref<_brunch_point>();
		auto&	blk				= _cmplr.block_begin();
		blk.continueLabel	= make_ref<_brunch_point>();
		blk.breakLabel		= make_ref<_brunch_point>();

		// 初期化式
		m_init->compile(_cmplr, _asmblr);

		// 条件比較
		_asmblr.insert_label(labelLoop);
		m_conditions->compile(_cmplr, _asmblr);
		if (passion_is_gcpointer(m_conditions->type()))
		{
			_asmblr.add_ref2int();
		}
		_asmblr.add_jfalse(blk.breakLabel);

		// ルーブブロック
		m_loop_block->compile(_cmplr, _asmblr);

		// 次へ
		_asmblr.insert_label(blk.continueLabel);
		m_next->compile(_cmplr, _asmblr);
		_asmblr.add_jmp(labelLoop);

		_asmblr.insert_label(blk.breakLabel);
		if (m_local_allocate > 0)
		{
			_asmblr.add_free((int)m_local_allocate);
		}

		_cmplr.block_end();
	}

	template<class Expr, class C, class T>
	lex_result<ctrlflow_for> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				if (_token != "for")
				{
					THROW_ERROR(unkown_token, _token);
				}
				_cmplr.block_begin();
				return	phase_type::open;
			}

			case phase_type::open :
			{
				if (_token != "(")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::init;
			}

			case phase_type::init :
			{
				ref<node_type>	statement;
				if (statement.empty())	statement = _expr.template create_if<variable_local_allocate<Compiler>>(value_mode::void_mode, _cmplr, _token);
				if (statement.empty())	statement = expr<variant<Compiler>>::create_if(&_expr, value_mode::void_mode, _cmplr, _token);
				if (statement.empty())
				{
					THROW_ERROR(unkown_token, _token);
				}
				m_init		= statement;
				return	{
					phase_type::init_end,
					m_init->lex(_cmplr, _token)
				};
			}

			case phase_type::init_end :
			{
				if (_token != ";")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::conditions;
			};

			case phase_type::conditions :
			{
				m_conditions	= expr<variant<Compiler>>::create(&_expr, value_mode::rvalue, {});
				return	{
					phase_type::conditions_end,
					m_conditions->lex(_cmplr, _token)
				};
			}

			case phase_type::conditions_end :
			{
				if (_token != ";")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::next;
			};

			case phase_type::next :
			{
				m_next			= expr<variant<Compiler>>::create(&_expr, value_mode::void_mode, {});
				return	{
					phase_type::close,
					m_next->lex(_cmplr, _token)
				};
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::loop_block;
			};

			case phase_type::loop_block :
			{
				m_loop_block	= _expr.template create<codeblock<Compiler>>();
				return	{
					phase_type::end,
					m_loop_block->lex(_cmplr, _token)
				};
			}

			default :
			{
				m_local_allocate	= _cmplr.block_end();
				return	phase_type::pass;
			}
		}
	}

private:
	vmtypename		mTypename;
	ref<node_type>	m_init;
	ref<node_type>	m_conditions;
	ref<node_type>	m_next;
	ref<node_type>	m_loop_block;
	std::size_t		m_local_allocate;
};


PASSION_NAMESPACE_END
#endif
