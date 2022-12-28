/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__util__async_queue__hpp
#define	__libamtrs__util__async_queue__hpp
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

// 非同期キュー
//	foreach は一つのスレッドからしか呼ぶことはできませんが、
//	append を複数のスレッドから呼び出すことができます。
template<class T>
class	async_queue
{
	struct	item
	{
		template<class... Args>
		item(Args&&... _args)
			: e(std::forward<Args>(_args)...)
		{}
		item*	next	= nullptr;
		bool	del		= false;
		T		e;
	};
public:
	struct	location;
	using	value_type	= T;
	using	iterator	= location*;

	~async_queue()
	{
		foreach([](auto&) { return false; });
	}

	// multi thread safe.
	template<class... Args>
	iterator add(Args&&... _args)
	{
		auto	i	= new item(std::forward<Args>(_args)...);
		append(i);
		return	reinterpret_cast<iterator>(i);
	}

	// remove
	void remove(iterator _it)
	{
		reinterpret_cast<item*>(_it)->del	= true;
	}


	// one thread only.
	template<class Callback>
	value_type* find_if(Callback callback)
	{
		marge();
		auto*	cur	= &mQueue;
		while (*cur)
		{
			if (!(*cur)->del && callback((*cur)->e))
			{
				return	&(*cur)->e;
			}
			else
			{
				auto	del	= *cur;
				*cur	= (*cur)->next;
				delete	del;
			}
		}
		return	nullptr;
	}


	// one thread only.
	template<class Callback>
	void foreach(Callback callback)
	{
		marge();
		auto*	cur	= &mQueue;
		while (*cur)
		{
			if (!(*cur)->del && callback((*cur)->e))
			{
				cur	= &(*cur)->next;
			}
			else
			{
				auto	del	= *cur;
				*cur	= (*cur)->next;
				delete	del;
			}
		}
	}


	bool empty() const
	{
		return	!mQueue && !mReserve.load();
	}


protected:

	void append(item* i)
	{
		do
		{
			i->next	= mReserve.load();
		} while(!std::atomic_compare_exchange_strong(&mReserve, &i->next, i));
	}

	void marge()
	{
		item*	top;
		do
		{
			top	= mReserve.load();
			if (!top)
			{
				return;
			}

		} while (!std::atomic_compare_exchange_strong(&mReserve, &top, nullptr));

		auto**	tail	= &mQueue;
		if (*tail)
		{
			tail	= &(*tail)->next;
			while (*tail)
			{
				tail	= &(*tail)->next;
			}
		}

		while (top)
		{
			auto*	cur	= top;
			top			= cur->next;
			cur->next	= *tail;
			*tail	= cur;
		}
	}

	item*				mQueue		= nullptr;
	std::atomic<item*>	mReserve	= nullptr;
};
AMTRS_NAMESPACE_END


#if	0
// examples.

	async_queue<int>	q;
	bool				th1End	= false;
	bool				th2End	= false;

	std::thread th1([&]()
	{
		for (int i = 0; i < 100000; ++i)
		{
			q.add(i);
		}
		th1End	= true;
	});
	std::thread th2([&]()
	{
		for (int i = 0; i < 100000; ++i)
		{
			q.add(i);
		}
		th2End	= true;
	});
	while (!th1End || !th2End)
	{
		q.foreach([&](auto& i) -> bool
		{
			std::cout << i << std::endl;
			return	false;
		});
	}
// example end
#endif
#endif
