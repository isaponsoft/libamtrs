#ifndef	__passion__compiler__decl_class__hpp
#define	__passion__compiler__decl_class__hpp
#include "node.hpp"
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	decl_class
{
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		clazz,				// classキーワード
		name,				// 名前読み取り
		extends,			// 継承リスト
		open,				// { が来る
		block,				// {} の中を処理中
		terminate,			// ;
		access_type_end,	// :
		end,
		pass,
	};

	static constexpr std::string_view	start_token	= std::string_view("class");


	decl_class(attribute_flag _access)
		: mClassAccess(_access)
	{}


	// ====================================================================
	//!	インスタンスの識別名(名前空間を含めた名称)を返す。
	// --------------------------------------------------------------------
	vmtypename identify() const
	{
		return	mIdentify;
	}


	// ========================================================================
	//! 型情報を取得する。
	// ------------------------------------------------------------------------
	//! クラス定義の場合は型名=識別名になります。
	// ------------------------------------------------------------------------
	vmtypename type() const noexcept { return identify(); }


	// ====================================================================
	//!	クラス名を返す。
	// --------------------------------------------------------------------
	//! 親スコープに対するローカルな名前になります。
	// --------------------------------------------------------------------
	vmstring name() const noexcept
	{
		return	mClassName;
	}

	template<class Lex, class C, class T>
	lex_result<decl_class> lex(Lex& _lex, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::clazz :
			{
				mParentClass	= _cmplr.clazz;
				return	phase_type::name;
			}

			case phase_type::name :
			{
				mNameNode		= _lex.template create<classname<Compiler>>();
				return	{ phase_type::extends, mNameNode->lex(_cmplr, _token) };
			}

			case phase_type::extends :
			{
				// 名前を生成する
				mClassName		= mNameNode->type();
				_cmplr.clazz	= mClassName;
				mNameNode.clear();

				auto	parent	= _lex.parent()->identify();
				mIdentify	= parent.empty()
							? vmtypename(mClassName)
							: vmtypename(parent + "::" + mClassName);

				_cmplr.classes()->prototype_class(identify());


				// ; で終わっていればプロトタイプモード
				if (_token == ";")
				{
					mPrototype	= true;
					return	phase_type::end;
				}

				// : があれば継承するクラスの列挙
				if (_token == ":")
				{
					m_extends	= _lex.template create<decl_extends<Compiler>>();
					return	{ phase_type::open, m_extends };
				}

				// それ以外のトークンが来た。
				if (_token != "{")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _lex.location(), declaration_class_unkown_token, identify().c_str(), _token.str().c_str());
				}
				return	phase_type::block;
			}

			case phase_type::open :
			{
				if (_token != "{")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _lex.location(), declaration_class_has_not_open, identify().c_str(), _token.str().c_str());
				}
				return	phase_type::block;
			}

			case phase_type::block :
			{
				if (_token == ";")
				{
					return	phase_type::block;
				}

				if (_token == "}")
				{
					return	phase_type::terminate;
				}

				if (_token == "private")
				{
					mAccessMode	= attribute_flag::private_attribute;
					return	phase_type::access_type_end;
				}
				if (_token == "protected")
				{
					mAccessMode	= attribute_flag::protected_attribute;
					return	phase_type::access_type_end;
				}
				if (_token == "public")
				{
					mAccessMode	= attribute_flag::public_attribute;
					return	phase_type::access_type_end;
				}

				auto	member	= _lex.template create<decl_member<Compiler>>(mAccessMode);
				if (member)
				{
					m_members.push_back(member);
					return	{ phase_type::block, member->lex(_cmplr, _token) };
				}

				PSNCMPLR_SYNTAX_ERROR(_cmplr, _lex.location(), unkown_token_in_class_block, identify().c_str(), _token.str().c_str());
			}

			case phase_type::terminate :
			{
				if (_token != ";")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _lex.location(), declaration_class_unkown_token, identify().c_str(), _token.str().c_str());
				}
				declere(_lex, _cmplr);
				return	phase_type::end;
			}

			case phase_type::access_type_end :
			{
				if (_token != ":")
				{
					PSNCMPLR_COMPILER_ERROR();
				}
				return	phase_type::block;
			}

			default:
			{
				return	phase_type::pass;
			}
		}

		PSNCMPLR_COMPILER_ERROR();
	}


