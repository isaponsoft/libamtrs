/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__component__scheduler_component__hpp
#define	__libamtrs__node__component__scheduler_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {
AMTRS_IMPLEMENTS_BEGIN(schedule_component)
template<class...> struct	schedule_data;			// Prototype. Not use.
AMTRS_IMPLEMENTS_END(schedule_component)

template<class...> class	scheduler_component;	// Prototype. Not use.
template<class...> class	schede_dispatcher;		// Prototype. Not use.


// ****************************************************************************
//! スケジューラを管理します。
// ----------------------------------------------------------------------------
template<class NodeT, class R, class... Args>
class	scheduler_component<NodeT, R(Args...)>
{
	using	_this_type	= scheduler_component<NodeT, R(Args...)>;
public:
	using	schedule_type			= AMTRS_IMPLEMENTS(schedule_component)::schedule_data<NodeT, R(Args...)>;
	using	schede_dispatcher_type	= schede_dispatcher<NodeT, R(Args...)>;

	static constexpr std::size_t	nolimit	= static_cast<std::size_t>(-1);

	schede_dispatcher_type* get_scheduler() const noexcept { return mRootScheduler; }

	static void update(NodeT* _root, Args... _args)
	{
		if (!_root)
		{
			return;
		}
		if (!_root->get_scheduler())
		{
			_root->set_scheduler(new schede_dispatcher_type());
		}
		_root->get_scheduler()->execute(std::forward<Args>(_args)...);
	}

	scheduler_component()
	{
	}


	~scheduler_component()
	{
		unschedule_all();
	}

	// ========================================================================
	//! スケジュールを登録します。
	// ------------------------------------------------------------------------
	template<class Callback>
	void schedule(Callback&& _callback)
	{
		schedule(this, nolimit, std::forward<Callback>(_callback));
	}


	// ========================================================================
	//! １度だけ実行されるスケジュールを登録します。
	// ------------------------------------------------------------------------
	template<class Callback>
	void schedule_once(Callback&& _callback)
	{
		schedule(this, 1, std::forward<Callback>(_callback));
	}


	// ========================================================================
	//! 削除用のキーを指定してスケジュールを登録します。
	// ------------------------------------------------------------------------
	template<class KeyT, class Callback>
	void schedule(const KeyT& _unscheduleKey, Callback&& _callback)
	{
		schedule(_unscheduleKey, nolimit, std::forward<Callback>(_callback));
	}


	// ========================================================================
	//! 削除用のキーとリピート回数を指定してスケジュールを登録します。
	// ------------------------------------------------------------------------
	template<class KeyT, class Callback>
	void schedule(const KeyT& _unscheduleKey, std::size_t _repeatCount, Callback&& _callback)
	{
		using		type	= AMTRS_IMPLEMENTS(schedule_component)::schedule_data<NodeT, KeyT, Callback, R(Args...)>;
		ref<type>	s		= new type(this, _unscheduleKey, _repeatCount, std::forward<Callback>(_callback));
		mSchedules.push_back(s);
		if (mRootScheduler)
		{
			mRootScheduler->schedule(s);
		}
	}


	// ========================================================================
	//! 削除用のキーを指定し、キーが一致するスケジュールを削除します。
	// ------------------------------------------------------------------------
	template<class KeyT>
	std::size_t unschedule(const KeyT& _key)
	{
		std::size_t	usCount		= 0;
		for (auto it = mSchedules.begin(); it != mSchedules.end(); )
		{
			if ((*it)->is_enable() && (*it)->template is_equals<KeyT>(_key))
			{
				(*it)->disable();
				if (mRootScheduler)
				{
					mRootScheduler->unschedule((*it));
				}
				it	= mSchedules.erase(it);
				++usCount;
			}
			else
			{
				++it;
			}
		}
		return	usCount;
	}


	// ========================================================================
	//! 削除用のキーの型を指定し、キーの型が一致するスケジュールを全て削除します。
	// ------------------------------------------------------------------------
	template<class KeyT>
	std::size_t unschedule_for_type()
	{
		std::size_t	usCount		= 0;
		for (auto it = mSchedules.begin(); it != mSchedules.end(); )
		{
			if ((*it)->is_enable() && (*it)->template is_type<KeyT>())
			{
				(*it)->disable();
				if (mRootScheduler)
				{
					mRootScheduler->unschedule((*it));
				}
				it	= mSchedules.erase(it);
				++usCount;
			}
			else
			{
				++it;
			}
		}
		return	usCount;
	}


