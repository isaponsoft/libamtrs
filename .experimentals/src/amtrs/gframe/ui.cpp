/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <clocale>
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/g3d/@>
#include <amtrs/node/@>
#include <gframe/@>
#include <app-config.hpp>		// Application configuration.

using namespace	amtrs;
using namespace	gframe;


GFRAME_NAMESPACE_BEGIN namespace ui {
using namespace	amtrs;

static float	sFontSize	= 32;


float get_font_size()
{
	return	sFontSize;
}


void set_font_size(float _size)
{
	sFontSize	= _size;
}



ref<button> button::create(vec3<float> _position, std::string const& _title, std::string const& _image, amtrs::rgba<float> _color)
{
	ref<button>	btn	= new button(load_texture(_image));
	btn->set_trigger_callback([b = btn.get()](auto pos)
	{
		AMTRS_DEBUG_LOG("Button push");
	});
	btn->set_position(_position);

	AMTRS_DEBUG_LOG("[%p]->add_child(button)", static_cast<node*>(scene::current()));

	scene::current()->add_child(btn);
	return	btn;
}


} GFRAME_NAMESPACE_END
