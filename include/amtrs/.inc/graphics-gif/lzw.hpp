/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__gif__lzw__hpp
#define	__libamtrs__graphics__gif__lzw__hpp
#include "header.hpp"
AMTRS_GRAPHICS_GIF_NAMESPACE_BEGIN


struct	lzwentry
{
	uint16_t	prev;	//!< 前のエントリー
	uint16_t	length;	//!< エントリーを出力するために必要な長さ
	uint8_t		first;	//!< 最初の値(prevを辿った先)
	uint8_t		value;	//!< 新しく追加された値
};


class	lzwentrylist
{
public:
	using	size_type		= size_t;
	static constexpr size_type	max_bits	= 12;
	static constexpr size_type	max_index	= 1<<max_bits;
	static constexpr uint16_t	noentry		= static_cast<uint16_t>(-1);


	void reset(size_t _size)
	{
		mUsingDicSize	= _size + 2;
		for (auto& dic : mDictionaries)
		{
			dic	= { noentry, 0, 0, 0 };
		}
		for (int i = 0; i < mUsingDicSize; ++i)
		{
			mDictionaries[i]	= { noentry, 1, (uint8_t)i, (uint8_t)i };
		}
	}


	bool add(uint16_t _prev, uint16_t _add)
	{
		mDictionaries[mUsingDicSize]	=
		{
			_prev,
			static_cast<uint16_t>(mDictionaries[_prev                        ].length + 1),
			                      mDictionaries[_prev                        ].first,
			                      mDictionaries[_add >= size() ? _prev : _add].first
		};
		++mUsingDicSize;
		return	true;
	}


	bool free() const noexcept
	{
		return	mUsingDicSize < max_index;
	}

	size_type size() const noexcept
	{
		return	mUsingDicSize;
	}

	lzwentry const& operator [] (uint16_t _index)
	{
		return	mDictionaries[_index];
	}

	size_t write(uint8_t*& _buff, uint16_t _entry)
	{
		size_t	w = 1;
		if (mDictionaries[_entry].prev != noentry)
		{
			w	+= write(_buff, mDictionaries[_entry].prev);
		}
		*_buff++ = mDictionaries[_entry].value;
		return	w;
	}


protected:
	lzwentry		mDictionaries[max_index];
	size_type		mUsingDicSize	= 0;
};



AMTRS_IO_NAMESPACE_END
#endif
