#ifndef	__passion__compiler__decltype__hpp
#define	__passion__compiler__decltype__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//! decltype(式)
// --------------------------------------------------------------------
template<class Compiler>
struct	decltype_specifier
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<Compiler>;

	enum class	phase_type
	{
		start,
		open,
		value,
		close,
		end,
		pass
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("decltype");
	}


	vmtypename type() const
	{
		return	mTypename;
	}


	template<class Expr, class C, class T>
	lex_result<decltype_specifier> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				return	phase_type::open;
			}

			case phase_type::open :
			{
				if (_token != "(")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), excepted_after, "(", "decltype");
				}
				
				return	phase_type::value;
			}

			case phase_type::value :
			{
				mValueNode	= _expr.template create_if<variant<Compiler>>(_cmplr, _token);
				if (!mValueNode)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), excepted_expression, "decltype");
				}
				return	{ phase_type::close, mValueNode->lex(_cmplr, _token) };
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), excepted, ")");
				}
				mTypename	= passion_remove_reference(mValueNode->type());
				mValueNode.clear();
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmtypename				mTypename;
	ref<expression_type>	mValueNode;
};


PASSION_NAMESPACE_END
#endif
