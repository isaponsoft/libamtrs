#ifndef	__libamtrs__sound__types__hpp
#define	__libamtrs__sound__types__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


struct	sound_format
{
	int		channels;
	int		bitsParSamples;
	int		samplesParSecond;
};


AMTRS_NAMESPACE_END
#endif
