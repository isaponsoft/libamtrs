/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__cmdlist__hpp
#define	__libamtrs__graphics__g3d__cmdlist__hpp
#include "commandlist.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


class	cmdlist_pipeline
		: public commandlist
{
public:
	using	commandlist::bind;
	using	commandlist::draw;
	using	commandlist::viewport_scissor;
	using	commandlist::renderstate_push;
	using	commandlist::renderstate_pop;

	cmdlist_pipeline() = default;

	cmdlist_pipeline(commandlist* _parent, class pipeline _pipe)
	{
		_parent->pipeline(std::move(_pipe));
		_parent->sub(this);
	}

};


class	cmdlist_renderpass
		: public commandlist
{
public:
	using	commandlist::viewport_scissor;
	using	commandlist::renderstate_push;
	using	commandlist::renderstate_pop;

	ref<cmdlist_pipeline> pipeline(class pipeline _pipe)
	{
		return	new cmdlist_pipeline(this, std::move(_pipe));
	}
};


class	cmdlist_root
		: public commandlist
{
public:
	ref<cmdlist_renderpass> renderpass(framebuffer _target, rgba<float> _clearColor, float _depth = 1, float _stencil = 0)
	{
		ref<cmdlist_renderpass>	retval	= new cmdlist_renderpass();
		commandlist::renderpass_begin(std::move(_target), std::move(_clearColor), _depth, _stencil);
		commandlist::sub(retval);
		commandlist::renderpass_end();
		return	retval;
	}
};


class	cmdlist_presenter
		: public commandlist
{
public:
	using	commandlist::present;

	ref<cmdlist_root> root()
	{
		ref<cmdlist_root>	retval	= new cmdlist_root();
		commandlist::begin();
		commandlist::sub(retval);
		commandlist::end();
		return	retval;
	}
};




AMTRS_G3D_NAMESPACE_END
#endif
