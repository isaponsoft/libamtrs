/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__os__winwt__window__hpp
#define	__libamtrs__os__winwt__window__hpp
#include "../../logging.hpp"
#include "../win32/.api-windows.hpp"
#include <windowsx.h>
AMTRS_NAMESPACE_BEGIN

template<class BaseT>
class	window_win32
		: public BaseT
{
	using	_base_type	= BaseT;
public:
	using	size_type	= typename _base_type::size_type;

};


AMTRS_NAMESPACE_END
#endif
