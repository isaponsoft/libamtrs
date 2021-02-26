/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__compiler__statement__hpp
#define	__passion__compiler__statement__hpp
#include "node.hpp"
PASSION_NAMESPACE_BEGIN 





// ********************************************************************
//! 識別子として認識可能な型。
// --------------------------------------------------------------------
//! 参照やポインタ、関数型など。
// --------------------------------------------------------------------
template<class Compiler, class Traits>
struct	typeidentify : node<Compiler>, Traits
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
		name_end,
		first_type_end,
		pointer_suffix,
		function_parameter_type_start,
		function_parameter_name,
		function_parameter_separator,
		function_parameter_separator_end,
		function_parameter_equals,
		function_parameter_value,
		function_parameter_end,
		function_suffix,
		array_size,
	};

	//! 関数型の時の引数パラメータの型と名前を保持する。
	//! 名前については省略可能なので empty() の場合が存在する。
	struct	parameter
	{
		ref<typeidentify<compiler_type>>	type;
		vmstring							name;
		ref<statement<compiler_type>>		value;
	};


	// コンパイル時は何もしない
	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override {}

	// 構文解析
	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				mTypeid	+= _token;
				return	phase_type::name_end;
			}

			case phase_type::name_end :
			{
				if (_token == "::")
				{
					mTypeid	+= _token;
					return	phase_type::start;
				}
			}

			case phase_type::first_type_end :
			{
				// ( があれば関数型
				if (_token == "(")
				{
					mReturnType	=  mTypeid;
					mTypeid		+= "(";
					return	{ phase_type::function_parameter_type_start };
				}

				// [ があれば配列型
				if (_token == "[")
				{
					mTypeid += "[";
					return	phase_type::array_size;
				}

				// & があれば参照型
				if (_token == "&")
				{
					mTypeid	= add_reference(_cmplr, mTypeid);
					return	phase_type::end;
				}

				// const
				if (_token == "const")
				{
					mTypeid	+= " const";
					return	phase_type::end;
				}

				// ^ もしくは * があればポインタ型
				if (_token == "*" || _token == "^")
				{
					mTypeid	+= _token;
					return	phase_type::pointer_suffix;
				}

				return	phase_type::pass;
			}


			case phase_type::function_parameter_type_start :
			{
				// 引数パラメータなし
				if (_token == ")")
				{
					mTypeid 	+= ")";
					return	phase_type::function_suffix;
				}

				auto	ti	= this->template create<typeidentify<compiler_type>>(value_mode::rvalue);
				mParameters.push_back({ti, vmstring(), nullptr});
				return	{ phase_type::function_parameter_name, ti->lex(_cmplr, _token) };
			}


			case phase_type::function_parameter_name :
			{
				mTypeid	+= mParameters.back().type->type();

				// 名前なしかつ引数終了
				if (_token == ")")
				{
					mTypeid 	+= ")";
					return	phase_type::function_suffix;
				}

				// 名前なしかつ次の引数へ
				if (_token == ",")
				{
					mTypeid 	+= ",";
					auto	ti	= this->template create<typeidentify<compiler_type>>(value_mode::rvalue);
					mParameters.push_back({ti, vmstring(), nullptr});
					return	{ phase_type::function_parameter_name, ti };
				}

				// 名前あり
				mParameters.back().name	= _token;
				return	phase_type::function_parameter_separator;
			}

			case phase_type::function_parameter_separator :
			{
				// 名前なしかつ引数終了
				if (_token == ")")
				{
					mTypeid 	+= ")";
					return	phase_type::function_suffix;
				}

				// 次の引数へ
				if (_token == ",")
				{
					mTypeid 	+= ",";
					auto	ti	= this->template create<typeidentify<compiler_type>>(value_mode::rvalue);
					mParameters.push_back({ti, vmstring(), nullptr});
					return	{ phase_type::function_parameter_name, ti };
				}

				// デフォルト引数
				if ((_token == "=") && mParameters.back().value.empty())
				{
					auto	stm	= this->template create<statement<compiler_type>>(value_mode::rvalue);
					mParameters.back().value	= stm;
					return	{ phase_type::function_parameter_separator, stm };
				}

				return	phase_type::pass;
			}


			case phase_type::function_suffix :
			{
				if (_token == "&")
				{
					mTypeid	= add_reference(_cmplr, mTypeid);
					return	phase_type::end;
				}

				if (_token == "[")
				{
					mTypeid	+= "[";
					return	phase_type::array_size;
				}

				if (_token == "*")
				{
					mTypeid	+= "*";
					return	phase_type::function_suffix;
				}

				return	phase_type::pass;
			}

			case phase_type::pointer_suffix :
			{
				if (_token == "*" || _token == "^")
				{
					mTypeid	+= _token;
					return	phase_type::pointer_suffix;
				}
				if (_token == "[")
				{
					mTypeid	+= _token;
					return	phase_type::array_size;
				}
				// ( があれば関数型
				if (_token == "(")
				{
					mTypeid		+= "(";
					return	{ phase_type::function_parameter_type_start };
				}
				return	phase_type::pass;
			}

			case phase_type::array_size :
			{
				if (_token == "]")
				{
					mTypeid	+= _token;
					return	phase_type::function_suffix;
				}
				return	phase_type::pass;
			}

			default:
				return	phase_type::pass;
		}
	}

	virtual vmstring on_type() const override { return mTypeid; }

	const vmstring& function_return_type() const noexcept { return mReturnType; }

	const auto& function_parameters() const noexcept { return mParameters; }

private:
	vmstring					mTypeid;
	vmstring					mReturnType;
	// 関数モード時
	std::vector<parameter>		mParameters;
};



// ********************************************************************
//! 関数の仮引数パラメータ
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	parameters : node<Compiler>, Traits
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
		value,
		separator,
	};

	parameters()
	{}

	//! アイテムを指定して初期化する。
	//! このコンストラクタを使用した場合は lex を呼び出さない。
	template<class It>
	parameters(It _first, It _last)
		: mItems(_first, _last)
	{
		make_types();
	}


	auto types() const noexcept { return mTypes; }

	auto nodes() const noexcept { return mItems; }


	virtual vmstring on_type() const override { return mTypename; }


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		for (auto e : mItems)
		{
			e->compile(_cmplr, _asmblr);
			if (passion_is_reference(e->type()))
			{
				_asmblr.add_load(passion_remove_reference(e->type()));
			}
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				if (_token == ")")
				{
					make_types();
					return	phase_type::pass;
				}
			}

			case phase_type::value :
			{
				auto	v	= this->template create<statement<Compiler>>(value_mode::rvalue);
				shared_buffer<ref<node_type>>	n(mItems.size() + 1);
				std::copy(mItems.begin(), mItems.end(), n.begin());
				n[mItems.size()]	= v;
				mItems		= std::move(n);
				return	{ phase_type::separator, v->lex(_cmplr, _token) };
			}

			case phase_type::separator :
			{
				if (_token == ",")
				{
					return	phase_type::value;
				}
				make_types();
				return	phase_type::pass;
			}

			default :
			{
				return	phase_type::pass;
			}
		}
	}

	//! パラメータが指定する関数の引数に一致するか調べる
	bool is_match(struct passion_functiontype_info* _finfo)
	{
		// 関数のパラメータの型を取り出す
		std::vector<amtrs_string_view>		parameterTypes;
		parameterTypes.resize(_finfo->parameter_size);
		passion_split_parameter_types(parameterTypes.data(), _finfo);

		// 引数の数が一致しない
		if (mTypes.size() != _finfo->parameter_size)
		{
			return	false;
		}
/*
		bool	match	= true;
		for (std::size_t i = 0; i < _argc; ++i)
		{
			auto	t1	= vmstring(extract_typename(*this, f->parameter(i)));
			auto	t2	= vmstring(extract_typename(*this, _argt[i]));

			if (t1 == "...")
			{
				break;
			}

			if (implicit_cast(t1, t2))
			{
				continue;
			}
			if (implicit_cast(t1, passion_remove_reference(t2)))
			{
				continue;
			}
			match	= false;
			break;
		}
		if (match)
		{
			retval	= {clz, f};
			if (!f->is_virtual())
			{
				return	retval;
			}
		}
*/
		return	true;
	}


private:
	void make_types()
	{
		mTypes		= shared_buffer<vmstring>(mItems.size());
		auto	d	= mTypes.begin();
		for (auto item : mItems)
		{
			*d++	= passion_remove_reference(item->type());
		}
		mTypename	= vmstring::join(mTypes.begin(), mTypes.end(), ",");
	}


	shared_buffer<ref<node_type>>	mItems;
	shared_buffer<vmstring>			mTypes;
	vmstring						mTypename;
};



