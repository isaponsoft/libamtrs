/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__amtrs__chrono__framerator__hpp
#define	__amtrs__chrono__framerator__hpp
AMTRS_NAMESPACE_BEGIN namespace chrono {



class	framerator
{
public:
	using	time_point		= std::chrono::time_point<std::chrono::system_clock>;
	using	elpased_type	= std::chrono::duration<double, std::chrono::milliseconds::period>;

	framerator(int _target_fps = 60)
		: mPeriodic(std::chrono::duration<double, std::chrono::seconds::period>(1.0 / (double)_target_fps))
		, mTargetFps(_target_fps)
	{}

	// ========================================================================
	//! 目標とする fps を返します。コンストラクタで指定した値になります。
	// ------------------------------------------------------------------------
	int target_fps() const noexcept { return mTargetFps; }

	// ========================================================================
	//! フレーム情報を更新します。
	// ------------------------------------------------------------------------
	int fps() const noexcept { return mFps; }

	elpased_type periodic() const noexcept { return mPeriodic; }

	elpased_type wait_time() const noexcept { return mWait; }

	int frame_count() const noexcept { return mFrameCount; }

	elpased_type delta() const noexcept { return mDelta; }

	void reset() noexcept
	{
		mStart		= std::chrono::system_clock::now();
		mPrev		= mStart;
		mFrameCount	= 0;
	}

	// ========================================================================
	//! フレーム情報を更新します。
	// ------------------------------------------------------------------------
	//! ウェイトすべき時間、もしくはウェイトを行った時間を返します。
	// ------------------------------------------------------------------------
	elpased_type update(bool _wait = true) noexcept
	{
		time_point		now		= std::chrono::system_clock::now();
		elpased_type	elp_sec	= now - mStart;

		if (elp_sec > std::chrono::seconds(1))
		{
			// 1秒以上経過したらフレームレートを設定
			mFps		= mFrameCount;
			mFrameCount	= 0;
			mStart		= now;
		}
		else
		{
			++mFrameCount;
		}

		elpased_type	crnttm	= elpased_type(1000.0 * (double)mFrameCount / (double)mTargetFps);
		mWait	= elp_sec < crnttm
				? elpased_type(crnttm - elp_sec)
				: elpased_type(0);

		elpased_type	retval	= mWait;
		if (_wait)
		{
			wait();
		}

		now			= std::chrono::system_clock::now();
		mDelta	= now - mPrev;
		mPrev		= now;
		return	retval;
	}

	// ========================================================================
	//! update() の結果に基づきウェイトをとります。
	// ------------------------------------------------------------------------
	elpased_type wait() noexcept
	{
		auto	retval	= mWait;
		if (mWait > elpased_type(0))
		{
			std::this_thread::sleep_for(mWait);
			mWait		= elpased_type(0);
		}
		return	retval;
	}

protected:
	elpased_type	mPeriodic;
	elpased_type	mWait;
	time_point		mStart;
	time_point		mPrev;

	int				mTargetFps	= 0;
	int				mFrameCount	= 0;
	int				mFps		= 0;
	elpased_type	mDelta;
};

} AMTRS_NAMESPACE_END
#endif
