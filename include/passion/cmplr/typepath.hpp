#ifndef	__passion__compiler__typepath__hpp
#define	__passion__compiler__typepath__hpp
PASSION_NAMESPACE_BEGIN

template<class Compiler>
struct	typepath
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<Compiler>;

	enum class	phase_type
	{
		start,
		name,
		separator,
		array_open,
		array_bracket,
		decltype_mode,
		end,
		pass,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _cmplr.find_class(vmtypename(_token.str())) || _token == "auto" || decltype_specifier<Compiler>::is_start(_cmplr, _token);
	}


	//! この式/文が示すデータの型
	vmtypename type() const
	{
		return	mTypename;
	}


	//! この式/文のトークン処理
	template<class Expr, class C, class T>
	lex_result<typepath> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
				if (_token == "...")
				{
					mTypename	= "...";
				return	phase_type::end;
				}

			case phase_type::name :
			{
				mDecltype	= _expr.template create_if<decltype_specifier<Compiler>>(value_mode::rvalue, _cmplr, _token);
				if (!mDecltype.empty())
				{
					return	{ phase_type::decltype_mode, mDecltype->lex(_cmplr, _token) };
				}
				if (_token != Token::COMMAND)
				{
					THROW_ERROR(unkown_token, _token);
				}
				mTypename	+= _token.str();
				if (!_cmplr.find_class(mTypename) && mTypename != "auto")
				{
					PSNCMPLR_SYNTAX_ERROR(_vm, _expr.location(), unkown_type_name, mTypename.c_str());
				}
				return	phase_type::separator;
			}

			case phase_type::decltype_mode :
			{
				mTypename	= mDecltype->type();
				mDecltype.clear();
			}

			case phase_type::separator :
			{
				if (_token == PASSION_TYPENAME_GCPOINTER_SUFFIX)
				{
					mTypename	+= PASSION_TYPENAME_GCPOINTER_SUFFIX;
					return	phase_type::array_open;
				}
				if (_token == PASSION_TYPENAME_POINTER_SUFFIX)
				{
					mTypename	+= PASSION_TYPENAME_POINTER_SUFFIX;
					return	phase_type::separator;
				}
				if (_token == PASSION_TYPENAME_REFERENCE_SUFFIX)
				{
					mTypename	+= PASSION_TYPENAME_REFERENCE_SUFFIX;
					return	phase_type::end;
				}
				if (_token == "[")
				{
					return	phase_type::array_bracket;
				}
				if (_token != "::")
				{
					return	phase_type::pass;
				}
				mTypename	+= "::";
				return	phase_type::name;
			}

			case phase_type::array_open :
			{
				if (_token == PASSION_TYPENAME_REFERENCE_SUFFIX)
				{
					mTypename	+= PASSION_TYPENAME_REFERENCE_SUFFIX;
					return	phase_type::end;
				}
				if (_token != "[")
				{
					return	phase_type::pass;
				}
				return	phase_type::array_bracket;
			}

			case phase_type::array_bracket :
			{
				if (_token != "]")
				{
					THROW_ERROR(unkown_token, _token);
				}
				mTypename	+= "[]";
				return	phase_type::array_open;
			}


			default: return phase_type::pass;
		}
	}

private:
	vmtypename					mTypename;
	ref<expression_type>		mDecltype;
};


PASSION_NAMESPACE_END
#endif