// ********************************************************************
//! 関数呼び出しを行うためのノード
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	function_caller : node<Compiler>, Traits
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
		close,
	};

	// 関数の参照呼び出し
	function_caller(node_type* _scope)
		: mScope(_scope)
	{}


	function_caller(node_type* _scope, vmstring _className, vmstring _functionName)
		: mScope(_scope)
		, mClassName(_className)
		, mFunctionName(_functionName)
	{}


	function_caller(vmfunction* _function, node_type* _scope, parameters<Compiler>* _arguments)
		: mScope(_scope)
		, mFunction(_function)
		, mNodeTypename(_function->result_type())
		, mArguments(_arguments)
	{}


	void set_auto_this()
	{
		mAutoThis	= true;
	}

	virtual vmstring on_type() const override { return mNodeTypename; }

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		bool	hasReturnValue	= mNodeTypename != "void";

		if (hasReturnValue)
		{
			_asmblr.add_alloc(1);					// 戻り値バッファ
		}

		bool		useThis	= mScope != nullptr;
		if (mFunction)
		{
			useThis	= mFunction->is_thisable();
		}

		if (!useThis)
		{
			// thisポインタが不要なのでスコープはコンパイルしない
		}
		else if (mScope)
		{
			auto	rt	= extract_typename(_cmplr, mScope->type());
			if (passion_is_function(nullptr, passion_trim_reference(rt)))
			{
				passion_functiontype_info			finfo;
				std::vector<amtrs_string_view>		parameterTypes;

				passion_parse_functiontype(&finfo, passion_trim_reference(mScope->type()));
				parameterTypes.resize(finfo.parameter_size);
				auto	arge	= mArguments->nodes().data();
				int		argSize	= (int)mArguments->nodes().size();
				passion_split_parameter_types(parameterTypes.data(), &finfo);
				for (int i = 0; i < argSize; ++i)
				{
					node_type*	expr	= arge[i];
					expr->compile(_cmplr, _asmblr);
					if (passion_is_reference(expr->type()) && !passion_is_reference(parameterTypes[i]))
					{
						_asmblr.add_load(
							_cmplr.find_class(passion_remove_reference(expr->type()).remove_pointer()),
							_cmplr.find_class(passion_remove_reference(vmstring(parameterTypes[i])).remove_pointer())
						);
					}
				}
				mScope->compile(_cmplr, _asmblr);
				_asmblr.add_call(argSize);

				for (int i = 0; i < argSize; ++i)
				{
					if (passion_is_gcpointerable(extract_typename(_cmplr, parameterTypes[i])))
					{
						_asmblr.add_lea_stack(i - argSize);
						_asmblr.add_delete_ref();
					}
				}

				auto	freeSize	= argSize;
				if (hasReturnValue && this->get_value_type() != value_mode::rvalue)
				{
					++freeSize;
				}
				_asmblr.add_free(freeSize);
				return;
			}
			else if (passion_is_reference(rt))
			{
				vmclass*		fclz	= mFunction
										? _cmplr.find_class(passion_function_get_class_name(mFunction->identify()))
										: nullptr;
				mScope->compile(_cmplr, _asmblr);
				_asmblr.add_load(_cmplr.find_class(passion_trim_gcpointer(passion_trim_reference(rt))), fclz);
			}
			else
			{
				mScope->compile(_cmplr, _asmblr);
			}
		}
		else if (mAutoThis)
		{
			_asmblr.add_load_this();
		}
		else
		{
		}

		vmclass*	scope	= mClassName.empty()
							? nullptr
							: _cmplr.find_class(mClassName);
		_cmplr.add_function_call(_asmblr, scope, mFunction, mArguments->nodes().data(), mArguments->nodes().size());

		// 戻り値を返す関数を呼び出したが戻り値を親に返す必要がないので破棄する。
		if (hasReturnValue && this->get_value_type() != value_mode::rvalue)
		{
			_asmblr.add_free(1);
		}
	}



	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				if (_token == "(")
				{
					mArguments		= this->template create<parameters<Compiler>>(value_mode::rvalue);
					return	{ phase_type::close, mArguments };
				}
				return	phase_type::pass;
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}

				struct passion_functiontype_info	info;
				if (passion_is_function(&info, passion_trim_reference(mScope->type())))
				{
					// 引数の型が一致するか調べる
					if (!mArguments->is_match(&info))
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), function_not_found, "function pointer is not match parameters.");
					}

					// 関数ポインタによる呼び出し
					mNodeTypename	= info.result_type;
				}
				else
				{
					// パラメータ込みで一致する関数を検索する
					mFunction	= _cmplr.find_function({_cmplr.find_class(mClassName)}, mFunctionName, mArguments->types().data(), mArguments->types().size()).second;
					if (!mFunction)
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), function_not_found, mFunctionName.c_str());
					}
					// このノードは関数の戻り値を返す
					mNodeTypename	= mFunction->result_type();
				}
				return	phase_type::end;
			}
				


			default :
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>						mScope;
	vmstring							mClassName;
	vmstring							mFunctionName;
	vmfunction*							mFunction		= nullptr;
	vmstring							mNodeTypename;
	ref<parameters<Compiler>>			mArguments;
	bool								mAutoThis		= false;
};



// ********************************************************************
//! 引数変数
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	variable_arg : node<Compiler>, Traits
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


	variable_arg(compiler_type& _cmplr, vmstring _name, size_t _position)
		: mVarName(std::move(_name))
		, mVarPosition(_position)
		, mVarType(_cmplr.parameters[_position].type)
	{}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_lea_arg(-(int)mVarPosition);
		// 参照型の変数なので常に開いておく
		if (passion_is_reference(extract_typename(_cmplr, mVarType)))
		{
			_asmblr.add_load_adr();
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		mVarTypeRefer	= add_reference(_cmplr, mVarType);
		return	phase_type::end;
	}


	virtual vmstring on_type() const override { return mVarTypeRefer; }


private:
	vmstring	mVarName;
	size_t		mVarPosition;
	vmstring	mVarType;
	vmstring	mVarTypeRefer;
};



// ********************************************************************
//! ローカル変数
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	variable_local : node<Compiler>, Traits
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


	variable_local(compiler_type& _cmplr, vmstring _name, size_t _position)
		: mVarName(std::move(_name))
		, mVarPosition(_position)
		, mVarType(_cmplr.get_local(_position).second->type)
	{}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_lea_local((int)mVarPosition);
		// 参照型の変数なので常に開いておく
		if (passion_is_reference(extract_typename(_cmplr, mVarType)))
		{
			_asmblr.add_load_adr();
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		mVarTypeRefer	= add_reference(_cmplr, mVarType);
		return	phase_type::end;
	}


	virtual vmstring on_type() const override { return mVarTypeRefer; }


private:
	vmstring	mVarName;
	size_t		mVarPosition;
	vmstring	mVarType;
	vmstring	mVarTypeRefer;
};



// ********************************************************************
//! 静的変数
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	variable_static : node<Compiler>, Traits
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

	variable_static(vmstring _type, vmstring _symbol)
		: mVarSymbol(std::move(_symbol))
		, mVarType(std::move(_type))
	{}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		amtrs_string_view	varname;
		amtrs_string_view	className	= passion_namespace_parent_namespace_and_current(&varname, mVarSymbol);
		_asmblr.add_lea_global(vmvariable_entry{_cmplr.find_class(className), varname});
		// 参照型の変数なので常に開いておく
		if (passion_is_reference(extract_typename(_cmplr, mVarType)))
		{
			_asmblr.add_load_adr();
		}
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		mVarTypeRefer	= add_reference(_cmplr, mVarType);
		return	phase_type::end;
	}


	virtual vmstring on_type() const override { return mVarTypeRefer; }


private:
	vmstring	mVarSymbol;
	vmstring	mVarType;
	vmstring	mVarTypeRefer;
};




// ********************************************************************
//! 動的メンバ変数／関数
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	dynamic_member : node<Compiler>, Traits
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
		function_start,
		function_end,
	};


	dynamic_member(node_type* _object_scope)
		: mObjectScope(std::move(_object_scope))
	{}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		if (mFunctionAddress)
		{
			_asmblr.add_load_function(mScopeClass->find_function(mMemberName));
			return;
		}

		if (mFunctionNode)
		{
			mFunctionNode->compile(_cmplr, _asmblr);
		}
		else if (passion_is_gcpointer(passion_trim_reference(mScopeType)))
		{
			vmclass*	clz			= _cmplr.find_class(passion_trim_gcpointer(passion_trim_reference(mScopeType)));

			// Load object scope.
			mObjectScope->compile(_cmplr, _asmblr);
			if (passion_is_reference(mScopeType))
			{
				_asmblr.add_load(passion_trim_reference(mScopeType));
			}

			if (mVariable)
			{
				// Load member address.
				_asmblr.add_lea_object(vmvariable_entry{clz, mVariable->name, vmstring(_cmplr.thizClass)});
			}
		}
		else
		{
			_asmblr.add_load_function(mScopeClass->find_function(mMemberName));
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				mMemberName	= _token;
				mScopeType	= extract_typename(_cmplr, mObjectScope->type());
				if (passion_is_gcpointer(passion_trim_reference(mScopeType)))
				{
					mScopeClass		= _cmplr.find_class(passion_trim_gcpointer(passion_trim_reference(mScopeType)));
				}
				else
				{
					mScopeClass		= _cmplr.find_class(mScopeType);
				}
				if (auto info = mScopeClass->get_variable_info(mMemberName); info.second)
				{
					mVariable	= info.second;
					mVarType	= extract_typename(_cmplr, mVariable->type);
					if (!passion_is_reference(mVarType))
					{
						mVarType = add_reference(_cmplr, mVarType);
					}
					return	phase_type::end;
				}
				if (mScopeClass->find_function(mMemberName))
				{
					return	phase_type::function_start;
				}
				PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), member_not_found, mMemberName.c_str(), mScopeClass->name().c_str());
			}

			case phase_type::function_start :
			{
				if (_token != "(")
				{
					// 関数が確定出来ない
					if (mScopeClass->count_function(mMemberName) != 1)
					{
						vmstring	name	= vmstring(mScopeClass->name()) + "::" + mMemberName;
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), function_did_arguments, name.c_str());
					}

					// 関数ポインタを返す
					auto	f	= mScopeClass->find_function(mMemberName);
					mStatics			= f->is_static();
					mFunctionAddress	= true;
					mVarType = f->type();
					return	phase_type::pass;
				}

				mFunctionNode	= this->template create<function_caller<compiler_type>>(
										this->get_value_type(),
										mObjectScope,
										mScopeClass->name(),
										mMemberName);
				return	{ phase_type::function_end, mFunctionNode->lex(_cmplr, _token) };
			}

			case phase_type::function_end :
			{
				mVarType	= mFunctionNode->type();
				return	phase_type::pass;
			}

			default :
				return	phase_type::end;
		}
	}


	virtual vmstring on_type() const override { return mVarType; }


private:
	ref<node_type>							mObjectScope;
	vmclass*								mScopeClass			= nullptr;
	vmstring								mMemberName;
	vmstring								mScopeType;
	bool									mStatics			= false;
	bool									mFunctionAddress	= false;

	vmstring								mVarType;

	// Variable mode.
	vmclass::declere_value const*			mVariable			= nullptr;

	// Function mode.
	ref<function_caller<Compiler>>	mFunctionNode;
};



// ********************************************************************
//! スコープを扱う。
// --------------------------------------------------------------------
//! このノードは名前空間を解決するためだけに存在する。
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	variable_scope : node<Compiler>, Traits
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

	variable_scope(vmstring _scope)
		: mScope(_scope)
	{}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		// 何もしない
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		return	phase_type::end;
	}


	virtual vmstring on_type() const override { return mScope; }


private:
	vmstring	mScope;
};



// ********************************************************************
//!	this
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	variable_this : node<Compiler>, Traits
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

	variable_this(vmstring _classname)
		: mClassName(std::move(_classname))
	{}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_load_this();
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		return	phase_type::end;
	}


	virtual vmstring on_type() const override { return mClassName + "^"; }


private:
	vmstring	mClassName;
};



