/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__message__hpp
#define	__libamtrs__message__hpp
#include <atomic>
#include <mutex>
#include <thread>
#include "../amtrs.hpp"
#include "../mutex/spinlock.hpp"
AMTRS_NAMESPACE_BEGIN

template<class T>
struct	message_traits;



template<class T, class Traits = message_traits<T>>
class	message
{
public:
	using	value_type	= typename Traits::value_type;
	using	mutex_type	= typename Traits::mutex_type;


	message() = default;
	message(message const&) = delete;
	message& operator = (message const&) = delete;

	message(message&& _r)
		: mSize(_r.mSize)
		, mHead(_r.nHead)
		, mTail(_r.mTail)
	{
		_r.mSize.store(0);
		_r.nHead	= nullptr;
		_r.mTail	= nullptr;
	}

	message& operator = (message&& _r)
	{
		mSize	= _r.mSize;
		mHead	= _r.nHead;
		mTail	= _r.mTail;
		_r.mSize.store(0);
		_r.nHead	= nullptr;
		_r.mTail	= nullptr;
		return	*this;
	}

	~message()
	{
		clear();
	}

	void clear()
	{
		while (mHead)
		{
			auto	q	= mHead;
			mHead	= mHead->next;
			delete	q;
		}
		mSize	= 0;
		mTail	= nullptr;
	}

	void post(value_type&& _val)
	{
		queue*	q	= new queue(std::move(_val));
		q->next		= nullptr;
		std::lock_guard<spinlock> lock(mLock);
		if (!mTail)
		{
			mHead		= q;
			mTail		= q;
		}
		else
		{
			mTail->next	= q;
			mTail		= q;
		}
		++mSize;
	}

	bool get(value_type& _buff)
	{
		queue*	q	= nullptr;
		{
			std::lock_guard<spinlock> lock(mLock);
			if (!mHead)
			{
				return	false;
			}
			q		= mHead;
			mHead	= q->next;
			if (q == mTail)
			{
				mTail	= nullptr;
			}
			--mSize;
		}
		_buff	= std::move(q->val);
		delete	q;
		return	true;
	}

	size_t size() const noexcept
	{
		return	mSize.load();
	}

	bool empty() const noexcept
	{
		return	size() == 0;
	}


protected:
	struct	queue
	{
		queue(value_type&& _val)
			: val(std::move(_val))
		{}
		value_type	val;
		queue*		next;
	};

	spinlock			mLock;
	std::atomic<size_t>	mSize;
	queue*				mHead	= nullptr;
	queue*				mTail	= nullptr;
};


template<class T>
struct	message_traits
{
	using	value_type	= T;
	using	mutex_type	= spinlock;
};


AMTRS_NAMESPACE_END
#endif
