#ifndef	__passion__compiler__decl_member__hpp
#define	__passion__compiler__decl_member__hpp
PASSION_NAMESPACE_BEGIN

//! メンバ変数もしくはメンバ関数ひとつを定義します。
template<class Compiler>
struct	decl_member
{
	using	compiler_type	= Compiler;

	enum class phase_type
	{
		start,
		type,
		attrib,
		name,				//!< 関数名解析フェーズ
		func_or_value,		//!< 関数か変数か調べる
		end,
		pass,
	};


	vmtypename identify() const noexcept
	{
		return	mIdentify;
	}

	decl_member(attribute_flag _access)
		: mAccessType(_access)
		, mAttributes((uint32_t)_access)
	{}

	vmtypename type() const noexcept { return mTypename; }

	//! 関数もしくは変数の名前、クラスの場合は常に empty() になる。
	vmstring name() const noexcept { return mName; }

	bool is_class() const noexcept { return !mInnerClassNode.empty(); }
	bool is_function() const noexcept { return !mFunctionNode.empty(); }
	bool is_variable() const noexcept { return !is_class() && !is_function(); }

	uint32_t attribute() const noexcept { return mAttributes; }

	template<class Lex, class C, class T>
	lex_result<decl_member> lex(Lex& _lex, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
				mIdentify	= _lex.parent()->identify();
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
				// クラス名と同じ名前なのでコンストラクタ/デストラクタ
				if (_cmplr.clazz == _token.str())
				{
					if (mAttributes & (uint32_t)attribute_flag::destractor_attribute)
					{
						mName		= "~" + _token.str();
						mTypename	= "void";
						return	phase_type::func_or_value;
					}
					else
					{
						mAttributes |= (uint32_t)attribute_flag::constractor_attribute;
						mName		= _token.str();
						mTypename	= _cmplr.clazz + "^";
						return	phase_type::func_or_value;
					}
				}


				// クラスノードに移行できるかどうかテスト
				auto	clz	= _lex.template create_if<decl_class<Compiler>>(_cmplr, _token, mAccessType);
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

				mTypenameNode	= expr<typepath<Compiler>>::create(&_lex, value_mode::rvalue, _token.location());
				return	{ phase_type::name, mTypenameNode->lex(_cmplr, _token) };
			}

			case phase_type::name :
			{
				if (_token != Token::COMMAND)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _lex.location(), expected_member_name_or, _token.str().c_str());
				}
				mName		= _token.str();
				mTypename	= mTypenameNode->type();
				return	phase_type::func_or_value;
			}

			case phase_type::func_or_value :
			{
				if (_token == "(")
				{
					auto	f = _lex.template create<decl_function<Compiler>>(mTypename, mName, mAttributes);
					mFunctionNode	= f;
					return	{ phase_type::end, f->lex(_cmplr, _token) };
				}
				if (_token == ";")
				{
					return	phase_type::pass;
				}
				THROW_ERROR(unkown_token, _token);
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
	vmtypename										mIdentify;
	vmstring										mName;
	vmtypename										mTypename;
	vmtypename										mClassName;
	uint32_t										mAttributes;
	attribute_flag									mAccessType;
	ref<expr<typepath<Compiler>>>					mTypenameNode;
	lexref<decl_function<Compiler>>					mFunctionNode;			// 関数の時
	lexref<decl_class<Compiler>>					mInnerClassNode;		// クラスの時
};

PASSION_NAMESPACE_END
#endif
