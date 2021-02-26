/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__sysinfo__hpp
#define	__libamtrs__system__sysinfo__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

template<class String>
class	sysinfo
{
public:
	using	string_type	= String;

	string_type device_name() const noexcept;
};

AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(sysinfo)
#endif
