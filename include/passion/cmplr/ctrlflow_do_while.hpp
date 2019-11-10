#ifndef	__passion__compiler__ctrlflow_do_while__hpp
#define	__passion__compiler__ctrlflow_do_while__hpp
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	ctrlflow_do_while
{
	using	compiler_type	= Compiler;
	using	node_type		= node<Compiler>;


	enum class phase_type
	{
		start,
		mLoopBlock,
		while_keyword,
		conditions,
		end,
		pass,
	};

	static constexpr std::string_view	start_token	= std::string_view("do");

	vmtypename type() const { return mTypename; }
	compute_type get_compute_type() const noexcept { return compute_type::none; }


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		auto&	blk				= _cmplr.block_begin();
		blk.continueLabel	= make_ref<_brunch_point>();
		blk.breakLabel		= make_ref<_brunch_point>();

		_asmblr.insert_label(blk.continueLabel);

		mLoopBlock->compile(_cmplr, _asmblr);
		mConditions->compile(_cmplr, _asmblr);
		if (passion_is_gcpointer(mConditions->type()))
		{
			_asmblr.add_ref2int();
		}
		if (passion_is_reference(mConditions->type()))
		{
			auto		clz		= _cmplr.find_class(passion_remove_reference(mConditions->type()));
			_asmblr.add_load(clz);
		}
		_asmblr.add_jtrue(blk.continueLabel);

		_asmblr.insert_label(blk.breakLabel);
		_cmplr.block_end();
	}


	template<class Expr, class C, class T>
	lex_result<ctrlflow_do_while> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				return	phase_type::mLoopBlock;
			}

			case phase_type::mLoopBlock :
			{
				mLoopBlock		= _expr.template create<codeblock<Compiler>>(value_mode::void_mode);
				return	{
					phase_type::while_keyword,
					mLoopBlock->lex(_cmplr, _token)
				};
			}

			case phase_type::while_keyword :
			{
				if (_token != "while")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::conditions;
			}

			case phase_type::conditions :
			{
				mConditions	= _expr.template create<operator_arithmetic_parenthesis<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::end,
					mConditions->lex(_cmplr, _token)
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
	ref<node_type>	mConditions;
	ref<node_type>	mLoopBlock;
};


PASSION_NAMESPACE_END
#endif
