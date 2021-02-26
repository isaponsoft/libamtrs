/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/language/json.hpp>
#include <amtrs/filesystem/filename.hpp>
#include <amtrs/variant/@>
#include <gframe/load_string.hpp>
#include <timerapp-core/dataset/bg.hpp>

using namespace amtrs;
using namespace gframe;

using	variant_type	= decltype(json_decode(std::declval<std::string_view>()));

static	bg	sBG;



bool bg::has_bg()
{
	return	!sBG.groups.empty();
}


bg const* bg::get_bg()
{
	return	&sBG;
}


bg::group const* bg::find_group(std::string const& _group)
{
	for (auto& g : sBG.groups)
	{
		if (g.name == _group)
		{
			return	&g;
		}
	}
	return	nullptr;
}


bg::item const* bg::find_item(std::string const& _item)
{
	for (auto& g : sBG.groups)
	{
		for (auto& i : g.items)
		{
			if (i.title == _item || i.filename == _item)
			{
				return	&i;
			}
		}
	}
	return	nullptr;
}


std::vector<bg::item const*> bg::enum_items()
{
	std::vector<bg::item const*>	retval;
	for (auto& g : sBG.groups)
	{
		for (auto& i : g.items)
		{
			retval.push_back(&i);
		}
	}
	return	retval;
}



std::string bg::prev_filename(std::string const& _filename)
{
	auto		stm	= filesystem::stem(_filename);
	auto		ext	= filesystem::extension(_filename);
	std::string	ret	= std::string(stm) + std::string("_prev") + std::string(ext);
	AMTRS_DEBUG_LOG("%s -> %s", _filename.c_str(), ret.c_str());
	return	ret;
}



void bg::initialize(std::string const& _configfile)
{
	static const auto	load_item	= [&](variant_type& _root) -> item
	{
		item		retval;
		auto&		bgNode	= std::get<variant_type::assoc_type>(_root);
		retval.title	= std::get<std::string>(bgNode["title"]);
		retval.filename	= std::string("bg/") + std::get<std::string>(bgNode["filename"]);
		if (auto* store = bgNode.has_if<std::string>("store"))
		{
			retval.productId	= *store;
		}
		AMTRS_DEBUG_LOG("title=%s, filename=%s, store=%s", retval.title.c_str(), retval.filename.c_str(), retval.productId.c_str());
		return	retval;
	};


	static const auto	load_group	= [&](variant_type& _root) -> group
	{
		group		retval;
		auto&		gnode	= std::get<variant_type::assoc_type>(_root);
		retval.name	= std::get<std::string>(gnode["group"]);
		for (auto& itm : std::get<variant_type::array_type>(gnode["items"]))
		{
			retval.items.push_back(load_item(itm));
		}
		return	retval;
	};


	auto	json			= load_string(_configfile);
	auto	root			= json_decode(json);
	for (auto& bgInfo : std::get<variant_type::array_type>(root))
	{
		auto	g	= load_group(bgInfo);
		if (!g.items.empty())
		{
			sBG.groups.push_back(std::move(g));
		}
	}
}

