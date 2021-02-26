/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__zip__types__hpp
#define __libamtrs__filesystem__zip__types__hpp
AMTRS_FILESYSTEM_ZIP_NAMESPACE_BEGIN

static constexpr char		lfhs[4]	= { 'P', 'K', 0x03, 0x04 };
static constexpr char		cdes[4]	= { 'P', 'K', 0x01, 0x02 };
static constexpr char		eocd[4]	= { 'P', 'K', 0x05, 0x06 };

using	size_type		= size_t;


static constexpr size_type	localfilehead_size			= 30;
static constexpr size_type	centraldirentry_size		= 46;
static constexpr size_type	end_of_centraldirentry_size	= 20;

struct	entry_data
{
	uint16_t	extracVersion;		//!< 展開に必要なバージョン
	uint16_t	basicBitFlags;		//!< 汎用ビットフラグ
	uint16_t	compressMethod;		//!< 圧縮メソッド
	uint16_t	lastModifiedTime;	//!< 最終変更時間
	uint16_t	lastModifiedDate;	//!< 最終変更日付
	uint32_t	crc32;				//!< チェックサム
	uint32_t	compressedSize;		//!< 圧縮サイズ
	uint32_t	uncompressSize;		//!< 非圧縮サイズ
	uint16_t	filenameSize;		//!< ファイル名の長さ
	uint16_t	extraFieldSize;		//!< 拡張フィールドの長さ
};


struct	localfilehead
{

	char		signature[4];		//!< = lfhs
	entry_data	fileInfo;
	// char	filename[filenameSize]
	// char	extraField[extraFieldSize]
};


struct	centraldirentry
{

	char		signature[4];		//!< = cdes
	uint16_t	createVersion;		//!< 作成されたバージョン
	entry_data	fileInfo;
	uint16_t	commentSize;		//!< コメントの長さ
	uint16_t	numberDiskWithCD;	//!< ファイルが開始するディスク番号
	uint16_t	intenalAttribute;	//!< 内部ファイル属性
	uint32_t	externalAttribute;	//!< 外部ファイル属性
	uint32_t	offsetCurFile;		//!< ローカルファイルヘッダの相対オフセット
	// char	filename[filenameSize]
	// char	extraField[extraFieldSize]
	// char	comment[commentSize]
};

struct	end_of_centraldirentry
{

	char		signature[4];			//!< = eocd
	uint16_t	numberDisk;				//!< このディスクの数
	uint16_t	numberDiskWithCD;		//!< セントラルディレクトリが開始するディスク
	uint16_t	numberEntry;			//!< このディスク上のセントラルディレクトリレコードの数
	uint16_t	numberEntryCD;			//!< セントラルディレクトリレコードの合計数
	uint32_t	sizeOfCentralDir;		//!< セントラルディレクトリのサイズ (バイト)
	uint32_t	offsetOfCentralDir;		//!< セントラルディレクトリの開始位置のオフセット
	uint16_t	commentSize;			//!< コメントの長さ
	// char	comment[commentSize]
};

AMTRS_FILESYSTEM_ZIP_NAMESPACE_END
AMTRS_IO_NAMESPACE_BEGIN

template<class In>
bool read(filesystem::zip::localfilehead& _lfh, In&& _in)
{
	char	buff[filesystem::zip::localfilehead_size];
	if (io::read(buff, std::forward<In>(_in)) != sizeof(buff))
	{
		return	false;
	}

	auto	i	= io::make_stream_in(make_view(buff));
	io::read(_lfh.signature, i);
	if (std::memcmp(_lfh.signature, filesystem::zip::lfhs, sizeof(filesystem::zip::lfhs)))
	{
		return	false;
	}
	io::read<endian::little>(_lfh.fileInfo.extracVersion,		i);
	io::read<endian::little>(_lfh.fileInfo.basicBitFlags,		i);
	io::read<endian::little>(_lfh.fileInfo.compressMethod,		i);
	io::read<endian::little>(_lfh.fileInfo.lastModifiedTime,	i);
	io::read<endian::little>(_lfh.fileInfo.lastModifiedDate,	i);
	io::read<endian::little>(_lfh.fileInfo.crc32,				i);
	io::read<endian::little>(_lfh.fileInfo.compressedSize,		i);
	io::read<endian::little>(_lfh.fileInfo.uncompressSize,		i);
	io::read<endian::little>(_lfh.fileInfo.filenameSize,		i);
	io::read<endian::little>(_lfh.fileInfo.extraFieldSize,		i);
	return	true;
}

template<class In>
bool read(filesystem::zip::centraldirentry& _cde, In&& _in)
{
	char	buff[filesystem::zip::centraldirentry_size];
	if (io::read(buff, std::forward<In>(_in)) != sizeof(buff))
	{
		return	false;
	}

	auto	i	= io::make_stream_in(make_view(buff));
	io::read(_cde.signature, i);
	if (std::memcmp(_cde.signature, filesystem::zip::cdes, sizeof(filesystem::zip::cdes)))
	{
		return	false;
	}
	io::read<endian::little>(_cde.createVersion,				i);
	io::read<endian::little>(_cde.fileInfo.extracVersion,		i);
	io::read<endian::little>(_cde.fileInfo.basicBitFlags,		i);
	io::read<endian::little>(_cde.fileInfo.compressMethod,		i);
	io::read<endian::little>(_cde.fileInfo.lastModifiedTime,	i);
	io::read<endian::little>(_cde.fileInfo.lastModifiedDate,	i);
	io::read<endian::little>(_cde.fileInfo.crc32,				i);
	io::read<endian::little>(_cde.fileInfo.compressedSize,		i);
	io::read<endian::little>(_cde.fileInfo.uncompressSize,		i);
	io::read<endian::little>(_cde.fileInfo.filenameSize,		i);
	io::read<endian::little>(_cde.fileInfo.extraFieldSize,		i);
	io::read<endian::little>(_cde.commentSize,					i);
	io::read<endian::little>(_cde.numberDiskWithCD,				i);
	io::read<endian::little>(_cde.intenalAttribute,				i);
	io::read<endian::little>(_cde.externalAttribute,			i);
	io::read<endian::little>(_cde.offsetCurFile,				i);
	return	true;
}

template<class In>
bool read(filesystem::zip::end_of_centraldirentry& _eoc, In&& _in)
{
	char	buff[filesystem::zip::end_of_centraldirentry_size];
	if (io::read(buff, std::forward<In>(_in)) != sizeof(buff))
	{
		return	false;
	}

	auto	i	= io::make_stream_in(make_view(buff));
	io::read(_eoc.signature, i);
	if (std::memcmp(_eoc.signature, filesystem::zip::eocd, sizeof(filesystem::zip::eocd)))
	{
		return	false;
	}
	io::read<endian::little>(_eoc.numberDisk,				i);
	io::read<endian::little>(_eoc.numberDiskWithCD,			i);
	io::read<endian::little>(_eoc.numberEntry,				i);
	io::read<endian::little>(_eoc.numberEntryCD,			i);
	if (_eoc.numberDisk || _eoc.numberDiskWithCD || (_eoc.numberEntryCD != _eoc.numberEntry))
	{
		return	false;
	}
	io::read<endian::little>(_eoc.sizeOfCentralDir,			i);
	io::read<endian::little>(_eoc.offsetOfCentralDir,		i);
	io::read<endian::little>(_eoc.commentSize,				i);
	return	true;
}

AMTRS_IO_NAMESPACE_END
#endif
