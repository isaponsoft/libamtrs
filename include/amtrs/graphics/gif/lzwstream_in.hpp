/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__graphics__gif__lzwstream_in__hpp
#define	__libamtrs__graphics__gif__lzwstream_in__hpp
#include "lzw.hpp"
#include "reader.hpp"
AMTRS_GRAPHICS_GIF_NAMESPACE_BEGIN



template<class In>
class	lzwstream_in
{
public:
	using	value_type		= uint8_t;
	using	char_type		= value_type;
	using	pointer			= value_type*;
	using	const_pointer	= typename std::add_const<value_type>::type*;
	using	iostate			= std::ios_base::iostate;
	using	off_type		= intptr_t;
	using	pos_type		= intptr_t;
	using	size_type		= size_t;
	static constexpr iostate	goodbit		= std::ios_base::goodbit;
	static constexpr iostate	badbit		= std::ios_base::badbit;
	static constexpr iostate	failbit		= std::ios_base::failbit;
	static constexpr iostate	eofbit		= std::ios_base::eofbit;
	static constexpr size_type	max_bits	= 12;
	static constexpr size_type	max_index	= 1<<max_bits;
	static constexpr uint16_t	noentry		= static_cast<uint16_t>(-1);

	lzwstream_in(const lzwstream_in&) = delete;
	lzwstream_in& operator = (const lzwstream_in&) = delete;

	template<class In_>
	lzwstream_in(In_&& _in, int _lzwMinCodeSize)
		: mInput(_in)
		, mLzwMinCodeSize(_lzwMinCodeSize)
	{
		clear_dictionary();
		mWriteBuffer	= new uint8_t[max_index];
	}

	~lzwstream_in()
	{
		if (mWriteBuffer)
		{
			delete[] mWriteBuffer;
		}
	}

	size_type gcount() const noexcept { return mGCount; }

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == goodbit; }
	bool eof()  const noexcept { return rdstate() & eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = goodbit) noexcept { mStatus = _state; }


	uint8_t*	mWriteBuffer	= nullptr;
	size_t		mWritePos		= 0;
	size_t		mWriteSize		= 0;
	int			mOutputCode		= -1;

	lzwstream_in& read(uint8_t* _buff, size_type _size)
	{
		mGCount	= 0;


		uint8_t*	out		= _buff;
		while (_size > 0)
		{
			// 出力バッファにデータが残っているなら先にバッファの中身を出力する。
			if ((mWriteSize - mWritePos) > 0)
			{
				auto	copy	= std::min(mWriteSize - mWritePos, _size);
				std::copy_n(mWriteBuffer + mWritePos, copy, out);
				_size		-= copy;
				out			+= copy;
				mWritePos	+= copy;
				mGCount		+= copy;
				if (mWritePos == mWriteSize)
				{
					mWritePos	= 0;
					mWriteSize	= 0;
				}
				else
				{
					break;
				}
			}


			// 出力を控えているエントリーがあれば出力する
			int	outEntry	= noentry;
			if (mOutputCode != -1)
			{
				outEntry		= mOutputCode;

				// entry は後ろから順に辿るので、書き込みも後ろからになる。
				size_type	len		= mEntryList[outEntry].length;
				uint16_t	entry	= outEntry;

				// 直接出力できるデータのサイズ(前半)
				size_type	copy1	= std::min(len, _size);
				auto*		out1	= out;
				mGCount		+= copy1;
				out			+= copy1;
				_size		-= copy1;

				// 書き込みバッファに保存しておくサイズ(後半)
				size_type	copy2	= len - copy1;
				auto*		out2	= mWriteBuffer;
				mWriteSize	=  copy2;


				// 書き込みバッファに保存しておくものから出力
				while (copy2 > 0)
				{
					out2[--copy2]	= mEntryList[entry].value;
					entry			= mEntryList[entry].prev;
				}

				// 直接出力分を出力
				while (copy1 > 0)
				{
					out1[--copy1]	= mEntryList[entry].value;
					entry			= mEntryList[entry].prev;
				}

				mOutputCode		= -1;
			}

			// 次のエントリーコードを読み取る
			int	code	= mInput.read(code_bits());
			if (code == end_code() || mInput.eof())
			{
				// データが残っているかもしれないので空読みしておく
				while (!mInput.eof())
				{
					mInput.read(code_bits());
				}
				setstate(eofbit);
				break;
			}
			if (code == clear_code())
			{
				clear_dictionary();
				mOutputCode		= -1;
				continue;
			}
			mOutputCode	= code;

			// エントリーに空きがあるならエントリーを追加する。
			if (mEntryList.free() && outEntry != noentry)
			{
				mEntryList.add(outEntry, code);
				if (mEntryList.size() >= (1<<(mCodeBits)))
				{
					++mCodeBits;
					if (mCodeBits > max_bits)
					{
						mCodeBits	= max_bits;
					}
				}
			}
		}
		return	*this;
	}

protected:
	constexpr int code_bits() const noexcept { return mCodeBits; }
	constexpr int clear_code() const noexcept { return 1 << (mLzwMinCodeSize); }
	constexpr int end_code() const noexcept { return clear_code() + 1; }

	void clear_dictionary()
	{
		mCodeBits		= mLzwMinCodeSize+1;
		mEntryList.reset(1<<mLzwMinCodeSize);
	}


	iostate								mStatus	= goodbit;
	std::size_t							mGCount	= 0;

	int									mLzwMinCodeSize;
	int									mCodeBits;
	image_data_reader<In>				mInput;

	lzwentrylist						mEntryList;
};





AMTRS_IO_NAMESPACE_END
#endif
