#ifndef	__passion__compiler__decl_parameter_list__hpp
#define	__passion__compiler__decl_parameter_list__hpp
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	parameter_list
{
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		open,
		close_check,
		declere,		//!< 定義解析
		split,			//!< 区切り記号もしくは終了の解析中
		end,
		pass
	};


	template<class Lex, class C, class T>
	lex_result<parameter_list> lex(Lex& _lex, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start:
			case phase_type::open:
			{
				return	phase_type::close_check;
			}

			case phase_type::close_check :
			{
				if (_token == ")")
				{
					// ここで終わる場合は引数がない
					return	phase_type::end;
				}
			}

			case phase_type::declere :
			{
				auto	decl	= _lex.template create<decl_member_variable<Compiler>>();
				auto	next	= decl->lex(_cmplr, _token);
				if (next)
				{
					mParameters.push_back(decl);
					return	{ phase_type::split, next };
				}
				return	{ _phase, next };
			}

			case phase_type::split :
			{
				if (_token == ")")
				{
					// 引数解析終了
					return	phase_type::end;
				}
				if (_token != ",")
				{
					THROW_ERROR(nothing_close_circle, _token);
				}
				return	phase_type::declere;
			}

			default:
			{
			return	phase_type::pass;
			}
		}
	}


	const auto& parameters() const noexcept { return mParameters; }

private:
	std::vector<lexref<decl_member_variable<Compiler>>>	mParameters;
};

PASSION_NAMESPACE_END
#endif
