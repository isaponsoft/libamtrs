#ifndef	__passion__compiler__node_expression_new__hpp
#define	__passion__compiler__node_expression_new__hpp
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	expression_new
{
	using	compiler_type	= Compiler;
	using	node_type		= node<compiler_type>;
	using	expression_type	= expression_node<Compiler>;

	enum class	phase_type
	{
		start,
		type_name,				//!< 型名の読み取り

		check_gcpointer,		//!< ^

		open_bracket,			//!< [ もしくは ( の読み取り
		array_size,				//!< 配列のサイズの読み取り
		close_array,			//!< 配列を閉じる部分の読み取り
		array_dimentions_open,	//!< 多次元配列用
		array_dimentions_close,	//!< 多次元配列用

		parameters_close,				//!< 引数読み取り
		end,
		pass,
	};

	static constexpr std::string_view	start_token	= std::string_view("new");


	//! この式/文が示すデータの型
	vmtypename type() const
	{
		return	mTypename;
	}

	template<class Expr>
	void compile(Expr& _expr, compiler_type& _cmplr, assembler& _asmblr)
	{
		if (passion_is_array(mTypename))
		{
			// size
			auto	t	= passion_trim_array_last_dimention(mTypename);
			if (passion_is_gcpointer(t))
			{
				// 多次元配列もしくはオブジェクトの配列
				mArraySize->compile(_cmplr, _asmblr);
				_asmblr.add_new_arrayref();
			}
			else
			{
				// プリミティブの配列
				auto*	c	= _cmplr.find_class(vmtypename(t));
				auto	sz	= c->object_size();
				_asmblr.add_load_int((int)sz);
				mArraySize->compile(_cmplr, _asmblr);
				_asmblr.add_new_array();
			}
		}
		else
		{
			vmtypename	type	= vmtypename(mTypename.substr(0, mTypename.size() - 1));
			auto*		clz		= _cmplr.find_class(type);
			if (!clz)
			{
				THROW_COMPILER_ERROR();
			}

			auto		acss	= _cmplr.access_permitted(_cmplr.find_class(type)->identify(), _cmplr.clazz);
			if (acss.first != attribute_flag::public_attribute)
			{
				PSNCMPLR_SYNTAX_ERROR(_vm, _expr.location(), member_access_denay, clz->name().c_str(), (clz->access == attribute_flag::private_attribute ? "private" : "protected"), _cmplr.clazz.c_str());
			}


			// コンストラクタを検索
			vmfunction*	func	= nullptr;
			for (auto f : clz->functions())
			{
				if (f->is_constractor())
				{
					if (!mArguments.empty() && mArguments->arguments().size() == f->parameter_size())
					{
						func	= f;
					}
					if (mArguments.empty() && f->parameter_size() == 0)
					{
						func	= f;
					}
				}
			}

			// オブジェクトの確保
			_asmblr.add_new(type);
			if (func)
			{
				_cmplr.add_function_call(_asmblr, clz, func, mArguments->arguments().data(), mArguments->arguments().size());
			}
		}
		if (_expr != value_mode::rvalue)
		{
			_asmblr.add_free(1);
		}
	}


	//! この式/文のトークン処理
	template<class Expr, class C, class T>
	lex_result<expression_new> lex(Expr& _expr, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
			{
				return	phase_type::type_name;
			}

			case phase_type::type_name :
			{
				if (_token != Token::COMMAND)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, _token.str().c_str());
				}
				mTypename	+= _token.str().c_str();
				return	phase_type::check_gcpointer;
			}

			case phase_type::check_gcpointer :
			{
				if (_token == "^")
				{
					mTypename += "^";
					return	phase_type::open_bracket;
				}
			}

			case phase_type::open_bracket :
			{
				if (_token == "(")
				{
					mArguments		= tuple<Compiler>::create(&_expr, value_mode::rvalue);
					return	{ phase_type::parameters_close, mArguments };
				}
				if (_token == "::")
				{
					mTypename		+= "::";
					return	phase_type::type_name;
				}
				if (_token == "[")
				{
					// [ を見つけた場合、サイズを取得する必要があるのでいったん自分で処理する
					return	phase_type::array_size;
				}
				normalize_typename(_expr, _cmplr);
				return	phase_type::pass;
			}


			case phase_type::array_size :
			{
				mArraySize	= _expr.template create<variant<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::close_array,
					mArraySize->lex(_cmplr, _token)
				};
			}

			case phase_type::close_array :
			{
				if (_token != "]")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, _token.str().c_str());
				}
				mTypename += "[]";
				return	phase_type::array_dimentions_open;
			}

			case phase_type::array_dimentions_open :
			{
				if (_token != "[")
				{
					normalize_typename(_expr, _cmplr);
					return	phase_type::pass;
				}
				return	phase_type::array_dimentions_close;
			}

			case phase_type::array_dimentions_close :
			{
				if (_token != "]")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, _token.str().c_str());
				}
				mTypename += "[]";
				return	phase_type::array_dimentions_open;
			}

			case phase_type::parameters_close :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}
				normalize_typename(_expr, _cmplr);
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}


private:
	template<class Expr, class C>
	void normalize_typename(Expr& _expr, C& _cmplr)
	{
		if (!passion_is_array(mTypename))
		{
			mTypename	= _cmplr.normalize_class_path(mTypename);
			if (!_cmplr.find_class(mTypename))
			{
				PSNCMPLR_SYNTAX_ERROR(_cmplr, _expr.location(), unkown_type_name, mTypename.c_str());
			}
			mTypename	+= "^";
		}
	}


	vmtypename						mTypename;
	ref<tuple<Compiler>>			mArguments;
	ref<expr<variant<Compiler>>>	mArraySize;
};


PASSION_NAMESPACE_END
#endif
