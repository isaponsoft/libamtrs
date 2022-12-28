/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/window.hpp>
AMTRS_NAMESPACE_BEGIN

bool window::fire_activate(bool _active)
{
	if (!mHandler)
	{
		return	false;
	}
	event_data	e(event_type::activate);
	e.wm.activate	= _active;
	return	mHandler(*this, e);
}


bool window::fire_created()
{
	if (!mHandler)
	{
		return	false;
	}
	event_data	e(event_type::created);
	return	mHandler(*this, e);
}


bool window::fire_destroyed()
{
	if (!mHandler)
	{
		return	false;
	}
	event_data	e(event_type::destroyed);
	return	mHandler(*this, e);
}


bool window::fire_focus(bool _focus)
{
	if (!mHandler)
	{
		return	false;
	}
	event_data	e(event_type::focus);
	e.wm.focus	= _focus;
	return	mHandler(*this, e);
}


bool window::fire_input(input_event* _input)
{
	if (!mHandler)
	{
		return	false;
	}
	event_data	e(event_type::input);
	e.wm.input	= _input;
	return	mHandler(*this, e);
}


bool window::fire_sizechanged(size_type _size)
{
	if (!mHandler)
	{
		return	false;
	}
	event_data	e(event_type::sizechanged);
	e.wm.sizechanged	= _size;
	return	mHandler(*this, e);
}


bool window::fire_repaint(rect<int> _area)
{
	if (!mHandler)
	{
		return	false;
	}
	event_data	e(event_type::repaint);
	e.wm.repaint	= _area;
	return	mHandler(*this, e);
}

AMTRS_NAMESPACE_END
