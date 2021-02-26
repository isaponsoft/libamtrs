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

GFRAME_NAMESPACE_BEGIN

static	scene*	sCurrentScene	= nullptr;

scene::scene()
{
}


scene::~scene()
{
	//AMTRS_DEBUG_LOG("[%p] scene::~scene() with %s", sCurrentScene, this->name().c_str());
}


void scene::on_enter()
{
	//AMTRS_DEBUG_LOG("[%p] scene::on_enter() with %s", static_cast<node*>(this), this->name().c_str());
	super_type::on_enter();
	sCurrentScene	= this;
}

void scene::on_visible()
{
	//AMTRS_DEBUG_LOG("[%p] scene::on_visible() with %s", static_cast<node*>(this), this->name().c_str());
	super_type::on_visible();
	sCurrentScene	= this;
}


void scene::on_invisible()
{
	//AMTRS_DEBUG_LOG("[%p] scene::on_invisible() with %s", static_cast<node*>(this), this->name().c_str());
	super_type::on_invisible();
}

scene* scene::current()
{
	return	sCurrentScene;
}


GFRAME_NAMESPACE_END
