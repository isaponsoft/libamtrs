/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__component__renderer_component__hpp
#define	__libamtrs__node__component__renderer_component__hpp
AMTRS_NAMESPACE_BEGIN

template<class...>
struct	renderer;


template<class NodeT>
struct	renderer<NodeT>
{
	using	node_type		= NodeT;
	using	device_type		= g3d::device;
	using	command_type	= std::function<void(g3d::device*)>;


	g3d::commandlist* transfer() const noexcept { return transferQueue; }
	g3d::commandlist* draw() const noexcept { return drawCall; }

	device_type*			device;
	mat44<float>			projection;
	g3d::commandlist*		transferQueue	= nullptr;
	g3d::commandlist*		drawCall		= nullptr;
};



namespace component {

template<class NodeT>	class	renderer_component;

template<class NodeT>
class	renderer_component
{
public:
	using	renderer_type			= renderer<NodeT>;

	renderer_component()
		: mVisible(true)
	{
		mScheduling	= true;
		static_cast<NodeT*>(this)->schedule_once([this]()
		{
			mScheduling	= false;
			on_visibility(mVisible);
		});
	}


	// ========================================================================
	//! レンダラを呼び出します。
	// ------------------------------------------------------------------------
	static void render(NodeT* _root, renderer_type& _r)
	{
		_root->on_render(_r);
	}

	// ========================================================================
	//! 再レンダリクエストを発行します。
	// ------------------------------------------------------------------------
	static void rerender_request() { rerender_flag() = true; }


	// ========================================================================
	//! 再レンダフラグを取得します。
	// ------------------------------------------------------------------------
	static bool is_rerender_requested() { return rerender_flag(); }


	// ========================================================================
	//! 再レンダフラグをリセットします。
	// ------------------------------------------------------------------------
	static void reset_rerender_request() { rerender_flag()	= false; }

	virtual bool visibility() const { return mVisible; }

	// ========================================================================
	//! 可視状態を変更します。
	// ------------------------------------------------------------------------
	//! 不可視状態ではレンダリングは行われませんが、ほかの機能は有効です。
	// ------------------------------------------------------------------------
	void set_visibility(bool _visible) noexcept
	{
		if (mVisible != _visible)
		{
			mVisible = _visible;
			if (!mScheduling)
			{
				mScheduling	= true;
				static_cast<NodeT*>(this)->schedule_once([this]()
				{
					mScheduling	= false;
					on_visibility(mVisible);
				});
			}
		}
	}


	// ========================================================================
	//! 描画ステータスが変更された。
	// ------------------------------------------------------------------------
	virtual void on_change_render_status()
	{}


protected:
	virtual void on_render(renderer_type& _rq)
	{
		static_cast<NodeT*>(this)->visit_children([&](auto* c) -> bool
		{
			if (c->visibility() && c->is_enable())
			{
				c->on_render(_rq);
			}
			return	false;
		});
	}


	// ========================================================================
	//! 自身または親ノードの可視状態が変更したことを通知します。
	// ------------------------------------------------------------------------
	virtual void on_visibility(bool _visible)
	{
		static_cast<NodeT*>(this)->visit_children([_visible](auto* c) -> bool
		{
			c->on_visibility(_visible);
			return	false;
		});
	}


private:

	static bool& rerender_flag() noexcept
	{
		static	bool		s_rerender	= false;
		return	s_rerender;
	}


	bool		mVisible	= false;
	bool		mScheduling	= false;
};


} // namespace component
AMTRS_NAMESPACE_END
#endif
