/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/system.hpp>
#define	AMTRS_PLIB_NAME	system-process.hpp
#include <amtrs/.inc/include-platform.hpp>
AMTRS_NAMESPACE_SYSTEM_BEGIN



int exec(amtrs_system_process_pipe const& _pipe, char const* _command)
{
	return	AMTRS_IMPLEMENTS(run)::cmd(_pipe, _command);
}


int exec(char const* _command)
{
	amtrs_system_process_pipe	p{};
	return	exec(p, _command);
}


AMTRS_NAMESPACE_SYSTEM_END
