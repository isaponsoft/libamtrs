/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <string.h>
AMTRS_NAMESPACE_BEGIN

int errorval::to_errno() const noexcept
{
	return	code();
}

char const* errorval::message() const noexcept
{
	if (type() == 0)
	{
		return	strerror(code());
	}
	return	_message();
}
AMTRS_NAMESPACE_END
