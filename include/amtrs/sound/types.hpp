#ifndef	__libamtrs__sound__types__hpp
#define	__libamtrs__sound__types__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


struct	sound_format
{
	int			channels;
	int			bitsParSamples;
	int64_t		samplesParSecond;
};


AMTRS_NAMESPACE_END
#endif
