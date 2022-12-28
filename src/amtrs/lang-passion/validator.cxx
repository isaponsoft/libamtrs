/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * 汎用ライブラリ
 * OSや環境問わずに使用できる。
 */
PSNVM_LANG_NAMESPACE_BEGIN
using namespace amtrs;
using namespace psnvm;
using namespace psnvm::lang;

namespace {
	struct	vn_tmpnode : vnodebase
	{
		static constexpr vnodetypes	type	= vnodetypes::_tmpnode;

		pflnode	expr;

		vn_tmpnode(pflnode _e) : expr(std::move(_e)) {}
	};
}


validator::validator()
{
	reset();
}


validator::~validator()
{
}


void validator::reset()
{
	localvars.clear();
	classes.clear();
	errors.clear();
	functions.clear();

	mCurrentClass	= nullptr;
	mBlockState.clear();
	mState	= {};



	mBlockState.push_back({});

	auto	make	= [this](shared_string n, size_t _size)
	{
		classes.emplace(n, new vdecl_scalar(n, _size));
	};


	// スカラ
	make("void",		0);
	make("bool",		1);
	make("byte",		1);
	make("int8_t",		1);
	make("uint8_t",		1);
	make("int16_t",		2);
	make("uint16_t",	2);
	make("int32_t",		4);
	make("uint32_t",	4);
	make("char",		1);
	make("short",		2);
	make("int",			sizeof(int));
	make("long",		sizeof(long));
	make("float",		sizeof(float));
	make("double",		sizeof(double));
	make("size_t",		sizeof(size_t));
	make("intmax_t",	sizeof(intmax_t));
	make("uintmax_t",	sizeof(uintmax_t));
	make("vint",		sizeof(intmax_t));


	// 組み込み文字列型
	ref<vdecl_class>	sstr	= new vdecl_class("psn::constring");
	classes.emplace(sstr->name, sstr);
}



bool validator::can_access(vdecl_class::member const& _member)
{
	auto	access	= _member.access();
	if (access == vdecl_class::_public)
	{
		return	true;
	}

	if (!mCurrentClass)
	{
		return	false;
	}


	return	false;
}



auto validator::cast_to(vdatatype const& _to, pvlnode _from) -> pvlnode
{
	// 同型なら未変換
	if (_to.name() == _from.datatype().name())
	{
		return	_from;
	}

	// 参照を外すだけで良い場合
	if (_to.name() == _from.datatype().remove_reference().name())
	{
		auto	to	= _from.datatype().remove_reference();
		return	pvlnode::make<vn_load_integral>(_from.location(), to, std::move(_from));
	}

	// 参照
	if (_to.is_reference())
	{
		auto	toType	= _to.remove_reference();
		bool	toConst	= toType.is_const();
		if (toConst)
		{
			toType	= toType.remove_const();
		}
		if (toType.name() == _from.datatype().remove_reference().name())
		{
			return	_from;
		}


		// initializer list による初期化
		auto	srcinit	= _from.at<vn_value_initializerlist>();
		if (srcinit)
		{
			AMTRS_FORCE_LOG("initializer list");
		}
	}


	// スカラへの変換
	if (_to.is_scalar())// == _from.datatype().remove_reference().is_scalar())
	{
		// 元がスカラ
		if (_from.datatype().remove_reference().is_scalar())
		{
			if (_from.datatype().is_reference())
			{
				return	pvlnode::make<vn_load_integral>(_from.location(), _from.datatype().remove_reference(), std::move(_from));
			}
		}
	}
	return	{};
}


auto validator::make_symbol(pflnode& _symbol) -> shared_string
{
	auto	make = [this](auto self, sstring const& _scope, pflnode& _n) -> sstring
	{
		sstring		r;
		auto		t	= _n.type();
		if (t == pn_symbol_name::type)
		{
			r	= _n.at<pn_symbol_name>()->name;
		}
		else if (t == pn_symbol_scope::type)
		{
			for (auto& s : _n.at<pn_symbol_scope>()->scopes)
			{
				r	= self(self, r, s);
			}
		}
		else if (t == pn_spec_symbol_reference::type)
		{
			r	= self(self, r, _n.at<pn_spec_symbol_reference>()->name) + "&";
		}
		else if (t == pn_spec_symbol_pointer::type)
		{
			r	= self(self, r, _n.at<pn_spec_symbol_pointer>()->name) + "*";
		}
		else if (t == pn_spec_symbol_const::type)
		{
			r	= self(self, r, _n.at<pn_spec_symbol_const>()->name) + " const";
		}
		else
		{
			AMTRS_FORCE_LOG("NODE ERROR {} `{}`", (int)_n.type(), _n.location());
		}
		if (!_scope.empty())
		{
			r	= _scope + "::" + r;
		}
		return	r;
	};
	return	(text)make(make, "", _symbol);
}


