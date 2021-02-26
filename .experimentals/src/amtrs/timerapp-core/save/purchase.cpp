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
#include <timerapp-core/save/desirializer.hpp>
using namespace	amtrs;
namespace save {




bool purchasedata::is_purchased(std::string const& _product) const noexcept
{
	auto	it	= std::find_if(eternals.begin(), eternals.end(), [&](auto& e)
			{
				return	e.product == _product;
			});
	return	eternals.end() != it;
}



bool deserialize(purchasedata& _data, std::istream& _in)
{
	auto	r	= subchunks_desirialize(_data, _in, [](auto& out, auto& in, auto identify)
	{
		if (identify == "ETERNALS")
		{
			return	deserialize(out.eternals, in) ? 1 : 0;
		}
		return	-1;
	});
	if (!r)
	{
		return	0;
	}
	return	read_checksum32(_in, 0xffccffee);
}



bool serialize(std::ostream& _out, purchasedata const& _data)
{
	// 基本情報
	std::ostringstream	ens;
	if (!serialize(ens, _data.eternals)
	 || !write_chunk(_out, "ETERNALS", ens.str()))
	{
		return	false;
	}

	if (!write_terminate_chunk(_out))
	{
		return	false;
	}

	return	write_checksum32(_out, 0xffccffee);
}


bool deserialize(purchasedata::eternalsdata& _data, std::istream& _in)
{
	auto	r	= versionable_deserialize(_data, _in, [](auto& out, auto& in, auto version)
	{
		if (version == 1)
		{
			uint32_t	count;
			if (!read_int(count, in)) { return 0; }

			for (uint32_t i = 0; i < count; ++i)
			{
				purchasedata::eternal	value;
				if (!deserialize(value, in)) { return 0; }
				out.push_back(std::move(value));
			}
			return	1;
		}
		// 不明なバージョン
		return	-1;
	});
	if (!r)
	{
		return	false;
	}
	return	read_checksum32(_in, 0xffccffee);
}


bool serialize(std::ostream& _out, purchasedata::eternalsdata const& _data)
{
	if (!write_int(_out, (int16_t)1)				// Version code
	 || !write_int(_out, (uint32_t)_data.size())	// Version code
	)
	{
		return	false;
	}
	for (uint32_t i = 0; i < _data.size(); ++i)
	{
		if (!serialize(_out, _data[i]))
		{
			return	false;
		}
	}
	return	true;
}


bool deserialize(purchasedata::eternal& _data, std::istream& _in)
{
	return	versionable_deserialize(_data, _in, [](auto& out, auto& in, auto version)
	{
		if (version == 1)
		{
			// 基本情報
			if (!read_str(out.product, in)
			 || !deserialize(out.date, in)
			)
			{
				return	0;
			}
			return	1;
		}
		// 不明なバージョン
		return	-1;
	});
}


bool serialize(std::ostream& _out, purchasedata::eternal const& _data)
{
	if (!write_int(_out, (int16_t)1)		// Version code
	 || !write_str(_out, _data.product)
	 || !serialize(_out, _data.date)
	)
	{
		return	false;
	}
	return	true;
}



}
