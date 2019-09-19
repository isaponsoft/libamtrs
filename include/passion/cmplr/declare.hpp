#ifndef	__passion__compiler__declare__hpp
#define	__passion__compiler__declare__hpp
PASSION_NAMESPACE_BEGIN


//!	関数／メソッドの構文木
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	decl_function : node<Compiler>, Traits
{
	using	traits_type		= Traits;
	using	node_type		= typename traits_type::node_type;
	using	compiler_type	= typename traits_type::compiler_type;
	using	input_type		= typename traits_type::input_type;

	enum class phase_type
	{
		start		= node_type::phase_start,
		end			= node_type::phase_end,
		pass		= node_type::phase_pass,
	};



	virtual vmstring on_type() const override { return mResultType; }


	decl_function(vmstring _resultType, vmstring _name, typeidentify<compiler_type>* _declare, uint32_t _attributes)
		: mResultType(_resultType)
		, mFunctionName(_name)
		, mDeclare(_declare)
		, mAttributes(_attributes)
	{}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, const input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
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
				return	phase_type::start;
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
		auto&	params	= mDeclare->function_parameters();

		std::vector<vmstring>	args;
		for (auto t : params)
		{
			args.push_back(t.type->type());
		}

		ref<vmbytecode>	f		= new vmbytecode(_clz->name(), mFunctionName, on_type(), args.begin(), args.end());
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
		{
			vmstring	name	= 
						_clz->name().empty()
						? vmstring(f->name())
						: vmstring(_clz->name() + "::" + f->name());
			_cmplr.add_function(name, f->function());
		}

		// 定義が終わった後にコンパイル
		_cmplr.add_compile_callback([this, &_cmplr, _clz, f, params](auto& /*_cmp*/) -> bool
		{
			auto			cm		= _cmplr.classes();
			compiler_type	cs(cm, &_cmplr);

			PSNCMPLR_TRACE_LOG("bytecode compile  [%p] %s", (void*)f, f->identify().data());
			PSNASM_TRACE_LOG  ("bytecode assemble [%p] %s", (void*)f, f->identify().data());
			cs.clazz		= _clz->identify();
			cs.thizClass	= (!f->is_destractor() && f->is_virtual())
							? vmstring(_cmplr.find_class(vmstring(cs.clazz))->find_base_class(f)->name())
							: vmstring(cs.clazz);


			// 作業用のコンパイラを生成
			if (!f->is_static())
			{
				cs.parameters.push_back({passion_add_pointer(vmstring(cs.clazz)), "$"});	// this
			}

			for (const auto& param : params)
			{
				cs.parameters.push_back({param.type->type(), param.name});
			}

			_cmplr.block_begin();
			auto					block	= this->template create<codeblock<Compiler>>(value_mode::void_mode);
			ref<node<Compiler>>		current	= block;
			Token					lasttoken;
			for (auto token = this->mStatementBuffer.read(); token != Token::EOFTOKEN; token = this->mStatementBuffer.read())
			{
				current		= current->lex(cs, token);
				if (auto e = cs.get_error(); e)
				{
					_cmplr.set_error(e);
					return	false;
				}
				lasttoken	= token;
			}


			asmblr::assembler	asmblr;

			if (f->is_constractor())
			{
				// コンストラクタならベースクラスのコンストラクタを呼び出す。
				for (vmclass* bc : _clz->base_classes())
				{
					vmfunction*	defconst	= nullptr;
					for (vmfunction* f2 : bc->functions())
					{
						if (f2->is_constractor() && f2->parameter_size() == 0)
						{
							defconst	= f2;
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

					auto	retNode	= this->template create<ctrlflow_return<Compiler>>(value_mode::void_mode, nullptr);
					retNode->compile(_cmplr, asmblr);
				}
				else if (f->is_constractor())
				{
					auto	retNode	= this->template create<ctrlflow_return<Compiler>>(value_mode::void_mode, nullptr);
					retNode->compile(_cmplr, asmblr);
				}
				else
				{
					auto	retNode	= this->template create<ctrlflow_return<Compiler>>(value_mode::void_mode, nullptr);
					retNode->compile(_cmplr, asmblr);
				}
			}
			_cmplr.block_end();


			// バイトコードをセットする
			f->bc.code	= asmblr.assemble();

			// 文字列プールをセットする
			f->bc.set_string_pool(asmblr.string_pool().begin(), asmblr.string_pool().end());

			// クラスプールをセットする
			{
				std::vector<ref<vmclass>>	classes;
				classes.reserve(asmblr.classes_pool().size());
				for (auto& c : asmblr.classes_pool())
				{
					classes.push_back(_cmplr.find_class(vmstring(c)));
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
					auto*	clz			= _cmplr.find_class(vmstring(className));
					auto	sym			= amtrs_string_view(std::string_view(symbol));
					if (!clz)
					{
						return	false;
					}
					passion_function*	func	= nullptr;
					for (auto f2 : clz->function_list())
					{
						if (f2->symbol == sym)
						{
							func	= f2;
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
			f->bc.set_variable_entry(asmblr.variable_table().begin(), asmblr.variable_table().end());

			return	true;
		});
	}


protected:
	vmstring								mResultType;
	vmstring								mFunctionName;
	ref<typeidentify<compiler_type>>		mDeclare;

	tokenbuffer								mStatementBuffer;				// バイトコード用のトークンバッファ
	std::size_t								mStatementBlockDeprh	= 0;	// バイトコード用のコードの {} の深さ
	uint32_t								mAttributes;

	friend	struct dynamic_compile_bytecode;
};



//! メンバ変数もしくはメンバ関数ひとつを定義します。
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	decl_member : node<Compiler>, Traits
{
	using	traits_type		= Traits;
	using	node_type		= typename traits_type::node_type;
	using	compiler_type	= typename traits_type::compiler_type;
	using	input_type		= typename traits_type::input_type;

	enum class phase_type
	{
		start		= node_type::phase_start,
		end			= node_type::phase_end,
		pass		= node_type::phase_pass,
		type,
		attrib,
		name,				//!< 関数名解析フェーズ
		func_or_value,		//!< 関数か変数か調べる
	};


	virtual vmstring identify() const override
	{
		return	mIdentify;
	}

	decl_member(attribute_flag _access)
		: mAttributes((uint32_t)_access)
		, mAccessType(_access)
	{}

	virtual vmstring on_type() const override { return mTypename; }

	//! 関数もしくは変数の名前、クラスの場合は常に empty() になる。
	vmstring name() const noexcept { return mName; }

	bool is_class() const noexcept { return !mInnerClassNode.empty(); }
	bool is_function() const noexcept { return !mFunctionNode.empty(); }
	bool is_variable() const noexcept { return !is_class() && !is_function(); }

	uint32_t attribute() const noexcept { return mAttributes; }

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
				mIdentify	= this->parent()->identify();
				mClassName	= _cmplr.clazz;

			case phase_type::attrib :
				if (_token == "~")
				{
					mAttributes |= (uint32_t)attribute_flag::destractor_attribute;
					return	phase_type::type;
				}

				if (_token == "static")
				{
					mAttributes |= (uint32_t)attribute_flag::static_attribute;
					return	phase_type::attrib;
				}

				if (_token == "virtual")
				{
					mAttributes |= (uint32_t)attribute_flag::virtual_attribute;
					return	phase_type::attrib;
				}

			case phase_type::type :
			{
				// クラスノードに移行できるかどうかテスト
				auto	clz	= this->template create_if<decl_class<Compiler>>(_cmplr, _token, mAccessType);
				if (clz)
				{
					// クラスに移行できたならクラス解析へ
					mInnerClassNode	= clz;
					return	{ phase_type::end, clz->lex(_cmplr, _token) };
				}

				// ; は無視する
				if (_token == ";")
				{
					return	phase_type::type;
				}

				// 閉じられたので呼び出し元で処理する
				if (_token == "}")
				{
					return	phase_type::pass;
				}

				// 型名を読み取る
				mTypenameNode	= this->template create<typeidentify<Compiler>>(value_mode::rvalue);
				return	{ phase_type::name, mTypenameNode->lex(_cmplr, _token) };
			}

			case phase_type::name :
			{
				mTypename		= mTypenameNode->type();

				// 型名の直後に関数を検出したのでコンストラクタかデストラクタである
				if (_token == "{")
				{
					passion_functiontype_info	finfo;
					if (!passion_is_function(&finfo, mTypename))
					{
						// 型名は関数ではないのでエラー
						THROW_ERROR(unkown_token, _token);
					}

					mName	= vmstring(finfo.result_type);
					if (mName != _cmplr.clazz)
					{
						// 名前が一致しない
						THROW_ERROR(unkown_token, _token);
					}

					if (mAttributes &(uint32_t)attribute_flag::destractor_attribute)
					{
						mName		= vmstring("~") + mName;
					}
					else
					{
						mAttributes |= (uint32_t)attribute_flag::constractor_attribute;
					}
					mTypename		= "void";
					mFunctionNode	= this->template create<decl_function<Compiler>>(mTypename, mName, mTypenameNode, mAttributes);
					mTypenameNode.clear();
					return	{ phase_type::end, mFunctionNode->lex(_cmplr, _token) };
				}

				mTypenameNode	= this->template create<typeidentify<Compiler>>(value_mode::rvalue);
				return	{ phase_type::func_or_value, mTypenameNode->lex(_cmplr, _token) };
			}

			case phase_type::func_or_value :
			{
				passion_functiontype_info	finfo;
				if (passion_is_function(&finfo, mTypenameNode->type()))
				{
					// 関数を発見
					mName			= vmstring(finfo.result_type);
					mFunctionNode = this->template create<decl_function<Compiler>>(mTypename, mName, mTypenameNode, mAttributes);
					return	{ phase_type::end, mFunctionNode->lex(_cmplr, _token) };
				}

				// それ以外なので変数扱い
				mName			= vmstring(mTypenameNode->type());
				return	phase_type::pass;
			}

			default:
			{
				_cmplr.clazz	= mClassName;
				return	phase_type::pass;
			}
		}
		THROW_ERROR(compiler_error, _token);
	}
	
	void declere(compiler_type& _cmplr, vmclass* _clz)
	{
		// 関数なら関数を展開する
		if (!mFunctionNode.empty())
		{
			mFunctionNode->declere(_cmplr, _clz);
			return;
		}
	}

private:
	vmstring										mIdentify;
	vmstring										mName;
	vmstring										mTypename;
	vmstring										mClassName;
	uint32_t										mAttributes;
	attribute_flag									mAccessType;
	ref<typeidentify<Compiler>>					mTypenameNode;
	ref<decl_function<Compiler>>				mFunctionNode;			// 関数の時
	ref<decl_class<Compiler>>					mInnerClassNode;		// クラスの時
};



template<class Compiler, class Traits>
struct	decl_class : node<Compiler>, Traits
{
	using	traits_type		= Traits;
	using	node_type		= typename traits_type::node_type;
	using	compiler_type	= typename traits_type::compiler_type;
	using	input_type		= typename traits_type::input_type;

	enum class phase_type
	{
		start		= node_type::phase_start,
		end			= node_type::phase_end,
		pass		= node_type::phase_pass,
		clazz,				// classキーワード
		name,				// 名前読み取り
		extends,			// 継承リスト
		open,				// { が来る
		block,				// {} の中を処理中
		terminate,			// ;
		access_type_end,	// :
	};


	struct	extend_info
	{
		attribute_flag						access;
		ref<typeidentify<compiler_type>>	clazz;
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token, ...)
	{
		return _token.str() == std::string_view("class");
	}


	decl_class(attribute_flag _access)
		: mClassAccess(_access)
	{}


	// ====================================================================
	//!	インスタンスの識別名(名前空間を含めた名称)を返す。
	// --------------------------------------------------------------------
	virtual vmstring identify() const override
	{
		return	mIdentify;
	}


	// ========================================================================
	//! 型情報を取得する。
	// ------------------------------------------------------------------------
	//! クラス定義の場合は型名=識別名になります。
	// ------------------------------------------------------------------------
	virtual vmstring on_type() const override { return identify(); }


	// ====================================================================
	//!	クラス名を返す。
	// --------------------------------------------------------------------
	//! 親スコープに対するローカルな名前になります。
	// --------------------------------------------------------------------
	vmstring name() const noexcept
	{
		return	mClassName;
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			case phase_type::clazz :
			{
				mParentClass	= _cmplr.clazz;
				return	phase_type::name;
			}

			case phase_type::name :
			{
				mNameNode		= this->template create<typeidentify<Compiler>>(value_mode::rvalue);
				return	{ phase_type::extends, mNameNode->lex(_cmplr, _token) };
			}

			case phase_type::extends :
			{
				// 名前を生成する
				mClassName		= mNameNode->type();
				_cmplr.clazz	= mClassName;
				_cmplr.add_class(mClassName);
				mNameNode.clear();

				auto	parent	= this->parent()->identify();
				mIdentify	= parent.empty()
							? vmstring(mClassName)
							: vmstring(parent + "::" + mClassName);
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
					mExtends	= this->template create<decl_extends>();
					return	{ phase_type::open, mExtends };
				}

				// それ以外のトークンが来た。
				if (_token != "{")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), declaration_class_unkown_token, identify().c_str(), _token.str().c_str());
				}
				return	phase_type::block;
			}

			case phase_type::open :
			{
				if (_token != "{")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), declaration_class_has_not_open, identify().c_str(), _token.str().c_str());
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

				auto	member	= this->template create<decl_member<Compiler>>(mAccessMode);
				if (member)
				{
					mMembers.push_back(member);
					return	{ phase_type::block, member->lex(_cmplr, _token) };
				}

				PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), unkown_token_in_class_block, identify().c_str(), _token.str().c_str());
			}

			case phase_type::terminate :
			{
				if (_token != ";")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), declaration_class_unkown_token, identify().c_str(), _token.str().c_str());
				}
				declere(_cmplr);
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
	parser_result<compiler_type> declere(compiler_type& _cmplr)
	{
		auto	c	= _cmplr.classes()->prototype_class(identify());
		if (!mPrototype)
		{
			if (!c->prototype)
			{
				PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), redeclare_class, identify().c_str());
			}
			c->prototype	= false;
			c->access		= mClassAccess;
			if (!mExtends.empty())
			{
				mExtends->declere(_cmplr, c);
			}

			std::vector<extend_info>			extends		= mExtends.empty() ? std::vector<extend_info>() : mExtends->get_extends();
			std::vector<vmclass::declere_value>	variables;
			for (auto& m : mMembers)
			{
				if (m->is_variable())
				{
					vmstring	name	= c->name().empty()
										? vmstring(m->name())
										: vmstring(c->name()) + "::" + vmstring(m->name());
					_cmplr.add_variable(name, m->type());
					variables.push_back({m->type(), m->name()});
				}
			}
			c->set_variables(variables.data(), variables.size());

			for (auto& m : mMembers)
			{
				m->declere(_cmplr, c);
			}