auto validator::declare(pflnode& _n) -> bool
{
	switch (_n.type())
	{
		case pn_declare_class::type :
		{
			auto	old	= mCurrentClass;
			auto	n	= _n.at<pn_declare_class>();
			mCurrentClass	= add(n);
			for (auto& m : n->members)
			{
				declare(m);
			}
			mCurrentClass	= old;
			return	true;
		}

		case pn_declare_function::type :
		{
			auto	n	= _n.at<pn_declare_function>();
			add(n);
			declare(n->codes);
			return	true;
		}

		case pn_declare_function_constractor::type :
		{
			auto	n	= _n.at<pn_declare_function_constractor>();
			add(n);
			declare(n->codes);
			return	true;
		}

		case pn_declare_function_destractor::type :
		{
			auto	n	= _n.at<pn_declare_function_destractor>();
			add(n);
			declare(n->codes);
			return	true;
		}

		case pn_declare_function_operator::type :
		{
			auto	n	= _n.at<pn_declare_function_operator>();
			add(n);
			declare(n->codes);
			return	true;
		}

		default :
		{
			return	false;
		}
	}
}


auto	validator::expr_assign(pvlnode& _dst, pvlnode& _src) -> pvlnode
{
	auto	rettype	= _dst.datatype();

	auto	srctype	= _src.datatype();
	if (!rettype.is_reference())
	{
		return	{};
	}


	// 右辺値が initializer list
	auto	il	= _src.at<vn_value_initializerlist>();
	if (il)
	{
		auto	clstype	= rettype.remove_reference();
		// コンストラクタがあるなら初期化リストが適合するか調べる
		if (clstype.is_constructible())
		{
		}
		// スカラではないのならメンバ変数に適合できるか調べる
		else if  (!clstype.is_scalar())
		{
			auto	cls	= clstype.at<vdecl_class>();
			// 初期化する値が多い場合は不適合エラー
			if (il->values.size() > cls->variables.size())
			{
				AMTRS_FORCE_LOG("ERROR");
				return	{};
			}

			int						mindex	= -1;
			int						vindex	= 0;
			simplearray<pvlnode>	initlist;
			for (auto& m : cls->variables)
			{
				++mindex;
				if (m.type != vdecl_class::_value)
				{
					continue;
				}

				pvlnode	init;

				// 初期化リストに候補があれば初期化リストを優先
				for (auto& i : il->values)
				{
					if (i.name == m.name)
					{
						// コンストラクタがあればコンストラクタを使用する
						;

						// メンバ変数に代入を行う
						pvlnode	obj	= pvlnode::make<vn_address_memberref>(_src.location(), m.datatype.add_reference(), _dst, mCurrentClass, cls, mindex);
						pvlnode	src	= cast_to(m.datatype, i.value);
						if (bad())
						{
							return	{};
						}
						init	= pvlnode::make<vn_assign>(_src.location(), obj.datatype(), std::move(obj), std::move(src));
						break;
					}
				}
				if (init.empty())
				{
					pvlnode	obj	= pvlnode::make<vn_address_memberref>(_src.location(), m.datatype.add_reference(), _dst, mCurrentClass, cls, mindex);
					pvlnode	src	= cast_to(m.datatype, il->values[vindex++].value);
					if (bad())
					{
						return	{};
					}
					init	= pvlnode::make<vn_assign>(_src.location(), obj.datatype(), std::move(obj), std::move(src));
				}
				if (!init.empty())
				{
					initlist.push_back(std::move(init));
				}
			}
			return	pvlnode::make<vn_compound>(_src.location(), get_class("void"), std::move(initlist));
		}
	}


	// ソースデータを読み取る
	if (_src.datatype().is_reference())
	{
		_src	= pvlnode::make<vn_load_integral>(_src.location(), srctype.remove_reference(), std::move(_src));
	}


	vdatatype	dsttype	= _dst.datatype().remove_reference();
	// ポインターからポインターであればキャストの必要性を確認する
	if (dsttype.is_pointer() && _src.datatype().is_pointer())
	{
		auto	dt	= dsttype.remove_pointer();
		auto	st	= _src.datatype().remove_pointer();
		if (dt != st)
		{
			auto	dcls	= static_cast<vdecl_class*>(dt.cls());
			auto	scls	= static_cast<vdecl_class*>(st.cls());
			_src	= pvlnode::make<vn_address_cast>(_dst.location(), dt.add_pointer(), dcls, scls, _src);
		}
	}
	return	pvlnode::make<vn_assign>(_dst.location(), rettype, std::move(_dst), std::move(_src));
}


