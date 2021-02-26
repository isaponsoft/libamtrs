/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__sound__sound_device_android__hpp
#define	__libamtrs__sound__sound_device_android__hpp
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>
#include "android-api-android_app.hpp"
#include "../logging/@"
#include "../java/android/media/AudioManager.hpp"
AMTRS_NAMESPACE_BEGIN

class	sound_device_android;
class	sound_player_android;

class	sound_buffer_android
		: public sound_buffer
{
public:
	sound_buffer_android(sound_device_android* _device, const sound_format& _format, std::size_t _bytesSize)
		: mDevice(_device)
		, mBufferSize(_bytesSize)
		, mFormat(_format)
	{}

	~sound_buffer_android()
	{
		release();
	}

	virtual std::size_t size() const noexcept
	{
		return	mBufferSize;
	}

	void initialize(const void* _buffer)
	{
		mBuffer.resize(mBufferSize);
		std::memcpy(mBuffer.data(), _buffer, mBufferSize);
	}

	void release()
	{
	}

	const std::vector<char>& buffer() noexcept { return mBuffer; }

	sound_device_android*	mDevice			= nullptr;
	std::size_t				mBufferSize		= 0;
	std::vector<char>		mBuffer;
	sound_format			mFormat;

	friend	sound_device_android;
	friend	sound_player_android;
};



