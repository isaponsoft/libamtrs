/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__component__renderer__hpp
#define	__libamtrs__g3d__component__renderer__hpp
#include "../../component/component_base.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


template<class RendererT>
class	renderer_queue
{
public:
	renderer_queue(RendererT& _renderer)
		: mRenderer(_renderer)
	{}

	template<class FuncT>
	renderer_queue& operator += (FuncT&& _func)
	{
		_func();
		return	*this;
	}

	RendererT* operator -> () const noexcept { return &mRenderer; }

protected:
	RendererT&	mRenderer;
};


template<class RendererT, class QueueT = renderer_queue<RendererT>>
struct	renderer
		: component_base<renderer<RendererT, QueueT>>
{
	using	renderer_type	= renderer<RendererT, QueueT>;

	template<class NodeT>
	class	handler : public handler_base<NodeT>
	{
	public:
		using	queue_type	= QueueT;

		virtual void on_render(queue_type& _queue)
		{
			owner().visit_children([&](auto* _c)
			{
				_c->on_render(_queue);
			});
		}
	};


	template<class NodeT>
	class	dispatcher : public dispatcher_base<NodeT>
	{
	public:
		using	queue_type	= QueueT;
/*
		void render(RendererT& _renderer)
		{
			queue_type	q(_renderer);
			render(q);
		}

		void render(queue_type& _queue)
		{
			for (auto& c : *static_cast<g3d::dispatcher_node<NodeT>*>(this))
			{
				c.template get_component<renderer_type>().on_render(_queue);
			}
		}
*/
	};
};

AMTRS_G3D_NAMESPACE_END
#endif