// _sizeで指定する数のローカル変数になるように調整します
void validator::free_locals(simplearray<pvlnode>& _statements, text _location, size_t _size)
{
	if (localvars.size() <= _size || mState.is_return)
	{
		return;
	}

	// デストラクタがあればデストラクタを呼び出す
	int	localindex	= (int)localvars.size();
	for (auto it = localvars.rbegin(); it != localvars.rend(); ++it)
	{
		--localindex;
		auto	f	= (*it)->datatype.is_destructible();
		if (!f) continue;
		pvlnode	obj	= pvlnode::make<vn_address_localref>(_location, (*it)->datatype.add_reference(), localindex);
		simplearray<pvlnode>		argv;
		argv.push_back(std::move(obj));
		_statements.push_back(pvlnode::make<vn_expr_call>(_location, get_class("void"), f, std::move(argv)));
	}

	// 解放式を生成する
	simplearray<ref<vn_local_free::varstate>>	freelist;
	while (localvars.size() > _size)
	{
		ref<vn_local_free::varstate>	v	= new vn_local_free::varstate();
		v->datatype	= localvars.back()->datatype;
		v->name		= localvars.back()->name;
		freelist.push_back(std::move(v));
		localvars.pop_back();
	}
	_statements.push_back(pvlnode::make<vn_local_free>(_location, get_class("void"), std::move(freelist)));
}



auto validator::validate(text _src, sourcemode _setting) -> pvlnode
{
	psn		p;
	p.mode	=  _setting;
	auto		codes	= p.parse(_src);
	return	validate(codes.data(), codes.size());
}


auto validator::validate(pflnode* _nodes, size_t _size) -> pvlnode
{
	simplearray<pflnode>	globalcodes;
	simplearray<pvlnode>	ret;

	// phas1 まずは関数やクラスの宣言を展開する
	for (auto& n : view<pflnode>(_nodes, _size))
	{
		if (!declare(n))
		{
			// declare 以外のコードはグローバルコードとして扱う
			globalcodes.push_back(n);
		}
	}

	// phase2 クラスの展開、ベースクラスとメンバの解決
	for (auto& cls : classes)
	{
		mCurrentClass	= cls.second.at<vdecl_class>();
		validate_class(mCurrentClass);
		mCurrentClass	= nullptr;
	}

	// テスト（メモリレイアウトのダンプ）
#if	0
	for (auto& cls : classes)
	{
		auto		c		= cls.second.at<vdecl_class>();
		if (!c) { continue; }
		std::cout << "----------------------------" << std::endl;
		std::cout << "class " << (text)c->name << std::endl;

		std::cout << " vars" << std::endl;
		for (auto& l : c->variables)
		{
			size_t	sz	= l.datatype.size_of();
			std::cout << (text)format("  +{04X} {2X} {-14s} {}", { l.offset, sz, (text)l.datatype.name(), (text)l.name });
			if (((text)l.name).starts_with("$_vtbl"))
			{
				std::cout << " + " << l.vtblidx;
			}
			std::cout << std::endl;
		}
		std::cout << (text)format("  size : {1$d}({1$04X}), align : {2$d}({2$04X})", {c->size_of(), c->align_of()}) << std::endl;
		if (c->castoffset.size() > 0)
		{
			std::cout << std::endl;
			std::cout << " cast" << std::endl;
			for (auto& cpos : c->castoffset)
			{
				std::cout << (text)format("  {04X} {}", { cpos.offset, (text)cpos.cls->name }) << std::endl;
			}
		}
		if (c->vtbl.size() > 0)
		{
			std::cout << std::endl;
			std::cout << " vtbl" << std::endl;
			for (auto& vf : c->vtbl)
			{
				std::cout << (text)format("  {}", { (text)vf->name }) << std::endl;
			}
		}
		std::cout << std::endl;
	}
#endif


	// phase3 各関数を展開する
	for (auto& func : functions)
	{
		for (auto& e : func.second)
		{
			mCurrentClass	= e->cls;
			validate_function(e);
			mCurrentClass	= nullptr;
		}
	}

	if (globalcodes.empty())
	{
		return	{};
	}

	// @pase4 グローバルスコープのコードをcompoundにまとめたうえで展開する
	auto	gc	= (pflnode)pflnode::make<pn_stmt_compound>(globalcodes.front().location(), std::move(globalcodes));
	return	validate(gc);
}




PSNVM_LANG_NAMESPACE_END
