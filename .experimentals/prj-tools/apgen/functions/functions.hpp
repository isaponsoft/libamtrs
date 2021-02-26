/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__functions__hpp
#define	__functions__hpp
#include <string>
#include "../apgen.hpp"
namespace apgen {


template<class T, class Selector>
class	dynamic_linker
{
private:
	using	this_type	= dynamic_linker<T, Selector>;

public:
	using	value_type		= T;
	using	selector_type	= Selector;

	dynamic_linker(selector_type&& _selector, value_type&& _value)
		: mSelector(_selector)
		, mValue(_value)
		, mNext(_link())
	{
		_link()	= this;
	}

	template<class S>
	static T const* find(S const& _select) noexcept
	{
		this_type*	linker	= _link();
		while (linker)
		{
			if (linker->mSelector == _select)
			{
				return	&(linker->mValue);
			}
			linker	= linker->mNext;
		}
		return	nullptr;
	}

	template<class Callback>
	static void enumrate(Callback&& _callback)
	{
		this_type*	linker	= _link();
		while (linker)
		{
			if (_callback(static_cast<value_type const&>(linker->mValue)))
			{
				break;
			}
		}
	}

private:
	static this_type*& _link() noexcept
	{
		static this_type*	_;
		return	_;
	}

	this_type*		mNext;
	selector_type	mSelector;
	value_type		mValue;
};

struct	function_params
{
	std::string	arguments;
};


using	function_processeor	= bool(status&, std::string&, function_params const&);
using	function_generator	= dynamic_linker<function_processeor*, std::string>;



bool process_function(status& _status, std::string& line);

}
#endif
