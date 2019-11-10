#ifndef	__passion__compiler__function_call__hpp
#define	__passion__compiler__function_call__hpp
PASSION_NAMESPACE_BEGIN



// ********************************************************************
//!	クラスメンバ
// --------------------------------------------------------------------
template<class Compiler>
class	function_call
{
public:
	using	node_type		= node<Compiler>;
	using	expression_type	= expression_node<Compiler>;

	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		function,
		close,
		end,
		pass
	};


	function_call(node_type* _scope)
		: mFunctionNode(_scope)
	{}

	~function_call()
	{
		if (mParameterTypes)
		{
			delete[] mParameterTypes;
		}
	}

	vmtypename type() const noexcept
	{
		return	mTypename;
	}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (!passion_is_void(mTypename))
		{
			_asmblr.add_alloc(1);					// 戻り値バッファ
		}

		auto	arge	= mArguments->arguments().data();
		int		argSize	= (int)mArguments->arguments().size();
		for (int i = 0; i < argSize; ++i)
		{
			expression_node<compiler_type>*	expr	= arge[i];
			expr->compile(_cmplr, _asmblr);

			if (passion_is_reference(expr->type()) && !passion_is_reference(mParameterTypes[i]))
			{
				_asmblr.add_load(
					_cmplr.find_class(passion_remove_reference(expr->type()).remove_pointer()),
					_cmplr.find_class(passion_remove_reference(vmtypename(mParameterTypes[i])).remove_pointer())
				);
			}
		}

		mFunctionNode->compile(_cmplr, _asmblr);
		_asmblr.add_call(argSize);


		for (int i = 0; i < argSize; ++i)
		{
			expression_node<compiler_type>*	expr	= arge[i];
			if (passion_is_gcpointerable(mParameterTypes[i]))
			{
				_asmblr.add_lea_stack(i - argSize);
				_asmblr.add_delete_ref();
			}
		}

		// 戻り値を破棄する
		if (!passion_is_void(mTypename) && _expr != value_mode::rvalue)
		{
			_asmblr.add_free(argSize + 1);
		}
		else
		{
			_asmblr.add_free(argSize);
		}
	}


	template<class Expr, class C, class T>
	lex_result<function_call> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				mArguments		= tuple<compiler_type>::create(&_expr, value_mode::rvalue);
				return	{ phase_type::close, mArguments };
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}

auto	t = vmstring(passion_remove_reference(mFunctionNode->type()));
AMTRS_TRACE_LOG("%s", t.c_str());

				passion_parse_functiontype(&mFunctionInfo, passion_remove_reference(mFunctionNode->type()));
				mTypename		= vmtypename(mFunctionInfo.result_type);
				mParameterTypes	= new passion_string_view[mFunctionInfo.parameter_size];
				passion_split_parameter_types(mParameterTypes, &mFunctionInfo);
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	passion_functiontype_info	mFunctionInfo;
	passion_string_view*		mParameterTypes	= nullptr;
	vmtypename					mTypename;
	ref<tuple<Compiler>>		mArguments;
	ref<node_type>				mFunctionNode;
};


PASSION_NAMESPACE_END
#endif
