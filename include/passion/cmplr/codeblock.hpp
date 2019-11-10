#ifndef	__passion__compiler__codeblock__hpp
#define	__passion__compiler__codeblock__hpp
#include "expression.hpp"
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	codeblock
{
	using	compiler_type	= Compiler;
	using	node_type		= expression_node<compiler_type>;

	enum class phase_type
	{
		start,
		open,				//!< {
		codes,				//!< 
		end,
		pass,
	};

	struct	delete_queue
	{
		vmtypename	type;
		std::size_t	lp;
	};


	static constexpr std::string_view	start_token	= std::string_view("{");


	vmtypename type() { return mStatements.empty() ? vmtypename() : mStatements.back()->type(); }
	compute_type get_compute_type() const noexcept { return mStatements.empty() ? compute_type::none : mStatements.back()->get_compute_type(); }

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		for (auto& statement : mStatements)
		{
			statement->compile(_cmplr, _asmblr);
		}
		if (get_compute_type() != compute_type::end)
		{
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
	}

	template<class Expr, class C, class T>
	lex_result<codeblock> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::open :
			{
				if (_token != "{")
				{
					THROW_ERROR(unkown_token, _token);
				}
				_cmplr.block_begin();
				return	phase_type::codes;
			}

			case phase_type::codes :
			{
				if (_token == ";")
				{
					return	phase_type::codes;
				}
				if (_token == "}")
				{
					mLocalAllocate	= _cmplr.local_variable_size();
					for (std::size_t i = 0; i < mLocalAllocate; ++i)
					{
						auto[pos, v]	= _cmplr.get_local_from_back(i);
						if (v && passion_is_gcpointerable(v->type))
						{
							mDelete.push_back({v->type, pos});
						}
					}
					_cmplr.block_end();
					return	phase_type::end;
				}

				ref<node_type>	statement;
				if (statement.empty())	statement = _expr.template create_if<codeblock<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<variable_local_allocate<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<ctrlflow_return<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<ctrlflow_if<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<ctrlflow_while<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<ctrlflow_do_while<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<ctrlflow_for<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<ctrlflow_break<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<ctrlflow_continue<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<variant<Compiler>>(_cmplr, _token);
				if (statement.empty())	statement = _expr.template create_if<incremental_operator<Compiler>>(_cmplr, _token);
				if (statement.empty())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, _token.str().c_str());
				}

				if (get_compute_type() == compute_type::end)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _token.location(), code_will_never_be_executed, "");
				}


				mStatements.push_back(statement);
				return	{
					phase_type::codes,
					statement->lex(_cmplr, _token)
				};
			}


			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	std::size_t					mLocalAllocate;
	std::vector<delete_queue>	mDelete;
	std::vector<ref<node_type>>	mStatements;
};

PASSION_NAMESPACE_END
#endif
