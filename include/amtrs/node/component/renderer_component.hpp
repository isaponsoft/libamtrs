/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__node__component__renderer_component__hpp
#define	__libamtrs__node__component__renderer_component__hpp
#include "../../g3d/@"
AMTRS_NAMESPACE_BEGIN

namespace component {

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

		void on_draw(draw_argument device)
		{
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
		void transfer(ref_object* _obj, std::function<void(g3d::device*)>&& _renderer)
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
			std::function<void(g3d::device*)>	func;
		};

		std::vector<queue>								mTransferQueue;
		std::vector<std::function<void(g3d::device*)>>	mDrawCalls;
		layer_order_type								mLayerOrder	= 0;

		friend	render_queue;
	};


	struct	render_queue
	{
		void transfer(ref_object* _obj, std::function<void(g3d::device*)>&& _renderer)
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

	// ========================================================================
	//! 描画レイヤーを取得します。
	// ------------------------------------------------------------------------
	layer* get_layer() const noexcept { return mLayer; }


	// ========================================================================
	//! 描画レイヤーを設定します。
	// ------------------------------------------------------------------------
	//! nullptr を設定した場合は親ノードに従います。
	// ------------------------------------------------------------------------
	void set_layer(layer* _layer) noexcept { mLayer = _layer; }

	virtual bool visibility() const noexcept { return mVisible; }

	// ========================================================================
	//! 可視状態を変更します。
	// ------------------------------------------------------------------------
	//! 不可視状態ではレンダリングは行われませんが、ほかの機能は有効です。
	// ------------------------------------------------------------------------
	void set_visibility(bool _visible) noexcept { if (mVisible != _visible) { mVisible = _visible; on_visibility(mVisible); } }

protected:
	virtual void on_render(render_queue& _rs)
	{}


private:

	static bool& rerender_flag() noexcept
	{
		static	bool		s_rerender	= false;
		return	s_rerender;
	}


	layer*		mLayer		= nullptr;
	bool		mVisible	= true;
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