// ********************************************************************
//!	@brief ローカル変数の定義
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	local_alloc : node<Compiler>, Traits
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
		variable_name,
		assign_operator,
	};

	local_alloc() = default;

	// 型名を指定して初期化する。
	local_alloc(vmstring _typename)
		: mTypename(_typename)
	{}



	virtual vmstring on_type() const override { return mTypename; }
	virtual compute_type on_compute_type() const noexcept override { return compute_type::none; }


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		if (mAssginRightValue.empty())
		{
			_asmblr.add_alloc(1);
		}
		else
		{
			mAssginRightValue->compile(_cmplr, _asmblr);

			auto	type	= extract_typename(_cmplr, mTypename);
			auto	rtype	= extract_typename(_cmplr, mAssginRightValue->type());
			if (passion_is_pointer(type) && passion_is_reference(rtype))
			{
				_asmblr.add_load_adr(_cmplr.find_class(passion_remove_reference(rtype).remove_pointer()), _cmplr.find_class(passion_remove_reference(type).remove_pointer()));
			}
			else if (!passion_is_reference(type) && passion_is_reference(rtype))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(rtype).remove_pointer()), _cmplr.find_class(passion_remove_reference(type).remove_pointer()));
			}
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			case phase_type::variable_name :
			{
				if (_token != Token::COMMAND)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), expected_unqualified_id, _token.str().c_str());
				}
				mVariableName	= _token.str();
				return	phase_type::assign_operator;
			}

			case phase_type::assign_operator :
			{
				auto	type	= extract_typename(_cmplr, mTypename);
				if (_token == ";")
				{
					if (passion_is_reference(type))
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), declaration_of_reference_variable, mVariableName.c_str());
					}
					_cmplr.local_allocate({mTypename, mVariableName});
					return	phase_type::pass;
				}
				if (_token != "=")
				{
					return	phase_type::pass;
				}
				mAssginRightValue	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::end,
					mAssginRightValue
				};
			}

			case phase_type::end :
			{
				if (passion_is_reference(mTypename) && !passion_is_reference(mAssginRightValue->type()))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), declaration_of_reference_variable_no_variable, mVariableName.c_str());
				}

				if (mTypename == "auto")
				{
					mTypename	= mAssginRightValue->type();
				}
				else if (!is_assign_type(_cmplr, mTypename, mAssginRightValue->type()))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), err_can_not_assign_type, mTypename.c_str(), mAssginRightValue->type().c_str())
				}
				_cmplr.local_allocate({mTypename, mVariableName});
				return	phase_type::pass;
			}

			default :
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>						mAssginRightValue;
	vmstring							mTypename;
	vmstring							mVariableName;
};




// ********************************************************************
//! decltype(式)
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	specifier_decltype : node<Compiler>, Traits
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
		open,
		value,
		close,
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("decltype");
	}


	virtual vmstring on_type() const override
	{
		return	mTypename;
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				return	phase_type::open;
			}

			case phase_type::open :
			{
				if (_token != "(")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), excepted_after, "(", "decltype");
				}
				
				return	phase_type::value;
			}

			case phase_type::value :
			{
				mValueNode	= this->template create_if<statement<Compiler>>(_cmplr, _token);
				if (!mValueNode)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), excepted_expression, "decltype");
				}
				return	{ phase_type::close, mValueNode->lex(_cmplr, _token) };
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), excepted, ")");
				}
				mTypename	= passion_trim_reference(mValueNode->type());
				mValueNode.clear();
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmstring		mTypename;
	ref<node_type>	mValueNode;
};




// ********************************************************************
//!	@brief 整数値
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	literal_integer : node<Compiler>, Traits
{
	using	traits_type		= Traits;
	using	node_type		= typename traits_type::node_type;
	using	compiler_type	= typename traits_type::compiler_type;
	using	input_type		= typename traits_type::input_type;


	enum class	prefix_type
	{
		none,
		minus,
		plus
	};

	enum class	phase_type
	{
		start		= node_type::phase_start,
		end			= node_type::phase_end,
		pass		= node_type::phase_pass,
		integer,
	};

	literal_integer(){}
	literal_integer(vmint_t _n) : mValue(_n) {}

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		if (_token != Token::NUMBER && _token != "-" && _token != "+")
		{
			return	false;
		}
		return	true;
	}


	virtual compute_type on_compute_type() const noexcept override
	{
		return	compute_type::static_integer;
	}

	virtual vmvariant on_constexpr_value() const noexcept override
	{
		return	vmvariant(mValue);
	}

	virtual vmstring on_type() const override
	{
		return	mTypename;
	}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_load_int(mValue);
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				if (_token == "-")
				{
					mPrefix	= prefix_type::minus;
					return	phase_type::integer;
				}
				else if (_token == "+")
				{
					mPrefix	= prefix_type::plus;
					return	phase_type::integer;
				}
			}

			case phase_type::integer :
			{
				if (_token != Token::NUMBER)
				{
					THROW_ERROR(_dummy_, _token);
				}
				mValue		= _token.number();
				if (mPrefix == prefix_type::minus)
				{
					mValue	= -mValue;
				}
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmint_t			mValue		= 0;
	vmstring		mTypename	= "int";
	prefix_type		mPrefix		= prefix_type::none;

};




// ********************************************************************
//!	@brief 文字列値
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	literal_string : node<Compiler>, Traits
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
		strvalue,
	};

	literal_string(){}
	literal_string(const vmstring& _n) : mValue(_n) {}

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		if (_token != Token::STRING)
		{
			return	false;
		}
		return	true;
	}

	virtual compute_type on_compute_type() const noexcept override
	{
//		return	compute_type::static_string;
		return	compute_type::dynamics;
	}

	virtual vmvariant on_constexpr_value() const noexcept override
	{
//		return	vmvariant(vmstring(mValue));
		return	{};
	}

	virtual vmstring on_type() const override
	{
		return	mTypename;
	}

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_load_string(mValue);
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			case phase_type::strvalue :
			{
				if (_token != Token::STRING)
				{
					THROW_ERROR(_dummy_, _token);
				}
				mValue		= _token.str();
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmstring		mValue;
	vmstring		mTypename	= "string";
};



// ****************************************************************************
//! true, false
// ----------------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	literal_boolean : node<Compiler>, Traits
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

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token == "true" || _token == "false";
	}


	virtual compute_type on_compute_type() const noexcept override
	{
		return	compute_type::dynamics;
	}

	virtual vmvariant on_constexpr_value() const noexcept override
	{
		return	{};
	}

	virtual vmstring on_type() const override
	{
		return	"bool";
	}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_load_int(mValue ? 1 : 0);
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				mValue	= _token == "true" ;
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	bool		mValue;
};




// ****************************************************************************
//! nullptr型
// ----------------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	literal_nullptr : node<Compiler>, Traits
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

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token == "nullptr";
	}


	virtual compute_type on_compute_type() const noexcept override
	{
		return	compute_type::dynamics;
	}

	virtual vmvariant on_constexpr_value() const noexcept override
	{
		return	{};
	}

	virtual vmstring on_type() const override
	{
		return	"nullptr_t";
	}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		_asmblr.add_load_int(0);
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}
};



// ********************************************************************
//!	operator []
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	operator_arraysubscript : node<Compiler>, Traits
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
		close,
	};


	operator_arraysubscript(node_type* _array)
		: mArrayNode(std::move(_array))
	{}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		mArrayNode->compile(_cmplr, _asmblr);
		if (passion_is_reference(mArrayNode->type()))
		{
			_asmblr.add_load_adr();
		}
		_asmblr.add_larray();
		mIndexNode->compile(_cmplr, _asmblr);
		if (passion_is_reference(mIndexNode->type()))
		{
			_asmblr.add_load(passion_trim_reference(mIndexNode->type()));
		}
		_asmblr.add_lea(sizeof(void*), 0);
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				mIndexNode	= this->template create<statement<compiler_type>>(value_mode::rvalue);
				return	{phase_type::close, mIndexNode};
			}

			case phase_type::close :
			{
				if (_token != "]")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), err_array_size_not_closed, _token.c_str());
				}
				mVarType	= vmstring(passion_trim_array_last_dimention(passion_trim_reference(mArrayNode->type()))) + "&";
				return	phase_type::end;
			}

			default :
				return	phase_type::end;
		}
	}


	virtual vmstring on_type() const override { return mVarType; }


private:
	vmstring						mVarType;
	ref<node_type>					mArrayNode;
	ref<node_type>					mIndexNode;
};


// ********************************************************************
//!	++演算子, --演算子
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	operator_before_incremental : node<Compiler>, Traits
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


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return	(_token == "++") || (_token == "--");
	}

	virtual compute_type on_compute_type() const noexcept override
	{
		return	compute_type::dynamics;
	}

	virtual vmvariant on_constexpr_value() const noexcept override
	{
		return	{};
	}

	virtual vmstring on_type() const override
	{
		return	mVarType;
	}

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		auto		clz		= _cmplr.find_class(passion_remove_reference(mValue->type()));
		mValue->compile(_cmplr, _asmblr);
		if (this->get_value_type() == value_mode::rvalue)
		{
			_asmblr.add_dup_adr();
		}
		_asmblr.add_load_int(1);
		if (!mOpeFunction.first)
		{
			// 直接加減算できる。
			if (mDecriment)
			{
				_asmblr.add_dec(clz);
			}
			else
			{
				_asmblr.add_inc(clz);
			}
		}
		else
		{
			_asmblr.add_alloc(1);
			_asmblr.add_call(mOpeFunction.second, 0);
			if (this->get_value_type() != value_mode::rvalue)
			{
				_asmblr.add_free(1);
			}
		}
	}

	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				mDecriment	= _token == "--";
				mValue	= this->template create<statement<compiler_type>>(value_mode::rvalue);
				return	{ phase_type::end, mValue };
			}

			case phase_type::end :
			{
				auto	lvtype	= mValue->type();
				if (!passion_is_reference(lvtype))
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _token.location(), expression_is_not_assignable, "");
				}
				mVarType	= lvtype;

				// オブジェクト型なら operator ++ を検索する
				auto	clz		= _cmplr.find_class(passion_remove_reference(mValue->type()));
				if (passion_is_gcpointerable(clz->name()))
				{
					mOpeFunction	= _cmplr.find_operator("++", clz);
					if (!mOpeFunction.first)
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), not_found_operator, clz->identify().c_str(), "++", "");
					}
					mVarType		= mOpeFunction.second->result_type();
				}
				return	phase_type::pass;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>						mValue;
	bool								mDecriment		= false;
	std::pair<vmclass*, vmfunction*>	mOpeFunction	= { nullptr, nullptr };
	vmstring							mVarType;
};



