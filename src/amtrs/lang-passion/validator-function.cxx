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

auto validator::add(ref<pn_declare_function> _decl) -> vdecl_function*
{
	shared_string				name	= make_symbol(_decl->name);
	if (mCurrentClass)
	{
		name	= mCurrentClass->name + "::" + name;
	}
	auto						ftbl	= functions.try_emplace(name, simplearray<ref<vdecl_function>>{});
	name	= ftbl.first->first;


	// 引数リストを生成する
	simplearray<shared_string>	params;
	for (auto& p : _decl->parameters.at<pn_parameter_list>()->parameters)
	{
		auto	s	= make_symbol(p.first);
		params.push_back(std::move(s));
	}

	using					parameter	= vdecl_function::parameter;
	vdatatype				rettype;
	shared_string			signature;
	simplearray<parameter>	parameters;


	// 戻り値の型
	rettype		= get_class(make_symbol(_decl->rettype));

	int	count	= 0;
	signature	= "(";

	// パラメータ分析
	for (auto& p : _decl->parameters.at<pn_parameter_list>()->parameters)
	{
		parameter	prm;
		prm.datatype	= get_class(make_symbol(p.first));
		if (!p.second.empty())
		{
			prm.name		= make_symbol(p.second);
		}
		if (!p.init)
		{
			prm.defvalue	= validate(p.init);
		}
		if (count++ > 0)	signature += ",";
		signature += prm.datatype.name();

		parameters.push_back(std::move(prm));
	}

	// シグネチャ生成
	signature += ")";
	signature += rettype.name();

	// 修飾子
	unsigned int	spec		= _decl->specifier;

	// 同パラメータの関数があるか調べる
	bool			prototype	= _decl->codes.type() == pflnode_types::none;
	for (auto& of : ftbl.first->second)
	{
		if (signature != of->signature)
		{
			continue;
		}
		// 自身がプロトタイプなら何もしない
		if (prototype)
		{
			return	nullptr;
		}

		// すでに本体が登録されているなら２重定義なのでエラー
		if (!of->is_prototype())
		{
			int		count		= 0;
			sstring	signature 	= name + "(";
			for (auto& a : of->parameters)
			{
				if (count++ > 0)	signature += ",";
				signature += a.datatype.name();
			}
			signature += ")";

			errorinfo	err(_decl->location(), errorcode::ec_declare_function_duplicate, (text)signature, {});
			err.ambitiouses.push_back({ of->location, signature });
			errors.push_back(std::move(err));
			return	nullptr;
		}

		// 上書き
		pvlnode	vd	= pvlnode::make<vdecl_function>(_decl->location(), vdatatype{}, _decl, mCurrentClass, name, rettype, signature, std::move(parameters), pvlnode());
		vd.at<vdecl_function>()->spec	= spec;
		of	= vd.at<vdecl_function>();
		if (mCurrentClass)
		{
			for (auto& cf : mCurrentClass->functions)
			{
				if (cf == of)
				{
					cf	= vd.at<vdecl_function>();
				}
			}
		}
		return	vd.at<vdecl_function>();
	}

	pvlnode			vd	= pvlnode::make<vdecl_function>(_decl->location(), vdatatype{}, _decl, mCurrentClass, name, rettype, signature, std::move(parameters), pvlnode());
	vdecl_function*	vf	= vd.at<vdecl_function>();
	vf->access	= spec & (uint32_t)pn_declare_specifier::spec_private   ? vdecl_function::accesstypes::_private
				: spec & (uint32_t)pn_declare_specifier::spec_protected ? vdecl_function::accesstypes::_protected
				: vdecl_function::accesstypes::_public;
	vf->spec	= spec;
	ftbl.first->second.push_back(vf);
	if (mCurrentClass)
	{
		mCurrentClass->functions.push_back(vf);
	}
	return	vd.at<vdecl_function>();
}


