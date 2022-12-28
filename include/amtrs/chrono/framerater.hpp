/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__chrono__framerater__hpp
#define	__libamtrs__chrono__framerater__hpp
#include <ctime>
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
class	framerater
{
public:
	static constexpr double	period		= 1.0;
	static constexpr double	diraly_rate	= 0.8;

	void fps(int _fps)
	{
		mFrameRate	= _fps;
		mSpan		= 1.0 / (double)(mFrameRate);
	}

	double waittime()
	{
		if (mFrameRate <= 0)
		{
			return	0;
		}

		if (!mUpdated)
		{
			std::timespec_get(&mPeriodStartTime, TIME_UTC);
			mUpdated	= true;
			mFrameCount	= 0;
		}
		++mFrameCount;

		struct timespec	currentTime;
		std::timespec_get(&currentTime, TIME_UTC);

		long	seconds		= (long)currentTime.tv_sec  - (long)mPeriodStartTime.tv_sec;
		long	nseconds	= (long)currentTime.tv_nsec - (long)mPeriodStartTime.tv_nsec;
		mPreviousTime	= currentTime;

		double	elapsed_ms	= (((double)seconds * 1000.0) + (double)nseconds / (1000 * 1000));
		double	time		= (double)(mFrameCount) * 1000.0 / (double)(mFrameRate);
		if (elapsed_ms >= period * 1000)
	    {
	        mFrameCount			= 0;
	        mPeriodStartTime	= currentTime;
	    }
		double	wait		= time - elapsed_ms;
		if (wait <= 0)
		{
			wait	= 0;
		}
		// あまりにも短い時間の場合はスパイク発生の可能性があるので少しずつ調整
		if (wait < mSpan * diraly_rate)
		{
//			wait	= mSpan * diraly_rate;
		}
		return	wait;
	}

protected:
	struct timespec	mPeriodStartTime;
	struct timespec	mPreviousTime;
	double			mSpan		= 0;
	bool			mUpdated	= false;
	int				mFrameCount	= 0;
	int				mFrameRate	= -1;
};
AMTRS_NAMESPACE_END
#endif