// ********************************************************************
//!	operator +, operator -, ... 二項演算子
// --------------------------------------------------------------------
template<class Compiler,  class Traits = expression_traits<Compiler>>
struct	operator_binary : node<Compiler>, Traits
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
	};

	enum class	operator_type
	{
		assign,
		add,
		sub,
		mul,
		div,
		mod,
		add_assign,
		sub_assign,
		mul_assign,
		div_assign,
		mod_assign,
		equals,
		not_equals,
		grater,
		grater_equals,
		letter,
		letter_equals,
		logic_and,
		logic_or,
		unkown
	};

	enum class	compute_mode
	{
		basic,				//!< 通常の演算
		pointer,			//!< ポインタの演算
		functional,			//!< operator function による演算
	};


	operator_binary(vmstring _operator, node_type* _destinate, node_type* _source)
		: mDestinate(_destinate)
		, mSource(_source)
		, mOperator(get_operator_type(_operator))
		, mOperatorString(std::move(_operator))
	{
		mVarType	= mDestinate->type();
		if (mOperator != operator_type::assign)
		{
			mVarType	= vmstring(passion_trim_reference(mVarType));
		}
	}

	static bool is_assignable(operator_type _operator)
	{
		return	_operator == operator_type::assign
			||	_operator == operator_type::add_assign
			||	_operator == operator_type::sub_assign
			||	_operator == operator_type::mul_assign
			||	_operator == operator_type::div_assign
			||	_operator == operator_type::mod_assign;
	}

	static bool is_compaund_assign(operator_type _operator)
	{
		return	_operator == operator_type::add_assign
			||	_operator == operator_type::sub_assign
			||	_operator == operator_type::mul_assign
			||	_operator == operator_type::div_assign
			||	_operator == operator_type::mod_assign;
	}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		if (mComputeMode == compute_mode::basic)
		{
			// 第１オペランドはアサイン演算子の時のみロードしない
			mDestinate->compile(_cmplr, _asmblr);
			if (is_compaund_assign(mOperator))
			{
				_asmblr.add_dup_adr();
			}
			if ((mOperator != operator_type::assign) && passion_is_reference(mDestType))
			{
				_asmblr.add_load(vmstring(passion_trim_reference(mDestType)));
			}


			// 第２オペランドは常にロード
			mSource->compile(_cmplr, _asmblr);
			if (passion_is_reference(mSrcType))
			{
				_asmblr.add_load(vmstring(passion_trim_reference(mSrcType)));
			}
			switch (mOperator)
			{
				case operator_type::add :
				case operator_type::add_assign :
						_asmblr.add_add();		break;
				case operator_type::sub :
				case operator_type::sub_assign :
						_asmblr.add_sub();		break;
				case operator_type::mul :
				case operator_type::mul_assign :
						_asmblr.add_mul();		break;
				case operator_type::div :
				case operator_type::div_assign :
						_asmblr.add_div();		break;
				case operator_type::mod :
				case operator_type::mod_assign :
						_asmblr.add_mod();		break;
				case operator_type::equals :		_asmblr.add_cmpe();		break;
				case operator_type::not_equals :	_asmblr.add_cmpn();		break;
				case operator_type::grater :		_asmblr.add_cmpl();		break;
				case operator_type::grater_equals :	_asmblr.add_cmple();	break;
				case operator_type::letter :		_asmblr.add_cmps();		break;
				case operator_type::letter_equals :	_asmblr.add_cmpse();	break;
				case operator_type::logic_and :		_asmblr.add_land();		break;
				case operator_type::logic_or :		_asmblr.add_lor();		break;
				default:							break;
			}
			if (is_assignable(mOperator))
			{
				_asmblr.add_store(vmstring(passion_trim_reference(mDestType)), this->get_value_type() == value_mode::rvalue);
			}
		}
		else if (mComputeMode == compute_mode::functional)
		{
			// 関数による演算
			mFunction->compile(_cmplr, _asmblr);
		}
	}



	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				vmstring	types[2]	= {
								mDestType = vmstring(extract_typename(_cmplr, mDestinate->type())),
								mSrcType  = vmstring(extract_typename(_cmplr, mSource->type()))
							};
				if (mOperator == operator_type::assign)
				{
//					if (vmstring(passion_trim_reference(mDestType)) == vmstring(passion_trim_reference(mSrcType)))
					if (is_assign_type(_cmplr, passion_trim_reference(mDestType), passion_trim_reference(mSrcType)))
					{
						mComputeMode	= compute_mode::basic;
						return	phase_type::end;
					}
				}
				else
				{
					// primitive type + primitive type
					if (passion_size_of_primitive(passion_trim_reference(mDestType)) && passion_size_of_primitive(passion_trim_reference(mSrcType)))
					{
						mComputeMode	= compute_mode::basic;
						return	phase_type::end;
					}

					// pointer + integer
					if ((mOperator == operator_type::add || mOperator == operator_type::sub) && passion_is_pointer(types[0]) && passion_is_integer(types[1]))
					{
						mComputeMode	= compute_mode::pointer;
						return	phase_type::end;
					}
				}

				// operator function
				vmstring	funcname	= "operator" + mOperatorString;
				vmstring	clazzName	= passion_trim_gcpointer(passion_trim_reference(types[0]));
				auto		match		= _cmplr.find_function({_cmplr.find_class(clazzName)}, funcname, &types[1], 1);
				if (!match.second)
				{
					match	= _cmplr.find_function({_cmplr.find_class("")}, funcname, types, 2);
					if (!match.second)
					{
						PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), not_found_operator, clazzName.c_str(), mOperatorString.c_str(), types[1].c_str());
					}
				}
				mVarType		= vmstring(match.second->result_type());
				mComputeMode	= compute_mode::functional;

				if (match.second->is_thisable())
				{
					ref<node_type>	tp[1] = {mSource};
					mFunction		= this->template create<function_caller<compiler_type>>(
											value_mode::rvalue,
											match.second,
											mDestinate,
											this->template create<parameters<compiler_type>>(value_mode::rvalue, std::begin(tp), std::end(tp))
									);
				}
				else
				{
					ref<node_type>	tp[2] = {mDestinate, mSource};
					mFunction		= this->template create<function_caller<compiler_type>>(
											value_mode::rvalue,
											match.second,
											nullptr,
											this->template create<parameters<compiler_type>>(value_mode::rvalue, std::begin(tp), std::end(tp))
									);
				}
				return	phase_type::end;
			}

			default :
				return	phase_type::end;
		}
	}


	virtual vmstring on_type() const override { return mVarType; }


	static bool is_binary_operator(const vmstring& _operator)
	{
		return	get_operator_type(_operator) != operator_type::unkown;
	}

	static operator_type get_operator_type(const vmstring& _operator)
	{
		return	(_operator == "=")	? operator_type::assign
			// Alithmetics
			:	(_operator == "+")	? operator_type::add
			:	(_operator == "-")	? operator_type::sub
			:	(_operator == "*")	? operator_type::mul
			:	(_operator == "/")	? operator_type::div
			:	(_operator == "%")	? operator_type::mod
			// Compound Assignment
			:	(_operator == "+=")	? operator_type::add_assign
			:	(_operator == "-=")	? operator_type::sub_assign
			:	(_operator == "*=")	? operator_type::mul_assign
			:	(_operator == "/=")	? operator_type::div_assign
			:	(_operator == "%=")	? operator_type::mod_assign
			// Compare
			:	(_operator == "==")	? operator_type::equals
			:	(_operator == "!=")	? operator_type::not_equals
			:	(_operator == ">")	? operator_type::grater
			:	(_operator == ">=")	? operator_type::grater_equals
			:	(_operator == "<")	? operator_type::letter
			:	(_operator == "<=")	? operator_type::letter_equals
			// Logical compare
			:	(_operator == "||")	? operator_type::logic_and
			:	(_operator == "&&")	? operator_type::logic_or
			:	operator_type::unkown;
	}

	static opecode_priority get_priority(operator_type _operator)
	{
		switch (_operator)
		{
			case operator_type::assign :		return	opecode_priority::assign;
			case operator_type::add :			return	opecode_priority::add_sub;
			case operator_type::sub :			return	opecode_priority::add_sub;
			case operator_type::mul :			return	opecode_priority::mul_div;
			case operator_type::div :			return	opecode_priority::mul_div;
			case operator_type::mod :			return	opecode_priority::mul_div;
			case operator_type::add_assign :	return	opecode_priority::assign;
			case operator_type::sub_assign :	return	opecode_priority::assign;
			case operator_type::mul_assign :	return	opecode_priority::assign;
			case operator_type::div_assign :	return	opecode_priority::assign;
			case operator_type::mod_assign :	return	opecode_priority::assign;
			case operator_type::equals :		return	opecode_priority::compare;
			case operator_type::not_equals :	return	opecode_priority::compare;
			case operator_type::grater :		return	opecode_priority::compare;
			case operator_type::grater_equals :	return	opecode_priority::compare;
			case operator_type::letter :		return	opecode_priority::compare;
			case operator_type::letter_equals :	return	opecode_priority::compare;
			case operator_type::logic_and :		return	opecode_priority::logic_and;
			case operator_type::logic_or :		return	opecode_priority::logic_and;
			default:							return	opecode_priority::none;
		}
	}

	static opecode_priority get_priority(const vmstring& _operator)
	{
		return	get_priority(get_operator_type(_operator));
	}

private:
	ref<node_type>			mDestinate;
	ref<node_type>			mSource;
	vmstring				mVarType;
	operator_type			mOperator;
	vmstring				mOperatorString;
	compute_mode			mComputeMode;
	ref<node_type>			mFunction;

	vmstring				mDestType;
	vmstring				mSrcType;
};