auto validator::add(ref<pn_declare_function_constractor> _decl) -> vdecl_function*
{
	_decl->rettype	= (pflnode)pflnode::make<pn_symbol_name>(_decl->location(), "void");

	vdecl_function*	f	= add(static_cast<pn_declare_function*>(_decl));
	if (f)
	{
		mCurrentClass->constructors.push_back(f);
	}
	return	f;
}


auto validator::add(ref<pn_declare_function_destractor> _decl) -> vdecl_function*
{
	shared_string				name	= make_symbol(_decl->name);
	auto						ret		= functions.emplace(mCurrentClass->name + "::" + name, simplearray<ref<vdecl_function>>{});
	name	= ret.first->first;

	// 引数リストを生成する
	simplearray<shared_string>	params;
	for (auto& p : _decl->parameters.at<pn_parameter_list>()->parameters)
	{
		auto	s	= make_symbol(p.first);
		params.push_back(std::move(s));
	}

	using					parameter	= vdecl_function::parameter;
	shared_string			signature;
	simplearray<parameter>	parameters;

	int	count	= 0;
	signature	= "(";

	// パラメータ分析
	for (auto& p : _decl->parameters.at<pn_parameter_list>()->parameters)
	{
		parameter	prm;
		prm.datatype	= get_class(make_symbol(p.first));
		prm.name		= make_symbol(p.second);
		prm.defvalue	= validate(p.init);

		if (count++ > 0)	signature += ",";
		signature += prm.datatype.name();

		parameters.push_back(std::move(prm));
	}

	// シグネチャ生成
	signature += ")";

	pvlnode	vd	= pvlnode::make<vdecl_function>(_decl->location(), vdatatype{}, _decl, mCurrentClass, name, get_class("void"), signature, std::move(parameters), pvlnode());
	auto	fn	= vd.at<vdecl_function>();
	ret.first->second.push_back(fn);

	mCurrentClass->destractor	= fn;
	mCurrentClass->functions.push_back(vd.at<vdecl_function>());
	return	vd.at<vdecl_function>();
}


auto validator::add(ref<pn_declare_function_operator> _decl) -> vdecl_function*
{
	shared_string				name	= shared_string("operator") + get_operator_string(_decl->ope);
	auto						ret		= functions.try_emplace(mCurrentClass ? (mCurrentClass->name + "::" + name) : name, simplearray<ref<vdecl_function>>{});
	name	= ret.first->first;

	// 引数リストを生成する
	simplearray<shared_string>	params;
	for (auto& p : _decl->parameters.at<pn_parameter_list>()->parameters)
	{
		auto	s	= make_symbol(p.first);
		params.push_back(std::move(s));
	}

	using					parameter	= vdecl_function::parameter;
	vdatatype				rettype;
	shared_string			signature;
	simplearray<parameter>	parameters;


	// 戻り値の型
	rettype		= get_class(make_symbol(_decl->rettype));

	int	count	= 0;
	signature	= "(";

	// パラメータ分析
	for (auto& p : _decl->parameters.at<pn_parameter_list>()->parameters)
	{
		parameter	prm;
		prm.datatype	= get_class(make_symbol(p.first));
		prm.name		= make_symbol(p.second);
		prm.defvalue	= validate(p.init);

		if (count++ > 0)	signature += ",";
		signature += prm.datatype.name();

		parameters.push_back(std::move(prm));
	}

	// シグネチャ生成
	signature += ")";
	signature += rettype.name();

	pvlnode	vd	= pvlnode::make<vdecl_function>(_decl->location(), vdatatype{}, _decl, mCurrentClass, name, rettype, signature, std::move(parameters), pvlnode());
	auto	fn	= vd.at<vdecl_function>();
	ret.first->second.push_back(fn);
	if (mCurrentClass)
	{
		mCurrentClass->functions.push_back(vd.at<vdecl_function>());
	}
	return	vd.at<vdecl_function>();
}


