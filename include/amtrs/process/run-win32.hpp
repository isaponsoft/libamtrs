/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#include <Windows.h> 
#include <amtrs/process.hpp>
AMTRS_NAMESPACE_PROCESS_BEGIN
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




// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
std::string exec(char** _command_and_args)
{
	PROCESS_INFORMATION		procInfo; 
	STARTUPINFOA			startInfo;
	ZeroMemory(&procInfo,  sizeof(procInfo));
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb			= sizeof(STARTUPINFO);

	SECURITY_ATTRIBUTES		sa; 
	sa.nLength				= sizeof(SECURITY_ATTRIBUTES); 
	sa.bInheritHandle		= TRUE; 
	sa.lpSecurityDescriptor	= nullptr; 

	pipe	sto;
	pipe	ste;
	if (!sto.create_pipe(sa))
	{
		throw std::runtime_error(_command_and_args[0]);
	}
	if (!ste.create_pipe(sa))
	{
		throw std::runtime_error(_command_and_args[0]);
	}

	//startInfo.hStdInput		= sto.reader;
	startInfo.hStdOutput	= sto.writer;
	//startInfo.hStdError		= ste.writer;
	startInfo.dwFlags		= STARTF_USESTDHANDLES;


	std::string	argv;
	for (std::size_t i = 0; _command_and_args[i]; ++i)
	{
		if (i > 0)
		{
			argv += " ";
		}
		argv += _command_and_args[i];
	}

	auto	bSuccess	= CreateProcessA
						(
							nullptr,
							(char*)argv.data(),
							nullptr,
							nullptr,
							TRUE,
							0,
							nullptr,
							nullptr,
							&startInfo,
							&procInfo
						);
	if (!bSuccess)
	{
		throw std::runtime_error(_command_and_args[0]);
	}

	WaitForSingleObject(procInfo.hProcess, INFINITE);
	CloseHandle(sto.writer);
	sto.writer	= nullptr;

	DWORD	rc;
	GetExitCodeProcess(procInfo.hProcess, &rc);

	std::string	out;
	for (;;)
	{
		char	buff[128];
		auto	size	= sto.read_buffer(buff, sizeof(buff));
		if (size == 0 || size == -1)
		{
			break;
		}
		out += std::string(buff, size);
	} 

	CloseHandle(procInfo.hProcess);
	return	out;
}


// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
int run(char** _command_and_args)
{
	PROCESS_INFORMATION		procInfo; 
	STARTUPINFOA			startInfo;
	ZeroMemory(&procInfo,  sizeof(procInfo));
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb			= sizeof(STARTUPINFO);

	std::string	argv;
	for (std::size_t i = 0; _command_and_args[i]; ++i)
	{
		if (i > 0)
		{
			argv += " ";
		}
		argv += _command_and_args[i];
	}

	auto	bSuccess	= CreateProcessA
						(
							nullptr,
							(char*)argv.data(),
							nullptr,
							nullptr,
							TRUE,
							0,
							nullptr,
							nullptr,
							&startInfo,
							&procInfo
						);
	if (!bSuccess)
	{
		throw std::runtime_error(_command_and_args[0]);
	}

	WaitForSingleObject(procInfo.hProcess, INFINITE);

	DWORD	rc;
	GetExitCodeProcess(procInfo.hProcess, &rc);
	CloseHandle(procInfo.hProcess);

	return	static_cast<int>(rc);
}



AMTRS_IMPLEMENTS_END(run)
AMTRS_NAMESPACE_PROCESS_END
#if 0

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
};


inline void register_process(vm* _vm)
{
	auto	clazz	= _vm->get_class("");

	auto	f		= vmnativefunction::create("process", [](vmvariant& _result, vmcontext&, vmobject*, const vmvariant* _argv) -> int
	{
//		HANDLE					hIn	= GetStdHandle(STD_INPUT_HANDLE);
		SECURITY_ATTRIBUTES		sa; 
		sa.nLength				= sizeof(SECURITY_ATTRIBUTES); 
		sa.bInheritHandle		= TRUE; 
		sa.lpSecurityDescriptor	= nullptr; 

		pipe	sin;
		pipe	sto;
		pipe	ste;
		if (!sto.create_pipe(sa))
		{
			_result	= vmvariant(std::in_place_type_t<vmint_t>(), -2);
			return	CR_CHANGEMETHOD;
		}
		if (!sto.create_pipe(sa))
		{
			_result	= vmvariant(std::in_place_type_t<vmint_t>(), -2);
			return	CR_CHANGEMETHOD;
		}
		if (!ste.create_pipe(sa))
		{
			_result	= vmvariant(std::in_place_type_t<vmint_t>(), -2);
			return	CR_CHANGEMETHOD;
		}


		PROCESS_INFORMATION		procInfo; 
		STARTUPINFOA			startInfo;
		ZeroMemory(&procInfo,  sizeof(procInfo));
		ZeroMemory(&startInfo, sizeof(startInfo));
		startInfo.cb			= sizeof(STARTUPINFO); 
//		startInfo.hStdInput		= sto.reader;
//		startInfo.hStdOutput	= sto.writer;
//		startInfo.hStdError		= ste.writer;
//		startInfo.dwFlags		= STARTF_USESTDHANDLES;
/*
		bool			bRunThread	= true;
		std::thread		t([&]()
		{
			CHAR	read_buff[256];
			DWORD	nBytesRead,nBytesWrote;
			
			// Get input from our console and send it to child through the pipe.
			while (bRunThread)
			{
				ReadConsole(hIn, read_buff, 1, &nBytesRead, NULL);
				read_buff[nBytesRead]	= '\0';
				WriteFile(sto.writer, read_buff, nBytesRead, &nBytesWrote, NULL);
			}
		});

*/
		auto	command		= std::get<vmstring>(_argv[0]);
		auto	bSuccess	= CreateProcessA
							(
								nullptr,
								(char*)command.c_str(),
								nullptr,
								nullptr,
								TRUE,
								0,
								nullptr,
								nullptr,
								&startInfo,
								&procInfo
							);
		WaitForSingleObject(procInfo.hProcess, INFINITE);
		CloseHandle(sto.writer);
		CloseHandle(ste.writer);
//		bRunThread	= false;
//		t.join();

		if (!bSuccess)
		{
			_result	= vmvariant(std::in_place_type_t<vmint_t>(), -1);
			return	CR_CHANGEMETHOD;
		}

		
		vmstring	out;
		vmstring	err;
		for (;;)
		{
			char	buff[128];
			auto	size	= sto.read_buffer(buff, sizeof(buff));
			if (size == 0 || size == -1)
			{
				break;
			}
			out += vmstring(buff, size);
		} 
		for (;;)
		{
			char	buff[128];
			auto	size	= ste.read_buffer(buff, sizeof(buff));
			if (size == 0 || size == -1)
			{
				break;
			}
			out += vmstring(buff, size);
		} 
		_result	= vmvariant(std::in_place_type_t<vmstring>(), out);
		return	CR_CHANGEMETHOD;
	});
	f->parameters.push_back("string");
	f->return_type	= "string";
	clazz->add_function(f);
}
#endif