// ********************************************************************
//!	条件演算子  conditions ? true : false
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	operator_conditional : node<Compiler>, Traits
{
	using	traits_type		= Traits;
	using	node_type		= typename traits_type::node_type;
	using	compiler_type	= typename traits_type::compiler_type;
	using	input_type		= typename traits_type::input_type;

	//! 式の解析フェーズ
	enum class phase_type
	{
		start		= node_type::phase_start,
		end			= node_type::phase_end,
		pass		= node_type::phase_pass,
		opecode,
		true_value,
		separator,
		false_value,
	};

	//! 演算子の優先順位を指定
	static constexpr opecode_priority	priority	= opecode_priority::conditions;


	//! 解析開始に使用するトークン
	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("?");
	}


	// ========================================================================
	//! コンストラクタ
	// ------------------------------------------------------------------------
	//! 先行して解析した条件式を示すノードを渡してください。
	// ------------------------------------------------------------------------
	operator_conditional(node_type* _conditions)
		: mConditions(_conditions)
	{}



	virtual compute_type on_compute_type() const noexcept override
	{
		return	mAnswer ? mAnswer->get_compute_type() : mTrueStatement->get_compute_type();
	}

	virtual vmvariant on_constexpr_value() const noexcept override
	{
		return	mAnswer ? mAnswer->get_static_variable() : mTrueStatement->get_static_variable();
	}

	virtual vmstring on_type() const override
	{
		return	mAnswer ? mAnswer->type() : mTrueStatement->type();
	}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		if (mAnswer)
		{
			mAnswer->compile(_cmplr, _asmblr);
		}
		else
		{
			auto	labelFalse	= make_ref<_brunch_point>();
			auto	labelEnd	= make_ref<_brunch_point>();
			mConditions->compile(_cmplr, _asmblr);
			if (passion_is_reference(mConditions->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mConditions->type())));
			}
			_asmblr.add_jfalse(labelFalse);
			mTrueStatement->compile(_cmplr, _asmblr);
			if (passion_is_reference(mTrueStatement->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mTrueStatement->type())));
			}
			_asmblr.add_jmp(labelEnd);
			_asmblr.insert_label(labelFalse);
			mFalseStatement->compile(_cmplr, _asmblr);
			if (passion_is_reference(mFalseStatement->type()))
			{
				_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mFalseStatement->type())));
			}
			_asmblr.insert_label(labelEnd);
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			case phase_type::opecode :
			{
				return	phase_type::true_value;
			}

			case phase_type::true_value :
			{
				mTrueStatement	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{ phase_type::separator, mTrueStatement->lex(_cmplr, _token) };
			}

			case phase_type::separator :
			{
				if (_token != ":")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::false_value;
			}

			case phase_type::false_value :
			{
				mFalseStatement	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{ phase_type::end, mFalseStatement->lex(_cmplr, _token) };
			}

			case phase_type::end :
			{
				if (mTrueStatement->type() != mFalseStatement->type())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), condition_type_unmatch, mTrueStatement->type().c_str(), mFalseStatement->type().c_str())
				}
#if	0
				if (mConditions->get_compute_type() == compute_type::static_integer)
				{
					// 静的解決
					mAnswer	= std::get<vmvalue>(mConditions->get_static_variable()).i == 0
							? mFalseStatement
							: mTrueStatement;
					mConditions	= nullptr;
					mTrueStatement	= nullptr;
					mFalseStatement	= nullptr;
				}
				else if (mConditions->get_compute_type() == compute_type::static_string)
				{
					// 静的解決
					auto	tx	= removeQuotation(std::get<vmstring>(mConditions->get_static_variable()));
					mAnswer	= tx.empty()
							? mFalseStatement
							: mTrueStatement;
					mConditions	= nullptr;
					mTrueStatement	= nullptr;
					mFalseStatement	= nullptr;
				}
#endif
				return	phase_type::pass;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>		mConditions;
	ref<node_type>		mTrueStatement;
	ref<node_type>		mFalseStatement;
	ref<node_type>		mAnswer;
};




// ********************************************************************
//!	operator * ()
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	operator_dereference : node<Compiler>, Traits
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


	virtual vmstring on_type() const override { return mTypename; }


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		mOperand->compile(_cmplr, _asmblr);
		if (passion_is_reference(mTypename))
		{
			_asmblr.add_load_adr();
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				mOperand	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{ phase_type::end, mOperand };
			}

			case phase_type::end :
			{
				auto	t	= vmstring(passion_trim_reference(mOperand->type()));
				if (!passion_is_pointer(t))
				{
				}
				mTypename	= vmstring(passion_trim_pointer(t));
				if (passion_is_reference(mOperand->type()))
				{
					mTypename	+= "&";
				}
				return	phase_type::pass;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmstring		mTypename;
	ref<node_type>	mOperand;
};



// ********************************************************************
//!	new operator
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	operator_new : node<Compiler>, Traits
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
		type_name,				//!< 型名の読み取り

		check_gcpointer,		//!< ^

		open_bracket,			//!< [ もしくは ( の読み取り
		array_size,				//!< 配列のサイズの読み取り
		close_array,			//!< 配列を閉じる部分の読み取り
		array_dimentions_open,	//!< 多次元配列用
		array_dimentions_close,	//!< 多次元配列用

		parameters_close,				//!< 引数読み取り
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("new");
	}


	//! この式/文が示すデータの型
	virtual vmstring on_type() const override
	{
		return	mTypename;
	}

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		if (passion_is_array(mTypename))
		{
			// size
			auto	t	= passion_trim_array_last_dimention(mTypename);
			if (passion_is_gcpointerable(t))
			{
				// 多次元配列もしくはオブジェクトの配列
				mArraySize->compile(_cmplr, _asmblr);
				_asmblr.add_new_arrayref();
			}
			else
			{
				// プリミティブの配列
				auto*	c	= _cmplr.find_class(vmstring(t));
				auto	sz	= c->struct_size();
				_asmblr.add_load_int((int)passion_variable_size_compute(sz));
				mArraySize->compile(_cmplr, _asmblr);
				_asmblr.add_new_array();
			}
		}
		else
		{
			vmstring	type	= vmstring(mTypename.substr(0, mTypename.size() - 1));
			auto*		clz		= _cmplr.find_class(type);
			if (!clz)
			{
				THROW_COMPILER_ERROR();
			}

			auto		acss	= _cmplr.access_permitted(_cmplr.find_class(type)->identify(), _cmplr.clazz);
			if (acss.first != attribute_flag::public_attribute)
			{
//				PSNCMPLR_SYNTAX_ERROR(_vm, _cmplr.location(), member_access_denay, clz->name().c_str(), (clz->access == attribute_flag::private_attribute ? "private" : "protected"), _cmplr.clazz.c_str());
			}


			// コンストラクタを検索
			vmfunction*	func	= nullptr;
			for (auto f : clz->functions())
			{
				if (f->is_constractor())
				{
					if (!mArguments.empty() && mArguments->nodes().size() == f->parameter_size())
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
				_cmplr.add_function_call(_asmblr, clz, func, mArguments->nodes().data(), mArguments->nodes().size());
			}
		}
		if (this->get_value_type() != value_mode::rvalue)
		{
			_asmblr.add_free(1);
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				return	phase_type::type_name;
			}


			case phase_type::type_name :
			{
				if (_token != Token::COMMAND)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), unkown_type_name, _token.str().c_str());
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
					mArguments		= this->template create<parameters<Compiler>>(value_mode::rvalue);
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
				if (auto r = normalize_typename(_cmplr); r.phase)
				{
					return	r;
				}
				return	phase_type::pass;
			}


			case phase_type::array_size :
			{
				mArraySize	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::close_array,
					mArraySize->lex(_cmplr, _token)
				};
			}


			case phase_type::close_array :
			{
				if (_token != "]")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), unkown_type_name, _token.str().c_str());
				}
				mTypename += "[]";
				return	phase_type::array_dimentions_open;
			}


			case phase_type::array_dimentions_open :
			{
				if (_token != "[")
				{
					if (auto r = normalize_typename(_cmplr); r.phase)
					{
						return	r;
					}
					return	phase_type::pass;
				}
				return	phase_type::array_dimentions_close;
			}


			case phase_type::array_dimentions_close :
			{
				if (_token != "]")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), unkown_type_name, _token.str().c_str());
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
				if (auto r = normalize_typename(_cmplr); r.phase)
				{
					return	r;
				}
				return	phase_type::end;
			}


			default:
			{
				return	phase_type::pass;
			}
		}
	}


private:
	parser_result<compiler_type> normalize_typename(compiler_type& _cmplr)
	{
		if (!passion_is_array(mTypename))
		{
			mTypename	= _cmplr.normalize_class_path(mTypename);
			if (!_cmplr.find_class(mTypename))
			{
				PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), unkown_type_name, mTypename.c_str());
			}
			mTypename	+= "^";
		}
		return	0;
	}


	vmstring						mTypename;
	ref<parameters<Compiler>>		mArguments;
	ref<statement<Compiler>>		mArraySize;
};



// ********************************************************************
//!	sizeof()
// --------------------------------------------------------------------
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	operator_sizeof : node<Compiler>, Traits
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
		keyword,
		variadic,
		open,
		name,
		close,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("sizeof");
	}


	virtual vmstring on_type() const override { return mTypename; }


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::keyword :
			{
				return	phase_type::variadic;
			}

			case phase_type::variadic :
			{
				if (_token != "...")
				{
					// 現状では可変引数以外には利用できない
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::open;
			}

			case phase_type::open :
			{
				if (_token != "(")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::name;
			}

			case phase_type::name :
			{
				if (_token != "...")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::close;
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					THROW_ERROR(err_not_close_parenthesis, _token);
				}
				return	phase_type::end;
			}

			default:
			{
				return	phase_type::end;
			}
		}
	}

private:
	vmstring		mTypename;
};



// ********************************************************************
//!	@brief !expression
// --------------------------------------------------------------------
//!	
// ********************************************************************
template<class Compiler, class Traits = expression_traits<Compiler>>
struct	operator_to_address : node<Compiler>, Traits
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
		keyword,
		value,
		value_end,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view(PASSION_TYPENAME_REFERENCE_SUFFIX);
	}



	virtual compute_type on_compute_type() const noexcept override
	{
		return	mRValue->get_compute_type();
	}

	virtual vmvariant on_constexpr_value() const noexcept override
	{
		return	{};
	}

	virtual vmstring on_type() const override
	{
		return	mTypename;
	}

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		mRValue->compile(_cmplr, _asmblr);
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			case phase_type::keyword :
			{
				this->set_priority(opecode_priority::single_operation);
				return	phase_type::value;
			}

			case phase_type::value :
			{
				mRValue		= this->template create<statement<Compiler>>(value_mode::rvalue);
				mRValue->set_priority(opecode_priority::single_operation);
				return	{ phase_type::value_end, mRValue->lex(_cmplr, _token) };
			}

			case phase_type::value_end :
			{
				mTypename	= vmstring(passion_trim_reference(mRValue->type())) + PASSION_TYPENAME_POINTER_SUFFIX;
				return	phase_type::pass;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>		mRValue;
	vmstring			mTypename;
};




template<class Compiler, class Traits = expression_traits<Compiler>>
struct	ctrlflow_break : node<Compiler>, Traits
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
		value,
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("break");
	}

	virtual vmstring on_type() const override { return mResult->type(); }

	virtual compute_type on_compute_type() const noexcept override { return compute_type::end; }


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		auto&	block	= _cmplr.current_block();
		if (!block.breakLabel)
		{
//			PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), break_not_in, "break");
		}
		_asmblr.add_jmp(block.breakLabel);
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				return	phase_type::value;
			}

			case phase_type::value :
			{
				if (_token == ";")
				{
					return	phase_type::end;
				}
				mResult	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::end,
					mResult->lex(_cmplr, _token)
				};
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>	mResult;
};




