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


auto vn_address_cast::offset() const noexcept -> size_t
{
	for (auto& v : from->variables)
	{
		if (v.type == vdecl_class::_castposition && v.cls == to)
		{
			return	v.offset;
		}
	}
	return	0;
}


auto vn_address_memberref::offset() const noexcept -> size_t
{
	// 仮想関数の場合、仮想関数が所属するクラスから検索する
	if (scope)
	{
		intmax_t	off		= scope->variables[index_base_of].offset;
		intmax_t	base	= 0;
		for (auto& v : scope->variables)
		{
			if (v.type == vdecl_class::_castposition && v.cls == thiscls)
			{
				base	= v.offset;
				break;
			}
		}
		return	off - base;
	}
	// 非仮想関数なので見える範囲で検索する
	else
	{
		//AMTRS_FORCE_LOG("{}.{} {}", (text)thiscls->name, (text)thiscls->variables[index_base_of].name, index_base_of);
		return	thiscls->variables[index_base_of].offset;
	}
}


auto vn_expr_vcall::offset() const noexcept -> size_t
{
	return	cls->size_of_bases();
}


auto vdatatype::align_of() const noexcept -> size_t
{
	return	empty() ? 0 : mDecl->align_of();

}


auto vdatatype::can_cast(vdatatype const& _src) const noexcept -> casttypes
{
	return	empty() || _src.empty() ? casttypes::_nocast : mDecl->can_cast(_src);
}


bool vdatatype::is_const() const noexcept
{
	return	at<vdecl_const>() ? true : false;
}

bool vdatatype::is_scalar() const noexcept
{
	auto	t	= remove_reference();
	return	t.is_pointer()			? true
		:	t.at<vdecl_const>()		? t.at<vdecl_const>()->t.is_scalar()
		:	t.at<vdecl_scalar>()	? true : false;
}

bool vdatatype::is_pointer() const noexcept
{
	return	at<vdecl_pointer>() ? true : false;
}

bool vdatatype::is_reference() const noexcept
{
	return	at<vdecl_reference>() ? true : false;
}


vdatatype vdatatype::add_pointer() const
{
	return	new vdecl_pointer(*this, name() + "*");
}


vdatatype vdatatype::add_reference() const
{
	return	new vdecl_reference(*this, name() + "&");
}

vdatatype vdatatype::remove_const() const noexcept
{
	auto	decl	= at<vdecl_const>();
	if (!decl)
	{
		return	*this;
	}
	return	decl->t;
}

vdatatype vdatatype::remove_pointer() const noexcept
{
	auto	decl	= at<vdecl_pointer>();
	if (!decl)
	{
		return	*this;
	}
	return	decl->t;
}

vdatatype vdatatype::remove_reference() const noexcept
{
	auto	decl	= at<vdecl_reference>();
	if (!decl)
	{
		return	*this;
	}
	return	decl->t;
}


auto vdatatype::add_extents(size_t _size) -> vdatatype
{
	shared_string		t	= (text)format("{}[{}]", {(text)name(), (intmax_t)_size});
	ref<vdecl_array>	v	= new vdecl_array(*this, t);
	return	(vdatatype)v;
}


bool vdatatype::has_constructor() const noexcept
{
	auto	cls	= at<vdecl_class>();
	if (!cls)
	{
		return	false;
	}
	return	cls->constructors.size() > 0;
}


auto vdatatype::is_constructible(view<text> _arguments) const noexcept -> vdecl_function*
{
	auto	cls	= at<vdecl_class>();
	if (!cls)
	{
		return	nullptr;
	}
	// とりあえず型のテストなし
	for (auto& f : cls->constructors)
	{
		return	f;
	}
	return	nullptr;
}


auto vdatatype::is_destructible() const noexcept -> vdecl_function*
{
	auto	cls	= at<vdecl_class>();
	if (!cls)
	{
		return	nullptr;
	}
	return	cls->destractor.get();
}

auto vdatatype::size_of(bool _extract_ref) const noexcept -> size_t
{
	if (empty())
	{
		return	0;
	}
	if (_extract_ref && is_reference())
	{
		return	at<vdecl_reference>()->t.size_of(true);
	}
	return	mDecl->size_of();
}


auto vdecl_scalar::can_cast(vdatatype const& _src) const noexcept -> casttypes
{
	if (_src.is_scalar())
	{
		return	casttypes::_cast;
	}
	return	casttypes::_nocast;
}


size_t vdecl_array::size_of() const noexcept
{
	return	sizeof(void*);
}


auto vdecl_array::can_cast(vdatatype const& _src) const noexcept -> casttypes
{
	return	casttypes::_nocast;
}


size_t vdecl_pointer::size_of() const noexcept
{
	return	sizeof(void*);
}


size_t vdecl_pointer::align_of() const noexcept
{
	return	sizeof(void*);
}


