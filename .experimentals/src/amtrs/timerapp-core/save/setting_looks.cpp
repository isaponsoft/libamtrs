/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <sstream>
#include <iostream>
#include <amtrs/filesystem/@>
#include <timerapp-core/save/fileutil.hpp>
#include <timerapp-core/save/storage.hpp>
#include <timerapp-core/save/schedule.hpp>
using namespace	amtrs;
namespace save {

bool deserialize(setting_looks& _data, std::istream& _in)
{
	AMTRS_DEBUG_LOG("Load looks");
	int16_t		version;
	if (!read_int(version, _in))
	{
		return	false;
	}

	if (version == 1)
	{
		if (!read_str(_data.bgimage, _in)) { return false; }

		int16_t	tmp;
		if (!read_int(tmp, _in)) { return false; }
		_data.transpalency	= tmp;
		_data.mode			= setting_looks::bgmode_none;
		_data.color			= {0, 0, 0};
		return	true;
	}
	if (version == 2)
	{
		if (!read_int(_data.mode,			_in)) { return false; }
		if (!read_str(_data.bgimage,		_in)) { return false; }
		if (!read_int(_data.color.r,		_in)) { return false; }
		if (!read_int(_data.color.g,		_in)) { return false; }
		if (!read_int(_data.color.b,		_in)) { return false; }
		if (!read_int(_data.transpalency,	_in)) { return false; }
		if (!read_int(_data.flags,			_in)) { return false; }
		return	true;
	}

	AMTRS_DEBUG_LOG("setting_looks unkown version = %d", (int)version);
	return	false;
}



bool serialize(std::ostream& _out, setting_looks const& _data)
{
	if (!write_int(_out, (int16_t)2)		// Version code
	 || !write_int(_out, _data.mode)
	 || !write_str(_out, _data.bgimage)
	 || !write_int(_out, _data.color.r)
	 || !write_int(_out, _data.color.g)
	 || !write_int(_out, _data.color.b)
	 || !write_int(_out, _data.transpalency)
	 || !write_int(_out, _data.flags)
	)
	{
		return	false;
	}
	return	true;
}



}
