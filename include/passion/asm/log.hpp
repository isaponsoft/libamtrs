#ifndef	__passion__assembler__log__hpp
#define	__passion__assembler__log__hpp
#include <amtrs/logging/@>
#include "def.hpp"

#ifdef	PASSION_ASSEMBLER_TRACE
#	define	PSNASM_TRACE_LOG(...)	AMTRS_TRACE_LOG(__VA_ARGS__)
#else
#	define	PSNASM_TRACE_LOG(...)
#endif

#endif
