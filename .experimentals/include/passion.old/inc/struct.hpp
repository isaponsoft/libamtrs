/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__struct__hpp
#define	__passion__struct__hpp
#include <amtrs/memory/@>
PASSION_NAMESPACE_BEGIN


using namespace amtrs;
using	passion_string		= std::string;

enum	passion_member_attribute
{
	attr_public		= 0,
	attr_protected	= 1,
	attr_private	= 2,
};


struct	passion_size
{
	size_t	byte_size;			//!< 固定バイト長
	size_t	pointer_count;		//!< ポインターの数

	passion_size& operator += (const passion_size& _r) noexcept
	{
		byte_size		+= _r.byte_size;
		pointer_count	+= _r.pointer_count;
		return	*this;
	}

	size_t compute_size(size_t _pointerSize = sizeof(void*)) const noexcept
	{
		return	byte_size + _pointerSize * pointer_count;
	}
};



struct	passion_var_info
{
	passion_string				name;
	passion_string				type;
	passion_size				position;
	passion_size				size;
	passion_member_attribute	attribute = passion_member_attribute::attr_public;

	passion_var_info() = default;
	passion_var_info(const passion_var_info&) = default;
	passion_var_info(passion_var_info&&) = default;
	passion_var_info& operator = (const passion_var_info&) = default;
	passion_var_info& operator = (passion_var_info&&) = default;

	passion_var_info(passion_string _name, passion_string _type, passion_size _size, passion_member_attribute _attr = passion_member_attribute::attr_public)
		: name(std::move(_name))
		, type(std::move(_type))
		, position({0, 0})
		, size(std::move(_size))
		, attribute(_attr)
	{}

	passion_var_info(passion_string _name, passion_string _type, passion_size _position, passion_size _size, passion_member_attribute _attr = passion_member_attribute::attr_public)
		: name(std::move(_name))
		, type(std::move(_type))
		, position(std::move(_position))
		, size(std::move(_size))
		, attribute(_attr)
	{}
};

class	passion_struct;

struct	passion_extends
{
	passion_extends() = default;
	passion_extends(const passion_extends&) = default;
	passion_extends(passion_extends&&) = default;
	passion_extends& operator = (const passion_extends&) = default;
	passion_extends& operator = (passion_extends&&) = default;

	passion_extends(ref<passion_struct> _clazz, passion_member_attribute _attr = passion_member_attribute::attr_public)
		: clazz(std::move(_clazz))
		, attribute(_attr)
	{}

	ref<passion_struct>			clazz;
	passion_member_attribute	attribute 	= passion_member_attribute::attr_public;
};



template<class T>
struct	view
{
	using	iterator	= T;
	using	size_type	= size_t;

	view(view&&) = default;
	view(const view&) = default;
	view& operator = (view&&) = default;
	view& operator = (const view&) = default;

	view(iterator _begin, iterator _end)
		: mBegin(_begin)
		, mEnd(_end)
	{}

	template<class IN>
	view(IN& _in)
		: mBegin(std::begin(_in))
		, mEnd(std::end(_in))
	{}

	iterator begin() const noexcept { return mBegin; }
	iterator end() const noexcept { return mEnd; }
	size_type size() const noexcept { return end() - begin(); }

private:
	iterator	mBegin;
	iterator	mEnd;
};



class	passion_struct
		: public ref_object
{
public:
	passion_struct(passion_string _name, bool _class, view<passion_var_info*> _vars, view<passion_extends*> _extends = {nullptr, nullptr})
		: mName(std::move(_name))
		, mIsClass(_class)
	{
		update(_vars, _extends);
	}


	const passion_string& name()
	{
		return	mName;
	}


	//! クラスの場合 true, 構造体の場合 false を返す。
	bool is_class() const noexcept
	{
		return	mIsClass;
	}


	passion_size size() noexcept
	{
		passion_size	retval{0, 0};
		for (auto& v : mVariables)
		{
			retval += v.size;
		}
		return	retval;
	}


	//! メンバ変数を取得する。
	const amtrs::shared_buffer<passion_var_info>& variables()
	{
		return	mVariables;
	}


private:
	void update(view<passion_var_info*> _vars, view<passion_extends*> _extends)
	{
		mVariables	= shared_buffer<passion_var_info>(_vars.size());
		{
			passion_size	pos	= {0, 0};
			auto			dest	= mVariables.begin();
			for (auto& v : _vars)
			{
				*dest 			=  std::move(v);
				dest->position	=  pos;
				pos				+= dest->size;
				++dest;
			}
		}
	}



	passion_string						mName;
	shared_buffer<ref<passion_struct>>	mExtends;
	shared_buffer<passion_var_info>		mVariables;
	bool								mIsClass	= false;
};





PASSION_NAMESPACE_END
#endif
