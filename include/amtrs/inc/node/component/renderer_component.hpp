/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__node__component__renderer_component__hpp
#define	__libamtrs__node__component__renderer_component__hpp
#include "../../../g3d.hpp"
AMTRS_NAMESPACE_BEGIN namespace component {

template<class Traits>
class	renderer_component
{
public:
	using	device_type		= g3d::device;
	using	draw_argument	= typename Traits::draw_argument;

	struct	render_queue;


	class	layer
	{
	public:
		using	layer_order_type	= int;

		layer(layer_order_type _order = 0)
			: mLayerOrder(_order)
		{}

		void on_transfer(draw_argument _darg)
		{
			for (auto& q : this->mTransferQueue)
			{
				q.func(_darg);
			}
		}

		void on_draw(draw_argument _darg)
		{
			auto*	device	= get_g3ddevice(_darg);
			for (auto& q : this->mDrawCalls)
			{
				q(device);
			}
		}

		void clear()
		{
			mTransferQueue.clear();
			mDrawCalls.clear();
		}

		//! レイヤーの順番を指定します。
		//! 値が低いほど先に処理されます。つまり、奥に描画されることになります。
		void set_layer_order(layer_order_type _priority)
		{
			mLayerOrder	= _priority;
		}

		layer_order_type get_layer_order() const noexcept { return mLayerOrder; }


	protected:
		void transfer(ref_object* _obj, std::function<void(draw_argument)>&& _renderer)
		{
			mTransferQueue.push_back({_obj, _renderer});
		}

		void draw(g3d::draw_command* _cmd)
		{
			mDrawCalls.push_back([cmd = ref<g3d::draw_command>(_cmd)](auto g){ g->draw(cmd); });
		}

		void draw(std::function<void(g3d::device*)>&& _cmd)
		{
			mDrawCalls.push_back(std::move(_cmd));
		}

	private:
		struct	queue
		{
			ref<ref_object>						obj;
			std::function<void(draw_argument)>	func;
		};

		std::vector<queue>								mTransferQueue;
		std::vector<std::function<void(g3d::device*)>>	mDrawCalls;
		layer_order_type								mLayerOrder	= 0;

		friend	render_queue;
	};


	struct	render_queue
	{
		void transfer(ref_object* _obj, std::function<void(draw_argument)>&& _renderer)
		{
			current->transfer(_obj, std::move(_renderer));
		}

		void draw(g3d::draw_command* _cmd)
		{
			current->draw(_cmd);
		}

		void draw(std::function<void(g3d::device*)>&& _cmd)
		{
			current->draw(std::move(_cmd));
		}

		device_type*									device;
		mat44<float>									projection;
		layer*											current	= nullptr;

		operator device_type& () const noexcept { return *device; }
		operator const mat44<float>& () const noexcept { return projection; }
	};


	// ========================================================================
	//! 再レンダリクエストを発行します。
	// ------------------------------------------------------------------------
	static void rerender_request();


	// ========================================================================
	//! 再レンダフラグを取得します。
	// ------------------------------------------------------------------------
	static bool is_rerender_requested();


	// ========================================================================
	//! 再レンダフラグをリセットします。
	// ------------------------------------------------------------------------
	static void reset_rerender_request();

protected:
	virtual void on_render(render_queue& _rs)
	{}


private:

	static bool& rerender_flag() noexcept
	{
		static	bool		s_rerender	= false;
		return	s_rerender;
	}
};


template<class Traits>
void renderer_component<Traits>::rerender_request()
{
	rerender_flag() = true;
}


template<class Traits>
bool renderer_component<Traits>::is_rerender_requested()
{
	return	rerender_flag();
}


template<class Traits>
void renderer_component<Traits>::reset_rerender_request()
{
	rerender_flag()	= false;
}




} AMTRS_NAMESPACE_END
#endif
