﻿/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__logging__hpp
#define	__libamtrs__logging__hpp

#if		defined(AMTRS_DOC_JAPANEASE)
	ログ出力に関する機能を提供します。

	amtrs::logging::prefix([]() -> std::string { return "MYLOG"; });
	amtrs::logging::output("Log");
#endif

#include "amtrs.hpp"
#include "inc/logging/def.hpp"
#include "inc/logging/logging.hpp"
#include "string.hpp"

#define	AMTRS_TRACE_LOG(...)	amtrs::logging::output(amtrs::logging::location{__FILE__, __LINE__}, amtrs::format<std::string>(__VA_ARGS__));

#ifdef	DEBUG
#define	AMTRS_DEBUG_LOG(...)	AMTRS_TRACE_LOG(__VA_ARGS__)
#else
#define	AMTRS_DEBUG_LOG(...)	{};
#endif

#endif
