﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__power_osx__hpp
#define	__libamtrs__system__power_osx__hpp
AMTRS_NAMESPACE_BEGIN

class	power_osx : public power
{
public:
	//! 画面のパワーを常にONするように指定します。
	virtual void keep_screen_power(bool _enable) override
	{
		(void)_enable;
	}
};

inline ref<power> power::create()
{
	ref<power_osx>	retval	= new power_osx();
	return	retval;
}

AMTRS_NAMESPACE_END
#endif