private:
	template<class Lex>
	void declere(Lex& _lex, compiler_type& _cmplr)
	{
		auto	c	= _cmplr.classes()->prototype_class(identify());
		if (!mPrototype)
		{
			if (!c->prototype)
			{
				PSNCMPLR_SYNTAX_ERROR(_cmplr, _lex.location(), redeclare_class, identify().c_str());
			}
			c->prototype	= false;
			c->access		= mClassAccess;
			if (!m_extends.empty())
			{
				m_extends->declere(_cmplr, c);
			}
			std::vector<vmclass::declere_value>	variables;
			for (auto& m : m_members)
			{
				if (m->is_variable())
				{
					variables.push_back({ { m->type(), _cmplr.find_class(m->type()) }, m->name() });
				}
			}
			c->set_variables(variables.data(), variables.size());

			for (auto& m : m_members)
			{
				m->declere(_cmplr, c);
			}


			// デストラクタが未定義ならデフォルトデストラクタを自動作成
			if (!c->created_destractor())
			{
				std::vector<vmtypename>	args;
				ref<vmbytecode>	f		= new vmbytecode(c->name(), vmstring("~") + c->name(), "", args.begin(), args.end());
				f->add_attribute(attribute_flag::destractor_attribute);
				c->set_destractor(f);

				_cmplr.add_compile_callback([this, &_cmplr, c, f](auto& _cmp)
				{
					asmblr::assembler	asmblr;
					for (auto& dec : c->variables())
					{
						if (passion_is_gcpointerable(dec.type))
						{
							asmblr.add_load_this();
							asmblr.add_lea_object(vmvariable_entry{c, dec.name});
							asmblr.add_delete_ref();
						}
					}

					for (vmclass* bc : c->base_classes())
					{
						vmfunction*	defdest	= bc->get_destractor();
						asmblr.add_lea_arg(0);
						asmblr.add_load_adr(c, bc);
						asmblr.add_call(defdest, 1);
						asmblr.add_free(1);
					}
					asmblr.add_ret();


					// バイトコードをセットする
					f->bc.code	= asmblr.assemble();

					// 文字列プールをセットする
					f->bc.set_string_pool(_cmplr.classes(), asmblr.string_pool().begin(), asmblr.string_pool().end());

					// クラスプールをセットする
					{
						std::vector<ref<vmclass>>	classes;
						classes.reserve(asmblr.classes_pool().size());
						for (auto& c : asmblr.classes_pool())
						{
							classes.push_back(_cmplr.find_class(vmtypename(c)));
						}
						f->bc.set_class_pool(classes.begin(), classes.end());
					}

					// 呼び出す関数テーブルをセットする。
					{
						std::vector<passion_function*>	functions;
						functions.reserve(asmblr.function_table().size());
						for (auto& symbol : asmblr.function_table())
						{
							auto	className	= std::string_view(passion_function_get_class_name(std::string_view(symbol)));
							auto*	clz			= _cmplr.find_class(vmtypename(className));
							auto	sym			= passion_string_view(std::string_view(symbol));
							if (!clz)
							{
								return;
							}
							passion_function*	func	= nullptr;
							for (auto f : clz->function_list())
							{
								if (f->symbol == sym)
								{
									func	= f;
									break;
								}
							}
							if (clz->get_destractor()->function()->symbol == sym)
							{
								func	= clz->get_destractor()->function();
							}
							functions.push_back(func);
						}
						f->bc.set_invoke_entry(functions.data(), functions.data() + functions.size());
					}

					// アクセス先のメンバ変数のテーブルを作成する
					f->bc.set_variable_entry(_cmplr.classes(), asmblr.variable_table().begin(), asmblr.variable_table().end());
				});
			}
			c->build();
		}
	}


	vmstring										mClassName;
	vmstring										mParentClass;
	vmtypename										mIdentify;
	lexref<classname<Compiler>>						mNameNode;
	lexref<decl_extends<Compiler>>					m_extends;
	std::vector<lexref<decl_member<Compiler>>>		m_members;
	attribute_flag									mClassAccess	= attribute_flag::private_attribute;
	attribute_flag									mAccessMode		= attribute_flag::private_attribute;
	bool											mPrototype		= false;


};

PASSION_NAMESPACE_END
#endif
