/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__device__ios__api__error__hpp
#define	__libamtrs__g3d__device__ios__api__error__hpp
#include <UIKit/UIKit.h>
#include <system_error>
#include "def.hpp"
namespace std {

	inline std::error_code make_error_code(NSError* _err)
	{
		return	std::error_code(_err.code, std::generic_category());
	}

}
AMTRS_OS_IOS_NAMESPACE_BEGIN

inline std::system_error make_system_error(NSError* _err)
{
	return	std::system_error(std::make_error_code(_err), [_err.localizedDescription UTF8String]);
}


AMTRS_OS_IOS_NAMESPACE_END
#endif
