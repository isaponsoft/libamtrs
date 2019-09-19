#ifndef	__libamtrs__node__scene_node_stack__hpp
#define	__libamtrs__node__scene_node_stack__hpp
#include <stack>
AMTRS_NAMESPACE_BEGIN


template<class BaseNodeT>
class	scene_node_stack
{
	using	_ref		= ref<BaseNodeT>;
	using	_value_type	= std::pair<scene_node_if<BaseNodeT>*, _ref>;
public:
	using	node_type		= BaseNodeT;
	using	scene_type		= scene_node_if<BaseNodeT>;
	using	traits_type		= typename BaseNodeT::traits_type;
	using	draw_argument	= typename traits_type::draw_argument;
	using	render_argument	= typename traits_type::render_argument;


	node_type* current() const noexcept
	{
		return	empty() ? nullptr : c.top().second;
	}


	template<class T>
	void jump(scene_node<T>* _scene)
	{
		pop();
		push(_scene);
	}


	void pop()
	{
		if (!empty())
		{
			c.top().first->mPop			= true;
		}
	}

	template<class T>
	void push(scene_node<T>* _scene)
	{
		if (!empty())
		{
			_scene->set_size(current()->size());
		}
		mPushQueue.push_back(_value_type(_scene, _scene));
	}

	bool empty() const noexcept { return c.empty(); }

	std::size_t size() const noexcept { return c.size(); }

	std::size_t queue_size() const noexcept { return mPushQueue.size(); }

	void update(float _delta)
	{
		bool			hasSize	= false;
		size3<float>	cSize	= {0, 0, 0};

		// トップシーンにポップ要求があればポップ
		if (!empty() && c.top().first->mPop)
		{
			hasSize	= true;
			cSize	= c.top().second->size();

			c.top().first->on_deactive();
			c.top().first->on_invisible();
			c.top().first->exit();
			c.top().first->mSceneStack	= nullptr;
			c.pop();
		}

		if (!mPushQueue.empty())
		{
			// 実行中のシーンを非アクティブ化
			if (!empty())
			{
				if (hasSize)
				{
					c.top().second->set_size(cSize);
				}
				c.top().first->on_deactive();
				c.top().first->on_invisible();
			}

			// シーンをスタックに積む
			for (auto& q : mPushQueue)
			{
				c.push(q);
				c.top().first->mSceneStack	= this;
				c.top().first->enter();
			}
			mPushQueue.clear();
		}


		if (!empty())
		{
			_ref	keep(c.top().second);
			auto	s = c.top().first;
			if (!s->mVisibled)
			{
				c.top().second->schedule_once([s](float)
				{
					s->on_visible();
				});
			}
			if (!s->mActivated)
			{
				c.top().second->schedule_once([s](float)
				{
					s->on_active();
				});
			}
			c.top().first->update(_delta);
		}
	}


	void render(render_argument _rarg)
	{
		if (!empty())
		{
			_ref	keep(c.top().second);
			c.top().first->render(_rarg);
		}
	}


	void draw_exec(draw_argument _darg, bool _remove_queue = true)
	{
		if (!empty())
		{
			_ref	keep(c.top().second);
			c.top().first->draw_exec(_darg, _remove_queue);
		}
	}

	void draw_clear()
	{
		if (!empty())
		{
			_ref	keep(c.top().second);
			c.top().first->draw_clear();
		}
	}

	node_type* input(input_event* _event)
	{
		if (!empty())
		{
			_ref	keep(c.top().second);
			return	c.top().first->input(_event);
		}
		return	nullptr;
	}



protected:
	std::stack<_value_type>		c;
	std::vector<_value_type>	mPushQueue;
};





AMTRS_NAMESPACE_END
#endif
