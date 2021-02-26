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


bool deserialize(setting_energy& _data, std::istream& _in)
{
	int16_t		version;
	if (!read_int(version, _in))
	{
		return	false;
	}

	if (version == 1)
	{
		if (!read_int(_data.flags, _in)) { return false; }
		return	true;
	}

	return	false;
}



bool serialize(std::ostream& _out, setting_energy const& _data)
{
	if (!write_int(_out, (int16_t)1)		// Version code
	 || !write_int(_out, _data.flags)
	)
	{
		return	false;
	}
	return	true;
}



}
