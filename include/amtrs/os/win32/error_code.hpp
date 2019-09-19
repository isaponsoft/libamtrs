#ifndef	__libamtrs__os__win32__error_code__hpp
#define	__libamtrs__os__win32__error_code__hpp
#include <system_error>
#include "windows.hpp"
#include "def.hpp"
AMTRS_OS_WIN32_NAMESPACE_BEGIN


inline std::error_code make_last_error_code()
{
	auto	ec = GetLastError();
	return	std::error_code(ec, std::generic_category());
}


template<class... Args>
inline std::system_error make_system_error(Args&&... _args)
{
	return	std::system_error(make_last_error_code(), std::forward<Args>(_args)...);
}


template<class... Args>
inline void throw_system_error(Args&&... _args)
{
	throw make_system_error(std::forward<Args>(_args)...);
}




AMTRS_OS_WIN32_NAMESPACE_END
#endif
