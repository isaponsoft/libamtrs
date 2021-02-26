/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <X11/keysym.h>
#include <amtrs/amtrs.hpp>
#include <amtrs/application/@>
#include AMTRS_DRIVER_CUSTOM_INCLUDE(x11/input-keyboard.hpp)
AMTRS_NAMESPACE_BEGIN


int application::run(int _argc, char* _args[])
{
	AMTRS_WARN_LOG("Application::Run");
	this->on_created(_argc, _args);

	auto*	app		= application_xcb_hook::get_instance();
	while (!this->is_quit() && !app->mWindowList.empty())
	{
		xcb_connection_t*		conn	= app->mConn;
		xcb_generic_event_t*	event;
		while ((event = xcb_poll_for_event(conn)) != nullptr)
		{
			switch (event->response_type & ~0x80)
			{
				case XCB_BUTTON_PRESS:		fire_xcb_event_with_event<xcb_button_press_event_t>(app, event, true);	break;
				case XCB_BUTTON_RELEASE:	fire_xcb_event_with_event<xcb_button_press_event_t>(app, event, false);	break;
				case XCB_CLIENT_MESSAGE:	fire_xcb_event_with_window<xcb_client_message_event_t>(app, event);		break;
				case XCB_CONFIGURE_NOTIFY:	fire_xcb_event_with_event<xcb_configure_notify_event_t>(app, event);	break;
				case XCB_CONFIGURE_REQUEST:	fire_xcb_event_with_window<xcb_configure_request_event_t>(app, event);	break;
				case XCB_DESTROY_NOTIFY:	fire_xcb_event_with_window<xcb_destroy_notify_event_t>(app, event);		break;
				case XCB_ENTER_NOTIFY:		fire_xcb_event_with_event<xcb_enter_notify_event_t>(app, event);		break;
				case XCB_EXPOSE:			fire_xcb_event_with_window<xcb_expose_event_t>(app, event);				break;
				case XCB_KEY_PRESS:			fire_xcb_event_with_event<xcb_key_press_event_t>(app, event, true);		break;
				case XCB_KEY_RELEASE:		fire_xcb_event_with_event<xcb_key_press_event_t>(app, event, false);	break;
				case XCB_MAP_REQUEST:		fire_xcb_event_with_window<xcb_map_request_event_t>(app, event);		break;
				case XCB_MOTION_NOTIFY:		fire_xcb_event_with_event<xcb_motion_notify_event_t>(app, event);		break;
				case XCB_PROPERTY_NOTIFY:	fire_xcb_event_with_window<xcb_property_notify_event_t>(app, event);	break;
				case XCB_UNMAP_NOTIFY:		fire_xcb_event_with_window<xcb_unmap_notify_event_t>(app, event);		break;
			}
			free(event);
		}
		app->gc();
		if (this->is_quit() || app->mWindowList.empty())
		{
			break;
		}
		this->on_update();
	}
	app->gc();
	this->on_event(amtrs::application::system_event::stop);
	app->gc();
	return	0;
}

AMTRS_NAMESPACE_END
