/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <amtrs/system/log.hpp>
#include <iostream>
#include <amtrs/memory/simplearray.hpp>
AMTRS_NAMESPACE_BEGIN

struct	errorval_items
{
	size_t					hash;
	errorval::fn_errormsg*	fn;
};
static simplearray<errorval_items>	sErrorvalItems;

void errorval::add_error_type(size_t _type_index_hash, fn_errormsg* _msg)
{
	for (auto& i : sErrorvalItems)
	{
		if (i.hash == _type_index_hash)
		{
			i.fn	= _msg;
			return;
		}
	}
	sErrorvalItems.push_back({_type_index_hash, _msg});
}

char const* errorval::_message() const noexcept
{
	for (auto const& i : sErrorvalItems)
	{
		if (i.hash == type())
		{
			return	i.fn(code());
		}
	}
	return	"Unkown error type.";
}


AMTRS_NAMESPACE_END
