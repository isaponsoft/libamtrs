/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__fileloader__hpp
#define	__libamtrs__filesystem__fileloader__hpp
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
