#ifndef	__passion__compiler__decl_globalscope__hpp
#define	__passion__compiler__decl_globalscope__hpp
PASSION_NAMESPACE_BEGIN


//!	グローバルスコープの構文木
template<class Compiler>
struct	globalscope
{
	using	compiler_type	= Compiler;

	enum class	phase_type
	{
		start,
		main,
		end,
		pass
	};


	// ====================================================================
	//!	グローバルスコープにトークンを一つ食べさせます。
	// --------------------------------------------------------------------
	//!	トークンを処理し、自分が処理できないのなら代わりに処理できる
	//!	ノードを返します。
	// --------------------------------------------------------------------
	template<class Lex, class C, class T>
	lex_result<globalscope> lex(Lex& _lex, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start:
			case phase_type::main:
			{
				_cmplr.clazz	= "";

				// ';' は何も意味を持たない。
				if (_token == ";")
				{
					return	phase_type::main;
				}

				// クラスノードに移行できるかどうかテスト
				auto	clz	= _lex.template create_if<decl_class<Compiler>>(_cmplr, _token, attribute_flag::public_attribute);
				if (clz)
				{
					// クラスに移行できたならクラス解析へ
					m_classes.push_back(clz);
					return	{ phase_type::main, clz->lex(_cmplr, _token) };
				}

				// グローバルスコープのメンバ
				auto	member	= _lex.template create<decl_member<Compiler>>(attribute_flag::public_attribute);
				if (member)
				{
					m_functions.push_back(member);
					return	{ phase_type::main, member->lex(_cmplr, _token) };
				}

				PSNLEX_ERROR(unkown_token, _token.str().c_str());
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

	// ====================================================================
	//!	VMに対して関数やメンバの定義を行います。
	// --------------------------------------------------------------------
	//! vmclass や vmfunction などを生成し、vmに登録します。
	//!	ただし、コードは展開されません。
	// --------------------------------------------------------------------
	void declere(compiler_type& _cmplr)
	{
		auto								c	= _cmplr.find_class("");
		std::vector<vmclass::declere_value>	variables;
		for (auto& m : m_functions)
		{
			if (m->is_variable())
			{
				variables.push_back({ {m->type(), _cmplr.find_class(m->type())}, m->name(), (uint32_t)attribute_flag::static_attribute });
			}
			else
			{
				m->declere(_cmplr, _cmplr.find_class(""));
			}
		}
		c->set_variables(variables.data(), variables.size());
		c->build();
	}



	std::vector<ref<lexar<decl_class<Compiler>>>>		m_classes;
	std::vector<ref<lexar<decl_member<Compiler>>>>		m_functions;
};


PASSION_NAMESPACE_END
#endif