template<class Compiler, class Traits = expression_traits<Compiler>>
struct	ctrlflow_continue : node<Compiler>, Traits
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
		value,
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("continue");
	}

	virtual vmstring on_type() const override { return mResult->type(); }

	virtual compute_type on_compute_type() const noexcept override { return compute_type::end; }

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		auto&	block	= _cmplr.current_block();
		if (!block.continueLabel)
		{
//			PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), continue_not_in, "continue");
		}
		_asmblr.add_jmp(block.continueLabel);
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				return	phase_type::value;
			}

			case phase_type::value :
			{
				if (_token == ";")
				{
					return	phase_type::end;
				}
				mResult	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::end,
					mResult->lex(_cmplr, _token)
				};
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>	mResult;
};





template<class Compiler, class Traits = expression_traits<Compiler>>
struct	ctrlflow_do_while : node<Compiler>, Traits
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
		loopblock,
		while_keyword,
		conditions_start,
		conditions_end,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("do");
	}

	virtual vmstring on_type() const override { return mTypename; }
	virtual compute_type on_compute_type() const noexcept override { return compute_type::none; }


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		auto&	blk				= _cmplr.block_begin();
		blk.continueLabel	= make_ref<_brunch_point>();
		blk.breakLabel		= make_ref<_brunch_point>();

		_asmblr.insert_label(blk.continueLabel);

		mLoopBlock->compile(_cmplr, _asmblr);
		mConditions->compile(_cmplr, _asmblr);
		if (passion_is_gcpointer(mConditions->type()))
		{
			_asmblr.add_ref2int();
		}
		if (passion_is_reference(mConditions->type()))
		{
			auto		clz		= _cmplr.find_class(passion_remove_reference(mConditions->type()));
			_asmblr.add_load(clz);
		}
		_asmblr.add_jtrue(blk.continueLabel);

		_asmblr.insert_label(blk.breakLabel);
		_cmplr.block_end();
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				return	phase_type::loopblock;
			}

			case phase_type::loopblock :
			{
				mLoopBlock		= this->template create<codeblock<Compiler>>(value_mode::void_mode);
				return	{
					phase_type::while_keyword,
					mLoopBlock->lex(_cmplr, _token)
				};
			}

			case phase_type::while_keyword :
			{
				if (_token != "while")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::conditions_start;
			}

			case phase_type::conditions_start :
			{
				if (_token != "(")
				{
					THROW_ERROR(unkown_token, _token);
				}

				mConditions	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{ phase_type::conditions_end, mConditions };
			}

			case phase_type::conditions_end :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::end;
			}

			default :
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmstring		mTypename;
	ref<node_type>	mConditions;
	ref<node_type>	mLoopBlock;
};




template<class Compiler, class Traits = expression_traits<Compiler>>
struct	ctrlflow_for : node<Compiler>, Traits
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
		open,
		init,
		init_end,
		conditions,
		conditions_end,
		next,
		close,
		loop_block,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("for");
	}

	virtual vmstring on_type() const override { return mTypename; }
	virtual compute_type on_compute_type() const noexcept override { return compute_type::none; }


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		auto	labelLoop		= make_ref<_brunch_point>();
		auto&	blk				= _cmplr.block_begin();
		blk.continueLabel	= make_ref<_brunch_point>();
		blk.breakLabel		= make_ref<_brunch_point>();

		// 初期化式
		mInit->compile(_cmplr, _asmblr);

		// 条件比較
		_asmblr.insert_label(labelLoop);
		mConditions->compile(_cmplr, _asmblr);
		if (passion_is_gcpointer(mConditions->type()))
		{
			_asmblr.add_ref2int();
		}
		_asmblr.add_jfalse(blk.breakLabel);

		// ルーブブロック
		mLoopBlock->compile(_cmplr, _asmblr);

		// 次へ
		_asmblr.insert_label(blk.continueLabel);
		mIteration->compile(_cmplr, _asmblr);
		_asmblr.add_jmp(labelLoop);

		_asmblr.insert_label(blk.breakLabel);
		if (mLocalAllocSize > 0)
		{
			_asmblr.add_free((int)mLocalAllocSize);
		}

		_cmplr.block_end();
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				if (_token != "for")
				{
					THROW_ERROR(unkown_token, _token);
				}
				_cmplr.block_begin();
				return	phase_type::open;
			}

			case phase_type::open :
			{
				if (_token != "(")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::init;
			}

			case phase_type::init :
			{
				mInit		= this->template create<statement<Compiler>>(value_mode::void_mode);
				return	{
					phase_type::init_end,
					mInit->lex(_cmplr, _token)
				};
			}

			case phase_type::init_end :
			{
				if (_token != ";")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::conditions;
			};

			case phase_type::conditions :
			{
				mConditions	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::conditions_end,
					mConditions->lex(_cmplr, _token)
				};
			}

			case phase_type::conditions_end :
			{
				if (_token != ";")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::next;
			};

			case phase_type::next :
			{
				mIteration			= this->template create<statement<Compiler>>(value_mode::void_mode);
				return	{
					phase_type::close,
					mIteration->lex(_cmplr, _token)
				};
			}

			case phase_type::close :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::loop_block;
			};

			case phase_type::loop_block :
			{
				mLoopBlock	= this->template create<codeblock<Compiler>>();
				return	{
					phase_type::end,
					mLoopBlock->lex(_cmplr, _token)
				};
			}

			default :
			{
				mLocalAllocSize	= _cmplr.block_end();
				return	phase_type::pass;
			}
		}
	}

private:
	vmstring		mTypename;
	ref<node_type>	mInit;
	ref<node_type>	mConditions;
	ref<node_type>	mIteration;
	ref<node_type>	mLoopBlock;
	std::size_t		mLocalAllocSize;
};




template<class Compiler, class Traits = expression_traits<Compiler>>
struct	ctrlflow_if : node<Compiler>, Traits
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
		keyword,
		open_bracket,
		init_or_conditions,
		init_end,
		condition,
		close_bracket,
		true_block,
		false_keyword,
		false_block,
	};

	struct	delete_queue
	{
		vmstring	type;
		std::size_t	lp;
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("if");
	}


	virtual compute_type on_compute_type() const noexcept override
	{
		return	compute_type::dynamics;//mAnswer ? mAnswer->get_compute_type() : m_true_block->get_compute_type();
	}

	virtual vmvariant on_constexpr_value() const noexcept override
	{
		return	{};//mAnswer ? mAnswer->get_static_variable() : m_true_block->get_static_variable();
	}

	virtual vmstring on_type() const override
	{
		return	{};//mAnswer ? mAnswer->type() : m_true_block->type();
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			case phase_type::keyword :
			{
				return	phase_type::open_bracket;
			}

			case phase_type::open_bracket :
			{
				if (_token != "(")
				{
					THROW_ERROR(unkown_token, _token);
				}
				_cmplr.block_begin();
				return	phase_type::init_or_conditions;
			}

			case phase_type::init_or_conditions :
			{
				mConditions = this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{ phase_type::init_end, mConditions->lex(_cmplr, _token) };
			}

			case phase_type::init_end :
			{
				if (_token == ";") 
				{
					mInit 			= std::move(mConditions);
					return	phase_type::condition;
				}
				if (_token == ")")
				{
					return	phase_type::true_block;
				}
				THROW_ERROR(unkown_token, _token);
			}

			case phase_type::condition :
			{
				if (mConditions = this->template create_if<statement<Compiler>>(value_mode::rvalue, _cmplr, _token); !mConditions.empty())
				{
					return	{
						phase_type::close_bracket,
						mConditions->lex(_cmplr, _token)
					};
				}
				THROW_ERROR(unkown_token, _token);
			}

			case phase_type::close_bracket :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::true_block;
			}

			case phase_type::true_block :
			{
/*				if (mConditions->type() != "bool")
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), value_is_not_convertible, mConditions->type().c_str(), "bool");
				}
*/
				m_true_block	= this->template create<codeblock<Compiler>>();
				return	{
					phase_type::false_keyword,
					m_true_block->lex(_cmplr, _token)
				};
			}

			case phase_type::false_keyword :
			{
				if (_token != "else")
				{
					constexpression(_cmplr);
					return	phase_type::pass;
				}
				return	phase_type::false_block;
			}

			case phase_type::false_block :
			{
				if (_token == "if")
				{
					m_false_block	= this->template create<ctrlflow_if<Compiler>>();
				}
				else if (_token == "{")
				{
					m_false_block	= this->template create<codeblock<Compiler>>();
				}
				else
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	{
					phase_type::end,
					m_false_block->lex(_cmplr, _token)
				};
			}

			case phase_type::end :
			default :
			{
				constexpression(_cmplr);
				return	phase_type::pass;
			}
		}
	}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		if (!mInit.empty())
		{
			mInit->compile(_cmplr, _asmblr);
		}

		if (mConditions.empty())
		{
			if (!mAnswer.empty())
			{
				mAnswer->compile(_cmplr, _asmblr);
			}
		}
		else
		{
			auto	labelFalse	= make_ref<_brunch_point>();
			mConditions->compile(_cmplr, _asmblr);
			if (passion_is_gcpointer(mConditions->type()))
			{
				_asmblr.add_ref2int();
			}
			if (passion_is_reference(mConditions->type()))
			{
				auto		st		= passion_remove_reference(mConditions->type());
				if (passion_is_gcpointer(st))
				{
					_asmblr.add_load_adr();
				}
				else
				{
					auto		clz		= _cmplr.find_class(passion_remove_reference(mConditions->type()));
					_asmblr.add_load(clz);
				}
			}
			_asmblr.add_jfalse(labelFalse);
			m_true_block->compile(_cmplr, _asmblr);
			if (m_false_block.empty())
			{
				_asmblr.insert_label(labelFalse);
			}
			else
			{
				auto	labelEnd	= make_ref<_brunch_point>();
				_asmblr.add_jmp(labelEnd);
				_asmblr.insert_label(labelFalse);
				m_false_block->compile(_cmplr, _asmblr);
				_asmblr.insert_label(labelEnd);
			}
		}

		for (auto& d : mDelete)
		{
			// gcpointer のデストラクタを呼び出し
			_asmblr.add_lea_local(d.lp);
			_asmblr.add_delete_ref();
		}
		if (mLocalAllocate > 0)
		{
			_asmblr.add_free((int)mLocalAllocate);
		}
	}

