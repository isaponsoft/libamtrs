#ifndef	__toyoukebime__singleton_creater__hpp
#define	__toyoukebime__singleton_creater__hpp
#include <amtrs/utility/@>
#include "tyuk-def.hpp"
TYUK_NAMESPACE_BEGIN
template<class T>
struct singleton_creator
{
	ref<T> operator () ()
	{
		return new T();
	}
};
TYUK_NAMESPACE_END
#endif
