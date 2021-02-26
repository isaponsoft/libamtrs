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

bool __deserialize(settingdata& _data, std::istream& _in)
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


bool __serialize(std::ostream& _out, settingdata const& _data)
{
	if (!write_int(_out, (int16_t)1)		// Version code
	 || !write_int(_out, _data.flags)
	)
	{
		return	false;
	}
	return	true;
}



bool deserialize(settingdata& _data, std::istream& _in)
{
	AMTRS_DEBUG_LOG("Load settingdata");
	for (;;)
	{
		std::string			identify;
		std::string			chunk;
		size_t				len;
		if (!read_chunk(identify, chunk, len, _in))
		{
			AMTRS_DEBUG_LOG("Chunk error");
			return	false;
		}
		if (identify.empty())
		{
			break;
		}
		if (identify == "CONFIGS")
		{
			std::istringstream	i(chunk);
			__deserialize(_data, i);
			continue;
		}
		if (identify == "LOOKS")
		{
			deserialize(_data.looks, std::istringstream(chunk));
			continue;
		}
		if (identify == "SOUNDS")
		{
			deserialize(_data.sound, std::istringstream(chunk));
			continue;
		}
		if (identify == "EVENT")
		{
			deserialize(_data.event, std::istringstream(chunk));
			continue;
		}
	}
	return	true;
}



bool serialize(std::ostream& _out, settingdata const& _data)
{
	std::ostringstream	ss;

	ss	= std::ostringstream();
	__serialize(ss, _data);
	write_chunk(_out, "CONFIGS", ss.str());

	ss	= std::ostringstream();
	serialize(ss, _data.looks);
	write_chunk(_out, "LOOKS", ss.str());

	ss	= std::ostringstream();
	serialize(ss, _data.sound);
	write_chunk(_out, "SOUNDS", ss.str());

	ss	= std::ostringstream();
	serialize(ss, _data.event);
	write_chunk(_out, "EVENT", ss.str());

	write_chunk(_out, "", "");

	return	true;
}


}
