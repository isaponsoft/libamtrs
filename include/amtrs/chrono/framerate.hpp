/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__chrono__framerate__hpp
#define	__libamtrs__chrono__framerate__hpp
#include <ctime>
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
class	framerate
{
public:
	using	value_type	= unsigned int;

	framerate()
	{
		start();
	}

	void start()
	{
		std::timespec_get(&mStartTime, TIME_UTC);
		mFrameCount	= 0;
	}

	// return
	//		true:	reset period
	//		false:	not reset period
	bool update()
	{
		struct timespec	currentTime;
		std::timespec_get(&currentTime, TIME_UTC);

		// elapsed by refresh period
		{
			long	seconds		= (long)currentTime.tv_sec  - (long)mStartTime.tv_sec;
			long	nseconds	= (long)currentTime.tv_nsec - (long)mStartTime.tv_nsec;
			long	mtime		= static_cast<long>(((seconds) * 1000.0 + nseconds / 1000000.0) + 0.5);
			mElapsedByPeriod	= ((double)mtime) / 1000.0;
		}

		// elpased by previous update
		{
			long	seconds		= (long)currentTime.tv_sec  - (long)mPreviousTime.tv_sec;
			long	nseconds	= (long)currentTime.tv_nsec - (long)mPreviousTime.tv_nsec;
			long	mtime		= static_cast<long>(((seconds) * 1000.0 + nseconds / 1000000.0) + 0.5);
			mPreviousTime		= currentTime;
			mElapsedByPrevious	= ((double)mtime) / 1000.0;
		}

		++mFrameCount;
		if (mElapsedByPeriod >= mRefreshPeriod)
	    {
	        mFrameRate	= (mFrameCount / (mElapsedByPeriod));
	        mFrameCount	= 0;
	        mStartTime	= currentTime;
			return	true;
	    }
		return	false;
	}

	double fps()
	{
		return	mFrameRate;
	}

	double elapsed_period()
	{
		return	mElapsedByPeriod;
	}

	double elapsed_previous()
	{
		return	mElapsedByPrevious;
	}

protected:
	value_type			mFrameCount			= 0;
	double				mRefreshPeriod		= 1.0;
	double				mFrameRate			= 0;
	double				mElapsedByPeriod	= 0;
	double				mElapsedByPrevious	= 0;
	struct timespec		mStartTime;
	struct timespec		mPreviousTime;
};
AMTRS_NAMESPACE_END
#endif
