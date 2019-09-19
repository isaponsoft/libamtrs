/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__os__winwt__window__hpp
#define	__libamtrs__os__winwt__window__hpp
#include "../../logging.hpp"
#include "../../os/win32/def.hpp"
#include "../../os/win32/error_code.hpp"
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
