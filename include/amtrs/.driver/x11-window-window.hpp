/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platforms__x11__window_window_xcb__hpp
#define	__libamtrs__platforms__x11__window_window_xcb__hpp
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <X11/Xlib.h>
#include "x11-window-display.hpp"
AMTRS_NAMESPACE_BEGIN


template<class BaseT>
class	window_xcb
		: public BaseT
		, public application_xcb_hook::xcbwindow
{
public:
	using	super_type	= BaseT;
	using	size_type	= typename super_type::size_type;

	window_xcb()
	{
		application_xcb_hook::get_instance()->init();
		application_xcb_hook::get_instance()->add_window(this);
		mDisplay	= ref<display_xlib>(static_cast<display_xlib*>(display::get_display().get()));
		mConn		= application_xcb_hook::get_instance()->mConn;
	}

	~window_xcb()
	{
		destroyed();
	}

	void destroyed()
	{
		application_xcb_hook::get_instance()->remove_window(this);
		if (mCreating)
		{
			mCreating	= false;
			xcb_destroy_window(mConn, mWindow);
			if (mDelete)
			{
				free(mDelete);
			}
		}
	}


	bool initialize(size_type _size)
	{
		return	create_window(_size);
	}

	bool create_window(size_type _size, Colormap* _cmap = nullptr, int _depth = 0, Visual* _visual = nullptr)
	{
		mWindow	= xcb_generate_id(mConn);

		xcb_screen_iterator_t	iter	= xcb_setup_roots_iterator(xcb_get_setup(mConn));
		xcb_void_cookie_t		cook;

		uint32_t window_values[] =
		{
			XCB_EVENT_MASK_EXPOSURE |
			XCB_EVENT_MASK_STRUCTURE_NOTIFY |
			XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
			XCB_EVENT_MASK_PROPERTY_CHANGE |
			XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE|
			XCB_EVENT_MASK_BUTTON_MOTION|XCB_EVENT_MASK_POINTER_MOTION|
			XCB_EVENT_MASK_KEY_PRESS|XCB_EVENT_MASK_KEY_RELEASE
		};

		int	scr	= application_xcb_hook::get_instance()->mScreenCount;
		while (scr-- > 0)	xcb_screen_next(&iter);
		mScreen		= iter.data;
		mWindowSize	= _size;

		cook	= xcb_create_window(
					mConn,
					XCB_COPY_FROM_PARENT,
					mWindow,
					mScreen->root,
					0, 0,
					_size.width,
					_size.height,
					0,
					XCB_WINDOW_CLASS_INPUT_OUTPUT,
					mScreen->root_visual,
					XCB_CW_EVENT_MASK, window_values
				);
		if (auto err = xcb_request_check(mConn, cook); err)
		{
			AMTRS_WARN_LOG("xcb_create_window: response=%d, error_code=%d", err->response_type, err->error_code);
			return	false;
		}

		xcb_intern_atom_cookie_t	cookProto		= xcb_intern_atom(mConn, 1, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
	    xcb_intern_atom_reply_t*	repProro		= xcb_intern_atom_reply(mConn, cookProto, 0);
		xcb_intern_atom_cookie_t	cookDel			= xcb_intern_atom(mConn, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
	    xcb_intern_atom_reply_t*	repDel			= xcb_intern_atom_reply(mConn, cookDel, 0);
		mDelete	= repDel;

		cook	= xcb_change_property(mConn,
					XCB_PROP_MODE_REPLACE,
					mWindow,
					repProro->atom,
					XCB_ATOM_ATOM,
					32,
					1, &mDelete->atom
				);
		free(repProro);
		xcb_map_window(mConn, mWindow);

		if (auto err = xcb_request_check(mConn, cook); err)
		{
			AMTRS_WARN_LOG("xcb_change_property(Expose hook): response=%d, error_code=%d", err->response_type, err->error_code);
			return	false;
		}

		static const char title[] = "amtrs";
		cook	= xcb_change_property(mConn,
					XCB_PROP_MODE_REPLACE,
					mWindow,
					get_atom(mConn, "_NET_WM_NAME"),
					get_atom(mConn, "UTF8_STRING"),
					8,
					strlen(title), title
				);
		if (auto err = xcb_request_check(mConn, cook); err)
		{
			AMTRS_WARN_LOG("xcb_change_property(Name): response=%d, error_code=%d", err->response_type, err->error_code);
			return	false;
		}		
		cook	= xcb_map_window(mConn, mWindow);
		if (auto err = xcb_request_check(mConn, cook); err)
		{
			AMTRS_WARN_LOG("xcb_map_window: response=%d, error_code=%d", err->response_type, err->error_code);
			return	false;
		}
		//xcb_aux_sync(mConn);
		xcb_flush(mConn);

		cook	= xcb_map_window_checked(mConn, mWindow);
		if (auto err = xcb_request_check(mConn, cook); err)
		{
			AMTRS_WARN_LOG("xcb_map_window_checked: response=%d, error_code=%d", err->response_type, err->error_code);
			return	false;
		}
		xcb_flush(mConn);


		mCreating	= true;
		return	true;
	}

	virtual void* get_context() const noexcept override
	{
		return	reinterpret_cast<void*>(mWindow);
	}

	virtual size_type size() const noexcept override
	{
		if (!mCreating)
		{
			return	size_type(0, 0);
		}
		return	mWindowSize;
	}

	virtual bool repaint(std::function<void()> _repevent) override
	{
		if (_repevent)
		{
			_repevent();
		}
		else
		{
			window::repaint_event	ev({{0,0}, this->size()});
			window::fire_event(window::event_type::repaint, &ev);
		}
		return	true;
	}

	virtual xcb_window_t get_window() const noexcept override
	{
		return	mWindow;
	}


	virtual void on_xevent(xcb_button_press_event_t* _event, bool _press) override
	{
		if (super_type::onInputEvent)
		{
			if (_event->detail == 4 || _event->detail == 5
			 || _event->detail == 6 || _event->detail == 7)
			{
				// wheel
				mouse_wheel_event	e;
				e.axis				= (_event->detail == 4 || _event->detail == 5)
									? mouse_wheel_event::axis_x
									: mouse_wheel_event::axis_y;
				e.delta				= (_event->detail == 4 || _event->detail == 6)
									? -1
									:  1;
				e.point		= mouse_event::point_type(_event->event_x, _event->event_y);
				super_type::onInputEvent(&e);
			}
			else
			{
				mouse_event	e;
				e.buttons	= _event->detail == 1 ? mouse_event::left
							: _event->detail == 2 ? mouse_event::center
							: _event->detail == 3 ? mouse_event::right
							: 0;
				e.state		= _press ? mouse_event::status::press : mouse_event::status::release;
				e.point		= mouse_event::point_type(_event->event_x, _event->event_y);
				super_type::onInputEvent(&e);
			}
		}
	}


	virtual void on_xevent(xcb_client_message_event_t* _event) override
	{
		if (!mCreating)
		{
			return;
		}
		if (_event->data.data32[0] == mDelete->atom)
		{
			destroyed();
		}
		return;
	}

	virtual void on_xevent(xcb_configure_notify_event_t* _event) override
	{
		mWindowSize	= size_type(_event->width, _event->height);
	}

	virtual void on_xevent(xcb_configure_request_event_t*) override
	{
	}

	// ウィンドウが破棄された
	virtual void on_xevent(xcb_destroy_notify_event_t*) override
	{
		application_xcb_hook::get_instance()->remove_window(this);
	}

	virtual void on_xevent(xcb_enter_notify_event_t*) override
	{
	}

	virtual void on_xevent(xcb_expose_event_t*) override
	{
		window::repaint_event	ev({{0,0}, this->size()});
		this->fire_event(window::event_type::repaint, &ev);
	}

	virtual void on_xevent(xcb_key_press_event_t* _event, bool _press) override
	{
		if (super_type::onInputEvent)
		{
			keyboard_event	e;
			e.key		= x11key_to_keycode(mDisplay->get_display(), _event->detail);
			e.state		= mouse_event::status::press;
			super_type::onInputEvent(&e);
		}
	}

	virtual void on_xevent(xcb_map_request_event_t*) override
	{
	}

	virtual void on_xevent(xcb_motion_notify_event_t* _event) override
	{
		if (super_type::onInputEvent)
		{
			mouse_event	e;
			e.buttons	= (_event->state & 0x100 ? mouse_event::left   : 0)
						| (_event->state & 0x200 ? mouse_event::center : 0)
						| (_event->state & 0x400 ? mouse_event::right  : 0)
			;
			e.state		= e.buttons ? mouse_event::status::drag : mouse_event::status::move;
			e.point		= mouse_event::point_type(_event->event_x, _event->event_y);
			super_type::onInputEvent(&e);
		}
	}

	virtual void on_xevent(xcb_property_notify_event_t*) override
	{
	}

	virtual void on_xevent(xcb_unmap_notify_event_t*) override
	{
	}


	static xcb_atom_t get_atom(struct xcb_connection_t* _conn, char const* _name)
	{
		xcb_intern_atom_cookie_t	cookie;
		xcb_intern_atom_reply_t*	reply;
		xcb_atom_t					atom;

		cookie	= xcb_intern_atom(_conn, 0, strlen(_name), _name);
		reply	= xcb_intern_atom_reply(_conn, cookie, NULL);
		if (reply)
		{
			atom = reply->atom;
		}
		else
		{
			atom = XCB_NONE;
		}
		free(reply);
		return	atom;
	}

	xcb_connection_t* get_xcb_connection() const noexcept { return mConn; }
	xcb_window_t get_xcb_window() const noexcept { return mWindow; }

protected:
	bool						mCreating	= false;
	ref<display_xlib>			mDisplay;

	xcb_screen_t*				mScreen;
	size_type					mWindowSize;

	xcb_connection_t*			mConn	= nullptr;
	xcb_window_t				mWindow	= 0;

	xcb_intern_atom_reply_t*	mDelete	= nullptr;

	std::function<void()>		mRepaintCallback;
};



AMTRS_NAMESPACE_END
#endif
