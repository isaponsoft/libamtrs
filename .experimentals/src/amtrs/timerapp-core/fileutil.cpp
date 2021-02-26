/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <string_view>
#include <amtrs/filesystem/@>
#include <amtrs/memory/endian.hpp>
#include <timerapp-core/save/fileutil.hpp>


bool writefile(std::string const& _filename, std::string const& _data)
{
	try
	{
		auto			tmpfilename	= _filename + "_";
		std::ofstream	o;
		o.open(tmpfilename);
		if (o.is_open())
		{
			o.write(_data.data(), _data.size());
			o.close();

			//AMTRS_DEBUG_LOG("%s (temporary) save success.", tmpfilename.c_str());
			if (amtrs::filesystem::exists(_filename.c_str()))
			{
				amtrs::filesystem::remove(_filename.c_str());
			}
			//AMTRS_DEBUG_LOG("%s to %s save success.", tmpfilename.c_str(), _filename.c_str());
			amtrs::filesystem::rename(tmpfilename.c_str(), _filename.c_str());
			AMTRS_INFO_LOG("%s save success.", _filename.c_str());
			return	true;
		}
		AMTRS_DEBUG_LOG("%s save failure.", _filename.c_str());
	}
	catch (std::exception& _e)
	{
		AMTRS_ERROR_LOG("%s save : %s.", _filename.c_str(), _e.what());
	}
	catch (...)
	{
		AMTRS_CRITICAL_LOG("%s save unkown exception.", _filename.c_str());
	}
	return	false;
}


bool readfile(std::string& _data, std::string const& _filename)
{
	try
	{
		_data	= amtrs::filesystem::file_get_contents<std::string>(_filename);
		if (!_data.empty())
		{
			return	true;
		}
	}
	catch (...)
	{}
	return	false;
}

