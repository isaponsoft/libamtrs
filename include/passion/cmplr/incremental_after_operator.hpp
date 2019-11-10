#ifndef	__passion__compiler__incremental_after_operator__hpp
#define	__passion__compiler__incremental_after_operator__hpp
PASSION_NAMESPACE_BEGIN



// ********************************************************************
//!	++演算子, --演算子
// --------------------------------------------------------------------
template<class Compiler>
struct	incremental_after_operator
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<Compiler>;

	enum class phase_type
	{
		start,
		end,
		pass
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token, expression_type* _exp)
	{
		return	(_token == "++") || (_token == "--");
	}

	incremental_after_operator(expression_type* _exp)
		: mVariable(_exp)
	{}

	compute_type get_compute_type() const
	{
		return	compute_type::dynamics;
	}

	vmvariant variable() const
	{
		return	{};
	}

	vmtypename type() const
	{
		return	mTypename;
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		auto		clz		= _cmplr.find_class(passion_remove_reference(mVariable->type()));
		if (!mOpeFunction.second)
		{
			// 直接加減算できる。
			mVariable->compile(_cmplr, _asmblr);
			_asmblr.add_load_int(1);
			if (mDecriment)
			{
				_asmblr.add_dec(clz);
			}
			else
			{
				_asmblr.add_inc(clz);
			}
		}
		else
		{
			mVariable->compile(_cmplr, _asmblr);
			_asmblr.add_load_int(1);
			_asmblr.add_alloc(1);
			_asmblr.add_call(mOpeFunction.second, 0);
			if (_expr != value_mode::rvalue)
			{
				_asmblr.add_free(1);
			}
		}
	}

	template<class Expr, class C, class T>
	lex_result<incremental_after_operator> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				mDecriment	= _token == "--";

				auto	lvtype	= mVariable->type();
				if (!passion_is_reference(lvtype))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _token.location(), expression_is_not_assignable, "");
				}
				mTypename	= lvtype;

				// オブジェクト型なら operator ++ を検索する
				auto	clz		= _cmplr.find_class(passion_remove_reference(mVariable->type()));
				if (is_objectable_type(clz->primitive_type()))
				{
					mOpeFunction	= _cmplr.find_operator("++", clz);
					if (!mOpeFunction.first)
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), not_found_operator, clz->identify().c_str(), "++", "");
					}
					mTypename		= mOpeFunction.second->result_type();
				}
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<expression_type>					mVariable;
	bool									mDecriment		= false;
	std::pair<vmclass*, vmfunction*>		mOpeFunction;
	vmtypename								mTypename;
};




PASSION_NAMESPACE_END
#endif