//			c->initialize_layout(extends.data(), extends.size(), mMembers.data(), mMembers



			// デストラクタが未定義ならデフォルトデストラクタを自動作成
			if (!c->created_destractor())
			{
				std::vector<vmstring>	args;
				ref<vmbytecode>	f		= new vmbytecode(c->name(), vmstring("~") + c->name(), "", args.begin(), args.end());
				f->add_attribute(attribute_flag::destractor_attribute);
				c->set_destractor(f);

				_cmplr.add_compile_callback([c, f](auto& _cmplr) -> bool
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
					f->bc.set_string_pool(asmblr.string_pool().begin(), asmblr.string_pool().end());

					// クラスプールをセットする
					{
						std::vector<ref<vmclass>>	classes;
						classes.reserve(asmblr.classes_pool().size());
						for (auto& c2 : asmblr.classes_pool())
						{
							classes.push_back(_cmplr.find_class(vmstring(c2)));
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
							auto*	clz			= _cmplr.find_class(vmstring(className));
							auto	sym			= amtrs_string_view(std::string_view(symbol));
							if (!clz)
							{
								return	false;
							}
							passion_function*	func	= nullptr;
							for (auto f2 : clz->function_list())
							{
								if (f2->symbol == sym)
								{
									func	= f2;
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
					f->bc.set_variable_entry(asmblr.variable_table().begin(), asmblr.variable_table().end());
					return	true;
				});
			}
			c->build();
		}
		return	0;
	}

	struct	decl_extends : node<Compiler>, Traits
	{
		using	traits_type		= Traits;
		using	node_type		= typename traits_type::node_type;
		using	compiler_type	= typename traits_type::compiler_type;
		using	input_type		= typename traits_type::input_type;

		enum	phase_type
		{
			start		= node_type::phase_start,
			end			= node_type::phase_end,
			pass		= node_type::phase_pass,
			access,
			type,			// 型
			separator,
		};

		virtual vmstring identify() const override
		{
			return	mIdentify;
		}

		virtual vmstring on_type() const override { return vmstring(); }


		const auto& types() const noexcept { return mBaseClasses; }


		virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
		{
			switch ((phase_type)_phase)
			{
				case phase_type::start :
					mIdentify	= this->parent()->identify();


				case phase_type::access :
				{
					if (_token == "private")
					{
						mNextAccess	= attribute_flag::private_attribute;
						return	phase_type::type;
					}
					if (_token == "protected")
					{
						mNextAccess	= attribute_flag::protected_attribute;
						return	phase_type::type;
					}
					if (_token == "public")
					{
						mNextAccess	= attribute_flag::public_attribute;
						return	phase_type::type;
					}
				}


				case phase_type::type :
				{
					auto	type	= this->template create<typeidentify<Compiler>>(value_mode::rvalue);
					mBaseClasses.push_back({ mNextAccess, type });
					return	{ phase_type::separator, type->lex(_cmplr, _token) };
				}


				case phase_type::separator :
				{
					if (_token == ",")
					{
						mNextAccess	= attribute_flag::private_attribute;
						return	phase_type::access;
					}
					return	phase_type::pass;
				}


				default:
				{
					return	phase_type::pass;
				}
			}
		}



		void declere(compiler_type& _cmplr, vmclass* _clazz)
		{
			for (auto& base : mBaseClasses)
			{
				_clazz->add_base_class(_cmplr.find_class(base.clazz->type()));
			}
		}

		const std::vector<extend_info>& get_extends() const noexcept
		{
			return	mBaseClasses;
		}

	private:
		vmstring					mIdentify;
		attribute_flag				mNextAccess	= attribute_flag::private_attribute;
		std::vector<extend_info>	mBaseClasses;
	};




	vmstring											mClassName;
	vmstring											mParentClass;
	vmstring											mIdentify;
	ref<typeidentify<compiler_type>>					mNameNode;
	ref<decl_extends>									mExtends;
	std::vector<ref<decl_member<compiler_type>>>		mMembers;
	attribute_flag										mClassAccess	= attribute_flag::private_attribute;
	attribute_flag										mAccessMode		= attribute_flag::private_attribute;
	bool												mPrototype		= false;
};


