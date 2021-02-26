/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__compati__variant__types__hpp
#define	__libamtrs__compati__variant__types__hpp
AMTRS_NAMESPACE_BEGIN

struct	bad_variant_access
		:  public std::exception
{
	virtual const char* what() const noexcept override { return "bad_variant_access"; }
};


AMTRS_NAMESPACE_END
#endif