class	sound_player_android
		: public sound_player
{
public:
	sound_player_android(sound_device_android* _device)
		: mDevice(_device)
	{}


	~sound_player_android()
	{
		release();
	}


	virtual void set_buffer(sound_buffer* _buffer) override
	{
		mActiveBuffer	= static_cast<sound_buffer_android*>(_buffer);
	}


	virtual void play() override
	{
		(*mPlayIf)->SetPlayState(mPlayIf, SL_PLAYSTATE_STOPPED);
		(*mBufferQueue)->Enqueue(mBufferQueue, mActiveBuffer->buffer().data(), (unsigned int)mActiveBuffer->buffer().size());
		(*mPlayIf)->SetPlayState(mPlayIf, SL_PLAYSTATE_PLAYING);
		mState	= status::playing;
		notification(message::play);
	}

	
	virtual void stop() override
	{
		SLuint32 state;
		(*mPlayIf)->GetPlayState(mPlayIf, &state);
		if (state != SL_PLAYSTATE_STOPPED)
		{
			std::lock_guard<std::mutex> lock(mMutex);
			(*mPlayIf)->SetPlayState(mPlayIf, SL_PLAYSTATE_STOPPED);
			(*mBufferQueue)->Clear(mBufferQueue);
		}
		if (status::stopped != mState)
		{
			mState	= status::stopped;
			notification(message::stop);
		}
	}


	virtual void pause() override
	{
		if (mPlayIf)
		{
			(*mPlayIf)->SetPlayState(mPlayIf, SL_PLAYSTATE_PAUSED);
			mState	= status::paused;
			notification(message::pause);
		}
	}


	virtual status state() const noexcept override
	{
		return	mState;
	}


	struct	source : ref_object
	{
		virtual SLDataSource* get_source_data() noexcept = 0;
	};

	struct	source_pcm : source
	{
		virtual SLDataSource* get_source_data() noexcept override
		{
			return	&mSource;
		}

		source_pcm(const sound_format& _format)
		{
			mSource	= {&mLocator, &mFormat};

			mFormat.formatType		= SL_DATAFORMAT_PCM;
			mFormat.samplesPerSec	= _format.samplesParSecond * 1000;	// SL_SAMPLINGRATE_44_1, SL_SAMPLINGRATE_32, SL_SAMPLINGRATE_22_05,
			mFormat.bitsPerSample	= _format.bitsParSamples;			// SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_8
			mFormat.containerSize	= _format.bitsParSamples;
			mFormat.numChannels		= _format.channels;
			mFormat.channelMask		= (_format.channels == 1) ? SL_SPEAKER_FRONT_CENTER  : (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT);
			mFormat.endianness		= SL_BYTEORDER_LITTLEENDIAN;
		}

		SLDataLocator_AndroidSimpleBufferQueue	mLocator	= { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1};
		SLDataFormat_PCM						mFormat;
		SLDataSource							mSource;
	};


	struct	data_sink : ref_object
	{
		virtual SLDataSink* get_datasink() noexcept = 0;
	};

	struct	simplebuffer_sink : data_sink
	{
		static constexpr	SLuint32	blocks_size		= 2;

		simplebuffer_sink()
		{
			mSink		= { &mLocator, nullptr};
		}

		virtual SLDataSink* get_datasink() noexcept override { return &mSink; }

		SLDataLocator_AndroidSimpleBufferQueue		mLocator	= { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, blocks_size };
		SLDataSink 									mSink;
	};

	struct	outmix_sink : data_sink
	{
		outmix_sink(SLObjectItf _mixer)
		{
			mSink		= { &mLocator, nullptr};
			mLocator	= { SL_DATALOCATOR_OUTPUTMIX, _mixer};
		}

		virtual SLDataSink* get_datasink() noexcept override { return &mSink; }

		SLDataLocator_OutputMix			mLocator;
		SLDataSink 						mSink;
	};


	void initialize(const sound_format& _format, SLEngineItf _engine, SLObjectItf _mix, sound_stream_type _stream)
	{
		SLresult 					res;

		mSource	= new source_pcm(_format);
		mSink	= new outmix_sink(_mix);


		// 取得したいインターフェースとインターフェースに対するパラメータ
		const SLInterfaceID			interfaceIds[]	= { SL_IID_PLAY,     SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_PREFETCHSTATUS, SL_IID_METADATAEXTRACTION, SL_IID_ANDROIDCONFIGURATION };
		const SLboolean				interfaceArgs[]	= { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,                 SL_BOOLEAN_TRUE,       SL_BOOLEAN_TRUE          , SL_BOOLEAN_TRUE             };
		static_assert(sizeof(interfaceIds)/sizeof(interfaceIds[0]) == sizeof(interfaceArgs)/sizeof(interfaceArgs[0]));
		res = (*_engine)->CreateAudioPlayer(_engine, &mAudioPlayer, mSource->get_source_data(), mSink->get_datasink(), sizeof(interfaceIds)/sizeof(interfaceIds[0]), interfaceIds, interfaceArgs);
		if (res != SL_RESULT_SUCCESS)
		{
			AMTRS_DEBUG_LOG("SLEngineItf::CreateAudioPlayer");
			throw	std::runtime_error("SLEngineItf::CreateAudioPlayer");
		}

		// AudioStream は実体化する前に設定すること。
		SLint32	streamType	= _stream == sound_stream_type::stream_default
							? SL_ANDROID_STREAM_MEDIA
							: SL_ANDROID_STREAM_ALARM;
		(*mAudioPlayer)->GetInterface(mAudioPlayer, SL_IID_ANDROIDCONFIGURATION,		&mConfigIf);
		res	= (*mConfigIf)->SetConfiguration(mConfigIf, SL_ANDROID_KEY_STREAM_TYPE, &streamType, sizeof(SLint32));


		// 実体化
		(*mAudioPlayer)->Realize(mAudioPlayer, SL_BOOLEAN_FALSE);

		(*mAudioPlayer)->GetInterface(mAudioPlayer, SL_IID_PLAY,						&mPlayIf);
		(*mAudioPlayer)->GetInterface(mAudioPlayer, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,	&mBufferQueue);
		(*mAudioPlayer)->GetInterface(mAudioPlayer, SL_IID_EFFECTSEND,					&mEffectSendIf);
		(*mAudioPlayer)->GetInterface(mAudioPlayer, SL_IID_VOLUME,						&mVolumeIf);

		res = (*mBufferQueue)->RegisterCallback(mBufferQueue, &sound_player_android::on_finish_stream, this);
	}

	void release()
	{
		std::lock_guard<std::mutex> lock(mMutex);
		if (mAudioPlayer)
		{
			(*mAudioPlayer)->Destroy(mAudioPlayer);
			mAudioPlayer	= nullptr;
		}
	}


	//! 再生終了時のコールバック
	static void on_finish_stream(SLAndroidSimpleBufferQueueItf _bufferQueue, void* _context)
	{
		auto*	thiz	= reinterpret_cast<sound_player_android*>(_context);
		std::lock_guard<std::mutex> lock(thiz->mMutex);
		thiz->mState	= status::stopped;
		thiz->notification(message::finish);
		thiz->notification(message::stop);
	}

	sound_device_android*			mDevice			= nullptr;
	ref<sound_buffer_android>		mActiveBuffer;
	status							mState			= status::stopped;
	SLObjectItf						mAudioPlayer	= nullptr;
	SLAndroidSimpleBufferQueueItf	mBufferQueue	= nullptr;
	SLPlayItf						mPlayIf			= nullptr;
	SLEffectSendItf					mEffectSendIf	= nullptr;
	SLVolumeItf						mVolumeIf		= nullptr;
	SLAndroidConfigurationItf		mConfigIf		= nullptr;
	ref<source>						mSource;
	std::mutex						mMutex;
	ref<data_sink>					mSink;

	friend	sound_device_android;
};






