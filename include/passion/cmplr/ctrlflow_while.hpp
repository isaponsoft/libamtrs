#ifndef	__passion__compiler__ctrlflow_while__hpp
#define	__passion__compiler__ctrlflow_while__hpp
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	ctrlflow_while
{
	using	compiler_type	= Compiler;
	using	node_type		= node<Compiler>;

	enum class phase_type
	{
		start,
		conditions,
		loop_block,
		end,
		pass,
	};

	static constexpr std::string_view	start_token	= std::string_view("while");


	vmtypename type() const { return mTypename; }
	compute_type get_compute_type() const noexcept { return compute_type::none; }

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		auto&	blk				= _cmplr.block_begin();
		blk.continueLabel	= make_ref<_brunch_point>();
		blk.breakLabel		= make_ref<_brunch_point>();

		_asmblr.insert_label(blk.continueLabel);

		m_conditions->compile(_cmplr, _asmblr);
		if (passion_is_gcpointer(m_conditions->type()))
		{
			_asmblr.add_ref2int();
		}
		if (passion_is_reference(m_conditions->type()))
		{
			_asmblr.add_load(_cmplr.find_class(passion_remove_reference(m_conditions->type())));
		}
		_asmblr.add_jfalse(blk.breakLabel);
		loop_block->compile(_cmplr, _asmblr);
		_asmblr.add_jmp(blk.continueLabel);

		_asmblr.insert_label(blk.breakLabel);
		_cmplr.block_end();
	}


	template<class Expr, class C, class T>
	lex_result<ctrlflow_while> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
				return	phase_type::conditions;

			case phase_type::conditions :
			{
				m_conditions	= _expr.template create<operator_arithmetic_parenthesis<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::loop_block,
					m_conditions->lex(_cmplr, _token)
				};
			}

			case phase_type::loop_block :
			{
				loop_block		= _expr.template create<codeblock<Compiler>>();
				return	{
					phase_type::end,
					loop_block->lex(_cmplr, _token)
				};
			}


			default :
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmtypename		mTypename;
	ref<node_type>	m_conditions;
	ref<node_type>	loop_block;
	std::size_t		m_local_allocate;
};



PASSION_NAMESPACE_END
#endif
