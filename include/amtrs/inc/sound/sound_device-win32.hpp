#ifndef	__libamtrs__sound__sound_device_win32__hpp
#define	__libamtrs__sound__sound_device_win32__hpp
#include "../../os/win32.hpp"
#include <xaudio2.h>
#include "../../logging.hpp"
#include "def.hpp"

#pragma comment(lib, "ole32.lib")
#pragma	comment(lib, "XAudio2.lib")

AMTRS_NAMESPACE_BEGIN

class	sound_device_win32;
class	sound_player_win32;


class	sound_buffer_win32
		: public sound_buffer
{
public:
	sound_buffer_win32(sound_device_win32* _device, std::size_t _bytesSize)
		: mDevice(_device)
		, mBufferSize(_bytesSize)
	{}

	~sound_buffer_win32()
	{
		release();
	}

	virtual std::size_t size() const noexcept
	{
		return	mBufferSize;
	}

	void initialize(const void* _buffer)
	{
		mBufferData.resize(mBufferSize);
		std::memcpy(mBufferData.data(), _buffer, mBufferSize);
	}

	void release()
	{
	}

	sound_device_win32*	mDevice			= nullptr;
	std::size_t			mBufferSize		= 0;
	std::vector<char>	mBufferData;


	friend	sound_device_win32;
	friend	sound_player_win32;
};



class	sound_player_win32
		: public sound_player
		, private IXAudio2VoiceCallback
{
public:
	sound_player_win32(sound_device_win32* _device)
		: mDevice(_device)
	{}


	~sound_player_win32()
	{
		release();
	}


	virtual void set_buffer(sound_buffer* _buffer) override
	{
		mActiveBuffer	= static_cast<sound_buffer_win32*>(_buffer);

		std::memset(&mAudioBuffer, 0, sizeof(mAudioBuffer));
		mAudioBuffer.AudioBytes		= (UINT32)mActiveBuffer->mBufferSize;
		mAudioBuffer.pAudioData		= (BYTE*)mActiveBuffer->mBufferData.data();
		mAudioBuffer.Flags			= XAUDIO2_END_OF_STREAM;
		mVoice->SubmitSourceBuffer(&mAudioBuffer);
	}


	virtual void play() override
	{
		if (mState == status::stopped || mState == status::paused)
		{
			if (mState == status::stopped)
			{
				mVoice->FlushSourceBuffers();
				mVoice->SubmitSourceBuffer(&mAudioBuffer);
			}
			mVoice->Start(0);
			mState	= status::playing;
			notification(message::play);
		}
	}

	
	virtual void stop() override
	{
		if (mState == status::playing)
		{
			mVoice->Stop(0, XAUDIO2_COMMIT_NOW);
			mState	= status::stopped;
			notification(message::stop);
		}
		if (mState == status::paused)
		{
			mState	= status::stopped;
			notification(message::stop);
		}
	}


	virtual void pause() override
	{
		if (mState == status::playing)
		{
			mVoice->Stop(0, XAUDIO2_COMMIT_NOW);
			mState	= status::paused;
			notification(message::pause);
		}
	}


	virtual status state() const noexcept override
	{
		return	mState;
	}

protected:
	// IXAudio2VoiceCallback ------------------------------
	virtual void WINAPI OnStreamEnd() override
	{
		mState	= status::stopped;
		notification(message::finish);
		notification(message::stop);
	}

	virtual void WINAPI OnVoiceProcessingPassEnd() override
	{}

	virtual void WINAPI OnVoiceProcessingPassStart(UINT32) override
	{
	}

	virtual void WINAPI OnBufferEnd(void *) override
	{}

	virtual void WINAPI OnBufferStart(void *) override
	{}

	virtual void WINAPI OnLoopEnd(void *) override
	{}

	virtual void WINAPI OnVoiceError(void *, HRESULT) override
	{}
	// ----------------------------------------------------


private:
	void initialize(IXAudio2* _audio)
	{
		HRESULT		hRes;

		// 出力用のマスターを作成
		WAVEFORMATEX	format;
		std::memset(&format, 0, sizeof(format));

        format.wFormatTag		= WAVE_FORMAT_PCM;
        format.nChannels		= (WORD)2;
        format.wBitsPerSample	= (WORD)16;
        format.nSamplesPerSec	= 44100;
        format.nBlockAlign		= format.wBitsPerSample / 8 * format.nChannels;
        format.nAvgBytesPerSec	= format.nSamplesPerSec * format.nBlockAlign;

		hRes	= _audio->CreateSourceVoice
					( &mVoice
					, &format
					, XAUDIO2_VOICE_NOPITCH //|XAUDIO2_VOICE_MUSIC
					, XAUDIO2_DEFAULT_FREQ_RATIO
					, this
					, nullptr
					, nullptr
					);
		if (FAILED(hRes))
		{
			throw os::win32::make_system_error("IXAudio2::CreateSourceVoice");
		}

		//AMTRS_DEBUG_LOG("IXAudio2SourceVoice : channels=%d, bit/sample=%d, samples/sec=%d", format.nChannels, format.wBitsPerSample, format.nSamplesPerSec);
	}

	void release()
	{
		if (mVoice)
		{
			mVoice->DestroyVoice();
			mVoice	= nullptr;
		}
	}

	sound_device_win32*		mDevice			= nullptr;
	IXAudio2SourceVoice*	mVoice			= nullptr;
	XAUDIO2_BUFFER			mAudioBuffer	= {0};
	ref<sound_buffer_win32>	mActiveBuffer;
	status					mState			= status::stopped;

	friend	sound_device_win32;
};






