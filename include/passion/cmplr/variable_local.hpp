#ifndef	__passion__compiler__variable_local__hpp
#define	__passion__compiler__variable_local__hpp
PASSION_NAMESPACE_BEGIN


// ********************************************************************
//!	@brief ローカル変数
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler>
struct	variable_local : variable_base<Compiler>
{
	using	_base_type		= variable_base<Compiler>;
	using	expression_type	= expression_node<Compiler>;
	using	compiler_type	= Compiler;

	using	_base_type::_base_type;

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return	_cmplr.find_local(_token.str()) == npos
				? false
				: true;
	}



private:
	virtual vmtypename set_variable_name(compiler_type& _cmplr, const Token& _token) override
	{
		vmtypename	retval;
		mPosition	= _cmplr.find_local(_token.str());
		if (mPosition != npos)
		{
			mReference	= passion_is_reference(_cmplr.current_block().local[mPosition].type);
			retval		= passion_add_reference(vmtypename(_cmplr.current_block().local[mPosition].type));
		}
		return	retval;
	}

	virtual void lea(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_lea_local((int)mPosition);
		if (mReference)
		{
			_asmblr.add_load_adr();
		}
	}


	std::size_t	mPosition;
	bool		mReference	= false;
};


PASSION_NAMESPACE_END
#endif
