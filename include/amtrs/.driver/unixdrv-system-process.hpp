﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__unixdrv__system_process__hpp
#define	__libamtrs__driver__unixdrv__system_process__hpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <vector>
#include <string>
#include <exception>

AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(run)

static int __exec(char const* _command);

struct	pipeinfo
{
	int read() const noexcept { return _pipe[0]; }
	int write() const noexcept { return _pipe[1]; }
	void init() { pipe(_pipe); }

	void read_close()  { if (_pipe[0] != -1) { ::close(_pipe[0]); _pipe[0] = -1; } }
	void write_close() { if (_pipe[1] != -1) { ::close(_pipe[1]); _pipe[1] = -1; } }

	void close()
	{
		read_close();
		write_close();
	}

	~pipeinfo()
	{
		close();
	}
	int	_pipe[2] = {-1, -1};
};



int cmd(amtrs_system_process_pipe const& _pipe, char const* _command)
{
	pid_t		pid;
	pipeinfo	pipe_stdin;
	pipeinfo	pipe_stdout;
	pipeinfo	pipe_stderr;
	if (_pipe.conntact)
	{
		pipe_stdin.init();
		pipe_stdout.init();
		pipe_stderr.init();
	}

	pid		= fork();
	if (pid < 0)
	{
		throw std::runtime_error(_command);
		return	-1;
	}

	if (pid == 0)
	{
		if (_pipe.conntact)
		{
			pipe_stdin.write_close();
			pipe_stdout.read_close();
			pipe_stderr.read_close();
			dup2(pipe_stdin.read(),   STDIN_FILENO);
			dup2(pipe_stdout.write(), STDOUT_FILENO);
			dup2(pipe_stderr.write(), STDERR_FILENO);
			pipe_stdin.read_close();
			pipe_stdout.write_close();
			pipe_stderr.write_close();
		}
		int	rc = __exec(_command);
		pipe_stdin.close();
		pipe_stdout.close();
		pipe_stderr.close();
		exit(rc);
		return	rc;
	}

	pipe_stdin .read_close();
	pipe_stdout.write_close();
	pipe_stderr.write_close();
	if (_pipe.conntact)
	{
		for (;;)
		{
			fd_set	descs;
			FD_ZERO(&descs);
			FD_SET(pipe_stdout.read(), &descs);
			FD_SET(pipe_stderr.read(), &descs);
			struct timeval  tv;
			tv.tv_sec	= 0;
			tv.tv_usec	= 1000 * 100;
			select(FD_SETSIZE, &descs, NULL, NULL, &tv);

			char	buff[1000];
			if (FD_ISSET(pipe_stdout.read(), &descs))
			{
				void*	data	= nullptr;
				size_t	bsize	= 0;
				_pipe.conntact(_pipe.object_stdout, &data, &bsize);
				if (!data)
				{
					data	= buff;
					bsize	= sizeof(buff);
				}
				auto	r	= read(pipe_stdout.read(), data, bsize);
				if (r == 0)
				{
					// pipe close.
					break;
				}
				if (r > 0)
				{
					bsize	= r;
					_pipe.conntact(_pipe.object_stdout, &data, &bsize);
				}
			}
			if (FD_ISSET(pipe_stderr.read(), &descs))
			{
				void*	data	= nullptr;
				size_t	bsize	= 0;
				_pipe.conntact(_pipe.object_stderr, &data, &bsize);
				if (!data)
				{
					data	= buff;
					bsize	= sizeof(buff);
				}
				auto	r	= read(pipe_stderr.read(), data, bsize);
				if (r == 0)
				{
					// pipe close.
					break;
				}
				if (r > 0)
				{
					bsize	= r;
					_pipe.conntact(_pipe.object_stderr, &data, &bsize);
				}
			}
		}
	}

	int	status;
	waitpid(pid, &status, 0);
	return	WIFEXITED(status)
			? WEXITSTATUS(status)
			: -1;
}


static int __exec(char const* _command)
{
	std::vector<std::string>	args;
	std::vector<char*>			argp;
	const char*					cursor	= _command;
	const char*					begin	= _command;
	const char*					end		= _command;
	while (*cursor)
	{
		while (*cursor && *cursor == ' ')
		{
			++cursor;
		}
		if (!*cursor)
		{
			break;
		}
		begin	= cursor;
		end		= cursor;
		while (*cursor && *cursor != ' ')
		{
			++cursor;
		}
		end		= cursor;
		if (begin != end)
		{
			args.push_back({begin, static_cast<std::size_t>(end - begin)});
		}
		begin	= end;
	}
	for (auto& s : args)
	{
		argp.push_back(s.data());
	}

	argp.push_back(nullptr);
	return	execvp(argp[0], argp.data());
}



AMTRS_IMPLEMENTS_END(run)
AMTRS_NAMESPACE_END
#endif
