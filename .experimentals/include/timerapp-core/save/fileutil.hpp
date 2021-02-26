/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__fileutil__hpp
#define	__fileutil__hpp
#include <fstream>
#include <string_view>
#include <amtrs/filesystem/@>
#include <amtrs/memory/endian.hpp>

//! 指定したサイズのデータをリトルエンディアンで書き込む
template<class T, class Stream>
bool write_int(Stream& _out, T _n)
{
	if (!_out.good())
	{
		return	false;
	}

	if constexpr (std::is_same<T,bool>::value)
	{
		return	write_int<uint8_t, Stream>(_out, static_cast<uint8_t>(_n ? 1 : 0));
	}
	else
	{
		union
		{
			T		i;
			char	b[sizeof(T)];
		}	t;

		t.i	= amtrs::endian_util::encode<amtrs::endian::little>(_n);
		_out.write(t.b, sizeof(t.b));
		return	!_out.fail();
	}
}



//! 指定したサイズのデータをリトルエンディアンで書き込む
template<class Str, class Stream>
bool write_str(Stream& _out, Str const& _n)
{
	std::string_view	t(_n);

	if (!_out.good())
	{
		return	false;
	}

	if (!write_int<uint16_t>(_out, static_cast<uint16_t>(t.size())))
	{
		return	false;
	}
	_out.write(t.data(), t.size());
	return	!_out.fail();
}


//! 32bitのチェックサムデータを書き込む
template<class Stream>
bool write_checksum32(Stream& _out, uint32_t _checksum)
{
	if (!_out.good())
	{
		return	false;
	}
	return	write_int<uint32_t>(_out, _checksum);
}



//! チャンクヘッダを付けて保存する
template<class Str, class Stream>
bool write_chunk(Stream& _out, Str _chunk, std::string const& _data)
{
	std::string_view	cnk(_chunk);
	if (!_out.good())
	{
		return	false;
	}

	// チャンクを含めた全体の長さを書き出す
	uint32_t			ts	= static_cast<uint32_t>(sizeof(uint16_t) + cnk.size() + _data.size() + sizeof(uint32_t));
	if (!write_int<uint32_t>(_out, ts))
	{
		return	false;
	}

	// チャンク名の長さを書き出す
	if (!write_int<uint16_t>(_out, static_cast<uint16_t>(cnk.size())))
	{
		return	false;
	}


	// チャンク名を書き出す
	_out.write(cnk.data(), cnk.size());
	if (!_out.good())
	{
		return	false;
	}

	// データ本体の長さを書き出す
	if (!write_int<uint32_t>(_out, static_cast<uint32_t>(_data.size())))
	{
		return	false;
	}

	// データ本体を書き出す
	_out.write(_data.data(), _data.size());
	if (!_out.good())
	{
		return	false;
	}

	return	write_checksum32(_out, ts ^ 0xfa9cfd3a);
}



//! チャンクヘッダの終端データを保存する
template<class Stream>
bool write_terminate_chunk(Stream& _out)
{
	return	write_chunk<std::string_view, Stream>(_out, "", "");
}




//! 指定したサイズのデータをリトルエンディアンで読み込む
template<class T, class Stream>
bool read_int(T& _value, Stream& _in)
{
	if (!_in.good())
	{
		return	false;
	}

	union
	{
		T		i;
		char	b[sizeof(T)];
	}	t;

	_in.read(t.b, sizeof(t.b));
	if (!_in.good())
	{
		return	false;
	}
	_value	= amtrs::endian_util::decode<amtrs::endian::little>(t.i);
	return	true;
}


template<class Str, class Stream>
bool read_str(Str& _value, Stream& _in)
{
	if (!_in.good())
	{
		return	false;
	}

	uint16_t	len;
	if (!read_int<uint16_t>(len, _in))
	{
		return	false;
	}
	_value.resize(len);
	if (len > 0)
	{
		_in.read(_value.data(), len);
	}
	return	!_in.fail();
}



//! 32bitのチェックサムデータを読み込み、指定した値と一致するか調べる
template<class Stream>
bool read_checksum32(Stream& _in, uint32_t _checksum)
{
	if (!_in.good())
	{
		return	false;
	}

	uint32_t	cs;
	if (!read_int<uint32_t>(cs, _in))
	{
		return	false;
	}
	return	cs == _checksum;
}




//! チャンクヘッダを付けて読み込む
//! _chunk
//!		チャンク識別用の文字列を格納するバッファ
//!	_stream
//!		チャンク全体を収めた文字列を格納するバッファ
//!	_size
//!		チャンク内のデータサイズを格納するバッファ
//!		チャンク識別子を示す文字列のデータサイズは含まれない。
template<class Str, class Stream>
bool read_chunk(Str& _identify, Str& _chunk, size_t& _size, Stream& _in)
{
	// チャンクを含めた全体の長さを読みだす
	uint32_t	totalLen;
	if (!read_int<uint32_t, Stream>(totalLen, _in))
	{
		AMTRS_DEBUG_LOG("No size");
		return	false;
	}
	uint32_t	ts	= totalLen;

	//! 長さを読み込むだけのサイズが存在しない場合はエラー
	if (totalLen < sizeof(uint16_t))
	{
		AMTRS_DEBUG_LOG("Has not identify length size");
		return	false;
	}

	// チャンク文字列の長さ
	uint16_t	identifyLen;
	if (!read_int<uint16_t, Stream>(identifyLen, _in))
	{
		AMTRS_DEBUG_LOG("Can not read identify length");
		return	false;
	}
	totalLen	-= sizeof(uint16_t);

	//! 読み込むだけのサイズが存在しない場合はエラー
	if (totalLen < identifyLen)
	{
		AMTRS_DEBUG_LOG("Has not identify size");
		return	false;
	}

	_identify.resize(identifyLen);
	_in.read(_identify.data(), identifyLen);
	if (!_in.good())
	{
		AMTRS_DEBUG_LOG("Can not read identify");
		return	false;
	}
	totalLen	-= identifyLen;

	// チャンク本体の長さを読み取る
	uint32_t	chunkLen;
	if (!read_int<uint32_t, Stream>(chunkLen, _in))
	{
		AMTRS_DEBUG_LOG("Can not read chunk length");
		return	false;
	}
	totalLen	-= sizeof(uint32_t);

	// データ確認
	if (chunkLen != totalLen)
	{
		AMTRS_DEBUG_LOG("Un match chunk length %zd != %zd", (size_t)chunkLen, (size_t)totalLen);
		return	false;
	}

	// ブロックの長さを返す
	_size		= totalLen;

	_chunk.resize(totalLen);
	if (totalLen > 0)
	{
		_in.read(_chunk.data(), totalLen);
		if (!_in.good())
		{
			AMTRS_DEBUG_LOG("Can not read chunk blocks");
			return	false;
		}
	}
	if (!read_checksum32(_in, ts ^ 0xfa9cfd3a))
	{
		AMTRS_DEBUG_LOG("Chunk checksum error %s", _identify.c_str());
		return	false;
	}

	//AMTRS_DEBUG_LOG("Chunk %s", _identify.c_str());
	return	true;
}




bool writefile(std::string const& _filename, std::string const& _data);
bool readfile(std::string& _data, std::string const& _filename);


#endif
