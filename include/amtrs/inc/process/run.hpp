/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__process__run__hpp
#define	__libamtrs__process__run__hpp
#include <string>
#include <vector>
#include "def.hpp"
AMTRS_NAMESPACE_PROCESS_BEGIN
AMTRS_IMPLEMENTS_BEGIN(run)
inline std::string exec(char** _command_and_args);
inline int run(char** _command_and_args);
AMTRS_IMPLEMENTS_END(run)



// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
inline std::string exec(const char* _command)
{
	std::vector<std::string>	argv;
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
			argv.push_back({begin, static_cast<std::size_t>(end - begin)});
		}
		begin	= end;
	}
	if (begin != end)
	{
		argv.push_back({begin, static_cast<std::size_t>(end - begin)});
	}


	std::vector<char*>			argp;
	for (auto& a : argv)
	{
		argp.push_back(const_cast<char*>(a.c_str()));
	}
	argp.push_back(nullptr);

	return	AMTRS_IMPLEMENTS(run)::exec(argp.data());
}




// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
inline int run(const char* _command)
{
	std::vector<std::string>	argv;
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
			argv.push_back({begin, static_cast<std::size_t>(end - begin)});
		}
		begin	= end;
	}
	if (begin != end)
	{
		argv.push_back({begin, static_cast<std::size_t>(end - begin)});
	}


	std::vector<char*>			argp;
	for (auto& a : argv)
	{
		argp.push_back(const_cast<char*>(a.c_str()));
	}
	argp.push_back(nullptr);

	return	AMTRS_IMPLEMENTS(run)::run(argp.data());
}




AMTRS_NAMESPACE_PROCESS_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(run)
#endif
