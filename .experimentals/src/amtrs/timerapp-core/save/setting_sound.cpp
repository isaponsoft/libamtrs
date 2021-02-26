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


bool deserialize(setting_sound& _data, std::istream& _in)
{
	AMTRS_DEBUG_LOG("Load sound");
	int16_t		version;
	if (!read_int(version, _in))
	{
		return	false;
	}

	if (version == 1)
	{
		bool	tmp;

		if (!read_int(tmp, _in)) { return false; }
		_data.systemSound		= tmp;

		if (!read_int(tmp, _in)) { return false; }
		_data.homeTouchSound	= tmp;

		return	true;
	}

	AMTRS_DEBUG_LOG("setting_sound unkown version = %d", (int)version);
	return	false;
}



bool serialize(std::ostream& _out, setting_sound const& _data)
{
	if (!write_int(_out, (int16_t)1)		// Version code
	 || !write_int(_out, (bool)(_data.systemSound))
	 || !write_int(_out, (bool)(_data.homeTouchSound))
	)
	{
		return	false;
	}
	return	true;
}



}
