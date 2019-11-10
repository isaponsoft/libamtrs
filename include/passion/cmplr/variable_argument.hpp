#ifndef	__passion__compiler__node_variable_argument__hpp
#define	__passion__compiler__node_variable_argument__hpp
PASSION_NAMESPACE_BEGIN



// ********************************************************************
//!	@brief 引数変数
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler>
struct	variable_argument : public variable_base<Compiler>
{
	using	_base_type		= variable_base<Compiler>;
	using	expression_type	= expression_node<Compiler>;

	using	compiler_type	= Compiler;

	using	_base_type::_base_type;

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return	_cmplr.find_argv(_token.str()) == npos
				? false
				: true;
	}


private:
	virtual vmtypename set_variable_name(compiler_type& _cmplr, const Token& _token) override
	{
		vmtypename	retval;
		m_var_index	= _cmplr.find_argv(_token.str());
		if (m_var_index != npos)
		{
			mReference	= passion_is_reference(_cmplr.parameters[m_var_index].type);
			retval		= passion_add_reference(vmtypename(_cmplr.parameters[m_var_index].type));
		}
		return	retval;
	}

	virtual void lea(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_lea_arg(-(int)m_var_index);
		if (mReference)
		{
			_asmblr.add_load_adr();
		}
	}


	std::size_t	m_var_index;
	bool		mReference	= false;
	
};




PASSION_NAMESPACE_END
#endif
