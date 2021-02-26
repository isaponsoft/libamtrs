/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__compiler__hpp
#define	__passion__compiler__hpp
#include <deque>
#include <stack>
#include "def.hpp"
#include "types.hpp"
#include "../err/error.hpp"
#include "../token/brunch_point.hpp"
#include "../token/token.hpp"
#include "../asm/assembler.hpp"
#include "../vm/vm.hpp"
#include "../vm/vmclass.hpp"

#include "node.hpp"
#include "statement.hpp"
#include "declare.hpp"
#include "pp_psnpp.hpp"
#include "pp_stringinter.hpp"

PASSION_NAMESPACE_BEGIN



struct	compiler
{
	using	traits_type		= expression_traits<compiler>;
	using	node_type		= typename traits_type::node_type;
	using	compiler_type	= typename traits_type::compiler_type;
	using	location_type	= location;
	using	input_type		= const Token;


	using	classmanager_type		= vmclass_manager;
	using	source_type				= vmstring;
	using	comple_callback_type	= bool(compiler& _cmplr);


	struct	error
			: public ref_object
	{
		vmstring	file;
		int			line;
		error_code	code;
		vmstring	msg;

		error(vmstring _file, int _line, error_code _code, vmstring _msg
			#ifdef	PASSION_COMPILER_SHOW_ERRORPOINT
			, vmstring _program_name, int _program_line
			#endif
		)
			: file(std::move(_file))
			, line(_line)
			, code(_code)
			, msg(std::move(_msg))
		{
		#ifdef	PASSION_COMPILER_SHOW_ERRORPOINT
			msg	+= " in ";
			msg	+= _program_name;
			msg	+= "(";
			msg	+= vmstring(std::to_string(_program_line));
			msg	+= ")";
		//	AMTRS_DEBUG_LOG("%s", msg.c_str());
		#endif
		}
	};


	void set_error(ref<error> _error)
	{
		mError = std::move(_error);
	}


	error* get_error() const noexcept { return mError; }


	struct	block_status
	{
		std::vector<vmclass::declere_value>	local;							//!< 現在のコードブロックで見えるローカル変数
		std::size_t							previousLocalSize	= 0;		//!< 一つ上のブロックのローカルサイズ
		brunch_point						breakLabel;						//!< break での飛び先
		brunch_point						continueLabel;					//!< continue での飛び先
		passion_symbole_list				symboles;						//!< シンボルリスト

		block_status()
		{
			passion_symbole_list_initialize(&symboles);
		}


		static block_status make(const block_status& _r)
		{
			block_status	retval;
			retval.local				= _r.local;
			retval.breakLabel			= _r.breakLabel;
			retval.continueLabel		= _r.continueLabel;
			retval.previousLocalSize	= _r.local.size();
			retval.symboles.cascade		= &_r.symboles;
			return	retval;
		}


		block_status(block_status&&) = default;
		block_status& operator = (block_status&&) = default;

		~block_status()
		{
			passion_symbole_list_destroy(&symboles);
		}

		block_status(const block_status&) = delete;
		block_status& operator = (const block_status&) = delete;
	};


	compiler(const compiler&) = delete;
	compiler& operator = (const compiler&) = delete;

	compiler(classmanager_type* _vm)
		: mVM(_vm)
		, mCreatingClasses(_vm)
	{
		mCodeBlocks.push_back({});	// 処理を簡略にするために最初からダミーのブロックをひとつ作っておく
		inport_classes(_vm);
	}

	compiler(classmanager_type* _vm, compiler const* _parent)
		: mVM(_vm)
		, mCreatingClasses(_vm)
	{
		mCodeBlocks.push_back({});	// 処理を簡略にするために最初からダミーのブロックをひとつ作っておく
		mCodeBlocks.back().symboles.cascade	= &(_parent->current_block().symboles);
	}

	void inport_classes(classmanager_type* _cm)
	{
		_cm->enumlate_classes([this](auto name, auto clz) -> bool
		{
			add_class(name);
			for (auto f : clz->functions())
			{
				vmstring	name	= 
							clz->name().empty()
							? vmstring(f->name())
							: vmstring(clz->name() + "::" + f->name());
				add_function(name, f->function());
			}
			return	true;
		});
	}


	classmanager_type* classes() noexcept { return &mCreatingClasses; }


	enum class	match_type
	{
		match_none,
		match_localvar,
		match_argvar,
		match_namespace,
		match_alias,
		match_class,
		match_function,
		match_variable,
	};

