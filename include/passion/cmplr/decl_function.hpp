#ifndef	__passion__compiler__decl_function__hpp
#define	__passion__compiler__decl_function__hpp
#include "log.hpp"
#include "node.hpp"
PASSION_NAMESPACE_BEGIN

//!	関数／メソッドの構文木
template<class Compiler>
struct	decl_function
{
	using	compiler_type	= Compiler;


	enum class phase_type
	{
		start,
		parameter,			//!< パラメータ解析フェーズ
		parameter_end,		//!< パラメータ解析フェーズ
		code_block,			//!< コードブロック解析フェーズ
		end,
		pass,
	};



	vmtypename type() const noexcept { return mResultType; }


	decl_function(vmtypename _resultType, vmstring _name, uint32_t _attributes)
		: mResultType(_resultType)
		, mFunctionName(_name)
		, mAttributes(_attributes)
	{}


	template<class Lex, class C, class T>
	lex_result<decl_function> lex(Lex& _lex, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			case phase_type::parameter :
			{
				mFunctionParameters	= lexar<parameter_list<Compiler>>::create(&_lex, _token);
				return	{ phase_type::parameter_end, mFunctionParameters->lex(_cmplr, _token) };
			}

			case phase_type::parameter_end :

			case phase_type::code_block :
			{
				// 関数内の式や文は定義が全て終わってからコンパイルすので、定義処理中はトークンを蓄えておく。
				mStatementBuffer.push_back(_token);
				if (_token == "{")
				{
					++mStatementBlockDeprh;
				}
				else if (_token == "}")
				{
					--mStatementBlockDeprh;
					if (mStatementBlockDeprh == 0)
					{
						return	phase_type::end;
					}
				}
				return	phase_type::code_block;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

	void declere(compiler_type& _cmplr, vmclass* _clz)
	{
		// 先に定義を行う
		std::vector<vmtypename>	args;
		for (auto& p : parameters())
		{
			args.push_back(p->type());
		}

		ref<vmbytecode>	f		= new vmbytecode(_clz->name(), mFunctionName, type(), args.begin(), args.end());
		if (_cmplr.classes()->get_global_scope() == _clz || (mAttributes & (uint32_t)attribute_flag::static_attribute))
		{
			f->add_attribute(attribute_flag::static_attribute);
		}

		f->add_attribute(mAttributes);
		if (f->has_attributes(attribute_flag::destractor_attribute))
		{
			_clz->set_destractor(f);
		}
		else
		{
			_clz->add_function(f);
		}

		// 定義が終わった後にコンパイル
		_cmplr.add_compile_callback([this, &_cmplr, _clz, f](auto& _cmp)
		{
			auto			cm		= _cmplr.classes();
			compiler_type	cs(cm);

			PSNCMPLR_TRACE_LOG("bytecode compile %s", f->identify().data());
			PSNASM_TRACE_LOG("assemble byyecode %s", f->identify().data());
			cs.clazz		= _clz->identify();
			cs.thizClass	= (!f->is_destractor() && f->is_virtual())
							? vmstring(_cmplr.find_class(vmtypename(cs.clazz))->find_base_class(f)->name())
							: vmstring(cs.clazz);


			// 作業用のコンパイラを生成
			if (!f->is_static())
			{
				cs.parameters.push_back({{passion_add_pointer(vmtypename(cs.clazz)), _cmplr.find_class(vmtypename(cs.clazz))}, "$"});	// this
			}
			for (const auto& param : this->parameters())
			{
				cs.parameters.push_back({{param->type(), _cmplr.find_class(param->type())}, param->name()});
			}

			_cmplr.block_begin();
			auto					block	= expr<codeblock<Compiler>>::create(nullptr, value_mode::void_mode, {});
			ref<node<Compiler>>		current	= block;
			Token					lasttoken;
			for (auto token = this->mStatementBuffer.read(); token != Token::EOFTOKEN; token = this->mStatementBuffer.read())
			{
				current		= current->lex(cs, token);
				lasttoken	= token;
			}


			asmblr::assembler	asmblr;

			if (f->is_constractor())
			{
				// コンストラクタならベースクラスのコンストラクタを呼び出す。
				for (vmclass* bc : _clz->base_classes())
				{
					vmfunction*	defconst	= nullptr;
					for (vmfunction* f : bc->functions())
					{
						if (f->is_constractor() && f->parameter_size() == 0)
						{
							defconst	= f;
							break;
						}
					}
					if (defconst)
					{
						asmblr.add_lea_arg(0);
						asmblr.add_load(_clz, bc);
						asmblr.add_call(defconst, 1);
						asmblr.add_lea_stack(-1);
						asmblr.add_delete_ref();
						asmblr.add_free(1);
					}
				}
			}

			block->compile(cs, asmblr);
			if (block->get_compute_type() != compute_type::end)
			{
				if (mAttributes & (uint32_t)attribute_flag::destractor_attribute)
				{
					// メンバ変数の破棄を追加
					for (auto& dec : _clz->variables())
					{
						if (passion_is_gcpointerable(dec.type))
						{
							asmblr.add_load_this();
							asmblr.add_lea_object(vmvariable_entry{_clz, dec.name});
							asmblr.add_delete_ref();
						}
					}

					// デストラクタならベースクラスのデストラクタを呼び出す。
					for (vmclass* bc : _clz->base_classes())
					{
						vmfunction*	defdest	= bc->get_destractor();
						asmblr.add_lea_arg(0);
						asmblr.add_load_adr(_clz, bc);
						asmblr.add_call(defdest, 1);
						asmblr.add_free(1);
					}

					auto	retNode	= expr<ctrlflow_return<Compiler>>::create(nullptr, value_mode::void_mode, {}, nullptr);
					retNode->compile(_cmplr, asmblr);
				}
				else if (f->is_constractor())
				{
					auto	retNode	= expr<ctrlflow_return<Compiler>>::create(nullptr, value_mode::void_mode, {}, nullptr);
					retNode->compile(_cmplr, asmblr);
				}
				else
				{
					auto	retNode	= expr<ctrlflow_return<Compiler>>::create(nullptr, value_mode::void_mode, {}, nullptr);
					retNode->compile(_cmplr, asmblr);
				}
			}
			_cmplr.block_end();


			// バイトコードをセットする
			f->bc.code	= asmblr.assemble();

			// 文字列プールをセットする
			f->bc.set_string_pool(cm, asmblr.string_pool().begin(), asmblr.string_pool().end());

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
			f->bc.set_variable_entry(cm, asmblr.variable_table().begin(), asmblr.variable_table().end());
		});
	}

	//!	仮引数リスト
	auto& parameters() const noexcept { return mFunctionParameters->parameters(); }


protected:
	vmtypename							mResultType;
	vmstring							mFunctionName;
	lexref<parameter_list<Compiler>>	mFunctionParameters;

	tokenbuffer							mStatementBuffer;				// バイトコード用のトークンバッファ
	std::size_t							mStatementBlockDeprh	= 0;	// バイトコード用のコードの {} の深さ
	uint32_t							mAttributes;

	friend	struct dynamic_compile_bytecode;
};

PASSION_NAMESPACE_END
#endif

