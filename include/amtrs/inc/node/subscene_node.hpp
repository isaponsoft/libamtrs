#ifndef	__libamtrs__node__subscene_node__hpp
#define	__libamtrs__node__subscene_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! 入れ子に出来るシーンです。
// ----------------------------------------------------------------------------
template<class NodeT, class Scene = scene_node<NodeT>>
class	subscene_node
		: public Scene
{
	using	_base_type	= Scene;
public:
	using	node_type	= typename _base_type::node_type;
	using	stack_type	= scene_node_stack<node_type>;

	// ************************************************************************
	//! 入れ子にしたシーンを取得します。
	// ------------------------------------------------------------------------
	      stack_type* stack() noexcept { return &mSceneStack; }
	const stack_type* stack() const noexcept { return &mSceneStack; }

protected:
	virtual void on_size_change(typename _base_type::size_type& _size) override
	{
		_base_type::on_size_change(_size);
		update_size();
	}

	virtual void update(float _delta) override
	{
		_base_type::update(_delta);
		mSceneStack.update(_delta);
	}

	virtual void render(typename _base_type::render_argument _rarg) override
	{
		_base_type::render(_rarg);
		mSceneStack.render(_rarg);
	}

	virtual node_type* input(input_event* _event) override
	{
		if (auto r = _base_type::input(_event))
		{
			return	r;
		}
		return	mSceneStack.input(_event);
	}


	virtual void draw_exec(typename _base_type::draw_argument _darg, bool _remove_queue) override
	{
		_base_type::draw_exec(_darg, _remove_queue);
		mSceneStack.draw_exec(_darg, _remove_queue);
	}


	void update_size()
	{
		if (!mSceneStack.empty())
		{
			mSceneStack.current()->set_size(this->size());
		}
	}

private:
	stack_type		mSceneStack;
};


AMTRS_NAMESPACE_END
#endif