	struct	match_info
	{
		vmstring	name;
		match_type	type;
		union
		{
			size_t				index;		//!< localvar, argvar
			passion_symbole*	nmspace;	//!< namespace
			passion_symbole*	alias;		//!< alias
			vmclass*			clazz;		//!< class
			passion_function*	func;		//!< function
			amtrs_string*		vartype;	//!< variable
		};

		bool empty() const noexcept { return type == match_type::match_none; }
	};

	const location_type& location() const noexcept { return mLocation; }

	location_type		mLocation;


	match_info find(amtrs_string_view _name)
	{
		match_info	retval;
		memset(&retval, 0, sizeof(retval));
		retval.type	= match_type::match_none;

		if (auto i = find_local(_name); i != npos)
		{
			retval.name		= _name;
			retval.type		= match_type::match_localvar;
			retval.index	= i;
			return	retval;
		}

		if (auto i = find_argv(_name); i != npos)
		{
			retval.name		= _name;
			retval.type		= match_type::match_argvar;
			retval.index	= i;
			return	retval;
		}

		vmstring	scope	= clazz;
		for (;;)
		{
			retval	= find_scope_of(scope, _name);
			if (!retval.empty())
			{
				return	retval;
			}
			if (scope.empty())
			{
				break;
			}
			scope	= passion_namespace_parent_namespace(scope);
		}

		vmclass*	clz		= find_class(clazz);
		if (clz)
		{
			// ベースクラスから検索
			return	find_class_of(clz, _name);
		}
		return	retval;
	}


	match_info find_class_of(vmclass* _scope, amtrs_string_view _name)
	{
		match_info	retval	= find_scope_of(_scope->name(), _name);
		if (retval.empty())
		{
			for (auto b : _scope->base_classes())
			{
				retval	= find_class_of(b, _name);
				if (!retval.empty())
				{
					return	retval;
				}
			}
		}
		return	retval;
	}


	match_info find_scope_of(vmstring _scope, amtrs_string_view _name)
	{
		match_info	retval;
		memset(&retval, 0, sizeof(retval));
		retval.type	= match_type::match_none;

		vmstring			name	= _scope.empty() ? vmstring(_name) : vmstring(_scope + "::" + _name);
		passion_symbole*	s		= passion_symbole_list_find(&mCodeBlocks.back().symboles, name, true);
		if (s)
		{
			if (s->type == passion_symbole_class)
			{
				retval.type		= match_type::match_class;
				retval.clazz	= find_class(name);
			}
			if (s->type == passion_symbole_type_alias)
			{
				retval.type		= match_type::match_alias;
				retval.alias	= s;
			}
			if (s->type == passion_symbole_namespace)
			{
				retval.type		= match_type::match_namespace;
				retval.nmspace	= s;
			}
			if (s->type == passion_symbole_function)
			{
				retval.type		= match_type::match_function;
				retval.func		= s->data.function;
			}
			if (s->type == passion_symbole_variable)
			{
				retval.type		= match_type::match_variable;
				retval.vartype	= s->data.var_type;
			}
			retval.name		= name;
			return	retval;
		}
		return	retval;
	}


	vmclass* find_class(vmstring _classname)
	{
		if (passion_is_reference(_classname))
		{
			_classname	= passion_remove_reference(_classname);
		}
		_classname	= extract_typename(*this, _classname);
		return	passion_is_array(_classname)	? classes()->get_class("[]")
			:	passion_is_pointer(_classname)	? classes()->get_class(passion_remove_reference(_classname))
			:	classes()->get_class(_classname);
	}


	// ========================================================================
	//! ソースコードをコンパイルし、VMで使用できるように展開する。
	// ------------------------------------------------------------------------
	inline bool compile(source_type _source, source_type _sourcename = {})
	{
		tokenreader						in;
		in.push(tokenbuffer::create({_source, _sourcename, 1}));

		ref<globalscope<compiler>>	gs				= new globalscope<compiler>();
		ref<node<compiler>>			process_node	= gs;

		add_type_alias("string", "psn::string^");


		while (!in.empty())
		{
			// Token base preprocessor
			if (mTPP(*this, in)) { continue; }
			if (get_error())
			{
				return	false;
			}

			// 文字列の補間を行う。
			string_interpolation<compiler>{}(*this, in);
			if (get_error())
			{
				return	false;
			}


			// 空白と改行コードは無視し、それ以外ならコンパイラに処理させる。
			if (in != Token::SPACE && in != Token::RETURN)
			{
				process_node	= process_node->lex(*this, in);
				if (get_error())
				{
					return	false;
				}
			}
			++in;
		}

		// グローバルスコープの定義を行う
		gs->declere(*this);
		if (get_error())
		{
			return	false;
		}

		// バイトコードのコンパイルを行う
		for (auto& f : mCompileCallbacks)
		{
			f(*this);
			if (get_error())
			{
				return	false;
			}
		}

		for (auto& c : classes()->classes())
		{
			mVM->add_class(c.second);
		}

		return	get_error() ? false : true;
	}



