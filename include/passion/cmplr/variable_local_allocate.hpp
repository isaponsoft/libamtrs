#ifndef	__passion__compiler__variable_local_allocate__hpp
#define	__passion__compiler__variable_local_allocate__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	@brief ローカル変数の定義
// --------------------------------------------------------------------
template<class Compiler>
struct	variable_local_allocate
{
	using	compiler_type	= Compiler;
	using	node_type		= node<Compiler>;
	using	expression_type	= expression_node<Compiler>;

	enum class phase_type
	{
		start,
		variable_name,
		assign_operator,
		end,
		pass,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return typepath<compiler_type>::is_start(_cmplr, _token);
	}


	vmtypename type() const { return mTypename; }
	compute_type get_compute_type() const noexcept { return compute_type::none; }


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (mAssginRightValue.empty())
		{
			_asmblr.add_alloc(1);
		}
		else
		{
			mAssginRightValue->compile(_cmplr, _asmblr);
			if (passion_is_pointer(mTypename) && passion_is_reference(mAssginRightValue->type()))
			{
				_asmblr.add_load_adr(_cmplr.find_class(passion_remove_reference(mAssginRightValue->type()).remove_pointer()), _cmplr.find_class(passion_remove_reference(mTypename).remove_pointer()));
			}
			else if (!passion_is_reference(mTypename) && passion_is_reference(mAssginRightValue->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mAssginRightValue->type()).remove_pointer()), _cmplr.find_class(passion_remove_reference(mTypename).remove_pointer()));
			}
		}
	}


	template<class Expr, class C, class T>
	lex_result<variable_local_allocate> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				mTypeNode	= _expr.template create<typepath<compiler_type>>(value_mode::void_mode);
				return	{
					phase_type::variable_name,
					mTypeNode->lex(_cmplr, _token)
				};
			}

			case phase_type::variable_name :
			{
				if (_token != Token::COMMAND)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), expected_unqualified_id, _token.str().c_str());
				}
				mVariableName	= _token.str();
				return	phase_type::assign_operator;
			}

			case phase_type::assign_operator :
			{
				mTypename	= mTypeNode->type();
				mTypeNode.clear();
				if (_token == ";")
				{
					if (passion_is_reference(mTypename))
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), declaration_of_reference_variable, mVariableName.c_str());
					}
					_cmplr.local_allocate({{mTypename, _cmplr.find_class(mTypename)}, mVariableName});
					return	phase_type::pass;
				}
				if (_token != "=")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), local_var_declare_after_unkown_token, _token.str().c_str());
				}
				mAssginRightValue	= _expr.template create<variant<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::end,
					mAssginRightValue
				};
			}

			case phase_type::end :
			{
				if (passion_is_reference(mTypename) && !passion_is_reference(mAssginRightValue->type()))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), declaration_of_reference_variable_no_variable, mVariableName.c_str());
				}

				if (mTypename == "auto")
				{
					mTypename	= mAssginRightValue->type();
				}
				else if (!is_assign_type(_cmplr, mTypename, mAssginRightValue->type()))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), err_can_not_assign_type, mTypename.c_str(), mAssginRightValue->type().c_str())
				}

				_cmplr.local_allocate({{mTypename, _cmplr.find_class(mTypename)}, mVariableName});
				return	phase_type::pass;
			}

			default :
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>								mAssginRightValue;
	ref<expr<typepath<compiler_type>>>			mTypeNode;
	vmtypename									mTypename;
	vmstring									mVariableName;
};




PASSION_NAMESPACE_END
#endif
