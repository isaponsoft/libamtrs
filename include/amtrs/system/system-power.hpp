/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__power__hpp
#define	__libamtrs__system__power__hpp
AMTRS_NAMESPACE_BEGIN

class	power : public ref_object
{
public:
	static ref<power> create();


	//! 画面のパワーを常にONするように指定します。
	virtual void keep_screen_power(bool _enable) = 0;
};


AMTRS_NAMESPACE_END
#include AMTRS_PLATFORM_INCLUDE(system-power.hpp)
#endif