	//! 引数を検索する
	std::size_t find_argv(const vmstring& _value_name) const
	{
		std::size_t	argIndex	= 0;
		for (const auto& p : parameters)
		{
			if (p.name == _value_name)
			{
				return	argIndex;
			}
			++argIndex;
		}
		return	npos;
	}

	//!	ローカル変数の位置を取得する。
	std::size_t find_local(const vmstring& _value_name) const
	{
		std::size_t	index	= 0;
		for (auto it = mCodeBlocks.back().local.begin() ; it != mCodeBlocks.back().local.end(); ++it)
		{
			if (it->name == _value_name)
			{
				return	index;
			}
			++index;
		}
		return	npos;
	}


	//!	ローカル変数を追加する。
	void local_allocate(const vmclass::declere_value& _declere)
	{
		mCodeBlocks.back().local.push_back(_declere);
	}

	//!	ローカル変数を取得する
	std::pair<std::size_t, const vmclass::declere_value*> get_local(std::size_t _index)
	{
		return	{ _index, &mCodeBlocks.back().local[_index] };
	}

	//!	ローカル変数を後ろから取得する
	std::pair<std::size_t, const vmclass::declere_value*> get_local_from_back(std::size_t _index)
	{
		auto	pos	= mCodeBlocks.back().local.size() - 1 - _index;
		return	{ pos, &mCodeBlocks.back().local[pos] };
	}



	//! 現在のコードブロックで確保したメモリのサイズを返す。
	std::size_t local_variable_size() const noexcept
	{
		auto	bs	= mCodeBlocks.size();
		auto	ls	= mCodeBlocks[bs - 1].local.size();
		if (bs == 1)
		{
			return	ls;
		}
		return	ls - mCodeBlocks[bs - 2].local.size();
	}



	//! 現在のスコープをもとにクラス名をフルパスに変換します。
	vmstring normalize_class_path(const vmstring& _type)
	{
		if (find_class(_type))
		{
			return	_type;
		}

		vmstring	t	= vmstring(clazz + "::" + _type);
		if (find_class(t))
		{
			return	t;
		}

		// クラスを一つずつたどる
		vmstring		c	= vmstring(clazz);
		while (!c.empty())
		{
			auto		pos	= ((std::string_view)c).rfind("::");
			if (pos == std::string_view::npos)
			{
				break;
			}
			c	= c.substr(0, pos);
			t	= vmstring(c + "::" + _type);
			if (find_class(t))
			{
				return	t;
			}
		}
		return	_type;
	}



	//!	コードブロックに入ったことを通知します。
	//!	コードブロックとは {} で囲まれた範囲を示します。すなわち、ifやforなど抜けるときにローカル変数が解放されるタイミングです。
	auto& block_begin()
	{
		// 現在のブロックの状態をコピーして新しいブロックに入る
		auto*	blk	= &mCodeBlocks.back().symboles;
		mCodeBlocks.push_back(block_status::make(mCodeBlocks.back()));
		mCodeBlocks.back().previousLocalSize	= mCodeBlocks.back().local.size();
		mCodeBlocks.back().symboles.cascade		= blk;
		return	mCodeBlocks.back();
	}


	//!	コードブロックを抜けたことを通知します。
	//!	コードブロックとは {} で囲まれた範囲を示します。すなわち、ifやforなど抜けるときにローカル変数が解放されるタイミングです。
	std::size_t block_end()
	{
		std::size_t	currentLocalSize	= mCodeBlocks.back().local.size();
		mCodeBlocks.pop_back();
		std::size_t	previousLocalSize	= mCodeBlocks.back().local.size();
		return	currentLocalSize - previousLocalSize;
	}


	block_status& current_block() { return mCodeBlocks.back(); }
	block_status const& current_block() const { return mCodeBlocks.back(); }

	void add_compile_callback(std::function<comple_callback_type>&& _callback)
	{
		mCompileCallbacks.push_back(std::move(_callback));
	}


	void add_class(amtrs_string_view _classname)
	{
		passion_symbole_list_add_class(passion_symbole_top_cascade(&current_block().symboles), _classname);
	}


	void add_function(amtrs_string_view _functionname, passion_function* _func)
	{
		passion_symbole_list_add_function(passion_symbole_top_cascade(&current_block().symboles), _functionname, _func);
	}


