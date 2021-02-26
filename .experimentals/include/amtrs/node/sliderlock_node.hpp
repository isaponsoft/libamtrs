/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__sliderlock_node__hpp
#define	__libamtrs__node__sliderlock_node__hpp
#include "fill_node.hpp"
#include "slider_node.hpp"
AMTRS_NAMESPACE_BEGIN


struct	sliderlock_node_base
{
	enum class	status_type
	{
		locking			= 1,		//!< マーカーの位置がロック状態になっている
		unlocking		= 2,		//!< マーカーの位置がアンロック状態になっている
		unlocked		= 3,		//!< アンロックが確定した
	};

};

//! スライダー式の鍵
template<class SuperT>
class	sliderlock_node
		: public slider_bar_node<float, SuperT>
		, public sliderlock_node_base
{
public:
	using	status_type	= typename sliderlock_node_base::status_type;
	using	super_type	= slider_bar_node<float, SuperT>;

	template<class... Args>
	sliderlock_node(Args&&... _args)
		: super_type(0.0f, 0.0f, 1.0f, std::forward<Args>(_args)...)
	{}


	void set_unlock_listener(std::function<void(status_type)> _listener)
	{
		mUnlockListener	= std::move(_listener);
	}

protected:
	virtual void on_lock_status(status_type _status)
	{
		mLastStatus	= _status;
		if (mUnlockListener)
		{
			mUnlockListener(_status);
		}
	}

	virtual float on_value_change(float _value) override
	{
		float	r	= super_type::on_value_change(_value);
		if (r == 1)
		{
			if (mLastStatus == status_type::locking)
			{
				on_lock_status(status_type::unlocking);
			}
		}
		else
		{
			if (mLastStatus != status_type::locking)
			{
				on_lock_status(status_type::locking);
			}
		}
		return	r;
	}


	virtual float on_input_release(float _value) override
	{
		_value	= super_type::on_input_release(_value);
		if (_value == 1)
		{
			mUnlocked	= true;
			on_lock_status(status_type::unlocked);
			return	1;
		}
		else
		{
			return	mUnlocked ? 1.0f : 0.0f;
		}
	}

private:
	std::function<void(status_type)>	mUnlockListener;
	status_type							mLastStatus	= status_type::locking;
	bool								mUnlocked	= false;
};

AMTRS_NAMESPACE_END
#endif
