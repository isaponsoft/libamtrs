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
	//! �ڕW�Ƃ��� fps ��Ԃ��܂��B�R���X�g���N�^�Ŏw�肵���l�ɂȂ�܂��B
	// ------------------------------------------------------------------------
	int target_fps() const noexcept { return mTargetFps; }

	// ========================================================================
	//! �t���[�������X�V���܂��B
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
	//! �t���[�������X�V���܂��B
	// ------------------------------------------------------------------------
	//! �E�F�C�g���ׂ����ԁA�������̓E�F�C�g���s�������Ԃ�Ԃ��܂��B
	// ------------------------------------------------------------------------
	elpased_type update(bool _wait = true) noexcept
	{
		time_point		now		= std::chrono::system_clock::now();
		elpased_type	elp_sec	= now - mStart;

		if (elp_sec > std::chrono::seconds(1))
		{
			// 1�b�ȏ�o�߂�����t���[�����[�g��ݒ�
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
	//! update() �̌��ʂɊ�Â��E�F�C�g���Ƃ�܂��B
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
