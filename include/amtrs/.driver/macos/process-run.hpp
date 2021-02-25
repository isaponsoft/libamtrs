/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__process__run_osx__hpp
#define	__libamtrs__process__run_osx__hpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
AMTRS_NAMESPACE_PROCESS_BEGIN
AMTRS_IMPLEMENTS_BEGIN(run)


// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
inline std::string exec(char** _command_and_args)
{
	return	{};
}

// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
inline int run(char** _command_and_args)
{
	int		status	= -1;
	pid_t	pid		= fork();
	if (pid == -1)
	{
		throw std::runtime_error(_command_and_args[0]);
	}
	if (pid == 0)
	{
		execv(_command_and_args[0], _command_and_args);
		exit(-1);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (!WIFEXITED(status))
		{
			return	-1;
		}
	}
	return	static_cast<int>(WEXITSTATUS(status));
}


AMTRS_IMPLEMENTS_END(run)
AMTRS_NAMESPACE_PROCESS_END
#endif
