/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__component__thread_component__hpp
#define	__libamtrs__component__thread_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {

// ==================================================================
//! バックグラウンド処理を行うためのスレッドスケジューラです
// ------------------------------------------------------------------
template<class NodeT>
struct	thread_component
{
	using	this_type		= thread_component<NodeT>;
public:
	using	node_type			= NodeT;
	using	sequence_func		= std::function<void()>;
	using	identify_type		= std::string;
	using	async_order_type	= int;

	thread_component();

	~thread_component();

	void async_schedule(identify_type _identify, sequence_func _proc, async_order_type _order = 0)
	{
		mScheduler->msgq.post(queue
		{
			static_cast<node_type*>(this),
			std::move(_identify),

			std::move(_order),
			0,

			std::move(_proc)
		});
	}

private:
	struct	queue
	{
		node_type*			owner;
		identify_type		identify;

		async_order_type	order	= 0;
		int					state	= 0;

		sequence_func		proc;
	};

	struct	changer
	{
		node_type*			owner;
		identify_type		identify;

		async_order_type	order	= 0;
		int					state	= 0;
	};


	struct	thread_scheduler
			: public ref_object
	{
		thread_scheduler()
		{
			thread	= std::thread([this]()
			{
				std::list<queue>	ql;
				while (!exit)
				{
					queue	qt;
					while (msgq.get(qt))
					{
						ql.push_back(std::move(qt));
					}

					{
						changer	ct;
						while (chgq.get(ct))
						{
							for (auto& q : ql)
							{
								if (q.owner == ct.owner || q.identify == ct.identify)
								{
									q.order	= ct.order;
									q.state	= ct.state;
								}
							}
						}
					}
					
					ql.remove_if([](auto& q) { return q.state == -1; });
					if (ql.empty())
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
						continue;
					}

					ql.sort([](auto& a, auto& b) { return a.order < b.order; });

					qt	= ql.front();
					ql.pop_front();

					qt.proc();
				}
			});
		}

		~thread_scheduler()
		{
			exit			= true;
			get_tcheduler()	= nullptr;
			thread.join();
		}

		std::thread			thread;
		message<queue>		msgq;
		message<changer>	chgq;
		bool				exit	= false;
	};


	static thread_scheduler*& get_tcheduler()
	{
		static thread_scheduler*	s;
		if (!s)
		{
			s	= new thread_scheduler();
		}
		return	s;
	}

	ref<thread_scheduler>	mScheduler;
};


template<class NodeT>
thread_component<NodeT>::thread_component()
	: mScheduler(get_tcheduler())
{}

template<class NodeT>
thread_component<NodeT>::~thread_component()
{
	mScheduler->chgq.post({
		static_cast<node_type*>(this),
		std::string(),
		0,
		-1
	});
}


} AMTRS_NAMESPACE_END
#endif
