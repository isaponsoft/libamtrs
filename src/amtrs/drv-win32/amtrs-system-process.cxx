/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <windows.h>
#include <stdexcept>
#include "error.hpp"
AMTRS_SYSTEM_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(run)

struct	pipe
{
	HANDLE	reader	= nullptr;
	HANDLE	writer	= nullptr;

	bool create_pipe(SECURITY_ATTRIBUTES& _sa)
	{
		if (!CreatePipe(&reader, &writer, &_sa, 0))
		{
			return	false;
		}
		if (!SetHandleInformation(reader, HANDLE_FLAG_INHERIT, 0))
		{
			return	false;
		}
		return	true;
	}


	std::size_t read_buffer(void* _buffer, std::size_t _size)
	{
		DWORD	dwSize;
		auto	res	= ReadFile(reader, _buffer, static_cast<DWORD>(_size), &dwSize, nullptr);
		if (!res)
		{
			return	static_cast<std::size_t>(-1);
		}
		return	static_cast<std::size_t>(dwSize);
	}

	~pipe()
	{
		if (reader)
		{
			CloseHandle(reader);
		}
		if (writer)
		{
			CloseHandle(writer);
		}
	}
};



errorval cmd(amtrs_system_process_pipe const& _pipe, char const* _command)
{
	// 開始情報
	STARTUPINFO				si	= {0};
	si.cb					= sizeof(si);
	si.dwFlags				= STARTF_USESTDHANDLES;
	si.wShowWindow			= SW_HIDE;

	// パイプの設定
	SECURITY_ATTRIBUTES		sa	= {0};
	sa.nLength				= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor	= nullptr;
	sa.bInheritHandle		= TRUE;

	pipe	pipe_stdout;	pipe_stdout;
	pipe	pipe_stderr;	pipe_stderr;
	pipe	pipe_stdin;		pipe_stdin;
	si.hStdOutput	= GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError	= GetStdHandle(STD_ERROR_HANDLE );
	si.hStdInput	= GetStdHandle(STD_INPUT_HANDLE );
	if (_pipe.object_stdout && _pipe.conntact)
	{
		pipe_stdout.create_pipe(sa);
		si.hStdOutput	= pipe_stdout.writer;
	}
	if (_pipe.object_stderr && _pipe.conntact)
	{
		pipe_stderr.create_pipe(sa);
		si.hStdError	= pipe_stderr.writer;
	}


	PROCESS_INFORMATION		pi{};
	CreateProcessA(NULL, const_cast<char*>(_command), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	DWORD					err	= GetLastError();
	if (err)
	{
		return	errorval(GetLastError());
	}

	// ブロックしたいのでスレッドハンドルは閉じておく
	if (pi.hThread)
	{
		CloseHandle(pi.hThread);
		pi.hThread	= nullptr;
	}

	for (;;)
	{
		// 何かしらシグナルが発生するまで待つ
		HANDLE	handles[10]	= { pi.hProcess };
		DWORD	count		= 1;
		if (pipe_stdout.reader)
		{
			handles[count]	= pipe_stdout.reader;
			++count;
		}
		if (pipe_stderr.reader)
		{
			handles[count]	= pipe_stderr.reader;
			++count;
		}
		auto	sig	= WaitForMultipleObjects(count, handles, FALSE, INFINITE);

		// handles[0] == WAIT_OBJECT_0
		if (sig == (WAIT_OBJECT_0 + 0))
		{
			break;
		}

		DWORD 	avile;
		char	buff[1024];
		while (pipe_stdout.reader && (PeekNamedPipe(pipe_stdout.reader, NULL, 0, NULL, &avile, NULL)))
		{
			if (avile <= 0) { break; }
			void*	data	= nullptr;
			size_t	bsize	= avile;
			_pipe.conntact(_pipe.object_stdout, &data, &bsize);
			if (!data)
			{
				data	= buff;
				bsize	= sizeof(buff);
			}
			ReadFile(pipe_stdout.reader, data, (DWORD)bsize, &avile, NULL);
			bsize	= avile;
			_pipe.conntact(_pipe.object_stdout, &data, &bsize);
		}

		while (pipe_stderr.reader && (PeekNamedPipe(pipe_stderr.reader, NULL, 0, NULL, &avile, NULL)))
		{
			if (avile <= 0) { break; }
			void*	data	= nullptr;
			size_t	bsize	= avile;
			_pipe.conntact(_pipe.object_stderr, &data, &bsize);
			if (!data)
			{
				data	= buff;
				bsize	= sizeof(buff);
			}
			ReadFile(pipe_stderr.reader, data, (DWORD)bsize, &avile, NULL);
			bsize	= avile;
			_pipe.conntact(_pipe.object_stderr, &data, &bsize);
		}
		if (sig == WAIT_OBJECT_0)
		{
			break;
		}
	}


	DWORD	rc;
	GetExitCodeProcess(pi.hProcess, &rc);
	CloseHandle(pi.hProcess);
	if (rc == 0xC0000005)
	{
		return	errorval((DWORD)ERROR_PROCESS_ABORTED);
	}
	return	errorval(rc);
}


AMTRS_IMPLEMENTS_END(run)
AMTRS_SYSTEM_NAMESPACE_END
