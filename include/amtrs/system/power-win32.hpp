#ifndef	__libamtrs__system__power_win32__hpp
#define	__libamtrs__system__power_win32__hpp
AMTRS_NAMESPACE_BEGIN

class	power_win32 : public power
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
	ref<power_win32>	retval	= new power_win32();
	return	retval;
}

AMTRS_NAMESPACE_END
#endif
