/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__x11__application_xcb__hpp
#define	__libamtrs__driver__x11__application_xcb__hpp
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include "../application.hpp"
AMTRS_NAMESPACE_BEGIN

class	application_xcb_hook
{
public:
	class	xcbdisplay
	{
	public:
		virtual Display* get_display() const noexcept = 0;
	};

	class	xcbwindow
	{
	public:
		virtual xcb_window_t get_window() const noexcept = 0;
		virtual void on_xevent(xcb_button_press_event_t*, bool _press) = 0;
		virtual void on_xevent(xcb_client_message_event_t*) = 0;
		virtual void on_xevent(xcb_configure_notify_event_t*) = 0;
		virtual void on_xevent(xcb_configure_request_event_t*) = 0;
		virtual void on_xevent(xcb_destroy_notify_event_t*) = 0;
		virtual void on_xevent(xcb_enter_notify_event_t*) = 0;
		virtual void on_xevent(xcb_expose_event_t*) = 0;
		virtual void on_xevent(xcb_key_press_event_t*, bool _press) = 0;
		virtual void on_xevent(xcb_map_request_event_t*) = 0;
		virtual void on_xevent(xcb_motion_notify_event_t*) = 0;
		virtual void on_xevent(xcb_property_notify_event_t*) = 0;
		virtual void on_xevent(xcb_unmap_notify_event_t*) = 0;
	};


	application_xcb_hook()
	{
	}

	~application_xcb_hook()
	{
		if (mConn)
		{
			xcb_disconnect(mConn);
		}
	}

	void init()
	{
		if (!mConn)
		{
			mConn	= xcb_connect(nullptr, &mScreenCount);
		}
	}

	void add_display(xcbdisplay* _display)
	{
		mDisplayList.push_back(_display);
	}

	void remove_display(xcbdisplay* _display)
	{
		auto	it	= std::find(mDisplayList.begin(), mDisplayList.end(), _display);
		if (it != mDisplayList.end())
		{
			mDisplayList.erase(it);
		}
	}

	void add_window(xcbwindow* _window)
	{
		mWindowList.push_back({
			.window	= _window,
			.enable	= true
		});
	}

	void remove_window(xcbwindow* _window)
	{
		auto	it	= std::find_if(mWindowList.begin(), mWindowList.end(), [_window](auto& i) { return i.window == _window; });
		if (it != mWindowList.end())
		{
			it->enable	= false;
		}
	}

	void gc()
	{
		mWindowList.remove_if([](auto& i)
		{
			return	i.enable == false;
		});
	}

	struct	windownode
	{
		xcbwindow*	window	= nullptr;
		bool		enable	= false;
	};

	xcb_connection_t*		mConn	= nullptr;
	int						mScreenCount;
	std::list<xcbdisplay*>	mDisplayList;
	std::list<windownode>	mWindowList;

	static application_xcb_hook* get_instance()
	{
		static	application_xcb_hook	sHook;
		return	&sHook;
	}

};

template<class EventT>
void fire_xcb_event_with_window(application_xcb_hook* _app, xcb_generic_event_t* _generic)
{
	EventT*	ev	= (EventT*)_generic;
	for (auto win : _app->mWindowList)
	{
		if (win.window->get_window() == ev->window)
		{
			win.window->on_xevent(ev);
		}
	}
}

template<class EventT>
void fire_xcb_event_with_event(application_xcb_hook* _app, xcb_generic_event_t* _generic)
{
	EventT*	ev	= (EventT*)_generic;
	for (auto win : _app->mWindowList)
	{
		if (win.window->get_window() == ev->event)
		{
			win.window->on_xevent(ev);
		}
	}
}

template<class EventT>
void fire_xcb_event_with_event(application_xcb_hook* _app, xcb_generic_event_t* _generic, bool _press)
{
	EventT*	ev	= (EventT*)_generic;
	for (auto win : _app->mWindowList)
	{
		if (win.window->get_window() == ev->event)
		{
			win.window->on_xevent(ev, _press);
		}
	}
}




AMTRS_NAMESPACE_END
#endif
