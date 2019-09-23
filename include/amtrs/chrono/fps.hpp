#ifndef	__amtrs__chrono__fps__hpp
#define	__amtrs__chrono__fps__hpp
#include <thread>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN namespace chrono {

class	fps
{
public:
	using	time_type	= std::chrono::time_point<std::chrono::system_clock>;

	fps(int _frameCount = 60)
	{
		set_frame_count(_frameCount);
	}

	void set_frame_count(int _frameCount)
	{
		mFrameCount	= _frameCount;
		mFrameTime	= 1.0 / (double)mFrameCount;					// 1�t���[��������̎���
		reset();
	}

	time_type get_current_time()
	{
		return	std::chrono::system_clock::now();
	}

	//! ���݂̃t���[�����[�g���擾���܂��B
	double frame_rate() const noexcept
	{
		return	mFrameRate;
	}

	//! �O�񂩂�̌o�ߎ��Ԃ��擾���܂�(update�ōX�V����܂�)�B
	double delta() const noexcept { return mDelta; }

	void update()
	{
		// 60�t���[���ڂȂ畽�ς��v�Z����
		time_type						nt		= get_current_time();
		mDelta	= std::chrono::duration<double>(nt - mPrevTime).count(); 
		if (mCount == mFrameCount)
		{
			std::chrono::duration<double>	took	= nt - mStartTime;		// �O��̌Ăяo������̌o�ߎ���
			mFrameRate		= 1000.0 / ((took.count())/(double)mFrameCount);
			mCount		= 0;
			mStartTime	= nt;
		}
		else
		{
			++mCount;
		}
		mPrevTime	= nt;
	}


	void reset()
	{
		mStartTime	= get_current_time();
		mPrevTime	= mStartTime;
		mCount		= 0;
	}


	void wait()
	{
		std::chrono::duration<double>	tookTime	= get_current_time() - mStartTime;			// ������������
		auto							targetTime	= mFrameTime * (double)mCount;				// strat ���牽�b�o�߂��Ă���ׂ���
		auto							waitTime	= targetTime - tookTime.count();
		if (waitTime > 0)
		{
			//AMTRS_DEBUG_LOG("Wait time = %f", waitTime);
			std::this_thread::sleep_for(std::chrono::duration<double>(waitTime));
		}
	}

private:
	time_type	mStartTime;				//!< ����J�n����
	time_type	mPrevTime;				//!< ����J�n����
	int			mFrameCount;
	int			mCount		= 0;		//!< �J�E���^
	double		mFrameRate	= 0;		//!< frame rate
	double		mFrameTime	= 0;
	double		mDelta		= 0;
};


} AMTRS_NAMESPACE_END
#endif