auto validator::add(text _name, text _rettype, std::initializer_list<text> _parameters, vdecl_function::nativehook* _native) -> vdecl_function*
{
	// シグネチャ生成
	shared_string								signature;
	simplearray<vdecl_function::parameter>	parameters;
	vdatatype									rettype	= get_class(_rettype);

	int	count	= 0;
	signature	= "(";
	for (auto& p : _parameters)
	{
		vdecl_function::parameter	prm;
		prm.datatype	= get_class(p);
		prm.name		= (text)format("argv{}", {count});

		if (count++ > 0)	signature += ",";
		signature += prm.datatype.name();
		parameters.push_back(std::move(prm));
	}
	signature += ")";
	signature += rettype.name();

	pvlnode	vd	= pvlnode::make<vdecl_function>(nullptr, vdatatype{}, nullptr, nullptr, _name, rettype, signature, std::move(parameters), pvlnode());
	auto	fn	= vd.at<vdecl_function>();
	fn->native	= _native;

	auto	ret	= functions.try_emplace(fn->name, simplearray<ref<vdecl_function>>{});
	ret.first->second.push_back(fn);

	return	vd.at<vdecl_function>();
}


bool validator::can_access(vdecl_function* _func)
{
	// private, protected 以外は常にアクセス可能
	if (_func->access == vdecl_class::_public)
	{
		return	true;
	}

	if (!mCurrentClass)
	{
		return	false;
	}
	

	return	false;
}


auto validator::find_function(text _scope, text _name) -> simplearray<ref<vdecl_function>>*
{
	auto	fullname	= format("{}::{}", {_scope, _name});
	if (((text)fullname).starts_with("::"))
	{
		fullname	= ((text)fullname).substr(2);
	}
	auto	it	= functions.find((text)fullname);
	if (it != functions.end())
	{
		return	&it->second;
	}

	// 親スコープの指定がないならこれ以上検索できない
	if (_scope.empty())
	{
		return	nullptr;
	}

	// クラスがあればベースクラスの名前に切り替えて検索する
	auto	cls		= get_class(_scope);
	if (!cls.empty())
	{
		for (auto& base : static_cast<vdecl_class*>(cls.cls())->bases)
		{
			// ベースクラスの親を検索しないようにスコープは empty にする
			auto	m	= find_function(nullptr, (text)format("{}::{}", {(text)base.cls->name, _name}));
			if (m)
			{
				return	m;
			}
		}
	}

	// スコープを1段上げる
	auto	ppos	= _scope.rfind("::");
	if (ppos == text::npos)
	{
		return	nullptr;
	}

	return	find_function(_scope.substr(0, ppos), _name);
}


auto validator::find_function(text _name, view<vdatatype> _args) -> vdecl_function*
{
	// まずは名前で検索
	auto	it	= functions.find(_name);
	if (it == functions.end())
	{
		return	nullptr;
	}

	vdecl_function*	ret	= nullptr;
	for (auto& f : it->second)
	{
		// 引数が一致するか確認する
		auto&					paramlist	= f->parameters;
		bool					match		= true;
		if (_args.size() > paramlist.size())
		{
			continue;
		}
		for (int i = 0; i < (int)paramlist.size(); ++i)
		{
			if (i >= _args.size())
			{
				match	= false;
				break;
			}
			if (!paramlist[i].datatype.can_cast(_args[i]))
			{
				match	= false;
				break;
			}
		}
		if (!match)
		{
			continue;
		}
		ret	= f;
	}
	if (!ret)
	{
//		AMTRS_FORCE_LOG("unmatch function {}", _name);
	}
	return	ret;
}


