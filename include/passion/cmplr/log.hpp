#ifndef	__passion__compiler__log__hpp
#define	__passion__compiler__log__hpp
#include <amtrs/logging.hpp>
#include "def.hpp"

#ifdef	PASSION_COMPILER_TRACE
#	define	PSNCMPLR_TRACE_LOG(...)	AMTRS_TRACE_LOG(__VA_ARGS__)
#else
#	define	PSNCMPLR_TRACE_LOG(...)
#endif

#endif
