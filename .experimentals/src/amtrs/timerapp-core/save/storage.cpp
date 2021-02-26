/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <sstream>
#include <iostream>
#include <amtrs/crypto/md5.hpp>
#include <amtrs/filesystem/@>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/memory/endian.hpp>
#include <timerapp-core/save/fileutil.hpp>
#include <timerapp-core/save/storage.hpp>
#include <timerapp-core/save/schedule.hpp>
using namespace	amtrs;
namespace save {


storage::storage()
{
}


storage::storage(std::string _filename)
	: filename(std::move(_filename))
{
	AMTRS_DEBUG_LOG("%s save filename.", filename.c_str());
}


storage::~storage()
{
}



void storage::dump(std::string const& _tag)
{
	for (auto& s : schedules)
	{
		std::string	title	= s->basic.title;
//		title.erase(std::remove_if(title.begin(), title.end(), [](char c){ return c == '\r' ||  c == '\n'; }), title.end());
		AMTRS_DEBUG_LOG("[%s] [%p] [%p]%s:[%p]%s", _tag.c_str(), &s, s->basic.providor.c_str(), s->basic.providor.c_str(), title.c_str(), title.c_str());
	}
}


std::string storage::create_schedule_identify()
{
	union
	{
		int			i;
		uint8_t		u[sizeof(int)];
	}	hs;
	hs.i	= static_cast<int>(schedules.size());

	for (;;)
	{
		uint8_t		h[16];
		amtrs::crypto::md5::compute(h, hs.u);
		char		s[32] = {0};
		for (int i = 0; i < 16; ++i)
		{
			static constexpr char	c[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
			s[i*2 + 0]	= c[(h[i]>>4) & 0x0f];
			s[i*2 + 1]	= c[(h[i]   ) & 0x0f];
		}
		bool	nm	= true;
		for (auto& scd : schedules)
		{
			if (std::string_view((char const*)s, (size_t)32) == scd->basic.identify)
			{
				nm	= false;
				break;
			}
		}
		if (nm)
		{
			return	std::string(std::string_view((char const*)s, (size_t)32));
		}
		++hs.i;
	}
}




bool storage::load()
{
	try
	{
		std::string	fd	= filesystem::file_get_contents<std::string>(filename);
		if (!fd.empty())
		{
			//AMTRS_DEBUG_LOG("Load ok %zd", fd.size());
			std::istringstream	in(std::move(fd));

			for (;;)
			{
				std::string		identify;
				std::string		chunk;
				size_t			len;
				if (!read_chunk(identify, chunk, len, in))
				{
					AMTRS_ERROR_LOG("Chunk error");
					return	false;
				}

				if (identify.empty())
				{
					//AMTRS_DEBUG_LOG("Chunk terminated.");
					break;
				}

				//AMTRS_DEBUG_LOG("Chunk is \"%s\", size=%zd.", identify.c_str(), chunk.size());

				if (identify == "SETTINGS")
				{
					deserialize(setting, std::istringstream(chunk));
					continue;
				}

				if (identify == "SCHEDULE")
				{
					deserialize(schedules, std::istringstream(chunk));
					continue;
				}

				if (identify == "PURCHASE")
				{
					deserialize(purchase, std::istringstream(chunk));
					continue;
				}
			}
			return	true;
		}
	}
	catch (...)
	{}
	AMTRS_DEBUG_LOG("%s load failure.", filename.c_str());
	return	false;
}



bool storage::save()
{
	AMTRS_DEBUG_LOG("%s saving.", filename.c_str());
	try
	{
		auto			tmpfilename	= filename + "_";
		std::ofstream	o(tmpfilename, std::ios::out);
		if (o.fail())
		{
			AMTRS_ERROR_LOG("%s can't cteated.", tmpfilename.c_str());
			return	false;
		}

		std::ostringstream	ss;

		ss	= std::ostringstream();
		serialize(ss, setting);
		write_chunk(o, "SETTINGS", ss.str());

		ss	= std::ostringstream();
		serialize(ss, schedules);
		write_chunk(o, "SCHEDULE", ss.str());

		ss	= std::ostringstream();
		serialize(ss, purchase);
		write_chunk(o, "PURCHASE", ss.str());

		// 終端
		write_chunk(o, "", "");

		o.close();
		if (!o.good())
		{
			AMTRS_ERROR_LOG("%s writing failed.", tmpfilename.c_str());
			return	false;
		}

		// 試験読み込み
		std::string	fd	= filesystem::file_get_contents<std::string>(tmpfilename);
		if (fd.empty())
		{
			AMTRS_ERROR_LOG("%s open test failed.", tmpfilename.c_str());
			return	false;
		}

		AMTRS_DEBUG_LOG("%s (temporary) save success.", tmpfilename.c_str());
	
		if (amtrs::filesystem::exists(filename.c_str()))
		{
			amtrs::filesystem::remove(filename.c_str());
		}
		if (!amtrs::filesystem::rename(tmpfilename.c_str(), filename.c_str()))
		{
			AMTRS_ERROR_LOG("%s rename failed.", filename.c_str());
			return	false;
		}
		fd	= filesystem::file_get_contents<std::string>(filename);
		if (fd.empty())
		{
			AMTRS_ERROR_LOG("%s open test failed.", filename.c_str());
			return	false;
		}
		AMTRS_INFO_LOG("%s save success.", filename.c_str());
		return	true;
	}
	catch (...)
	{}
	AMTRS_ERROR_LOG("%s save failure.", filename.c_str());
	return	false;
}






}
