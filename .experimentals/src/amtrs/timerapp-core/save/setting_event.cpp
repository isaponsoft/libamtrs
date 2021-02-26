/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <sstream>
#include <iostream>
#include <amtrs/filesystem/@>
#include <timerapp-core/save/fileutil.hpp>
#include <timerapp-core/save/storage.hpp>
#include <timerapp-core/save/setting_event.hpp>
using namespace	amtrs;
namespace save {


bool deserialize(setting_event& _data, std::istream& _in)
{
	AMTRS_DEBUG_LOG("Load event");
	int16_t		version;
	if (!read_int(version, _in))
	{
		return	false;
	}

	if (version == 1)
	{
		if (!read_str(_data.selectCalendar, _in)) { return false; }
		return	true;
	}

	AMTRS_ERROR_LOG("setting_event unkown version = %d", (int)version);
	return	false;
}



bool serialize(std::ostream& _out, setting_event const& _data)
{
	if (!write_int(_out, (int16_t)1)		// Version code
	 || !write_str(_out, _data.selectCalendar)
	)
	{
		return	false;
	}
	return	true;
}



}