	// ========================================================================
	//! スケジュールを全て削除します。
	// ------------------------------------------------------------------------
	void unschedule_all()
	{
		for (auto& e : mSchedules)
		{
			e->disable();
			if (mRootScheduler)
			{
				mRootScheduler->unschedule(e);
			}
		}
		mSchedules.clear();
	}


	// ========================================================================
	//! スケジュールを全て削除します。
	// ------------------------------------------------------------------------
	void unschedule(schedule_type* _schedule)
	{
		if (mRootScheduler)
		{
			mRootScheduler->unschedule(_schedule);
		}
		mSchedules.remove_if([_schedule](schedule_type* _s) { return _s == _schedule; });
	}



	void set_scheduler(schede_dispatcher_type* _scheduler)
	{
		if (_scheduler)
		{
			if (mRootScheduler != _scheduler)
			{
				mRootScheduler	= _scheduler;
				on_enter();
			}
		}
		else
		{
			if (mRootScheduler)
			{
				on_exit();
				mRootScheduler	= nullptr;
			}
		}
	}


protected:
	virtual void on_enter()
	{
		for (schedule_type* s : mSchedules)
		{
			if (s && s->is_enable())
			{
				mRootScheduler->schedule(s);
			}
		}
		static_cast<NodeT*>(this)->visit_children([this](auto c)
		{
			c->set_scheduler(mRootScheduler);
			return	false;
		}, false);
	}

	virtual void on_exit()
	{
		static_cast<NodeT*>(this)->visit_children([](auto c)
		{
			c->set_scheduler(nullptr);
			return	false;
		}, false);
		for (schedule_type* s : mSchedules)
		{
			mRootScheduler->unschedule(s);
		}
	}

private:
	schede_dispatcher_type*			mRootScheduler		= nullptr;
	std::list<ref<schedule_type>>	mSchedules;
};




template<class NodeT, class R, class... Args>
class	schede_dispatcher<NodeT, R(Args...)>
		: public ref_object
{
	using	schedule_type	= AMTRS_IMPLEMENTS(schedule_component)::schedule_data<NodeT, R(Args...)>;

public:	
	void execute(Args... _args)
	{
		auto	it = mSchedulerList.begin();
		for (;;)
		{
			while (it != mSchedulerList.end())
			{
				if (!(*it).empty() && (*it)->is_enable())
				{
					// 有効なスケジュールを発見したので実行する
					(*it)->execute(std::forward<Args>(_args)...);
					if (!(*it).empty() && (*it)->is_enable())
					{
						// リピートカウントが設定されているならリピートカウントを低下
						if ((*it)->mRepeat > 0)
						{
							--((*it)->mRepeat);
						}
						if ((*it)->mRepeat != 0)
						{
							++it;
							continue;
						}
					}
				}
				// スケジュールが無効になっているのでリストから削除
				if (!(*it).empty())
				{
					(*it)->mOwner->unschedule((*it).get());
				}
				it	= mSchedulerList.erase(it);
			}


			// 新しいスケジュールた追加されていればスケジュールの最後尾に追加
			std::lock_guard<safe_spinlock>	g(get_sync_object());
			if (mNewSchedules.empty())
			{
				break;
			}

			auto	position	= std::distance(mSchedulerList.begin(), it);
			mSchedulerList.reserve(mSchedulerList.size() + mNewSchedules.size());
			for (auto& ns : mNewSchedules)
			{
				mSchedulerList.push_back(std::move(ns));
			}
			mNewSchedules.clear();
			it	= mSchedulerList.begin() + position;
		}
	}


private:
	void schedule(schedule_type* _scheduler)
	{
		std::lock_guard<safe_spinlock>	g(get_sync_object());
		mNewSchedules.push_back({_scheduler});
	}

	void unschedule(schedule_type* _scheduler)
	{
		std::lock_guard<safe_spinlock>	g(get_sync_object());
		for (auto& e : mSchedulerList)
		{
			if (e == _scheduler)
			{
				e = nullptr;
			}
		}
		mNewSchedules.remove(_scheduler);
	}

	static safe_spinlock& get_sync_object()
	{
		static safe_spinlock	s;
		return	s;
	}



	std::vector<ref<schedule_type>>	mSchedulerList;
	std::list<ref<schedule_type>>	mNewSchedules;


	template<class...>
	friend	class	scheduler_component;
};




