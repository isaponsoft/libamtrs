/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <cwchar>
#include <locale>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/memory/hash_map.hpp>
#include <amtrs/string/format.hpp>
#include <amtrs/string/sourcecode.hpp>
#include <amtrs/string/split_iterator.hpp>
#include <amtrs/string/trnstext.hpp>
#include <amtrs/system/env.hpp>
using namespace amtrs::sourcecode;
AMTRS_NAMESPACE_BEGIN namespace trnstext {
namespace {
	using	maptype	= 	hash_map<sstring, sstring>;

	hash_map<sstring, maptype>	sTranslate;
}



bool add(text _transdata)
{
	return	add("", _transdata);
}

text get(text _txt)
{
	return	get("", _txt);
}

bool set(text _transdata)
{
	return	set("", _transdata);
}

bool set_from_file(text _path, text _filename)
{
	return	set_from_file("", _path, _filename);
}



text get(text _group, text _txt)
{
	text	tx(_txt);
	auto	it	= sTranslate[_group].find(tx);
	if (it == sTranslate[_group].end())
	{
		return	_txt;
	}
	return	it->second;
}


bool add(text _group, text _transdata)
{
	sstring	tag;
	for (text line : split_chars(_transdata, "\n"))
	{
		line	= line.trim("\r\n\t ");
		if (line.empty()){  continue; }
		if (line.front_at() == '@')
		{
					tag	= decode_escape_string(line.substr(1).trim("\r\n\t ").trim("\""));
			continue;
		}
		if (line.front_at() == '+')
		{
			sstring	val	= decode_escape_string(line.substr(1).trim("\r\n\t ").trim("\""));
			sTranslate[_group].emplace(std::move(tag), std::move(val));
			continue;
		}
	}

	return	true;
}


bool set_from_file(text _group, text _path, text _filename)
{
	struct	_
	{
		static bool load(text _lang, text _path, text _filename)
		{
			sstring	filename	= filesystem::normalize_path<sstring>(format("{}/{}/{}", {_path, _lang, _filename}));
			sstring	file		= filesystem::file_get_contents<sstring>(filename);
			if (file.empty())
			{
				return	false;
			}
			return	set((text)file);
		}
	};
	auto	lang3282	= system::get_locale_rfc3282();
	if (_::load(lang3282, _path, _filename))
	{
		return	true;
	}
	if (_::load("en-US", _path, _filename))
	{
		return	true;
	}
	return	false;
}


bool set(text _group, text _transdata)
{
	sTranslate[_group].clear();
	return	add(_transdata);
}


} AMTRS_NAMESPACE_END
