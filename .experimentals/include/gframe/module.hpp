/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libgframe__module__hpp
#define	__libgframe__module__hpp
#include <amtrs/amtrs.hpp>
#include "def.hpp"
GFRAME_NAMESPACE_BEGIN
using namespace amtrs;

template<class T>
class	module
{
public:
	using	value_type		= T;
	using	creator			= ref<value_type>();
	using	selector_type	= typename value_type::selector_type;

	module(selector_type _selector, creator* _creator)
		: mSelector(_selector)
		, mCreate(_creator)
	{
		if constexpr (std::is_same<selector_type, std::string>::value)
		{
			AMTRS_DEBUG_LOG("module<%s> \"%s\" install.", typeid(T).name(), _selector.c_str());
		}
		else
		{
			AMTRS_DEBUG_LOG("module<%s> install.", typeid(T).name());
		}
		mNext	= _linker();
		_linker()	= this;
	}

	static ref<value_type> create(selector_type const& _selector)
	{
		auto*	c	= _linker();
		while (c)
		{
			if (_selector == c->mSelector)
			{
				auto	ret	= (*c->mCreate)();
				if constexpr (std::is_same<selector_type, std::string>::value)
				{
					ret->set_name(_selector);
					AMTRS_DEBUG_LOG("module<%s> \"%s\" created.", typeid(T).name(), _selector.c_str());
				}
				else
				{
					AMTRS_DEBUG_LOG("module<%s> created.", typeid(T).name());
				}
				return	ret;
			}
			c	= c->mNext;
		}
/*
		if constexpr (std::is_same<selector_type, std::string>::value)
		{
			AMTRS_DEBUG_LOG("module<%s> \"%s\" no such.", typeid(T).name(), _selector.c_str());
		}
		else
		{
			AMTRS_DEBUG_LOG("module<%s> no such.", typeid(T).name());
		}
*/		return	{};
	}

	static ref<value_type> create(std::initializer_list<selector_type> _selectorList)
	{
		for (auto& sel : _selectorList)
		{
			if (ref<value_type> val = create(sel); !val.empty())
			{
				return	val;
			}
		}
		return	{};
	}

private:
	selector_type	mSelector;
	creator*		mCreate;
	module*			mNext	= nullptr;

	static module*& _linker()
	{
		static module*	_fast = nullptr;
		return	_fast;
	}

};


GFRAME_NAMESPACE_END
#define	MODULE_DEFINE(_type, _class, _selector)	module<_type> _##_type##_class(_selector, []() -> ref<_type> { return new _class(); });
#endif
