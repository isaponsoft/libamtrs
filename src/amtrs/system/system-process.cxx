/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <algorithm>
#include <amtrs/system/process.hpp>
AMTRS_SYSTEM_NAMESPACE_BEGIN

errorval exec(amtrs_system_process_pipe const& _pipe, char const* _command)
{
	return	AMTRS_IMPLEMENTS(run)::cmd(_pipe, _command);
}


errorval exec(char const* _command)
{
	amtrs_system_process_pipe	p{};
	p.object_stdin	= nullptr;
	p.object_stdout	= nullptr;
	p.object_stderr	= nullptr;
	p.conntact		= nullptr;
	return	exec(p, _command);
}

AMTRS_SYSTEM_NAMESPACE_END

