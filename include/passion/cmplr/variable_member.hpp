#ifndef	__passion__compiler__variable_member__hpp
#define	__passion__compiler__variable_member__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	クラスメンバ
// --------------------------------------------------------------------
template<class Compiler>
struct	variable_member : variable_base<Compiler>
{
	using	_base_type		= variable_base<Compiler>;
	using	compiler_type	= Compiler;
	using	node_type		= node<Compiler>;
	using	expression_type	= expression_node<Compiler>;


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token, expression_type* _scope, bool _arrow)
	{
		vmtypename	st;
		if (_scope)
		{
			st	= passion_remove_reference(_scope->type());
		}
		else
		{
			st	= passion_remove_reference(vmtypename(_cmplr.clazz));
		}
		if (_arrow)
		{
			st	= st.remove_pointer();
		}

		// メンバ
		auto		clz		= _cmplr.find_class(st);
		auto		info	= clz->get_variable_info(vmstring(_token.str()));
		if (info.second)
		{
			return	true;
		}
		auto	i	= clz->get_static_variable_info(vmstring(_token.str()));
		if (i)
		{
			return	true;
		}

		// グローバル変数
		clz		= _cmplr.find_class("");
		if (clz->get_static_variable_info(vmstring(_token.str())))
		{
			return	true;
		}
		return	false;
	}

	variable_member(expression_type* _scope, bool _arrow)
		: mScope(_scope)
		, mArrow(_arrow)
	{}


private:
	virtual vmtypename set_variable_name(compiler_type& _cmplr, const Token& _token) override
	{
		vmtypename	scope;
		if (!mScope)
		{
			scope	= _cmplr.clazz;
		}
		else
		{
			scope	= get_scope_type();
		}

		auto		clz		= _cmplr.find_class(scope);
		auto		r		= clz->get_variable_info(vmstring(_token.str()));
		mVariableClass		= r.first;
		mInfo				= r.second;
		mStaticVariable	= false;
		if (!mInfo)
		{
			mInfo				= clz->get_static_variable_info(vmstring(_token.str()));
			mStaticVariable	= true;
			if (!mInfo)
			{
				clz				= _cmplr.find_class("");
				mInfo			= clz->get_static_variable_info(vmstring(_token.str()));
				mGlobalVariable	= true;
			}
		}
		vmtypename	ret	= mInfo->type;
		return	passion_add_reference(ret);
	}

	virtual void lea(compiler_type& _cmplr, assembler& _asmblr) override
	{
		vmclass*	clz	= nullptr;
		if (!mScope)
		{
			clz	= _cmplr.find_class(vmtypename(_cmplr.clazz));
		}
		else
		{
			clz	= _cmplr.find_class(get_scope_type());
		}

		if (mGlobalVariable)
		{
			_asmblr.add_lea_global(vmvariable_entry{_cmplr.find_class(""), mInfo->name});
		}
		else if (mStaticVariable)
		{
			_asmblr.add_lea_global(vmvariable_entry{clz, mInfo->name});
		}
		else
		{
			if (mScope)
			{
				mScope->compile(_cmplr, _asmblr);
				if (passion_is_reference(mScope->type()))
				{
					_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mScope->type()).remove_pointer()));
				}
				_asmblr.add_lea_object(vmvariable_entry{clz, mInfo->name});
			}
			else
			{
				_asmblr.add_load_this();
				_asmblr.add_lea_object(vmvariable_entry{clz, mInfo->name, vmtypename(_cmplr.thizClass)});
			}
		}
	}


	vmtypename get_scope_type()
	{
		return	mArrow ? vmtypename(passion_remove_reference(mScope->type()).remove_pointer()) : vmtypename(passion_remove_reference(mScope->type()));
	}


	vmstring						mVariableClass;
	const vmclass::declere_value*	mInfo;
	ref<expression_type>			mScope;
	bool							mStaticVariable	= false;
	bool							mGlobalVariable	= false;
	bool							mArrow;
};


PASSION_NAMESPACE_END
#endif