auto vdecl_pointer::can_cast(vdatatype const& _src) const noexcept -> casttypes
{
	auto	src	= _src.remove_pointer();
	if (t.name() == src.name())
	{
		return	casttypes::_same;
	}
	return	casttypes::_nocast;
}


size_t vdecl_reference::size_of() const noexcept
{
	return	sizeof(void*);
}


size_t vdecl_reference::align_of() const noexcept
{
	return	sizeof(void*);
}


auto vdecl_reference::can_cast(vdatatype const& _src) const noexcept -> casttypes
{
	auto	src	= _src.remove_reference();
	if (t.name() == src.name())
	{
		return	casttypes::_same;
	}
	return	casttypes::_nocast;
}


auto vdecl_const::can_cast(vdatatype const& _src) const noexcept -> casttypes
{
	return	t.can_cast(_src.is_const() ? _src.remove_const() : _src);
}


size_t vdecl_const::align_of() const noexcept
{
	return	t.align_of();
}


vdecl_class::vdecl_class(shared_string _name)
	: vdecl_type(types::_class, std::move(_name))
{
}


vdecl_class::~vdecl_class()
{
}


auto vdecl_class::can_cast(vdatatype const& _src) const noexcept -> casttypes
{
	if (_src.is_reference())
	{
	}

	return	casttypes::_nocast;
}


size_t vdecl_class::align_of() const noexcept
{
	return	mAlignSize;
}


auto vdecl_class::layout() const -> simplearray<variable>
{
	simplearray<variable>	ret;
	for (auto& base : bases)
	{
		auto	l	= base.cls->layout();
		auto	idx	= ret.size();
		ret.push_back(l.begin(), l.end());
		for (auto i = idx; i < ret.size(); ++i)
		{
			ret[i].offset += base.offset;
		}
	}
	ret.push_back(variables.begin(), variables.end());
	return	ret;
}



size_t vdecl_class::size_of() const noexcept
{
	return	mSizeOf;
}


auto vdecl_class::find_function(text _name, text _signature) -> vdecl_function*
{
	for (auto& f : functions)
	{
		if (f->localname == _name && f->signature == _signature)
		{
			return	f;
		}
	}
	return	nullptr;
}


auto vdecl_class::find_member(text _name) -> member
{
	member	m;
	m.type		= vdecl_class::_none;
	m.index		= -1;
	m.p			= nullptr;
	m.cls		= this;

	text	name	= _name;
	text	scope	= nullptr;
	auto	seppos	= _name.rfind("::");
	if (seppos != text::npos)
	{
		scope	= _name.substr(0, seppos);
		name	= _name.substr(seppos + 2);
	}


	// 変数を検索
	{
		int	index	= 0;
		for (auto& v : variables)
		{
			if (v.name == name)
			{
				m.type		= vdecl_class::_variable;
				m.datatype	= v.datatype;
				m.index		= index;
				m.v			= &v;
				return	m;
			}
			++index;
		}
	}

	// 仮想関数を検索
	{
		int	index	= 0;
		for (auto& f : vtbl)
		{
			if (f->localname == name)
			{
				m.type		= vdecl_class::_vfunction;
				m.index		= index;
				m.f			= f;
				return	m;
			}
		}
		++index;
	}

	// 関数を検索
	{
		for (auto& f : functions)
		{
			if (f->localname == name)
			{
				m.type		= vdecl_class::_function;
				m.index		= 0;
				m.f			= f.get();
				return	m;
			}
		}
	}


	// ベースクラスから検索
	int	offset	= 0;
	for (auto& base : bases)
	{
		auto	r	= base.cls->find_member(_name);
		if (r.type != vdecl_class::_none)
		{
			r.index	+= offset;
			return	r;
		}
		offset += static_cast<int>(base.cls->variables_count());
	}

	return	m;
}



auto vdecl_class::variables_count() -> size_t
{
	size_t	sz	= 0;
	for (auto& base : bases)
	{
		sz += base.cls->variables_count();
	}
	return	sz + variables.size();
}


vdecl_function::vdecl_function(ref<pn_declare_function> _decl, vdecl_class* _cls, shared_string _name, vdatatype _rettype, shared_string _signature, simplearray<parameter> _params, pvlnode _stmt)
	: declare(std::move(_decl)), cls(_cls), name(std::move(_name)), rettype(std::move(_rettype)), signature(std::move(_signature)), parameters(std::move(_params)), statement(std::move(_stmt))
{
	auto	rpos	= ((text)name).rfind("::");
	if (rpos == text::npos)
	{
		localname	= name;
		if (_cls)
		{
			name		= _cls->name + "::" + localname;
		}
	}
	else
	{
		localname	= ((text)name).substr(rpos + 2);
	}

	{
		int		count	= 0;
		sstring	dname	= name + "(";
		for (auto& prm : parameters)
		{
			if (count++ > 0)	dname += ",";
			dname += prm.datatype.name();
		}
		dname += ")";
		display_name	= (text)dname;
	}
}

vdecl_function::~vdecl_function()
{
}




PSNVM_LANG_NAMESPACE_END