template<class Compiler, class Traits = expression_traits<Compiler>>
struct	decl_namespace : node<Compiler>, Traits
{
	using	traits_type		= Traits;
	using	node_type		= typename traits_type::node_type;
	using	compiler_type	= typename traits_type::compiler_type;
	using	input_type		= typename traits_type::input_type;

	enum class phase_type
	{
		start		= node_type::phase_start,
		end			= node_type::phase_end,
		pass		= node_type::phase_pass,
		keyword,		//!< keyword = 'namespace' read.
		name,			//!< read namespace.
		open_bracket,	//!< read '{'
		block,			//!< declare block.
		close_bracket,	//!< read '}'
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("namespace");
	}


	decl_namespace()
	{}


	// ====================================================================
	//!	インスタンスの識別名(名前空間を含めた名称)を返す。
	// --------------------------------------------------------------------
	virtual vmstring identify() const override
	{
		return	vmstring(mAbsNamespace);
	}


	// ========================================================================
	//! 型情報を取得する。
	// ------------------------------------------------------------------------
	//! クラス定義の場合は型名=識別名になります。
	// ------------------------------------------------------------------------
	virtual vmstring on_type() const override { return identify(); }

	vmstring name() const noexcept
	{
		return	mAbsNamespace;
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			case phase_type::keyword :
			{
				return	phase_type::name;
			}

			case phase_type::name :
			{
				mDeclareName += _token.str();
				return	phase_type::open_bracket;
			}

			case phase_type::open_bracket :
			{
				if (_token == "::")
				{
					mDeclareName += "::";
					return	phase_type::name;
				}

				if (_token != "{")
				{
				}

				mPreviousNamespace	= _cmplr.currentNamespace;
				mAbsNamespace		= mPreviousNamespace.empty()
									? vmstring(mDeclareName)
									: vmstring(mPreviousNamespace + "::" + mDeclareName);
				_cmplr.currentNamespace	= mAbsNamespace;
				return	phase_type::block;
			}

			case phase_type::block :
			{
				mBlock	= this->template create<globalscope<compiler_type>>();
				return	{ phase_type::close_bracket, mBlock->lex(_cmplr, _token) };
			}

			case phase_type::close_bracket :
			{
				mBlock->declere(_cmplr);
				_cmplr.currentNamespace	= mPreviousNamespace;
				_cmplr.add_namespace(mPreviousNamespace);
				if (_token != "}")
				{
				}
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}

		PSNCMPLR_COMPILER_ERROR();
	}


