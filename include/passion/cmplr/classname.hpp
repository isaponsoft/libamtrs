#ifndef	__passion__compiler__decl_typename__hpp
#define	__passion__compiler__decl_typename__hpp
PASSION_NAMESPACE_BEGIN

template<class Compiler>
struct	classname
{
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		name,
		scope_separator,
		end,
		pass
	};


	vmtypename type() const noexcept { return mClassname; }


	template<class Lex, class C, class T>
	lex_result<classname> lex(Lex& _lex, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::name :
			{
				if (_token != Token::COMMAND)
				{
					THROW_ERROR(unkown_token, _token);
				}
				mClassname	+= _token.str();
				return	phase_type::scope_separator;
			}

			case phase_type::scope_separator :
			{
				if (_token != "::")
				{
					return	phase_type::pass;
				}
				mClassname	+= _token.str();
				return	phase_type::name;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}


private:
	vmtypename				mClassname;
};


PASSION_NAMESPACE_END
#endif
