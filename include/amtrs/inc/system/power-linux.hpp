﻿#ifndef	__libamtrs__system__power_linux__hpp
#define	__libamtrs__system__power_linux__hpp
AMTRS_NAMESPACE_BEGIN

class	power_linux : public power
{
public:
	//! 画面のパワーを常にONするように指定します。
	virtual void keep_screen_power(bool _enable) override
	{
	}
};

inline ref<power> power::create()
{
	ref<power_linux>	retval	= new power_linux();
	return	retval;
}

AMTRS_NAMESPACE_END
#endif