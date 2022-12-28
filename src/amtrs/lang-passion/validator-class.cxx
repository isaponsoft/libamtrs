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




auto validator::add(ref<pn_declare_class> _decl) -> vdecl_class*
{
	shared_string				name	= make_symbol(_decl->name);
	ref<vdecl_class>			cls		= new vdecl_class(name);
	cls->declare	= _decl;
	classes.emplace(name, cls);
	return	cls;
}


auto validator::get_class(text _classname) -> vdatatype
{
	auto	it	= classes.find(_classname);
	if (it == classes.end())
	{
		if (_classname.ends_with(" const"))
		{
			auto					par		= get_class(_classname.substr(0, _classname.size() - 6));
			return	new vdecl_const(par, _classname);
		}
		if (_classname.ends_with("&"))
		{
			auto						par		= get_class(_classname.substr(0, _classname.size() - 1));
			return	new vdecl_reference(par, _classname);
		}
		if (_classname.ends_with("*"))
		{
			auto						par		= get_class(_classname.substr(0, _classname.size() - 1));
			return	new vdecl_pointer(par, _classname);
		}
		return	{};
	}
	return	it->second;
}


void validator::validate_class(vdecl_class* _cls)
{
	using				variable	= vdecl_class::variable;
	using				accesstypes	= vdecl_class::accesstypes;
	if (!_cls || _cls->is_validated())
	{
		return;
	}

	vdecl_class*		vcls		= _cls;
	pn_declare_class*	decl		= vcls->declare;
	if (!decl)
	{
		return;
	}

	bool				requireConstructor	= false;
	bool				requireDestructor	= false;
	uintptr_t			offset				= 0;
	uintptr_t			vtblindex			= 0;

	vcls->mAlignSize	= 0;
	vcls->mLastVtable	= nullptr;

	// Validate base classes.
	for (auto& base : decl->extends)
	{
		auto	clssym		= make_symbol(base.name);
		auto	bc			= get_class(clssym).cls();
		if (!bc)
		{
			AMTRS_FORCE_LOG("base class {} is not class.", (text)clssym);
			return;
		}
		auto	basecls		= static_cast<vdecl_class*>(bc);
		if (!basecls->is_validated())
		{
			auto	prev		= mCurrentClass;
			mCurrentClass		= _cls;
			validate_class(basecls);
			mCurrentClass		= prev;
		}
		vcls->bases.push_back(vdecl_class::base{ vdecl_class::accesstypes::_public, basecls, 0 });
	}

	// Vtblを持つクラスを先に配置する
	for (intmax_t i = 0; i < (intmax_t)vcls->bases.size(); ++i)
	{
		for (intmax_t j =  i + 1; j < (intmax_t)vcls->bases.size(); ++j)
		{
			if (vcls->bases[i].cls->vtbl.size() == 0 && vcls->bases[j].cls->vtbl.size() > 0)
			{
				std::swap(vcls->bases[i], vcls->bases[j]);
			}
		}
	}

	// Compiler traits test.
	struct	A
	{
		int32_t		vtbl;
		int16_t		a;
	};
	struct	B : A
	{
		int16_t		b;
	};
	struct	C : B
	{
		int16_t		c;
	};
	static constexpr bool	padding_compression	= sizeof(C) == 12;


	// Validate base classes.
	size_t					lastPadd	= 0;
	size_t					totalAlign	= 0;
	bool					lastHasVtbl	= false;
	for (auto& base : vcls->bases)
	{
		auto*	basecls	= base.cls.get();
		if (basecls->has_constructor())
		{
			requireConstructor	= true;
		}

		// Member variables.
		auto	alignOf		= basecls->align_of();
		if (totalAlign < alignOf)
		{
			totalAlign	= alignOf;
		}
		if constexpr (padding_compression)
		{
			offset	=  (offset + alignOf - 1) & ~(alignOf - 1);
		}
		else
		{
			vcls->mAlignSize	= totalAlign;
			offset	=  (offset + totalAlign - 1) & ~(totalAlign - 1);
		}

		auto		idx	= vcls->variables.size();
		auto		sz	= basecls->size_of_extends();
		vcls->variables.push_back(basecls->variables.begin(), basecls->variables.end());
		for (auto i = idx; i < vcls->variables.size(); ++i)
		{
			if (((text)vcls->variables[i].name).starts_with("$_vtbl"))
			{
				vcls->variables[i].vtblidx	+=  vtblindex;
			}
			vcls->variables[i].offset += offset;
		}
		lastPadd	=  basecls->size_of_padding();
		lastHasVtbl	=  basecls->vtbl.size() > 0;
		offset		+= sz;
		vtblindex	+= basecls->vtbl.size();
	}

	// 仮想関数テーブルを生成する
	size_t	baseSizes	= 0;
	for (auto& base : vcls->bases)
	{
		if (base.cls->vtbl.size() > 0)
		{
			vcls->vtbl.push_back(base.cls->vtbl.begin(), base.cls->vtbl.end());
			if (!vcls->mLastVtable)
			{
				vcls->mLastVtable	= base.cls->mLastVtable;
			}
		}
	}


	for (auto& f : vcls->functions)
	{
		f->thiscls	= vcls;
		if (!(f->spec & (uint32_t)pn_declare_specifier::spec_virtual))
		{
			continue;
		}
		if (vcls->mLastVtable)
		{
			f->thiscls	= vcls->mLastVtable;
		}
		bool	ovrd	= false;
		for (auto& vf : vcls->vtbl)
		{
			if (vf->localname == f->localname)
			{
				f->thiscls	= vf->thiscls;
				vf			= f;
				ovrd		= true;
				break;
			}
		}
		if (!ovrd)
		{
			vcls->vtbl.push_back(f.get());
		}
	}

	// 仮想関数テーブルがあるが
	// ベースクラスに仮想関数テーブルが無い場合は先頭に仮想関数テーブルを生成
	if ((vcls->vtbl.size() > 0) && !vcls->mLastVtable)
	{
		// vtbl 追加
		variable	v;
		v.type				= vdecl_class::_vtable;
		v.cls				= vcls;
		v.access			= vdecl_class::_invisivle;
		v.datatype			= get_class("void**");
		v.name				= (text)format("$_vtbl({})", {(text)vcls->name});
		auto	as	= v.datatype.align_of();
		auto	sz	=  v.datatype.size_of();
		v.offset	= 0;
		offset	+=  sz;
		vcls->mLastVtable	= vcls;
		vcls->mHasVtable	= true;
		vcls->variables.push_front(std::move(v));
		if (!padding_compression && vcls->mAlignSize < sizeof(void*))
		{
			vcls->mAlignSize	= sizeof(void*);
		}
		// 継承したメンバ変数のオフセット値を調整
		for (intmax_t i = 1; i < (intmax_t)vcls->variables.size(); ++i)
		{
			vcls->variables[i].offset += sz;
		}
		requireConstructor	= true;
	}

	// cast position
	{
		variable	v;
		v.type				= vdecl_class::_castposition;
		v.cls				= vcls;
		v.access			= vdecl_class::_invisivle;
		v.datatype			= (vdatatype)vcls;
		v.name				= (text)format("$_cast({})", {(text)vcls->name});
		vcls->variables.push_front(std::move(v));
	}

	// Validate member variables.
	for (auto& m : decl->members)
	{
		// member variables.
		if (m.type() == pn_declare_variable::type)
		{
			auto		dv			= m.at<pn_declare_variable>();
			auto		datatype	= make_symbol(dv->typesym);
			for (auto& s : dv->values)
			{
				variable	v;
				v.cls		= vcls;

				v.access	= dv->specifier & (uint32_t)pn_declare_specifier::spec_private   ? vdecl_class::_private
							: dv->specifier & (uint32_t)pn_declare_specifier::spec_protected ? vdecl_class::_protected
							:vdecl_class::_public;

				v.datatype	= get_class(datatype);
				v.name		= make_symbol(s.name);

				auto	as	= v.datatype.align_of();
				offset	=  (offset + as - 1) & ~(as - 1);
				v.offset	= offset;
				offset	+=  v.datatype.size_of();

				size_t		alignOf	= v.datatype.align_of();
				if (totalAlign < alignOf)
				{
					totalAlign	= alignOf;
				}
				if (vcls->mAlignSize < alignOf)
				{
					vcls->mAlignSize	= alignOf;
				}
				if (!s.init.empty())
				{
					v.init		= pvlnode::make<vn_tmpnode>(dv->location(), get_class("void"), s.init);
				}
				if (v.datatype.has_constructor() || !v.init.empty())
				{
					requireConstructor	= true;
				}
				vcls->variables.push_back(std::move(v));
			}
		}
	}

	if (vcls->mAlignSize == 0)
	{
		vcls->mAlignSize	= sizeof(int);
	}
	vcls->mSizeExtends	= (offset + vcls->mAlignSize - 1) & ~(vcls->mAlignSize - 1);	// アライメントを考慮したサイズ
	vcls->mSizeOf		= (offset + totalAlign - 1) & ~(totalAlign - 1);				// アライメントを考慮したサイズ
	// g++/clangでは vtbl は align に含まれないので size を調整する
	if (vcls->mLastVtable)
	{
		vcls->mSizeOf		= (offset + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
		vcls->mAlignSize	= sizeof(void*);
	}
	vcls->mPaddingSize	= vcls->mSizeOf - offset;										// アライメントのために追加されたサイズ

	// コンストラクタが必要だがコンストラクタを持っていない場合はデフォルトコンストラクタを生成する
	if (requireConstructor && !vcls->has_constructor())
	{
		pflnode									emp;
		ref<pn_declare_function_constractor>	decl	=
			pflnode::make<pn_declare_function_constractor>((text)nullptr, emp, emp, simplearray<pn_declare_function_constractor::initinfo>{}, emp, emp, 0);

		simplearray<pflnode>	stmts;
		stmts.push_back(pflnode::make<pn_stmt_return>((text)nullptr, pflnode()));
		decl->codes	= (pflnode)pflnode::make<pn_stmt_compound>((text)nullptr,  std::move(stmts));

		sstring		cname		= (text)_cls->name;
		text		nameonly	= cname;
		auto		pos			= nameonly.rfind("::");
		if (pos != text::npos)
		{
			nameonly	= nameonly.substr(pos + 2);
		}

		pvlnode		dcf	= pvlnode::make<vdecl_function>(
							(text)nullptr,
							vdatatype{},
							ref<pn_declare_function>(decl),
							vcls,
							(text)format("{}::{}", {(text)cname, (text)nameonly}),
							get_class("void"), (shared_string)"()void", simplearray<vdecl_function::parameter>{}, pvlnode());

		auto		ret		= functions.try_emplace(dcf.at<vdecl_function>()->name, simplearray<ref<vdecl_function>>{});
		ret.first->second.push_back(dcf.at<vdecl_function>());
		vcls->constructors.push_back(dcf.at<vdecl_function>());
	}
	vcls->validated	= true;
}

auto vdecl_class::member::access() const noexcept -> accesstypes
{
	if (type == _function || type == _vfunction)
	{
		return	f->access;
	}
	if (type == _variable)
	{
		return	v->access;
	}
	return	accesstypes::_invisivle;
}

shared_string vdecl_class::member::name() const noexcept
{
	if (type == membertypes::_function || type == membertypes::_vfunction)
	{
		return	f->localname;
	}
	if (type == membertypes::_variable)
	{
		return	v->name;
	}
	return	{};
}



PSNVM_LANG_NAMESPACE_END

