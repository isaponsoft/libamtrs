/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__sysinfo_ios__hpp
#define	__libamtrs__system__sysinfo_ios__hpp
#include <sys/sysctl.h>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

template<class String>
auto sysinfo<String>::device_name() const noexcept -> string_type
{
	size_t	size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	
	String	retval;
	retval.resize(size);
	sysctlbyname("hw.machine", retval.data(), &size, NULL, 0);
	return	retval;
}

AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(sysinfo)
#endif
