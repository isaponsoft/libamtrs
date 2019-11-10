#ifndef	__passion__compile__ctrlflow_continue__hpp
#define	__passion__compile__ctrlflow_continue__hpp
PASSION_NAMESPACE_BEGIN

template<class Compiler>
struct	ctrlflow_continue
{
	using	compiler_type	= Compiler;
	using	node_type		= expression_node<compiler_type>;


	enum class phase_type
	{
		start,
		value,
		end,
		pass,
	};


	static constexpr std::string_view	start_token	= std::string_view("continue");

	vmtypename type() const { return mResult->type(); }

	compute_type get_compute_type() const noexcept { return compute_type::end; }

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		auto&	block	= _cmplr.current_block();
		if (!block.continueLabel)
		{
			PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), continue_not_in, "continue");
		}
		_asmblr.add_jmp(block.continueLabel);
	}

	template<class Expr, class C, class T>
	lex_result<ctrlflow_continue> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				return	phase_type::value;
			}

			case phase_type::value :
			{
				if (_token == ";")
				{
					return	phase_type::end;
				}
				mResult	= expr<variant<Compiler>>::create(&_expr, value_mode::rvalue, {});
				return	{
					phase_type::end,
					mResult->lex(_cmplr, _token)
				};
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>	mResult;
};



PASSION_NAMESPACE_END
#endif
