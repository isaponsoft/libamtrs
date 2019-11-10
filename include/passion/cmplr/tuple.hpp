#ifndef	__passion__compiler__tuple__hpp
#define	__passion__compiler__tuple__hpp
PASSION_NAMESPACE_BEGIN



// ********************************************************************
//!	タプル
// --------------------------------------------------------------------
//!	複数の値を線形リストとして保持します。
//! リストをどのようなバイトコードに置き換えるかは、タプルを所持する
//! 親ノードに依存します(関数呼び出しなのか、配列の初期化なのか etc...)。
// --------------------------------------------------------------------
template<class Compiler>
class	tuple
		: public expr<variant<Compiler>>
{
	using	_base_type	= expr<variant<Compiler>>;
public:
	using	expression_type	= expression_node<Compiler>;

	using	compiler_type	= Compiler;

	using	_base_type::_base_type;

	enum class phase_type
	{
		first,
		value,
		split,
	};


	//! 引数として渡された値の型を返します。
	shared_buffer<vmtypename> argument_types() const
	{
		shared_buffer<vmtypename>	retval(m_arguments.size());
		auto	d	= retval.begin();
		for (auto& a : m_arguments)
		{
			*d++	= a->type();
		}
		return	retval;
	}


	static ref<tuple> create(expression_type* _parent, value_mode _vtype)
	{
		auto	thiz	= new tuple(_parent, _vtype, {});
		thiz->m_phase	= phase_type::first;
		thiz->m_type	= "";
		return	thiz;
	}

	virtual compute_type get_compute_type() const noexcept override
	{
		return	compute_type::dynamics;
	}

	virtual vmvariant get_static_variable() const noexcept override
	{
		return	{};
	}

	virtual vmtypename type() const noexcept override
	{
		return	m_type;
	}

	virtual void compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		for (auto a = m_arguments.begin(); a != m_arguments.end(); ++a)
		{
			(*a)->compile(_cmplr, _asmblr);
			if (passion_is_reference((*a)->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference((*a)->type())));
			}
		}
	}

	const std::vector<ref<expression_type>>& arguments() const noexcept
	{
		return	m_arguments;
	}

	virtual ref<node<compiler_type>> lex(compiler_type& _cmplr, const Token& _token) override
	{
		switch (m_phase)
		{
			case phase_type::first :
			{
				if (_token == ")")
				{
					return	expression_type::finish(_cmplr, _token);
				}
			}

			case phase_type::value :
			{
				auto	v	= expr<variant<Compiler>>::create(this, value_mode::rvalue, _token);
				m_arguments.push_back(v);
				m_phase		= phase_type::split;
				return	v->lex(_cmplr, _token);
			}

			case phase_type::split :
			{
				if (_token == ",")
				{
					m_phase		= phase_type::value;
					return	this;
				}
				return	expression_type::finish(_cmplr, _token);
			}

			default :
			{
				return	expression_type::finish();
			}
		}
	}

	phase_type								m_phase;
	vmtypename							m_type;
	std::vector<ref<expression_type>>	m_arguments;
};


PASSION_NAMESPACE_END
#endif
