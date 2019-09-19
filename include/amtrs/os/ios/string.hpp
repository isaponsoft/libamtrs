/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
