/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__gif__reader__hpp
#define	__libamtrs__graphics__gif__reader__hpp
#include "header.hpp"
AMTRS_GRAPHICS_GIF_NAMESPACE_BEGIN




template<class Stream>
class	image_data_reader
{
public:
	static constexpr size_t	octet_size	= sizeof(uint8_t)*8;

	image_data_reader(Stream& _stream)
		: mStream(_stream)
	{}


	bool eof() const noexcept { return mComplite; }


	int read(size_t _bits)
	{
		if (mByteSize * octet_size - mBitsCursor < _bits)
		{
			if (!fill(_bits))
			{
				mComplite	= true;
				return	0;
			}
		}

		int		retval	= 0;
		int		bit		= 1;
		for (;_bits > 0; ++mBitsCursor, --_bits)
		{
			if (mBuffer[mBitsCursor/octet_size] & (1<<(mBitsCursor%octet_size)))
			{
				retval	|= bit;
			}
			bit <<= 1;
		}
		return	retval;
	}

protected:
	bool fill(size_t _bits)
	{
		if (mComplite)
		{
			return	false;
		}

		uint8_t	blockSize;
		io::read<endian::little>(blockSize, mStream);
		if (!blockSize)
		{
			// データの終端に到着した
			return	false;
		}


		// 空き容量が不足しているならバッファを拡張する
		if (auto freeSize = sizeof(mBuffer) - mByteSize; freeSize < blockSize)
		{
			// データを前に詰める
			if (auto top = mBitsCursor/octet_size; top > 0)
			{
				size_t	cplen	= mByteSize - top;
				for (size_t i = 0; i < cplen; ++i)
				{
					mBuffer[i] = mBuffer[top + i];
				}
				mBitsCursor -= top * octet_size;
				mByteSize	-= top;
				freeSize	=  sizeof(mBuffer) - mByteSize;
			}
		}

		io::read(mBuffer + mByteSize, mStream, blockSize);
		mByteSize	+= mStream.gcount();

		return	true;
	}

	Stream&				mStream;
	intptr_t			mBitsCursor	= 0;		//!< 次の読み取り位置
	size_t				mByteSize	= 0;		//!< 有効なデータのサイズ
	uint8_t				mBuffer[255 + 3];		//!< gifのデータブロックが最大 255bytes + 最大で 11bits(1 + 8 + 2)のバッファが必要 
	bool				mComplite	= false;
};



AMTRS_IO_NAMESPACE_END
#endif
