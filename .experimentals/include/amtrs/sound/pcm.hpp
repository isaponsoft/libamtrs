/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__sound__pcm__hpp
#define	__libamtrs__sound__pcm__hpp
#include <vector>
#include "def.hpp"
#include "types.hpp"
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! PCMとして利用可能な音源データを抽象化します。
// ----------------------------------------------------------------------------
class	pcm
		: public ref_object
{
public:
	pcm(int _channels, int _bits_per_sample, int64_t _samples_per_sec);
	pcm(const pcm* _src);
	
	// ==============================================================
	//! サウンドから指定した時間のデータを読み取ります。
	// --------------------------------------------------------------
	//! サウンドが終端に到達すると flase を返します。
	// --------------------------------------------------------------
	//!	_copied_size[out]
	//!		読み込んだデータサイズ
	//!	_buffer[out]
	//!		データの読み込み先のバッファ
	//!	_bufferSize
	// --------------------------------------------------------------
	bool read(std::size_t* _copied_size, void* _buffer, std::size_t _bufferSize);


	// ==============================================================
	//! すべてのデータをデコードして返します。
	// --------------------------------------------------------------
	std::vector<uint8_t> read_all();

	// ==============================================================
	//! 指定した秒が何バイトに相当するか計算します。
	// --------------------------------------------------------------
	std::size_t compute_byte_size(double _sec) const noexcept
	{
		return	(std::size_t)(samples_par_sec() * (_sec / 1000.0)) * (bits_per_sample() * channels() / 8);
	}


	// ==============================================================
	//! 指定したバイト数のデータが何秒に相当するか計算します。
	// --------------------------------------------------------------
	double compute_second(std::size_t _byte_size) const noexcept { return static_cast<double>(_byte_size * 1000) / (samples_par_sec() * bits_per_sample() * channels() / 8); }


	// ==============================================================
	//! サンプルあたりのビット数を取得します。
	// --------------------------------------------------------------
	//! 
	// --------------------------------------------------------------
	std::size_t bits_per_sample() const noexcept { return mFormat.bitsParSamples; }

	// ==============================================================
	//! サウンドのチャンネル数を取得します。
	// --------------------------------------------------------------
	//! モノラルだと 1、ステレオだと 2 になります。
	// --------------------------------------------------------------
	std::size_t channels() const noexcept { return mFormat.channels; }


	// ==============================================================
	//! 1秒当たりのサンプル数を取得します。
	// --------------------------------------------------------------
	//! 44.1KHzのサウンドであれば 44100 になります。
	// --------------------------------------------------------------
	std::size_t samples_par_sec() const noexcept { return mFormat.samplesParSecond; }


	// ==============================================================
	//! decode() が終了した場合は true を返します。
	// --------------------------------------------------------------
	bool is_end() const noexcept { return mEnded; }


	virtual double seek(double _seek_milli_time)
	{
		return	0;
	}

	const sound_format& format() const noexcept { return mFormat; }


protected:
	virtual bool on_read(std::size_t* _copied_size, void* _buffer, std::size_t _bufferSize) = 0;

private:
	sound_format	mFormat;
	bool			mEnded			= false;
};


inline pcm::pcm(int _channels, int _bits_per_sample, int64_t _samples_per_sec)
{
	mFormat.channels			= _channels;
	mFormat.bitsParSamples		= _bits_per_sample;
	mFormat.samplesParSecond	= _samples_per_sec;
}


inline pcm::pcm(const pcm* _src)
	: mFormat(_src->mFormat)
{
}


inline bool pcm::read(std::size_t* _copied_size, void* _buffer, std::size_t _bufferSize)
{
	std::size_t	copied	= 0;
	if (!is_end())
	{
		if (!on_read(&copied, _buffer, _bufferSize))
		{
			mEnded	= true;
		}
	}
	if (_copied_size)
	{
		*_copied_size	= copied;
	}
	return	!is_end();
}


inline std::vector<uint8_t> pcm::read_all()
{
	uint8_t					buff[16384];
	std::vector<uint8_t>	retval;
	retval.reserve(16384);
	while (!is_end())
	{
		std::size_t	rs;
		read(&rs, buff, sizeof(buff));
		if (rs > 0)
		{
			auto	pos	= retval.size();
			retval.resize(pos + rs);
			memcpy(retval.data() + pos, buff, rs);
		}
	}
	return	retval;
}



AMTRS_NAMESPACE_END
#endif
