/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__ios__string__hpp
#define	__libamtrs__ios__string__hpp
#include <string>
#include "def.hpp"
AMTRS_OS_IOS_NAMESPACE_BEGIN
AMTRS_OS_IOS_NAMESPACE_END
namespace std {


	inline extern std::string to_string(NSString* _str)
	{
		std::string	retval([_str UTF8String]);
		return	retval;
	}



}
#endif
