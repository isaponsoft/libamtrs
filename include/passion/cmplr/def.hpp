#ifndef	__passion__inc__def__hpp
#define	__passion__inc__def__hpp
#include <amtrs/variant/@>
#include <amtrs/memory/shared_buffer.hpp>
#include <amtrs/logging/@>
#include "../def.hpp"
PASSION_NAMESPACE_BEGIN
PASSION_NAMESPACE_END

#define	THROW_ERROR(_code, _token)		AMTRS_TRACE_LOG("compiler error %s(%d) : %s", __FILE__, __LINE__); throw (error_code::_code, _token, _token, __FILE__, __LINE__)
#define	THROW_COMPILER_ERROR()			AMTRS_TRACE_LOG("compiler error %s(%d)", __FILE__, __LINE__); throw program_error(error_code::compiler_error, {__FILE__, __LINE__}, __FILE__, __LINE__);

#ifdef	PASSION_COMPILER_TRACE
#	define	PSNCMPLR_TRACE_LOG(...)	AMTRS_TRACE_LOG(__VA_ARGS__)
#else
#	define	PSNCMPLR_TRACE_LOG(...)
#endif


#endif
