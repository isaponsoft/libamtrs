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

using namespace amtrs;
using namespace gframe;

extern	module<scene>	__default_main_scene;
extern	module<scene>	__default_title_scene;
extern	module<scene>	__default_title_scene2;

extern	module<panel>	__panel_default;
extern	module<panel>	__panel_footer;
extern	module<panel>	__panel_header;


std::initializer_list<void*> const& initmodules()
{
	static std::initializer_list<void*>	modules = 	
	{
		  &__default_main_scene
		, &__default_title_scene
		, &__default_title_scene2
		, &__panel_default
		, &__panel_footer
		, &__panel_header
	};
	return	modules;
}