private:
	void constexpression(compiler_type& _cmplr)
	{
		mLocalAllocate	= _cmplr.local_variable_size();
		for (std::size_t i = 0; i < mLocalAllocate; ++i)
		{
			auto[pos, v]	= _cmplr.get_local_from_back(i);
			if (v && passion_is_gcpointer(v->type))
			{
				mDelete.push_back({v->type, pos});
			}
		}
		_cmplr.block_end();
		if (mConditions->get_compute_type() == compute_type::static_integer)
		{
			// 静的解決
			mAnswer	= std::get<vmvalue>(mConditions->get_static_variable()).i == 0
					? m_false_block
					: m_true_block;
			mConditions	= nullptr;
			m_true_block	= nullptr;
			m_false_block	= nullptr;
		}
#if	0
		else if (mConditions->get_compute_type() == compute_type::static_string)
		{
			// 静的解決
			auto	tx	= removeQuotation(std::get<vmstring>(mConditions->get_static_variable()));
			mAnswer	= tx.empty()
					? m_false_block
					: m_true_block;
			mConditions	= nullptr;
			m_true_block	= nullptr;
			m_false_block	= nullptr;
		}
#endif
	}

	std::size_t					mLocalAllocate;
	std::vector<delete_queue>	mDelete;
	ref<node_type>				mInit;
	ref<node_type>				mConditions;
	ref<node_type>				m_true_block;
	ref<node_type>				m_false_block;
	ref<node_type>				mAnswer;
};




template<class Compiler, class Traits = expression_traits<Compiler>>
struct	ctrlflow_return : node<Compiler>, Traits
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
		value,
		value_end,
	};

	struct	delete_queue
	{
		vmstring	type;
		std::size_t	lp;
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("return");
	}

	virtual vmstring on_type() const override { return mResult->type(); }
	virtual compute_type on_compute_type() const noexcept override { return compute_type::end; }


	ctrlflow_return()
	{}

	ctrlflow_return(node_type* _result)
		: mResult(_result)
	{}


	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		vmstring	rettype;
		if (!mResult.empty())
		{
			vmstring	type	= mResult->type();
			_asmblr.add_lea_arg(1);
			mResult->compile(_cmplr, _asmblr);
			if (passion_is_reference(type))
			{
				type	= passion_trim_reference(type);
				_asmblr.add_load(type);
			}
			_asmblr.add_store(type);
		}

		// ローカル変数の削除
		if (mFreeSize > 0)
		{
			for (auto& d : mDelete)
			{
				// gcpointer のデストラクタを呼び出し
				_asmblr.add_lea_local(d.lp);
				_asmblr.add_delete_ref();
			}
			_asmblr.add_free((int)mFreeSize);
		}

		_asmblr.add_ret();
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				return	phase_type::value;
			}

			case phase_type::value :
			{
				if (_token == ";")
				{
					mFreeSize	= _cmplr.current_block().local.size();;
					for (std::size_t i = 0; i < mFreeSize; ++i)
					{
						auto[pos, v]	= _cmplr.get_local_from_back(i);
						if (v && passion_is_gcpointerable(v->type))
						{
							mDelete.push_back({v->type, pos});
						}
					}
					return	phase_type::end;
				}
				mResult	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{
					phase_type::value_end,
					mResult->lex(_cmplr, _token)
				};
			}

			case phase_type::value_end :
			{
				mFreeSize	= _cmplr.current_block().local.size();;
				for (std::size_t i = 0; i < mFreeSize; ++i)
				{
					auto[pos, v]	= _cmplr.get_local_from_back(i);
					if (v && passion_is_gcpointerable(v->type))
					{
						mDelete.push_back({v->type, pos});
					}
				}
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	ref<node_type>				mResult;
	std::size_t					mFreeSize	= 0;
	std::vector<delete_queue>	mDelete;
};



template<class Compiler, class Traits = expression_traits<Compiler>>
struct	ctrlflow_while : node<Compiler>, Traits
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
		conditions_start,
		conditions_end,
		loop_block,
	};

	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return _token.str() == std::string_view("while");
	}


	virtual vmstring on_type() const override { return mTypename; }
	virtual compute_type on_compute_type() const noexcept override { return compute_type::none; }

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		auto&	blk				= _cmplr.block_begin();
		blk.continueLabel	= make_ref<_brunch_point>();
		blk.breakLabel		= make_ref<_brunch_point>();

		_asmblr.insert_label(blk.continueLabel);

		mConditions->compile(_cmplr, _asmblr);
		if (passion_is_gcpointer(mConditions->type()))
		{
			_asmblr.add_ref2int();
		}
		if (passion_is_reference(mConditions->type()))
		{
			_asmblr.add_load(_cmplr.find_class(passion_remove_reference(mConditions->type())));
		}
		_asmblr.add_jfalse(blk.breakLabel);
		mLoopBlock->compile(_cmplr, _asmblr);
		_asmblr.add_jmp(blk.continueLabel);

		_asmblr.insert_label(blk.breakLabel);
		_cmplr.block_end();
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
				return	phase_type::conditions_start;

			case phase_type::conditions_start :
			{
				if (_token != "(")
				{
					THROW_ERROR(unkown_token, _token);
				}

				mConditions	= this->template create<statement<Compiler>>(value_mode::rvalue);
				return	{ phase_type::conditions_end, mConditions };
			}

			case phase_type::conditions_end :
			{
				if (_token != ")")
				{
					THROW_ERROR(unkown_token, _token);
				}
				return	phase_type::loop_block;
			}

			case phase_type::loop_block :
			{
				mLoopBlock		= this->template create<codeblock<Compiler>>();
				return	{
					phase_type::end,
					mLoopBlock->lex(_cmplr, _token)
				};
			}


			default :
			{
				return	phase_type::pass;
			}
		}
	}

private:
	vmstring		mTypename;
	ref<node_type>	mConditions;
	ref<node_type>	mLoopBlock;
	std::size_t		mLocalAllocSize;
};

