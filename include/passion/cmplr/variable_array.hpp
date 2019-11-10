#ifndef	__passion__compiler__variable_array__hpp
#define	__passion__compiler__variable_array__hpp
PASSION_NAMESPACE_BEGIN



// ********************************************************************
//!	@brief operator []
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler>
struct	variable_array
{
	using	compiler_type	= Compiler;
	using	expression_type	= expression_node<Compiler>;


	enum class phase_type
	{
		start,
		opecode,
		index_value,
		close,
		next,
		end,
		pass
	};

	variable_array(expression_type* _array_value)
		: mArrayBase(_array_value)
	{}

	compute_type get_compute_type() const noexcept
	{
		return	compute_type::dynamics;
	}

	vmvariant variable() const noexcept
	{
		return	{};
	}

	vmtypename type() const noexcept
	{
		return	passion_add_reference(mLastType);
	}



	template<class Expr, class C, class T>
	lex_result<variable_array> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				mBaseType	= passion_add_reference(mArrayBase->type());
				mLastType	= passion_remove_reference(mBaseType);
			}

			case phase_type::opecode :
			{
				if (_token != "[")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::index_value;
			}

			case phase_type::index_value :
			{
				m_index.push_back(_expr.template create<variant<Compiler>>(value_mode::rvalue));
				return	{ phase_type::close, m_index.back()->lex(_cmplr, _token) };
			}

			case phase_type::close :
			{
				if (_token != "]")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::next;
			}

			case phase_type::next :
			{
				mLastType	= passion_remove_array(mLastType);
				if (_token == "[")
				{
					return	phase_type::index_value;
				}
				if (_token == "=")
				{
					mAssignValue	= _expr.template create<variant<Compiler>>(value_mode::rvalue);
					return	{ phase_type::end, mAssignValue };
				}
				return	phase_type::pass;
			}

			default :
			{
				return	phase_type::pass;
			}
		}
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		vmtypename	currentType	= passion_remove_reference(mBaseType);
		if (!mAssignValue.empty())
		{
			mAssignValue->compile(_cmplr, _asmblr);
			if (passion_is_reference(mAssignValue->type()))
			{
				auto		t		= passion_remove_reference(mAssignValue->type());
				if (passion_is_array(t))
				{
					_asmblr.add_load_ref();
				}
				else if (passion_is_gcpointer(t))
				{
					_asmblr.add_load_ref();
				}
				else
				{
					_asmblr.add_load(_cmplr.find_class(t));
				}
			}
			mArrayBase->compile(_cmplr, _asmblr);
			for (auto& idx : m_index)
			{
				_asmblr.add_load_adr();
				idx->compile(_cmplr, _asmblr);
				if (passion_is_reference(idx->type()))
				{
					_asmblr.add_load(_cmplr.find_class(passion_remove_reference(idx->type())));
				}
				makelea(_cmplr, _asmblr, currentType);
				currentType	= passion_remove_array(currentType);
			}

			auto		t		= passion_remove_reference(mAssignValue->type());
			if (passion_is_array(t))
			{
				if (_expr == value_mode::rvalue)
				{
					_asmblr.add_storenp(nullptr);
				}
				else
				{
					_asmblr.add_store(nullptr);
				}
			}
			else
			{
				auto	clz	= _cmplr.find_class(t.remove_pointer());
				if (_expr == value_mode::rvalue)
				{
					_asmblr.add_storenp(clz);
				}
				else
				{
					_asmblr.add_store(clz);
				}
			}
		}
		else
		{
			mArrayBase->compile(_cmplr, _asmblr);
			for (auto& idx : m_index)
			{
				_asmblr.add_load_adr();
				idx->compile(_cmplr, _asmblr);
				if (passion_is_reference(idx->type()))
				{
					_asmblr.add_load(_cmplr.find_class(passion_remove_reference(idx->type())));
				}
				makelea(_cmplr, _asmblr, currentType);
				currentType	= passion_remove_array(currentType);
			}
		}
	}

private:

	void makelea(compiler_type& _cmplr, assembler& _asmblr, vmtypename _current)
	{
		if (passion_is_array(_current))
		{
			_asmblr.add_lea_aref();
		}
		else
		{
			auto	clz	= _cmplr.find_class(passion_remove_reference(_current));
			_asmblr.add_lea_array(clz);
		}
	}


	vmtypename								mBaseType;
	vmtypename								mLastType;
	ref<expression_type>					mArrayBase;
	std::vector<ref<expression_type>>		m_index;
	ref<expression_type>					mAssignValue;
};



PASSION_NAMESPACE_END
#endif
