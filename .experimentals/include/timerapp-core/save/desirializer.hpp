/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__desirializer__hpp
#define	__libschedule__save__desirializer__hpp
#include <iostream>
#include <sstream>
#include <amtrs/amtrs.hpp>
#include "fileutil.hpp"
namespace save {

using namespace amtrs;


// バージョン別デシリアライザを作成するためのテンプレ
template<class T, class Desrializer>
bool versionable_deserialize(T& _value, std::istream& _in, Desrializer _proc)
{
	//AMTRS_DEBUG_LOG("%s deserializing", typeid(T).name());
	int16_t		version;
	if (!read_int(version, _in))
	{
		return	false;
	}

	// 1: 成功, 0:失敗, -1:バージョン不明
	auto	r	= _proc(_value, _in, version);
	if (r)
	{
		return	true;
	}
	if (r == -1)
	{
		AMTRS_DEBUG_LOG("%s unkown version = %d", typeid(T).name(), (int)version);
	}
	return	false;
}


template<class T, class Stream, class Desrializer>
bool versionable_deserialize(T& _value, Stream _in, Desrializer _proc)
{
	return	versionable_deserialize<T, Desrializer>(_value, (std::istream&)_in, _proc);
}



// サブチャックデシリアライザのテンプレ
template<class T, class Desrializer>
bool subchunks_desirialize(T& _value, std::istream& _in, Desrializer _proc)
{
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
			//AMTRS_DEBUG_LOG("Chunk terminate");
			break;
		}
		// 1: 成功, 0:失敗, -1:識別子不明
		std::istringstream	in(chunk);
		auto				r	= _proc(_value, (std::istream&)in, identify);
		if (r == 0)
		{
			return	false;
		}
		if (r == -1)
		{
			AMTRS_DEBUG_LOG("%s unkown identify = %s", typeid(T).name(), identify.c_str());
		}
	}
	return	true;
}

template<class T, class Stream, class Desrializer>
bool subchunks_desirialize(T& _value, Stream _in, Desrializer _proc)
{
	return	subchunks_desirialize<T, Desrializer>(_value, (std::istream&)_in, _proc);
}




// Key Value デシリアライザのテンプレ
template<class T, class Desrializer>
bool keyvalue_desirialize(T& _value, std::istream& _in, Desrializer _proc)
{
	for (;;)
	{
		std::string	key;
		if (!read_str(key, _in))
		{
			return	false;
		}

		std::string	value;
		if (!read_str(value, _in))
		{
			return	false;
		}

		// 1: 成功, 0:失敗, -1:識別子不明
		auto	r	= _proc(_value, key, value);
		if (r)
		{
			return	true;
		}
		if (r == -1)
		{
			AMTRS_DEBUG_LOG("%s unkown key = %s", typeid(T).name(), key.c_str());
		}
	}
	return	true;
}

template<class T, class Stream, class Desrializer>
bool keyvalue_desirialize(T& _value, Stream _in, Desrializer _proc)
{
	return	keyvalue_desirialize<T, Desrializer>(_value, (std::istream&)_in, _proc);
}




}
#endif
