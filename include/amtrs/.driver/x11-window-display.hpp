/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__x11__window_display__xlib__hpp
#define	__libamtrs__platforms__x11__window_display__xlib__hpp
#include <map>
#include <X11/Xlib.h>
#include "x11-application.hpp"
AMTRS_NAMESPACE_BEGIN

class	display_xlib
		: public display
		, public application_xcb_hook::xcbdisplay
{
public:
	static std::map<int, display_xlib*>& display_list()
	{
		static std::map<int, display_xlib*>	sDisplays;
		return	sDisplays;
	}

	~display_xlib()
	{
		if (mDisplay)
		{
			XCloseDisplay(mDisplay);
		}
		display_list().erase(mDisplayNumber);
		application_xcb_hook::get_instance()->remove_display(this);
	}

	void initialize(int _displayNumber)
	{
		errno			= 0;
		mDisplayNumber	= _displayNumber;
		mDisplay		= XOpenDisplay(NULL);
		if (!mDisplay)
		{
			
			AMTRS_WARN_LOG("Can't open XOpenDisplay %d", errno);
			throw	std::exception();
		}
		mScreen			= XDefaultScreenOfDisplay(mDisplay);
		if (!mScreen)
		{
			
			AMTRS_WARN_LOG("Can't open XDefaultScreenOfDisplay");
			throw	std::exception();
		}

		application_xcb_hook::get_instance()->add_display(this);

		display_list().insert({mDisplayNumber, this});
	}


	virtual float pixel_par_inch() override
	{
		return	-1;
	}

	virtual float pt_to_pixel_scale() override
	{
		return	1;
	}


	virtual size2<int> size() override
	{
		auto	retval	= size2<int>(XWidthOfScreen(mScreen), XHeightOfScreen(mScreen));
		//AMTRS_WARN_LOG("Display size %dx%d", retval.width, retval.height);
		return	retval;
	}


	virtual bool is_window_mode_enable() override
	{
		return	true;
	}

	Display* get_display() const noexcept override { return mDisplay; }
	Screen* get_screen() const noexcept { return mScreen; }

private:
	int			mDisplayNumber	= -1;
	Display*	mDisplay		= nullptr;
	Screen*		mScreen			= nullptr;
};


inline ref<display> display::get_display(int _display_number)
{
	auto	it	= display_xlib::display_list().find(_display_number);
	if (it != display_xlib::display_list().end())
	{
		return	it->second;
	}

	if (_display_number != 0)
	{
		AMTRS_WARN_LOG("Unsupported display number.");
		throw	std::exception();
	}
	ref<display_xlib>	thiz = new display_xlib();
	thiz->initialize(_display_number);
	return	thiz;
}

AMTRS_NAMESPACE_END
#endif
