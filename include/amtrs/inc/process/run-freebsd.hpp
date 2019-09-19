﻿/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__process__run_posix__hpp
#define	__libamtrs__process__run_posix__hpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "def.hpp"
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
