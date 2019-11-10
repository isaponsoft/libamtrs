#ifndef	__passion__compiler__decl_value__hpp
#define	__passion__compiler__decl_value__hpp
PASSION_NAMESPACE_BEGIN


// クラス変数
template<class Compiler>
struct	decl_member_variable
{
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		type,
		type_end,
		name,
		end,
		pass,
	};

	vmtypename type() const noexcept
	{
		return	mTypename;
	}

	vmtypename identify() const noexcept
	{
		return	mIdentify;
	}

	vmstring name() const noexcept { return mValueName; }



	template<class Lex, class C, class T>
	lex_result<decl_member_variable> lex(Lex& _lex, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				mTypeNode	= expr<typepath<Compiler>>::create(&_lex, value_mode::rvalue, _token.location());
			}

			case phase_type::type :
			{
				return	{ phase_type::type_end, mTypeNode->lex(_cmplr, _token) };
			}

			case phase_type::type_end:
			{
				mTypename	= mTypeNode->type();
				mTypeNode.clear();
			}

			case phase_type::name :
			{
				if (mTypename == "...")
				{
					mValueName	= "...";
					mIdentify	= vmtypename(_lex.parent()->identify() + "::" + vmtypename(mValueName));
					return	phase_type::pass;
				}

				if (_token != Token::COMMAND)
				{
					THROW_ERROR(unkown_token, _token);
				}

				mValueName	= _token.str();
				mIdentify	= vmtypename(_lex.parent()->identify() + "::" + vmtypename(mValueName));
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmtypename						mTypename;
	vmtypename						mIdentify;
	vmstring						mValueName;
	ref<expr<typepath<Compiler>>>	mTypeNode;
};

PASSION_NAMESPACE_END
#endif
