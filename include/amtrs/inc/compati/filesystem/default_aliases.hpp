#ifndef	__libamtrs__filesystem__default_aliases__hpp
#define	__libamtrs__filesystem__default_aliases__hpp
#include "filesystem.hpp"
#include "directory_entry.hpp"
#include "directory_iterator.hpp"
#include "path.hpp"
#include "types.hpp"
#ifndef	AMTRS_STL17_FILESYSTEM
// STL Emulation.
namespace std
{
	namespace filesystem = AMTRS_NAMESPACE_FILESYSTEM_FULL;
}
#endif
#endif
