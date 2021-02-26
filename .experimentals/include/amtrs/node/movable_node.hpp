/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__movable_node__hpp
#define	__libamtrs__node__movable_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! ロングプレスすることで移動可能なノード
// ----------------------------------------------------------------------------
template<class BaseT>
class	movable_node
		: public button_node<BaseT>
{
	using	_base_type	= button_node<BaseT>;
public:
	using	_base_type::_base_type;

	void set_moving_callback(std::function<void(vec3<float>&)>&& _callback)
	{
		mMoveCallback	= std::move(_callback);
	}


protected:
	virtual void on_change_press_state(bool _press) override
	{
		_base_type::on_change_press_state(_press);
		mMoving	= false;
	}

	virtual void on_input_cancel() override
	{
		_base_type::on_input_cancel();
	}


	virtual bool on_long_press_trigger(vec3<float> _pos) override
	{
		_base_type::on_long_press_trigger(_pos);
		mMoving		= true;
		mBegin	= _pos;
		return	true;
	}

	// ------------------------------------------------------------------------
	virtual void on_drag_trigger(vec3<float> _pos) override
	{
		_base_type::on_drag_trigger(_pos);
		if (mMoving)
		{
			vec3<float>	pos	= this->position() + (_pos - mBegin);
			if (mMoveCallback)
			{
				mMoveCallback(pos);
			}
			this->set_position(pos);
		}
	}
	
private:
	bool								mMoving	= false;
	vec3<float>							mBegin;
	std::function<void(vec3<float>&)>	mMoveCallback;
};

AMTRS_NAMESPACE_END
#endif
