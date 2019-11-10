#ifndef	__passion__compiler__ctrlflow_return__hpp
#define	__passion__compiler__ctrlflow_return__hpp
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	ctrlflow_return
{
	using	compiler_type	= Compiler;
	using	node_type		= node<compiler_type>;


	enum class phase_type
	{
		start,
		value,
		value_end,
		end,
		pass,
	};

	struct	delete_queue
	{
		vmtypename	type;
		std::size_t	lp;
	};


	static constexpr std::string_view	start_token	= std::string_view("return");

	vmtypename type() const { return mResult->type(); }
	compute_type get_compute_type() const noexcept { return compute_type::end; }


	ctrlflow_return()
	{}

	ctrlflow_return(node_type* _result)
		: mResult(_result)
	{}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		vmtypename	rettype;
		if (!mResult.empty())
		{
			mResult->compile(_cmplr, _asmblr);
			if (passion_is_reference(mResult->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mResult->type())));
			}
			rettype	= passion_remove_reference(mResult->type());
			_asmblr.add_lea_arg(1);
			auto		clz		= _cmplr.find_class(rettype);
			_asmblr.add_store(clz);
		}

		// ローカル変数の削除
		if (mFreeSize > 0)
		{
			for (auto& d : mDelete)
			{
				// gcpointer のデストラクタを呼び出し
				_asmblr.add_lea_local(d.lp);
				_asmblr.add_delete_ref();
			}
			_asmblr.add_free((int)mFreeSize);
		}

		_asmblr.add_ret();
	}

	template<class Expr, class C, class T>
	lex_result<ctrlflow_return> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
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
					mFreeSize	= _cmplr.current_block().local.size();;
					for (std::size_t i = 0; i < mFreeSize; ++i)
					{
						auto[pos, v]	= _cmplr.get_local_from_back(i);
						if (v && passion_is_gcpointerable(v->type))
						{
							mDelete.push_back({v->type, pos});
						}
					}
					return	phase_type::end;
				}
				mResult	= expr<variant<Compiler>>::create(&_expr, value_mode::rvalue, {});
				return	{
					phase_type::value_end,
					mResult->lex(_cmplr, _token)
				};
			}

			case phase_type::value_end :
			{
				mFreeSize	= _cmplr.current_block().local.size();;
				for (std::size_t i = 0; i < mFreeSize; ++i)
				{
					auto[pos, v]	= _cmplr.get_local_from_back(i);
					if (v && passion_is_gcpointerable(v->type))
					{
						mDelete.push_back({v->type, pos});
					}
				}
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>						mResult;
	std::size_t							mFreeSize	= 0;
	std::vector<delete_queue>			mDelete;
};



PASSION_NAMESPACE_END
#endif