class	sound_device_android
		: public sound_device
{
public:
	~sound_device_android()
	{
		release();
	}

	void release()
	{
		if (mSlenObject)
		{
			(*mSlenObject)->Destroy(mSlenObject);
			mSlenObject	= nullptr;
		}
	}

	virtual void set_volume_cntroller(sound_stream_type _stream) override
	{
		if (_stream == sound_stream_type::stream_default)
		{
			os::android::get_activity().setVolumeControlStream(java::classes::android::media::AudioManager::find().get_STREAM_MUSIC());
		}
		else
		{
			os::android::get_activity().setVolumeControlStream(java::classes::android::media::AudioManager::find().get_STREAM_ALARM());
		}
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
	virtual ref<sound_player> create_player(const sound_format& _format, sound_stream_type _stream) override
	{
		ref<sound_player_android>	retval	= new sound_player_android(this);
		retval->initialize(_format, mSlenEngine, mSleOutputMixObj, _stream);
		return	retval;
	}


	// ========================================================================
	//! プレイヤーに適合するサウンドバッファをを生成します。
	// ------------------------------------------------------------------------
	//! バッファのサイズを設定できます。
	// ------------------------------------------------------------------------
	virtual ref<sound_buffer> create_buffer(const sound_format& _format, std::size_t _bytesSize) override
	{
		ref<sound_buffer_android>	buff	= new sound_buffer_android(this, _format, _bytesSize);
		std::vector<char>		zero(_bytesSize);
		for (auto& c : zero) { c = 0; }
		buff->initialize(zero.data());
		return	buff;
	}


	virtual ref<sound_buffer> create_buffer(const sound_format& _format, const void* _pcmdata, std::size_t _bytesSize) override
	{
		ref<sound_buffer_android>	buff	= new sound_buffer_android(this, _format, _bytesSize);
		buff->initialize(_pcmdata);
		return	buff;
	}



	//! デバイスを初期化します。
	inline void initialize()
	{
		SLresult		res;

		const SLInterfaceID	lEngineMixIIDs[]	= {SL_IID_ENGINE}; 
		const SLboolean		lEngineMixReqs[]	= {SL_BOOLEAN_TRUE}; 
		res	= slCreateEngine(&mSlenObject, 0, nullptr, 1, lEngineMixIIDs, lEngineMixReqs);
		if (res != SL_RESULT_SUCCESS)
		{
			AMTRS_DEBUG_LOG("slCreateEngine");
			throw	std::runtime_error("slCreateEngine");
		}
		res = (*mSlenObject)->Realize(mSlenObject, SL_BOOLEAN_FALSE);
		if (res != SL_RESULT_SUCCESS)
		{
			AMTRS_DEBUG_LOG("SLObjectItf::Realize");
			throw	std::runtime_error("SLObjectItf::Realize");
		}
		res = (*mSlenObject)->GetInterface(mSlenObject, SL_IID_ENGINE, &mSlenEngine);
		if (res != SL_RESULT_SUCCESS)
		{
			AMTRS_DEBUG_LOG("SLObjectItf::GetInterface");
			throw	std::runtime_error("SLObjectItf::GetInterface");
		}


		const SLInterfaceID	ids[1] 			= { SL_IID_ENVIRONMENTALREVERB };
		const SLboolean		req[1] 			= { SL_BOOLEAN_TRUE            };
		res = (*mSlenEngine)->CreateOutputMix(mSlenEngine, &mSleOutputMixObj, 1, ids, req);
		if (res != SL_RESULT_SUCCESS)
		{
			AMTRS_DEBUG_LOG("SLEngineItf::CreateOutputMix");
			throw	std::runtime_error("SLEngineItf::CreateOutputMix");
		}

		res = (*mSleOutputMixObj)->Realize(mSleOutputMixObj, SL_BOOLEAN_FALSE);
		if (res != SL_RESULT_SUCCESS)
		{
			AMTRS_DEBUG_LOG("SLObjectItf::Realize");
			throw	std::runtime_error("SLObjectItf::Realize");
		}

		// ボリュームコントロールをメディアの音量に変更
		os::android::get_activity().setVolumeControlStream(java::classes::android::media::AudioManager::find().get_STREAM_MUSIC());
	}


private:
	SLObjectItf						mSlenObject			= nullptr;
	SLEngineItf						mSlenEngine			= nullptr;
	SLObjectItf						mSleOutputMixObj	= nullptr;
};





inline ref<sound_device> sound_device::create()
{
	ref<sound_device_android>	dev	= new sound_device_android();
	dev->initialize();
	return	dev;
}




AMTRS_NAMESPACE_END
#endif
