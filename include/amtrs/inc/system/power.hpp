#ifndef	__libamtrs__system__power__hpp
#define	__libamtrs__system__power__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

class	power : public ref_object
{
public:
	static ref<power> create();


	//! 画面のパワーを常にONするように指定します。
	virtual void keep_screen_power(bool _enable) = 0;
};


AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(power)
#endif