void validator::validate_function(vdecl_function* e)
{
	using					parameter	= vdecl_function::parameter;
	if (e->native) 			return;
	if (e->is_prototype())	return;

	// メンバ関数ならベースクラスから同名の関数があるか調べる
	simplearray<vdecl_function*>	ambs;
	if (e->cls)
	{
		for (auto& base : e->cls->bases)
		{
			auto	f	= base.cls->find_function(e->localname, e->signature);
			if (f)
			{
				ambs.push_back(f);
			}
		}
		if (ambs.size() >= 2)
		{
			errorinfo	err(e->location, errorcode::ec_declare_function_ambitious_override_base, e->display_name);
			for (auto& amb : ambs)
			{
				err.ambitiouses.push_back({ amb->location, amb->display_name });
			}
			errors.push_back(std::move(err));
			return;
		}
	}

	// override がある場合は
	if (ambs.empty() && (e->spec & (uint32_t)pn_declare_specifier::spec_virt_override))
	{
		errors.push_back(errorinfo(e->location, errorcode::ec_declare_function_nothing_override_base, e->display_name));
		return;
	}

	// base に final がある場合
	if (!ambs.empty() && ambs.front()->spec & (uint32_t)pn_declare_specifier::spec_virt_final)
	{
		errorinfo	err(e->location, errorcode::ec_declare_function_final_override, e->display_name);
		for (auto& amb : ambs)
		{
			err.ambitiouses.push_back({ amb->location, amb->display_name });
		}
		errors.push_back(std::move(err));
		return;
	}


	// プログラムコード
	codestate	cs			= mState;
	block_in();
	state().func			= e;
	simplearray<pvlnode>	inits;

	// コンストラクタであればメンバの初期化を行う初期化コードを生成する
	if (static_cast<pflnodebase*>(e->declare)->type() == pn_declare_function_constractor::type)
	{
		inits	= validate_function_constractor(e);
	}

	pvlnode					statement	= validate(e->declare->codes);
	if (bad())
	{
		return;
	}

	// marge inits
	if (inits.size() > 0)
	{
		auto	comp	= statement.at<vn_compound>();
		if (comp)
		{
			inits.push_back(comp->statements.begin(), comp->statements.end());
			comp->statements	= std::move(inits);
		}
		else
		{
			inits.push_back(std::move(statement));
			statement	= pvlnode::make<vn_compound>(e->declare->location(), get_class("void"), std::move(inits));
		}
	}
	if (!mState.is_return)
	{
		if (!e->rettype.name().empty() && e->rettype.name() != "void")
		{
			errors.push_back(errorinfo(e->location, errorcode::ec_declare_function_nothing_return, e->rettype.name()));
			return;
		}
		auto	comp	= statement.at<vn_compound>();
		free_locals(comp->statements, statement.location(), 0);
		comp->statements.push_back(pvlnode::make<vn_return>(statement.location(), get_class("void")));
	}

	blockstate	st			= block_out();
	mState	= cs;
	e->statement			= std::move(statement);
}