	void add_type_alias(amtrs_string_view _alias, amtrs_string_view _type)
	{
		passion_symbole_list_add_type_alias(&current_block().symboles, _alias, _type);
	}


	void add_variable(amtrs_string_view _name, amtrs_string_view _type)
	{
		passion_symbole_list_add_variable(&current_block().symboles, _name, _type);
	}


	passion_symbole* find_symbole(amtrs_string_view _type)
	{
		return	passion_symbole_list_find(&mCodeBlocks.back().symboles, _type, true);
	}



	// ========================================================================
	//! 型名として識別可能かどうか調べる。
	// ------------------------------------------------------------------------
	passion_symbole* find_typeid(amtrs_string_view _identify)
	{
		auto*	retval	= find_symbole(_identify);
		if (retval)
		{
			if (retval->type == passion_symbole_class || retval->type == passion_symbole_type_alias)
			{
				return	retval;
			}
		}
		return	nullptr;
	}


private:
	inline vmstring parent_path(const vmstring& _from) const
	{
		if (auto pos = _from.rfind("::"); pos != std::string_view::npos)
		{
			return	vmstring(_from.substr(0, pos));
		}
		return	{};
	}

public:


	// ========================================================================
	//! アクセス権限を調べる。
	// ------------------------------------------------------------------------
	//!	\return
	//!		アクセスできなかった場合、チェックにひっかかった権限とそのパスを
	//!		返す。
	// ------------------------------------------------------------------------
	std::pair<attribute_flag, vmstring> access_permitted(const vmstring& _to, const vmstring& _from)
	{
		// アクセス修飾子に関係なくアクセスできる場所かどうかチェック
		if ((_to == _from)														// 自分自身なら属性にかかわらずアクセス可能
		 || (starts_with((std::string_view)_from, (std::string_view)_to))		// _to が通り道なら属性にかかわらずアクセス可能
		 || ((!_from.empty()) && (parent_path(_to) == _from))					// _to が自身の直接の子に相当するならアクセス可能
		)
		{
			// 自分自身ならアクセス可能
			return	{ attribute_flag::public_attribute, _to };
		}

		auto*	clz	= this->find_class(vmstring(_to));

		// private クラスへのアクセスは不可能
		if (clz->access == attribute_flag::private_attribute)
		{
			return	{ attribute_flag::private_attribute, _to };
		}

		// protected クラスなので親クラスがベースクラスに含まれるか調べる。
		if (clz->access == attribute_flag::protected_attribute)
		{
			if (this->find_class(vmstring(_from))->is_base_of(parent_path(_to)))
			{
				return	{ attribute_flag::public_attribute, _to };
			}
			return	{ attribute_flag::protected_attribute, _to };
		}

		return	access_permitted(parent_path(_to), _from);
	}

	// ****************************************************************************
	//! オペレータを検索する。
	// ----------------------------------------------------------------------------
	std::pair<vmclass*, vmfunction*> find_operator(const char* _operator, vmclass* _left, vmstring _right = {})
	{
		vmstring	name	= format<vmstring>("operator%s", _operator);
		vmstring	args[]	= {vmstring(_left->identify()), _right};
		return	find_function({find_class(vmstring(_left->identify())), find_class("")}, name, args, _right.empty() ? 0 : 1);
	}



	// ========================================================================
	//! 引数に関わらず名前がマッチする関数を探す。
	// ------------------------------------------------------------------------
	std::pair<vmclass*, vmfunction*> find_function(std::initializer_list<vmclass*> _classes, const vmstring& _name)
	{
		for (vmclass* clz : _classes)
		{
			for (vmfunction* f : clz->functions())
			{
				if (f->name() == _name)
				{
					return	{ clz, f };
				}
			}
			for (vmclass* bc : clz->base_classes())
			{
				auto	r	= find_function({bc}, _name);
				if (r.second)
				{
					return	r;
				}
			}
		}
		return	{ nullptr, nullptr };
	}


	// ========================================================================
	//! 名前と引数がマッチする関数を探す。
	// ------------------------------------------------------------------------
	std::pair<vmclass*, vmfunction*> find_function(std::initializer_list<vmclass*> _classes, const vmstring& _name, const vmstring* _argt, std::size_t _argc)
	{
		std::pair<vmclass*, vmfunction*>	retval	= { nullptr, nullptr };
		for (vmclass* clz : _classes)
		{
			for (vmfunction* f : clz->functions())
			{
				if (f->name() != _name)
				{
					continue;
				}
				if ((_argc < f->parameter_size())						// 引数が足りない
				 || (_argc > f->parameter_size() && !f->is_variadic()))	// 可変引数でもないのに引数の数が多い
				{
					continue;
				}
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
			}
			for (vmclass* bc : clz->base_classes())
			{
				auto	r	= find_function({bc}, _name, _argt, _argc);
				if (r.second)
				{
					return	r;
				}
			}
		}
		return	retval;
	}