AMTRS_IMPLEMENTS_BEGIN(schedule_component)

// ****************************************************************************
// スケジュール本体
// ****************************************************************************
template<class NodeT, class R, class... Args>
struct	schedule_data<NodeT, R(Args...)>
		: public ref_object
{
	using	type_index_type	= std::type_index;
	using	result_type		= R;
	using	schedule_type	= schedule_data<NodeT, R(Args...)>;
	using	scheduler_type	= scheduler_component<NodeT, R(Args...)>;

	static constexpr std::size_t	nolimit	= static_cast<std::size_t>(-1);



	schedule_data(scheduler_type* _owner, type_index_type _ti, std::size_t _limit = nolimit)
		: mOwner(_owner)
		, mTypeIndex(_ti)
		, mRepeat(_limit)
	{}


	template<class KeyT>
	bool is_type() const noexcept
	{
		return	(mTypeIndex == typeid(KeyT))
				? true
				: false;
	}

	template<class KeyT>
	bool is_equals(const KeyT& _key) const noexcept;



	bool is_enable() const noexcept { return mEnable && ((mRepeat == nolimit) || mRepeat > 0); }

	void disable() noexcept { mEnable = false; }

	virtual R execute(Args...) const noexcept = 0;

	scheduler_type*	mOwner			= nullptr;
	schedule_type*	mNextEntry		= nullptr;
	type_index_type	mTypeIndex;
	std::size_t		mRepeat			= nolimit;
	bool			mEnable			= true;

	template<class...>
	friend	class	schede_dispatcher;
};


template<class NodeT, class KeyT, class R, class... Args>
struct	schedule_data<NodeT, KeyT, R(Args...)>
		: public schedule_data<NodeT, R(Args...)>
{
	using	_base_type		= schedule_data<NodeT, R(Args...)>;
	using	scheduler_type	= typename _base_type::scheduler_type;

	schedule_data(scheduler_type* _owner, const KeyT& _key, std::size_t _repeat)
		: _base_type(_owner, typeid(KeyT), _repeat)
		, mKey(_key)
	{}

	KeyT		mKey;

	template<class...>
	friend	class	schede_dispatcher;
};




template<class NodeT, class KeyT, class Callback, class R, class... Args>
struct	schedule_data<NodeT, KeyT, Callback, R(Args...)>
		: public schedule_data<NodeT, KeyT, R(Args...)>
{
	using	_base_type		= schedule_data<NodeT, KeyT, R(Args...)>;
	using	scheduler_type	= typename _base_type::scheduler_type;

	schedule_data(scheduler_type* _owner, const KeyT& _key, std::size_t _repeat, Callback&& _callback)
		: _base_type(_owner, _key, _repeat)
		, mCallback(std::forward<Callback>(_callback))
	{}

	virtual R execute(Args... _args) const noexcept
	{
		if constexpr (can_invoke<Callback, Args...>::value)
		{
			return	mCallback(std::forward<Args>(_args)...);
		}
		else
		{
			mCallback();
			return	R();
		}
	}

	Callback	mCallback;

	template<class...>
	friend	class	schede_dispatcher;
};


template<class NodeT, class R, class... Args>
template<class KeyT>
bool schedule_data<NodeT, R(Args...)>::is_equals(const KeyT& _key) const noexcept
{
	using	key_type	= schedule_data<NodeT, KeyT, R(Args...)>;
	auto*	thiz		= static_cast<const key_type*>(this);
	return	((mTypeIndex == typeid(KeyT)) && (thiz->mKey == _key))
			? true
			: false;
}

AMTRS_IMPLEMENTS_END(schedule_component)



} AMTRS_NAMESPACE_END
#endif
