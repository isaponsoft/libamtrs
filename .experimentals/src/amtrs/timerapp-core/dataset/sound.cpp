/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem/@>
#include <amtrs/string/@>
#include <timerapp-core/dataset/sound.hpp>
using namespace amtrs;


static	sound	sInstance;


bool sound::has_sound()
{
	return	!sInstance.groups.empty();
}



sound const* sound::get_instance()
{
	return &sInstance;
}



sound::group const* sound::find_group(std::string const& _group)
{
	for (auto& g : sInstance.groups)
	{
		if (g.name == _group)
		{
			return &g;
		}
	}
	return nullptr;
}


sound::item const* sound::find_item(std::string const& _item)
{
	for (auto& g : sInstance.groups)
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


auto sound::enum_items() -> std::vector<std::pair<group const*, item const*>>
{
	std::vector<std::pair<group const*, item const*>>	retval;
	for (auto& g : sInstance.groups)
	{
		for (auto& i : g.items)
		{
			retval.push_back({&g, &i});
		}
	}
	return	retval;
}



void sound::initialize(std::string const& _configfile)
{
	// コンフィグファイルを読み取る
	std::string	data		= amtrs::filesystem::file_get_contents<std::string>(_configfile);
	if (data.empty())
	{
		AMTRS_WARN_LOG("Nothing sound list file %s", _configfile.c_str());
		return;
	}
	int			linenumber	= 0;
	int			mode		= 0;
	for (auto l : split_chars(data, "\n"))
	{
		++linenumber;

		// 改行コードを取り除き、空行ならスキップする。
		std::string_view	line	= trim(l, " \t\r\n");
		if (line == "")
		{
			continue;
		}
		if (line == "group")
		{
			mode	= 1;
			sInstance.groups.push_back({});
			continue;
		}
		if (line == "files")
		{
			mode = 2;
			continue;
		}

		auto	pos	= line.find_first_of(':');
		if (pos == decltype(line)::npos)
		{
			AMTRS_DEBUG_LOG("Unkown command line %d.", linenumber);
			continue;
		}
		//AMTRS_DEBUG_LOG("line: %s", std::string(line).c_str());

		auto	cmd	= line.substr(0, pos);
		auto	val	= trim(line.substr(pos + 1), " \t");
		if (cmd == "store")
		{
			sInstance.groups.back().product	= val;
			continue;
		}

		if (cmd == "file")
		{
			if (mode == 2)
			{
				sInstance.groups.back().items.push_back({});
				sInstance.groups.back().items.back().filename	= std::string("voice/") + std::string(val);
			}
			continue;
		}

		if (cmd == "title")
		{
			if (mode == 1)
			{
				sInstance.groups.back().name	= val;
			}
			if (mode == 2)
			{
				sInstance.groups.back().items.back().title	= val;
			}
			continue;
		}
	}
}