template<class Compiler, class Traits>
struct	statement : node<Compiler>, Traits
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
		class_separator,
		class_member,
		operator_start,
		operator_binary_rvalue,
		operator_end,
		decltype_end,
	};


	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		// Match symboles.
		if (auto m = _cmplr.find(_token.str()); !m.empty() || (_token == "auto") || (_token == "(") || (_token == "&") || (_token == "*") || (_token == "decltype") || (_token == "this"))
		{
			return	true;
		}

		// Operator new
		if (operator_new<Compiler>::is_start(_cmplr, _token))
		{
			return	true;
		}

		// ++(before)
		if (operator_before_incremental<Compiler>::is_start(_cmplr, _token))
		{
			return	true;
		}

		// Match local variables.
		if (_cmplr.find_local(_token.str()) != npos)
		{
			return	true;
		}

		// No match names.
		return	_token.is_identifer();
	}


	virtual compute_type on_compute_type() const noexcept override { return mValue->get_compute_type(); }
	virtual vmvariant on_constexpr_value() const noexcept override { return mValue->get_static_variable(); }
	virtual vmstring on_type() const override { return mTypepath; }

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		mValue->compile(_cmplr, _asmblr);
	}

	size_t	mParenthesis	= 0;


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			{
				if (_token == "(")
				{
					++mParenthesis;
					mValue	= this->template create<statement<Compiler>>(value_mode::rvalue);
					return	{ phase_type::operator_start, mValue };
				}


				if (_token == "*")
				{
					mValue	= this->template create<operator_dereference<Compiler>>(value_mode::rvalue);
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				if (_token == "&")
				{
					mValue	= this->template create<operator_to_address<Compiler>>(value_mode::rvalue);
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				// Operator new
				if (operator_new<Compiler>::is_start(_cmplr, _token))
				{
					mValue	= this->template create<operator_new<Compiler>>(value_mode::rvalue);
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				// Local variables.
				if (auto lindex = _cmplr.find_local(_token.str()); lindex != npos)
				{
					mValue	= this->template create<variable_local<Compiler>>(value_mode::rvalue, _cmplr, _token.str(), lindex);
					mValue->lex(_cmplr, _token);
					return	phase_type::operator_start;
				}

				// Argument variables.
				if (auto lindex = _cmplr.find_argv(_token.str()); lindex != npos)
				{
					mValue	= this->template create<variable_arg<Compiler>>(value_mode::rvalue, _cmplr, _token.str(), lindex);
					mValue->lex(_cmplr, _token);
					return	phase_type::operator_start;
				}

				// integer.
				if (auto e = this->template create_if<literal_integer<Compiler>>(value_mode::rvalue, _cmplr, _token); e)
				{
					mValue	= e;
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				// string.
				if (auto e = this->template create_if<literal_string<Compiler>>(value_mode::rvalue, _cmplr, _token); e)
				{
					mValue	= e;
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				// boolean.
				if (auto e = this->template create_if<literal_boolean<Compiler>>(value_mode::rvalue, _cmplr, _token); e)
				{
					mValue	= e;
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				// nullptr.
				if (auto e = this->template create_if<literal_nullptr<Compiler>>(value_mode::rvalue, _cmplr, _token); e)
				{
					mValue	= e;
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				if (auto e = this->template create_if<operator_before_incremental<Compiler>>(this->get_value_type(), _cmplr, _token); e)
				{
					mValue	= e;
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				// this.
				if (_token == "this")
				{
					mValue	= this->template create<variable_this<Compiler>>(value_mode::rvalue, _cmplr.clazz);
					return	phase_type::operator_start;
				}


				// Class name.
				if (_token == "decltype")
				{
					// クラス名から始まっている場合の処理
					mValue	= this->template create<specifier_decltype<Compiler>>(value_mode::rvalue);
					return	{ phase_type::decltype_end, mValue->lex(_cmplr, _token) };
				}

				// Class name.
				if (_token == "auto")
				{
					// クラス名から始まっている場合の処理
					mTypepath	= _token.str();
					return	phase_type::class_separator;
				}

				// symbol.
				if (auto m = _cmplr.find((vmstring)_token); !m.empty())
				{
					if (m.type == compiler_type::match_type::match_function)
					{
						mTypepath		= m.func->class_name;
						mValue	= this->template create<dynamic_member<Compiler>>(
										this->get_value_type(),
										this->template create<variable_this<Compiler>>(mTypepath)
								);
						return	{ phase_type::end, mValue->lex(_cmplr, _token) };
					}

					if (m.type == compiler_type::match_type::match_variable)
					{
						amtrs_string_view	vn;
						amtrs_string_view	ns	= passion_namespace_parent_namespace_and_current(&vn, m.name);

						if (auto dec = _cmplr.find_class(ns)->get_static_variable_info(vn); dec)
						{
							mValue		= this->template create<variable_static<Compiler>>(
											this->get_value_type(),
											vmstring(m.vartype->text), vmstring(m.name)
										);
							return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
						}

						mValue		= this->template create<dynamic_member<Compiler>>(
										this->get_value_type(),
										this->template create<variable_this<Compiler>>(_cmplr.clazz)
									);
						return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
					}
				}

				// 識別子から始まっている場合、スコープ演算子の可能性がある。
				if (_token.is_identifer())
				{
					mValue	= this->template create<typeidentify<Compiler>>(value_mode::rvalue);
					return	{ phase_type::decltype_end, mValue->lex(_cmplr, _token) };
				}

				// No match.
				return	phase_type::pass;
			}

			// operator end from right value.
			case phase_type::operator_end :
			{
				if (operator_binary<compiler_type>::is_binary_operator(mOperator))
				{
					mValue		= this->template create<operator_binary<Compiler>>(this->get_value_type(), mOperator, mValue, mValue2);
					mValue->set_priority(operator_binary<compiler_type>::get_priority(mOperator));
					mValue->lex(_cmplr, _token);
				}
				mValue2		= nullptr;
				mOperator	= vmstring();
			}

			// operator start.
			case phase_type::operator_start :
			{
				if (!mValue.empty())
				{
					mTypepath	= mValue->type();
				}

				if (_token == ")")
				{
					if (mParenthesis == 0)
					{
						return	phase_type::pass;
					}
					--mParenthesis;
					return	phase_type::operator_start;
				}

				if (_token == "(" && !mValue.empty())
				{
					if (passion_is_function(nullptr, passion_trim_reference(mValue->type())))
					{
						mValue		= this->template create<function_caller<Compiler>>(this->get_value_type(), mValue);
						return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
					}
				}


				// complite
				if (_token == ";")
				{
					return	phase_type::pass;
				}

				// function operator
				if (_token == ".")
				{
					mValue		= this->template create<dynamic_member<Compiler>>(this->get_value_type(), mValue);
					return	{ phase_type::operator_start, mValue };
				}

				// array operator
				if (_token == "[")
				{
					mValue		= this->template create<operator_arraysubscript<Compiler>>(this->get_value_type(), mValue);
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				// binary operator
				if (auto ope = operator_binary<compiler_type>::get_operator_type(_token); ope != operator_binary<compiler_type>::operator_type::unkown)
				{
					auto	priority	= operator_binary<compiler_type>::get_priority(ope);
					if (this->get_priority() >= priority)
					{
						return	phase_type::pass;
					}
					mOperator	= _token;
					auto	rv	= this->template create<statement<Compiler>>(value_mode::rvalue);
					rv->set_priority(priority);
					mValue2		= rv;
					return	{ phase_type::operator_end, rv };
				}

				// ? :
				if (_token == "?")
				{
					if (this->get_priority() > operator_conditional<compiler_type>::priority)
					{
						return	phase_type::pass;
					}
					mValue	= this->template create<operator_conditional<compiler_type>>(value_mode::rvalue, mValue);
					return	{ phase_type::operator_end, mValue->lex(_cmplr, _token) };
				}



				// No match.
				return	phase_type::pass;
			}

			// decltype終了後
			case phase_type::decltype_end :
			{
				mTypepath	= mValue->type();
				mValue		= nullptr;
			}

			// クラスのセパレター
			case phase_type::class_separator :
			{
				// クラスパス
				if (_token.str() == "::")
				{
					return	phase_type::class_member;
				}

				// ポインタ、GCポインタ、参照の場合、ローカル変数確保扱い
				if (_token.str() == "^" || _token.str() == "*" || _token.str() == "&")
				{
					mTypepath	+= _token.str();
					return	phase_type::class_separator;
				}

				// ポインタ、GCポインタ、参照の場合、ローカル変数確保扱い
				if (_token.str() == "[" || _token.str() == "*" || _token.str() == "&")
				{
					mTypepath	+= _token.str();
					return	phase_type::class_separator;
				}


				// Typename	_token という書式なので変数確保
				if (_token == Token::COMMAND)
				{
					mValue	= this->template create<local_alloc<Compiler>>(value_mode::rvalue, mTypepath);
					return	{ phase_type::end, mValue->lex(_cmplr, _token) };
				}

				return	phase_type::pass;
			}

			// クラスのメンバー
			case phase_type::class_member :
			{
				vmstring	fullname	= mTypepath + "::" + _token.str();

				// 関数のチェック
				if (auto* func = _cmplr.find_class(mTypepath)->find_function(fullname); func)
				{
					mFunctionName	= func->name();
					mValue	= this->template create<dynamic_member<Compiler>>(
									this->get_value_type(),
									mValue
							);
					return	{ phase_type::end, mValue->lex(_cmplr, _token) };
				}

				// インナークラスのチェック
				if (auto* clzz = _cmplr.find_class(fullname); clzz)
				{
					mTypepath	= std::move(fullname);
					return	phase_type::class_separator;
				}

				if (auto m = _cmplr.find(fullname); !m.empty())
				{
					mValue		= this->template create<variable_static<Compiler>>(
									this->get_value_type(),
									vmstring(m.vartype->text), vmstring(m.name)
								);
					return	{ phase_type::operator_start, mValue->lex(_cmplr, _token) };
				}

				return	phase_type::pass;
			}

			case phase_type::end :
			{
				if (!mValue.empty())
				{
					mTypepath	= mValue->type();
				}
				return	phase_type::pass;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}


	
private:


	ref<node_type>		mValue;

	vmstring			mOperator;
	ref<node_type>		mValue2;

	vmstring			mTypepath;
	vmstring			mFunctionName;
};



template<class Compiler, class Traits = expression_traits<Compiler>>
struct	using_alias : node<Compiler>, Traits
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
		name,
		separator,
	};	

	// コンパイル時は何もしない
	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override	{}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
				if (_token != "using")
				{
					AMTRS_DEBUG_LOG("Not start using");
					return	phase_type::pass;
				}
				return	phase_type::name;

			case phase_type::name :
				mIdentify		= _token;
				return	phase_type::separator;

			case phase_type::separator :
				if (_token != "=")
				{
					AMTRS_DEBUG_LOG("usin has not separator");
				}
				mTypeid	= this->template create<typeidentify<compiler_type>>();
				return	{ phase_type::end, mTypeid };

			case phase_type::end :
			{
				if (extract_typename(_cmplr, mTypeid->type()).empty())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), unkown_type_name, mTypeid->type().c_str());
				}
				_cmplr.add_type_alias(mIdentify, mTypeid->type());
				mTypeid	= nullptr;
				return	phase_type::end;
			}

			default :
				return	phase_type::pass;
		}
	}


	virtual vmstring on_type() const override { return mIdentify; }
	


private:
	vmstring								mIdentify;
	ref<typeidentify<compiler_type>>	mTypeid;
};



template<class Compiler, class Traits>
struct	codeblock : node<Compiler>, Traits
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
		open,				//!< {
		codes,				//!< 
	};

	struct	delete_queue
	{
		vmstring	type;
		std::size_t	lp;
	};



	template<class C, class T>
	static bool is_start(C& _cmplr, const T& _token)
	{
		return	_token == std::string_view("{");
	}


	virtual vmstring on_type() const override { return mStatements.empty() ? vmstring() : mStatements.back()->type(); }
	virtual compute_type on_compute_type() const noexcept override { return mStatements.empty() ? compute_type::none : mStatements.back()->get_compute_type(); }

	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) override
	{
		for (auto& stm : mStatements)
		{
			stm->compile(_cmplr, _asmblr);
		}
		if (this->get_compute_type() != compute_type::end)
		{
			for (auto& d : mDelete)
			{
				// gcpointer のデストラクタを呼び出し
				_asmblr.add_lea_local(d.lp);
				_asmblr.add_delete_ref();
			}
			if (mLocalAllocate > 0)
			{
				_asmblr.add_free((int)mLocalAllocate);
			}
		}
	}


	virtual parser_result<compiler_type> on_parse(int _phase, compiler_type& _cmplr, input_type& _token) override
	{
		switch ((phase_type)_phase)
		{
			case phase_type::start :
			case phase_type::open :
			{
				if (_token != "{")
				{
					THROW_ERROR(unkown_token, _token);
				}
				_cmplr.block_begin();
				return	phase_type::codes;
			}

			case phase_type::codes :
			{
				if (_token == ";")
				{
					return	phase_type::codes;
				}
				if (_token == "}")
				{
					mLocalAllocate	= _cmplr.local_variable_size();
					for (std::size_t i = 0; i < mLocalAllocate; ++i)
					{
						auto[pos, v]	= _cmplr.get_local_from_back(i);
						if (v && passion_is_gcpointerable(v->type))
						{
							mDelete.push_back({v->type, pos});
						}
					}
					_cmplr.block_end();
					return	phase_type::end;
				}

				ref<node_type>	stm;
				if (_token == "using")
				{
					stm	= this->template create<using_alias<Compiler>>();
				}
				if (stm.empty())	stm = this->template create_if<ctrlflow_return<Compiler>>(_cmplr, _token);
				if (stm.empty())	stm = this->template create_if<ctrlflow_if<Compiler>>(_cmplr, _token);
				if (stm.empty())	stm = this->template create_if<ctrlflow_while<Compiler>>(_cmplr, _token);
				if (stm.empty())	stm = this->template create_if<ctrlflow_do_while<Compiler>>(_cmplr, _token);
				if (stm.empty())	stm = this->template create_if<ctrlflow_for<Compiler>>(_cmplr, _token);
				if (stm.empty())	stm = this->template create_if<ctrlflow_break<Compiler>>(_cmplr, _token);
				if (stm.empty())	stm = this->template create_if<ctrlflow_continue<Compiler>>(_cmplr, _token);
				if (stm.empty())	stm = this->template create_if<codeblock<Compiler>>(_cmplr, _token);
				if (stm.empty())	stm = this->template create_if<statement<Compiler>>(value_mode::void_mode, _cmplr, _token);
				if (stm.empty())
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _cmplr.location(), unkown_type_name, _token.str().c_str());
				}

				if (this->get_compute_type() == compute_type::end)
				{
					PSNCMPLR_SYNTAX_ERROR(_cmplr, _token.location(), code_will_never_be_executed, "");
				}

				mStatements.push_back(stm);
				return	{
					phase_type::codes,
					stm->lex(_cmplr, _token)
				};
			}


			default:
			{
				return	phase_type::pass;
			}
		}
	}

private:
	std::size_t					mLocalAllocate;
	std::vector<delete_queue>	mDelete;
	std::vector<ref<node_type>>	mStatements;
};

PASSION_NAMESPACE_END
#endif
