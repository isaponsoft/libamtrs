#ifndef	__passion__compiler__invoke_member__hpp
#define	__passion__compiler__invoke_member__hpp
PASSION_NAMESPACE_BEGIN



// ********************************************************************
//!	クラスメンバ
// --------------------------------------------------------------------
template<class Compiler>
class	invoke_member
{
public:
	using	node_type		= node<Compiler>;
	using	expression_type	= expression_node<Compiler>;

	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		name,
		function,
		close,
		end,
		pass
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token, node_type* _scope, bool _arrow)
	{
		vmtypename	st		= passion_remove_reference(_scope->type());
		vmstring	name	= _token.str();
		if (_arrow)
		{
			st	= st.remove_pointer();
		}
		if (st.find("[]") != vmtypename::npos)
		{
			st = "[]";
		}
		if (_cmplr.find_function({_cmplr.find_class(st), _cmplr.find_class("")}, name).second)
		{
			return	true;
		}
		return	false;
	}

	invoke_member(node_type* _scope, bool _arrow)
		: mScope(_scope)
		, mArrow(_arrow)
	{}

	invoke_member(node_type* _scope, std::in_place_type_t<void()>)
		: mScope(_scope)
		, mArrow(false)
		, mFunctionPtrMode(true)
	{}


	vmtypename type() const noexcept
	{
		return	mTypename;
	}


	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (mFunctionTarget)
		{
			_asmblr.add_load_function(mFunction);
		}
		else
		{
			// 関数の検索先のクラス
			auto		scopeClass	= _cmplr.find_class(get_scope_type());
			if (get_scope_type().find("[]") != vmtypename::npos)	// 仮
			{
				scopeClass = _cmplr.find_class("[]");
			}
			if (!scopeClass)
			{
				PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), compiler_error, get_scope_type().c_str());
			}

			// クラス内の関数を検索する
			std::pair<vmclass*, vmfunction*>	func;
			if (func = _cmplr.find_function({scopeClass, _cmplr.find_class("")}, mFunctionName, mArguments->argument_types().data(), mArguments->argument_types().size()); !func.second)
			{
				PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), function_not_found, mFunctionName.c_str());
			}

			// アクセスコントロールを確認
			if (func.second->has_attributes(attribute_flag::private_attribute))
			{
				if (vmtypename(func.first->name()) != vmtypename(_cmplr.clazz))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), member_access_denay, mFunctionName.c_str(), "private", func.first->name().c_str());
				}
			}
			if (func.second->has_attributes(attribute_flag::protected_attribute))
			{
				if (!_cmplr.find_class(vmtypename(_cmplr.clazz))->is_base_of(func.first->name()))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), member_access_denay, mFunctionName.c_str(), "protected", func.first->name().c_str());
				}
			}

			if (func.second->result_type() != std::string_view("void"))
			{
				_asmblr.add_alloc(1);					// 戻り値バッファ
			}

			// 第１引数に this を追加
			if (!func.second->is_static())
			{
				mScope->compile(_cmplr, _asmblr);
				if (passion_is_reference(mScope->type()))
				{
					_asmblr.add_load(scopeClass, func.first);
				}
			}

			_cmplr.add_function_call(_asmblr, func.first, func.second, mArguments->arguments().data(), mArguments->arguments().size());

			if ((func.second->result_type() != std::string_view("void")) && _expr != value_mode::rvalue)
			{
				_asmblr.add_free(1);			// 戻り値バッファ
			}
		}
	}


	template<class Expr, class C, class T>
	lex_result<invoke_member> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::name :
			{
				vmtypename	st		= passion_remove_reference(mScope->type());
				if (mArrow)
				{
					st	= st.remove_pointer();
				}
				if (st.find("[]") != vmtypename::npos)
				{
					st = "[]";
				}

				if (auto f = _cmplr.find_function({_cmplr.find_class(st), _cmplr.find_class("")}, _token.str()); f.second)
				{
					mFunction		= f.second;
					mTypename		= f.second->result_type();
				}
				mFunctionName	= _token.str();
				return	phase_type::function;
			}

			case phase_type::function :
			{
				if (_token != "(")
				{
					// 関数ポインタの取得
					mFunctionTarget	= true;
					mTypename		= mFunction->type();
					return	phase_type::pass;
				}
				mArguments		= tuple<Compiler>::create(&_expr, value_mode::rvalue);
				return	{ phase_type::close, mArguments };
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
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
	vmtypename get_scope_type()
	{
		return	mArrow
				? vmtypename(passion_remove_reference(mScope->type()).remove_pointer())
				: vmtypename(passion_remove_reference(mScope->type()));
	}


	vmtypename					mTypename;
	vmstring					mFunctionName;
	ref<tuple<Compiler>>		mArguments;
	ref<node_type>				mScope;
	vmfunction*					mFunction		= nullptr;
	bool						mArrow;
	bool						mFunctionTarget	= false;

	bool						mFunctionPtrMode= false;
};


PASSION_NAMESPACE_END
#endif