// コンストラクタ
auto validator::validate_function_constractor(vdecl_function* e) -> simplearray<pvlnode>
{
	auto					cls		= e->cls;
	auto*					cf		= static_cast<pn_declare_function_constractor*>(e->declare.get());
	pvlnode					thiz	= pvlnode::make<vn_this>(cf->location(), ((vdatatype)cls).add_reference());
	simplearray<pvlnode>	initlist;

	// 初期化リストの整合性調査
	for (auto& i : cf->initializers)
	{
		bool	match	= false;
		auto	sym		= make_symbol(i.name);
		for (auto& base : cls->bases)
		{
			match = base.cls->name == sym;
			if (match)	break;
		}
		if (match) continue;
		for (auto& m : cls->variables)
		{
			match = m.name == sym;
			if (match)	break;
		}
		if (!match)
		{
			errors.push_back(errorinfo(i.name.location(), errorcode::ec_declare_constractor_intializers_unkown_symbol, i.name.location()));
			return	{};
		}
	}

	// ベースクラスの初期化
	intmax_t				vtblindex	= 0;
	for (auto& base : cls->bases)
	{
		// コンストラクタが必要ならコンストラクタを呼び出す
		if (!base.cls->has_constructor())
		{
			continue;
		}

		auto						f	= base.cls->constructors.front();
		vdatatype					ct	= vdatatype(base.cls);
		pvlnode						obj	= pvlnode::make<vn_address_cast>(cf->location(), ct.add_reference(), base.cls, cls, thiz);
		simplearray<pvlnode>		argv;

		// 初期化リストにあれば引数追加
		for (auto& i : cf->initializers)
		{
			if (make_symbol(i.name) == base.cls->name)
			{
				for (auto& v : i.init)
				{
					argv.push_back(validate(v));
				}
				break;
			}
		}

		for (size_t i = 0; i < argv.size(); ++i)
		{
			argv[i] = cast_to(f->parameters[i].datatype, argv[i]);
		}

		// ベースクラスのコンストラクタを追加
		argv.push_front(std::move(obj));
		initlist.push_back(pvlnode::make<vn_expr_call>(cf->location(), get_class("void"), f, std::move(argv)));

		// vtblの初期化を追加する
		if (base.cls->vtbl.size() > 0)
		{
			auto	obj			= pvlnode::make<vn_address_cast>(cf->location(), get_class("void*&"), base.cls, cls, thiz);
			auto	vtbladdr	= pvlnode::make<vn_vtable>(cf->location(), get_class("void*"), cls, vtblindex);
			auto	assign		= pvlnode::make<vn_assign>(cf->location(), get_class("void*"), obj, std::move(vtbladdr));
			initlist.push_back(assign);
		}

		vtblindex	+= base.cls->vtbl.size();
	}
	intmax_t	mindex	= 0;
	
	// ベースクラスにvtblが存在しないのでvtbl初期化式を追加する
	if (cls->mLastVtable == cls)
	{
		auto	vtbladdr	= pvlnode::make<vn_vtable>(cf->location(), get_class("void*"), cls, 0);
		auto	obj			= pvlnode::make<vn_address_memberref>(cf->location(), get_class("void*&"), thiz, mCurrentClass, cls, vtblindex);
		auto	assign		= pvlnode::make<vn_assign>(cf->location(), get_class("void*"), obj, std::move(vtbladdr));
		initlist.push_back(assign);
	}

	// メンバ変数の初期化
	for (auto& m : cls->variables)
	{
		pvlnode	init;
		if (m.cls != cls)
		{
			++mindex;
			continue;
		}

		// 初期化リストに候補があれば初期化リストを優先
		for (auto& i : cf->initializers)
		{
			if (make_symbol(i.name) == m.name)
			{
				// コンストラクタがあればコンストラクタを使用する
				;
				// メンバ変数に代入を行う
				pvlnode	obj	= pvlnode::make<vn_address_memberref>(cf->location(), m.datatype.add_reference(), thiz, mCurrentClass, cls, mindex);
				pvlnode	src	= cast_to(m.datatype, validate(i.init[0]));
				init	= pvlnode::make<vn_assign>(cf->location(), obj.datatype(), std::move(obj), std::move(src));
				break;
			}
		}

		// 初期化リストに無く、デフォルト値があるならそちらを使う
		if (init.empty() && !m.init.empty())
		{
			pvlnode	obj	= pvlnode::make<vn_address_memberref>(cf->location(), m.datatype.add_reference(), thiz, mCurrentClass, cls, mindex);
			pflnode	val	= m.init.at<vn_tmpnode>()->expr;
			pvlnode	src	= cast_to(m.datatype, validate(val));
			init	= pvlnode::make<vn_assign>(cf->location(), obj.datatype(), std::move(obj), std::move(src));
		}

		if (!init.empty())
		{
			initlist.push_back(std::move(init));
		}

		++mindex;
	}
	return	initlist;
}



PSNVM_LANG_NAMESPACE_END