	// ========================================================================
	//! 関数呼び出し時の引数の削除を追加します。
	// ------------------------------------------------------------------------
	//! thiscall の場合は、事前に this ポインタをスタックに積む命令を追加してください。
	// ------------------------------------------------------------------------
	void add_function_call(asmblr::assembler& _asmblr, vmclass* _clz, vmfunction* _function, ref<node_type> const* _arge, std::size_t _argc)
	{
		int		argSize		= static_cast<int>(_argc);

		// 
		for (int i = 0; i < argSize; ++i)
		{
			node_type*	expr	= _arge[i];
			expr->compile(*this, _asmblr);

			vmstring	t1	= extract_typename(*this, expr->type());
			vmstring	t2	= extract_typename(*this, _function->parameter(i));
			if (passion_is_reference(t1) && !passion_is_reference(t2))
			{
				_asmblr.add_load(
					this->find_class(passion_remove_reference(t1).remove_pointer()),
					this->find_class(passion_remove_reference(t2).remove_pointer())
				);
			}
		}

		// 関数呼び出し
		if (_function->is_virtual())
		{
			// 仮想関数テーブル呼び出し
			_asmblr.add_vcall(_clz->get_vtbl_id(_function), argSize + 1);
		}
		else if (_function->is_static())
		{
			// 関数呼び出し
			_asmblr.add_call(_function, argSize);
		}
		else
		{
			// 関数呼び出し(thiscall)
			_asmblr.add_call(_function, argSize + 1);		// 暗黙的な第１引数を追加
		}

		for (int i = 0; i < argSize; ++i)
		{
			if (passion_is_gcpointerable(extract_typename(*this, _function->parameter(i))))
			{
				_asmblr.add_lea_stack(i - argSize);
				_asmblr.add_delete_ref();
			}
		}

		if (_function->is_static() || _function->is_constractor())
		{
			_asmblr.add_free(argSize);
			return;
		}

		_asmblr.add_lea_stack(-(argSize+1));
		_asmblr.add_delete_ref();
		_asmblr.add_free(argSize + 1);
	}

	// ========================================================================
	//! 名前空間を追加します。
	// ------------------------------------------------------------------------
	//! すでに存在する場合は何もしません。
	// ------------------------------------------------------------------------
	void add_namespace(vmstring _namespace)
	{
		mNamespaces.insert(_namespace);
	}




	vmstring											thizClass;			// thisが指しているクラス(仮想関数ではthisがベースクラスのものになっているので注意)
	vmstring											clazz;				// 現在解析中のクラス名
	vmstring											currentNamespace;	// 現在の名前空間
	std::vector<vmclass::declere_value>					parameters;			// 現在解析中の引数タイプ
private:
	classmanager_type*									mVM;
	vmclass_manager										mCreatingClasses;	//! コンパイル中のクラス
	std::vector<std::function<comple_callback_type>>	mCompileCallbacks;
	std::deque<block_status>							mCodeBlocks;		//!< コードブロックの情報
	std::unordered_set<vmstring>						mNamespaces;
	ref<error>											mError;
	pp_clangex<compiler>								mTPP;


	bool implicit_cast(const vmstring& _dest, const vmstring& _src)
	{
		vmstring	to		= passion_remove_reference(_dest);
		vmstring	from	= passion_remove_reference(_src);

		// 同一型ならキャストするまでもない
		if (to == from)
		{
			return	true;
		}

		// クラスのキャスト
		if (passion_is_pointer(to) && passion_is_pointerable(from))
		{
			if (find_class(from.remove_pointer())->is_base_of(find_class(to.remove_pointer())->name()))
			{
				return	true;
			}
		}
		if (passion_is_gcpointer(to) && passion_is_gcpointer(from))
		{
			if (find_class(from.remove_pointer())->is_base_of(find_class(to.remove_pointer())->name()))
			{
				return	true;
			}
		}

		// 整数型
		auto	dsize_i	= passion_size_of_primitive(to);
		auto	ssize_i	= passion_size_of_primitive(from);
		if (dsize_i > 0 && ssize_i > 0 && dsize_i >= ssize_i)
		{
			return	true;
		}


		return	false;
	}
};
PASSION_NAMESPACE_END
#endif

