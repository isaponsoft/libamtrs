#ifndef	__libamtrs__sound__sound_device_ios__hpp
#define	__libamtrs__sound__sound_device_ios__hpp
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <AudioToolbox/AudioToolbox.h>
#include "../../logging.hpp"
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

class	sound_device_ios;
class	sound_player_ios;


class	sound_buffer_ios
		: public sound_buffer
{
public:
	static constexpr ALuint		nosource	= static_cast<ALuint>(-1);

	sound_buffer_ios(sound_device_ios* _device, std::size_t _bytesSize)
		: mDevice(_device)
		, mBufferSize(_bytesSize)
	{}

	~sound_buffer_ios()
	{
		release();
	}

	virtual std::size_t size() const noexcept
	{
		return	mBufferSize;
	}

	ALuint buffer_handle() const noexcept { return mBuffer; }

private:
	void initialize(const void* _buffer)
	{
		alGenBuffers(1, &mBuffer);
		alBufferData(mBuffer, AL_FORMAT_STEREO16, _buffer, static_cast<ALsizei>(mBufferSize), 44100);
	}

	void release()
	{
		if (mBuffer != nosource)
		{
			alDeleteBuffers(1, &mBuffer);
			mBuffer	= nosource;
		}
	}

	sound_device_ios*	mDevice		= nullptr;
    ALuint				mBuffer		= nosource;
	std::size_t			mBufferSize	= 0;


	friend	sound_device_ios;
	friend	sound_player_ios;
};


class	sound_player_ios
		: public sound_player
{
public:
	static constexpr ALuint		nosource	= static_cast<ALuint>(-1);

	sound_player_ios(sound_device_ios* _device)
		: mDevice(_device)
	{}


	~sound_player_ios()
	{
		release();
	}


	virtual void set_buffer(sound_buffer* _buffer) override
	{
		mActiveBuffer	= static_cast<sound_buffer_ios*>(_buffer);
		alSourcei(mSource, AL_BUFFER, mActiveBuffer->buffer_handle());
	}


	virtual void play() override
	{
		alSourcei(mSource, AL_LOOPING, AL_FALSE);
		alSourcePlay(mSource);
	}

	
	virtual void stop() override
	{
		alSourceStop(mSource);
	}


	virtual void pause() override
	{
		alSourcePause(mSource);
	}


	virtual status state() const noexcept override
	{
		ALint	st;
		alGetSourcei(mSource, AL_SOURCE_STATE, &st);
		if (st == AL_PLAYING)
		{
			return	status::playing;
		}
		if (st == AL_PAUSED)
		{
			return	status::paused;
		}
		return	status::stopped;
	}


private:
	void initialize()
	{
		alGenSources(1, &mSource);		
	}

	void release()
	{
		if (mSource != nosource)
		{
			alDeleteSources(1, &mSource);
			mSource	= nosource;
		}
	}

	sound_device_ios*		mDevice		= nullptr;
    ALuint					mSource		= nosource;
	ref<sound_buffer_ios>	mActiveBuffer;

	friend	sound_device_ios;
};






class	sound_device_ios
		: public sound_device
{
public:
	~sound_device_ios();

	// ========================================================================
	//! プレイヤーの最大数を返します。
	// ------------------------------------------------------------------------
	virtual std::size_t get_max_player_count() const override
	{
		// OpenAL ではRAMサイズに依存する。
		return	static_cast<std::size_t>(-1);
	}


	// ========================================================================
	//! プレイヤーを生成します。
	// ------------------------------------------------------------------------
	virtual ref<sound_player> create_player(sound_stream_type _stream) override
	{
		ref<sound_player_ios>	retval	= new sound_player_ios(this);
		retval->initialize();
		return	retval;
	}


	// ========================================================================
	//! プレイヤーに適合するサウンドバッファをを生成します。
	// ------------------------------------------------------------------------
	//! バッファのサイズを設定できます。
	// ------------------------------------------------------------------------
	virtual ref<sound_buffer> create_buffer(std::size_t _bytesSize) override
	{
		ref<sound_buffer_ios>	buff	= new sound_buffer_ios(this, _bytesSize);
		std::vector<char>		zero(_bytesSize);
		for (auto& c : zero) { c = 0; }

		#if	0
		// テスト用のSIN波
		double	r	= 0;
		for (auto& c : zero)
		{
			c	= (char)(30000.0f * sin(2 * pi * r / (double)(44100/4)));
			++r;
		}
		#endif

		buff->initialize(zero.data());
		return	buff;
	}


	virtual ref<sound_buffer> create_buffer(const void* _pcmdata, std::size_t _bytesSize) override
	{
		ref<sound_buffer_ios>	buff	= new sound_buffer_ios(this, _bytesSize);
		buff->initialize(_pcmdata);
		return	buff;
	}



	//! デバイスを初期化します。
	inline void initialize();

	//! RAMの状態を更新します。
	inline void update_ram_status();
private:
    ALCdevice*                  mDevice				= nullptr;
    ALCcontext*                 mContext			= nullptr;

	std::size_t					mXRAMSize			= 0;
	std::size_t					mXRAMFree			= 0;
	std::size_t					mXRAMAuto 			= 0;
	std::size_t					mXRAMHardware		= 0;
	std::size_t					mXRAMAccessible		= 0;

	friend	sound_device;
};





inline ref<sound_device> sound_device::create()
{
	ref<sound_device_ios>	dev	= new sound_device_ios();
	dev->initialize();
	return	dev;
}


inline sound_device_ios::~sound_device_ios()
{
	alcMakeContextCurrent(nullptr);
	if (mContext)
	{
		alcDestroyContext(mContext);
		mContext	= nullptr;
	}

	if (mDevice)
	{
		alcCloseDevice(mDevice);
		mDevice	= nullptr;
	}
}


inline void sound_device_ios::initialize()
{
	if (!mDevice)
	{
		mDevice		= alcOpenDevice(nullptr);
		if (!mDevice)
		{
			throw	std::runtime_error("alcOpenDevice");
		}
		
		mContext	= alcCreateContext(mDevice, NULL);
		if (!mContext)
		{
			throw	std::runtime_error("alcCreateContext");
		}
		
		if (!alcMakeContextCurrent(mContext))
		{
			throw	std::runtime_error("alcMakeContextCurrent");
		}

		update_ram_status();
	}
}


inline void sound_device_ios::update_ram_status()
{
	// iOSでは未対応
	mXRAMSize			= alGetEnumValue("AL_EAX_RAM_SIZE");
	mXRAMFree			= alGetEnumValue("AL_EAX_RAM_FREE");;
	mXRAMAuto			= alGetEnumValue("AL_STORAGE_AUTOMATIC");
	mXRAMHardware		= alGetEnumValue("AL_STORAGE_HARDWARE");
	mXRAMAccessible		= alGetEnumValue("AL_STORAGE_ACCESSIBLE"); 
	AMTRS_DEBUG_LOG("OpenAL : RAM=%zd, Free=%zd, Auto=%zd, Hardware=%zd, Accessible=%zd", mXRAMSize, mXRAMFree, mXRAMAuto, mXRAMHardware, mXRAMAccessible);
}




AMTRS_NAMESPACE_END
#endif
