/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__intersects_notification_node__hpp
#define	__libamtrs__node__intersects_notification_node__hpp
#include "../graphics/matrix.hpp"
#include "../graphics/collision.hpp"
#include "../graphics/obb.hpp"
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! 親ノードのエリア内に含まれているかどうか切り替わったことを通知する
// ----------------------------------------------------------------------------
//! このノードを継承すると
// ----------------------------------------------------------------------------
template<class BaseT>
class	intersects_notification_node
		: public BaseT
{
public:
	using	super_type	= BaseT;

	void set_intersects_listener(std::function<void(bool)>&& _listener)
	{
		mCallback	= std::move(_listener);
	}

	bool is_intersects() const noexcept { return mIntersects; }

	obb oriented_bounding_box() noexcept
	{
		// obb はボックスの中心座標を基に計算するので中心位置にズラしておく
		mat44<float>	cm	= mat44<float>::translate(vec3<float>(0.5f, 0.5f, 0.5f) * vec3<float>(this->size()))
							* this->local_matrix();
		if (auto parent = this->parent(); parent)
		{
			cm	*= parent->children_matrix();
		}
		return	obb(vec3<float>{0,0,0}, this->size()/2, cm);
	}

protected:
	virtual void on_enter() override
	{
		super_type::on_enter();
		update_intersects();
	}

	// ==============================================================
	//! 表示／非表示が切り替わったときに呼ばれます。
	// --------------------------------------------------------------
	virtual void on_intersects(bool _contains)
	{
		if (mCallback)
		{
			mCallback(_contains);
		}
	}


	virtual void on_coordinate_modify() override
	{
		super_type::on_coordinate_modify();
		update_intersects();
	}


private:
	void update_intersects()
	{
		if (auto parent = this->parent(); parent)
		{
			bool			cont	= intersects<float, std::greater_equal<float>>(oriented_bounding_box(), obb(vec3<float>(parent->size())/2, parent->size()/2));
			if ((mIntersects ? 1 : 0) != (cont ? 1 : 0))
			{
				mIntersects	= cont;
				on_intersects(mIntersects);
			}
		}
	}

	std::function<void(bool)>	mCallback;
	bool						mIntersects	= false;
};

AMTRS_NAMESPACE_END
#endif