	vmstring								mAbsNamespace;		//!< 絶対パスに直した名前空間
	vmstring								mDeclareName;		//!< 宣言で指定されたの名前
	vmstring								mPreviousNamespace;	//!< 自身の前の名前空間
	ref<globalscope<compiler_type>>	mBlock;
};




//!	グローバルスコープの構文木
template<class Compiler, class Traits>
struct	globalscope : node<Compiler>, Traits
{
	using	traits_type		= Traits;
	using	node_type		= typename traits_type::node_type;
	using	compiler_type	= typename traits_type::compiler_type;
	using	input_type		= typename traits_type::input_type;

	enum class	phase_type
	{
		start		= node_type::phase_start,
		end			= node_type::phase_end,
		pass		= node_type::phase_pass,
		main,
	};

	using	node_type::node_type;

	virtual vmstring on_type() const override { return vmstring(); }

	// ====================================================================
	//!	グローバルスコープにトークンを一つ食べさせます。
	// --------------------------------------------------------------------
	//!	トークンを処理し、自分が処理できないのなら代わりに処理できる
	//!	ノードを返します。
	// --------------------------------------------------------------------
	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start:
			case phase_type::main:
			{
				_cmplr.clazz	= "";

				// ';' は何も意味を持たない。
				if (_token == ";")
				{
					return	parser_result<compiler_type>((int)phase_type::main);
				}

				if (_token == "}")
				{
					return	(int)phase_type::pass;
				}

				// namespace node check.
				auto	ns	= this->template create_if<decl_namespace<Compiler>>(_cmplr, _token);
				if (ns)
				{
					// クラスに移行できたならクラス解析へ
					mNamespaces.push_back(ns);
					return	{ (int)phase_type::main, ns->lex(_cmplr, _token) };
				}

				// クラスノードに移行できるかどうかテスト
				auto	clz	= this->template create_if<decl_class<Compiler>>(_cmplr, _token, attribute_flag::public_attribute);
				if (clz)
				{
					// クラスに移行できたならクラス解析へ
					mClasses.push_back(clz);
					return	{(int) phase_type::main, clz->lex(_cmplr, _token) };
				}

				// グローバルスコープのメンバ
				auto	member	= this->template create<decl_member<Compiler>>(attribute_flag::public_attribute);
				if (member)
				{
					mFunctions.push_back(member);
					return	{ (int)phase_type::main, member->lex(_cmplr, _token) };
				}


//				PSNLEX_ERROR(unkown_token, _token.str().c_str());
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
		for (auto& m : mFunctions)
		{
			if (m->is_variable())
			{
				_cmplr.add_variable(m->name(), m->type());
				variables.push_back({m->type(), m->name(), (uint32_t)attribute_flag::static_attribute });
			}
			else
			{
				m->declere(_cmplr, _cmplr.find_class(""));
			}
		}
		c->set_variables(variables.data(), variables.size());
		c->build();
	}



	std::vector<ref<decl_class<Compiler>>>		mClasses;
	std::vector<ref<decl_member<Compiler>>>		mFunctions;
	std::vector<ref<decl_namespace<Compiler>>>	mNamespaces;
};


PASSION_NAMESPACE_END
#endif
