#ifndef	__libamtrs__node__scene_node__hpp
#define	__libamtrs__node__scene_node__hpp
#include <stack>
AMTRS_NAMESPACE_BEGIN

template<class BaseNodeT>
class	scene_node_stack;


template<class BaseNodeT>
class	scene_node_if
{
public:
	using	node_type			= BaseNodeT;
	using	traits_type			= typename BaseNodeT::traits_type;
	using	layer_type			= typename BaseNodeT::layer;
	using	draw_argument		= typename traits_type::draw_argument;
	using	render_argument		= typename traits_type::render_argument;
	using	render_queue		= typename BaseNodeT::render_queue;
	using	scene_stack_type	= scene_node_stack<node_type>;


	layer_type* add_layer(layer_type* _layer)
	{
		mSortLayer	= true;
		mLayerList.push_back(_layer);
		return	_layer;
	}

	layer_type* find_layer(const std::string& _name) const noexcept
	{
		auto	it	= std::find_if(mLayerList.begin(), mLayerList.end(), [&](auto a) -> bool
					{
						return	a->name() == _name;
					});
		return it == mLayerList.end() ? nullptr : *it;
	}

	virtual void render(render_argument _rarg) {}
	virtual node_type* input(input_event* _event) { return nullptr; }
	virtual void update(float _delta) {}


	virtual void draw_exec(draw_argument _darg, bool _remove_queue = true)
	{
		for (auto& l : bucket())
		{
			l->on_transfer(_darg);
		}
		for (auto& l : bucket())
		{
			l->on_draw(_darg);
		}
		if (_remove_queue)
		{
			draw_clear();
		}
	}

	void draw_clear()
	{
		for (auto& l : bucket())
		{
			l->clear();
		}
	}


protected:
	virtual void on_scene_enter() = 0;
	virtual void on_scene_exit() = 0;
	virtual void on_visible() { mVisibled = true; }
	virtual void on_active() { mActivated = true; }
	virtual void on_deactive() { mActivated = false; }
	virtual void on_invisible() { mVisibled = false; }


	scene_stack_type* get_scene_stack() const noexcept { return mSceneStack; }

	auto bucket()
	{
		if (mSortLayer)
		{
			mSortLayer	= false;
			std::sort(mLayerList.begin(), mLayerList.end(), [](auto& a, auto& b) -> bool
			{
				return	a->get_layer_order() <= b->get_layer_order();
			});
		}
		return	make_bucket(mLayerList.begin(), mLayerList.end());
	}


private:
	void enter()
	{
		this->on_scene_enter();
	}

	void exit()
	{
		this->on_scene_exit();
	}


	std::vector<layer_type*>	mLayerList;
	scene_stack_type*			mSceneStack	= nullptr;
	bool						mSortLayer	= false;
	bool						mEnter		= false;
	bool						mPop		= false;
	bool						mVisibled	= false;
	bool						mActivated	= false;

	template<class>
	friend	class	scene_node_stack;
};


template<class NodeT>
class	scene_node
		: public NodeT
		, public scene_node_if<typename NodeT::node_type>
		, protected NodeT::layer
		, protected component::input_dispatcher<typename NodeT::node_type>
{
	using	_base_type				= NodeT;
	using	_if_type				= scene_node_if<typename NodeT::node_type>;
	using	_input_dispacher		= component::input_dispatcher<typename NodeT::node_type>;
public:
	using	node_type				= typename _base_type::node_type;
	using	traits_type				= typename _base_type::traits_type;
	using	layer_type				= typename _base_type::layer;
	using	draw_argument			= typename traits_type::draw_argument;
	using	render_argument			= typename traits_type::render_argument;
	using	render_queue			= typename _base_type::render_queue;
	using	schede_dispatcher_type	= typename _base_type::schede_dispatcher_type;

	using	_base_type::_base_type;

	scene_node()
	{
		_if_type::add_layer(this);
		this->set_layer(this);
		this->set_root_scheduler(mRootScheduler = new schede_dispatcher_type());
	}


	// 描画
	virtual void render(render_argument _rarg) override
	{
		render_queue	rs;
		size2<float>	canvas	= (size2<float>)this->size();
		rs.device		= _rarg;
		rs.projection	= mat44<float>::ortho(
							{float((int)canvas.width/2), float((int)canvas.height/2)},		// 左上を原点にする
							{canvas.width, -canvas.height},									// 仮想スクリーンの大きさ
							0, 1.0f
						);
		rs.current		= _base_type::get_layer();
		this->on_render(rs);
	}

	virtual void update(float _delta) override
	{
		this->get_root_scheduler()->execute(_delta);
	}

	virtual node_type* input(input_event* _event) override
	{
		return	_input_dispacher::input(this, _event);
	}

	virtual void on_scene_enter() override
	{
		this->on_enter();
	}

	virtual void on_scene_exit() override
	{
		this->on_exit();
	}

private:
	ref<schede_dispatcher_type>	mRootScheduler;
};



AMTRS_NAMESPACE_END
#endif