class	sound_device_win32
		: public sound_device
{
public:
	~sound_device_win32()
	{
		release();
	}

	void release()
	{
		if (mMasterVoice)
		{
			mMasterVoice->DestroyVoice();
			mMasterVoice	= nullptr;
		}
		if (mXaudio)
		{
			mXaudio->Release();
			mXaudio			= nullptr;
		}
		CoUninitialize();
	}

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
		ref<sound_player_win32>	retval	= new sound_player_win32(this);
		retval->initialize(mXaudio);
		return	retval;
	}


	// ========================================================================
	//! プレイヤーに適合するサウンドバッファをを生成します。
	// ------------------------------------------------------------------------
	//! バッファのサイズを設定できます。
	// ------------------------------------------------------------------------
	virtual ref<sound_buffer> create_buffer(std::size_t _bytesSize) override
	{
		ref<sound_buffer_win32>	buff	= new sound_buffer_win32(this, _bytesSize);
		std::vector<char>		zero(_bytesSize);
		for (auto& c : zero) { c = 0; }
		buff->initialize(zero.data());
		return	buff;
	}


	virtual ref<sound_buffer> create_buffer(const void* _pcmdata, std::size_t _bytesSize) override
	{
		ref<sound_buffer_win32>	buff	= new sound_buffer_win32(this, _bytesSize);
		buff->initialize(_pcmdata);
		return	buff;
	}



	//! デバイスを初期化します。
	inline void initialize()
	{
		HRESULT	hRes;
		UINT32	flags	= 0;

		// 初期化
		hRes	= CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hRes))
		{
			throw os::win32::make_system_error("CoInitializeEx");
		}

		hRes	= XAudio2Create(&mXaudio, flags, XAUDIO2_DEFAULT_PROCESSOR );
		if (FAILED(hRes))
		{
			throw os::win32::make_system_error("XAudio2Create");
		}

		hRes	= mXaudio->CreateMasteringVoice(&mMasterVoice);
		if (FAILED(hRes))
		{
			throw os::win32::make_system_error("CreateMasteringVoice");
		}
	}

private:
	IXAudio2*				mXaudio			= nullptr;
	IXAudio2MasteringVoice*	mMasterVoice	= nullptr;
};



inline ref<sound_device> sound_device::create()
{
	ref<sound_device_win32>	dev	= new sound_device_win32();
	dev->initialize();
	return	dev;
}





AMTRS_NAMESPACE_END
#endif
