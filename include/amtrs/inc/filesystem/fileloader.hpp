/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__fileloader__hpp
#define	__libamtrs__fileloader__hpp
#include <tuple>
#include <utility>
#include "cascadevfs.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

class	fileloader
		: public cascadevfs
{
public:
	~fileloader()
	{
		if (sInstance == this)
		{
			sInstance	= nullptr;
		}
	}

	static ref<fileloader> get_instance()
	{
		if (!sInstance)
		{
			sInstance	= new fileloader();
		}
		return	sInstance;
	}

private:
	static inline	fileloader*	sInstance	= nullptr;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
