/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs__blocklist__hpp
#define	__inarikvs__blocklist__hpp
#include "inarikvs-def.hpp"
#include "inarikvs-types.hpp"
INARIKVS_NAMESPACE_BEGIN



class	blocklist
{
public:
	// 逆のブロックを作成します。
	blocklist reverse(int64_t _totalSize)
	{
		blocklist	retval;
		int64_t		pos	= 0;
		for (auto* b = mFirst.get() ; b ; b = b->next)
		{
			if (pos != b->pos)
			{
				retval.add(pos, static_cast<uint32_t>(b->pos - pos));
			}
			pos	= b->tail();
		}		
		if (pos < _totalSize)
		{
			retval.add(pos, static_cast<uint32_t>(_totalSize - pos));
		}
		return	retval;
	}


	// 指定サイズのエリアをひとつ生成し、位置を返す
	int64_t get(uint32_t _size)
	{
		block_data*	prev	= nullptr;
		for (amtrs::ref<block_data>* b = &mFirst ; *b ; b = &(*b)->next)
		{
			// ブロックが同一サイズなのでそのまま使用する
			if ((*b)->size == _size)
			{
				int64_t	retval	= (*b)->pos;
				auto	next	= (*b)->next;
				*b	= next;
				if (next == nullptr)
				{
					mLast	= prev;
				}
				return	retval;
			}
			if ((*b)->size > _size)
			{
				int64_t	retval	= (*b)->pos;
				(*b)->pos	+= _size;
				(*b)->size	-= _size;
				return	retval;
			}
			prev	= *b;
		}

		// ブロックがなかった
		return	-1;
	}
	

	// 領域をひとつ追加する
	void add(int64_t _pos, uint32_t _size)
	{
		if (mFirst.empty())
		{
			mFirst		= new block_data(_pos, _size);
			mLast		= mFirst;
			return;
		}

		// 連結可能なら連結して終了
		if (mFirst->contacat(_pos, _size))
		{
			return;
		}

		// 最初のブロックよりも前に存在するブロックなら前に追加する
		if (mFirst->pos > _pos)
		{
			amtrs::ref<block_data>	b	= new block_data(_pos, _size);
			b->next	= mFirst;
			mFirst	= b;
			return;
		}

		// 最終ブロックより後ろにあるかチェックする
		if (_pos > mLast->pos)
		{
			if (!mLast->contacat(_pos, _size))
			{
				mLast->next	= new block_data(_pos, _size);
				mLast		= mLast->next;
			}
			return;
		}

		block_data*	scan	= mFirst;
		while (!scan->next.empty())
		{
			auto	next = scan->next.get();
			if (next->contacat(_pos, _size))
			{
				return;
			}
			if (next->pos > _pos)
			{
				amtrs::ref<block_data>	b	= new block_data(_pos, _size);
				b->next		= next;
				scan->next	= b;
				return;
			}
			scan	= scan->next;
		}
		scan->next	= new block_data(_pos, _size);
		mLast		= scan->next;
	}

	template<class CallbackT>
	void each(uint64_t _totalSize, CallbackT&& _callback)
	{
		int64_t	pos	= 0;
		if (!mFirst.empty())
		{
			for (auto* b = mFirst.get() ; b ; b = b->next)
			{
				if (pos < b->pos)
				{
					_callback(false, pos, b->pos - pos);
				}
				_callback(true, b->pos, b->size);
				pos	= b->pos + b->size;
			}
		}
		if (_totalSize != pos)
		{
			_callback(false, pos, _totalSize - pos);
		}
	}

	template<class CallbackT>
	void each_block(CallbackT&& _callback)
	{
		if (!mFirst.empty())
		{
			for (auto* b = mFirst.get() ; b ; b = b->next)
			{
				_callback(b->pos, b->size);
			}
		}
	}

private:
	struct	block_data
	{
		constexpr block_data(int64_t _pos, uint32_t _size)
			: pos(_pos)
			, size(_size)
		{}

		amtrs::ref<block_data>	next	= nullptr;
		int64_t					pos		= 0;
		uint32_t				size	= 0;

		constexpr bool contacat(int64_t _pos, uint32_t _size)
		{
			if (tail() == _pos)
			{
				size += _size;
				return	true;
			}
			if (pos == (_pos + _size))
			{
				pos		=  _pos;
				size	+= _size;
				return	true;
			}
			return	false;
		}

		constexpr int64_t tail() const noexcept { return pos + size; }

		constexpr void add_ref() const
		{
			++mUsingCount;
		}


		constexpr void dec_ref() const
		{
			if ((--mUsingCount) == 0)
			{
				delete	this;
			}
		}

	private:
		mutable uint8_t		mUsingCount	= 0;
	};


	amtrs::ref<block_data>	mFirst;
	block_data*				mLast	= nullptr;
};


INARIKVS_NAMESPACE_END
#endif
