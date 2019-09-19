﻿#ifndef	__libamtrs__sound__sound_device__hpp
#define	__libamtrs__sound__sound_device__hpp
#include <functional>
#include "../../utility.hpp"
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


class	sound_device;
class	sound_buffer;
class	sound_player;


enum class	sound_stream_type
{
	stream_default		= 0,		//!< プラットフォームののオーディオストリーム
	stream_alarm		= 1,		//!< アラーム用のオーディオストリーム(Androidなど)
};


// ****************************************************************************
//! サウンドデバイスの管理を行います。
// ----------------------------------------------------------------------------
//! 
// ****************************************************************************
class	sound_device
		: public ref_object
{
public:
	static ref<sound_device> create();

	// ========================================================================
	//! ボリューム変更を行う対象を指定します(Android以外は無視されます)。
	// ------------------------------------------------------------------------
	virtual void set_volume_cntroller(sound_stream_type _stream = sound_stream_type::stream_default) {}

	// ========================================================================
	//! 利用可能なプレイヤーの数を取得します。
	// ------------------------------------------------------------------------
	virtual std::size_t get_max_player_count() const = 0;


	// ========================================================================
	//! プレイヤーを生成します。
	// ------------------------------------------------------------------------
	virtual ref<sound_player> create_player(sound_stream_type _stream = sound_stream_type::stream_default) = 0;


	// ========================================================================
	//! 指定するサイズのサウンドバッファを生成します。
	// ------------------------------------------------------------------------
	//! バッファのサイズを指定して生成します。バッファは 0 でクリアされます。
	// ------------------------------------------------------------------------
	virtual ref<sound_buffer> create_buffer(std::size_t _bytesSize) = 0;


	// ========================================================================
	//! 指定するサイズのサウンドバッファを生成します。
	// ------------------------------------------------------------------------
	//! バッファのサイズを指定して生成します。バッファは 0 でクリアされます。
	// ------------------------------------------------------------------------
	virtual ref<sound_buffer> create_buffer(const void* _pcmdata, std::size_t _bytesSize) = 0;


	// ========================================================================
	//! サウンドバッファをを生成します。
	// ------------------------------------------------------------------------
	//! 指定されたデータでバッファ内を初期化します。
	// ------------------------------------------------------------------------
//	virtual ref<sound_buffer> create_buffer(std::size_t _bytesSize) = 0;
};



// ****************************************************************************
//! サウンドバッファ。
// ----------------------------------------------------------------------------
//! PCMデータ、もしくはサウンドプレイヤーが再生可能なデータを保持します。
// ****************************************************************************
class	sound_buffer
		: public ref_object
{
public:
	// ========================================================================
	//! 
	// ------------------------------------------------------------------------
//	virtual void transfer(const void* _buffer) = 0;

	// ========================================================================
	//! バッファのサイズを取得します。
	// ------------------------------------------------------------------------
	virtual std::size_t size() const noexcept = 0;
};



// ****************************************************************************
//! サウンドプレイヤーの管理を行います。
// ----------------------------------------------------------------------------
//! ひとつのプレイヤーはひとつのサウンドバッファを再生します。
// ****************************************************************************
class	sound_player
		: public ref_object
{
public:
	enum class status
	{
		playing,			//!< 再生中
		paused,				//!< 一時停止中
		stopped,			//!< 停止中(もしくは何もしていない)
	};

	enum class message
	{
		play,
		pause,
		finish,
		stop,
	};

	// ========================================================================
	//! 再生用のバッファをセットします。
	// ------------------------------------------------------------------------
	virtual void set_buffer(sound_buffer* _buffer) = 0;


	// ========================================================================
	//! 再生を開始します。pause()している場合は再開します。
	// ------------------------------------------------------------------------
	virtual void play() = 0;


	// ========================================================================
	//! 再生中の場合は停止します。
	// ------------------------------------------------------------------------
	virtual void stop() = 0;


	// ========================================================================
	//! 再生中の場合は一時停止します。play() で再開できます。
	// ------------------------------------------------------------------------
	virtual void pause() = 0;


	// ========================================================================
	//! プレイヤーの状態を取得します。
	// ------------------------------------------------------------------------
	virtual status state() const noexcept = 0;


	// ========================================================================
	//! 再生中かどうか問い合わせます。
	// ------------------------------------------------------------------------
	bool is_playing() const noexcept { return state() == status::playing; }


	// ========================================================================
	//! 一時停止中かどうか問い合わせます。
	// ------------------------------------------------------------------------
	bool is_paused() const noexcept { return state() == status::paused; }


	// ========================================================================
	//! 停止中かどうか問い合わせます。
	// ------------------------------------------------------------------------
	bool is_stopped() const noexcept { return state() == status::stopped; }


	// ========================================================================
	//! 状態が変化したときに呼ばれるコールバックを設定します。
	// ------------------------------------------------------------------------
	void set_notification_callback(std::function<void(message)>&& _callback) { mCallback = std::move(_callback); }

protected:
	void notification(message _msg) { if (mCallback) { mCallback(_msg); } }

private:
	std::function<void(message)>	mCallback;
};




AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(sound_device)
#endif
