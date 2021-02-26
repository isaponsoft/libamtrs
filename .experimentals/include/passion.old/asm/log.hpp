/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__assembler__log__hpp
#define	__passion__assembler__log__hpp
#include <amtrs/logging/@>
#include "def.hpp"

#ifdef	PASSION_ASSEMBLER_TRACE
#	define	PSNASM_TRACE_LOG(...)	AMTRS_DEBUG_LOG(__VA_ARGS__)
#else
#	define	PSNASM_TRACE_LOG(...)
#endif

#endif
