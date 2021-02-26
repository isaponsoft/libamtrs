/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__vmtypedata__hpp
#define	__libamtrs__passion__vmtypedata__hpp
#include <vector>
#include "string.hpp"
PASSION_NAMESPACE_BEGIN

struct	vmtypedata_impl;
struct	vmtypedata_interface;


// ****************************************************************************
//! 型データ
// ----------------------------------------------------------------------------
using	vmtypedata	= amtrs::basic_ref<vmtypedata_impl, vmtypedata_interface>;


// ****************************************************************************
//! 型データに対するインターフェース
// ----------------------------------------------------------------------------
struct	vmtypedata_interface : amtrs::ref_traits<vmtypedata_impl*>
{
	enum	types
	{
		undef		= 0,	//!< 未定義／定義途中
		fundamental	= 1,	//!< 基本型
		classes		= 2,	//!< ユーザー定義型
	};

	struct	member;

	vmtypedata_interface() = default;
	vmtypedata_interface(types _type, vmstring _name);

	void set_align(size_t _align);
	vmstring string() const noexcept;
	types type() const noexcept;

	//! メンバを追加する
	auto add_member(member _member) -> member&;

	//! 型の大きさ
	size_t size() const noexcept;


private:
	amtrs::basic_ref<vmtypedata_impl, vmtypedata_interface>& thiz() { return *static_cast<amtrs::basic_ref<vmtypedata_impl, vmtypedata_interface>*>(this); }
	amtrs::basic_ref<vmtypedata_impl, vmtypedata_interface> const& thiz() const { return *static_cast<amtrs::basic_ref<vmtypedata_impl, vmtypedata_interface> const*>(this); }
};


// ****************************************************************************
//! 型が持っているメンバ
// ----------------------------------------------------------------------------
struct	vmtypedata_interface::member
{
	vmtypedata	type;
	vmstring	name;
};


struct	vmtypedata_impl : amtrs::ref_object
{
	using	member	= vmtypedata_interface::member;

	size_t						align	= 0;	//!< アライメント
	vmtypedata_interface::types	type;
	vmstring					name;
	std::vector<member>			members;
};



inline vmtypedata_interface::vmtypedata_interface(types _type, vmstring _name)
{
	thiz().release();
	thiz()			= new vmtypedata_impl();
	thiz()->type	= _type;
	thiz()->name	= std::move(_name);
}


inline void vmtypedata_interface::set_align(size_t _align)
{
	thiz()->align	= _align;
}


inline vmstring vmtypedata_interface::string() const noexcept
{
	return	thiz().empty() ? vmstring() : thiz()->name;
}


inline auto vmtypedata_interface::type() const noexcept -> types
{
	return	thiz().empty() ? undef : thiz()->type;
}


inline auto vmtypedata_interface::add_member(member _member) -> member&
{
	auto&	members	= thiz()->members;
	members.push_back(std::move(_member));
	return	members.back();
}


inline size_t vmtypedata_interface::size() const noexcept
{
	size_t	s	= 0;
	if (s == 0)
	{
		++s;
	}
	return	thiz()->align ? ((s + thiz()->align - 1) & ~(s-1)) : s;
}

PASSION_NAMESPACE_END
#endif
