/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__util__hpp
#define	__libamtrs__filesystem__util__hpp
#include <exception>
#include "../memory/bufferif.h"
#include "../string/text.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

/*
 * ファイルを読み取ります。
 */
AMTRS_API errorval file_get_contents(amtrs_bufferif_one_init _destinate, text _path);


/*
 * ファイルに書き込みます。
 */
AMTRS_API errorval file_put_contents(text _path, void const* _data, size_t _size);


template<class T>
errorval file_get_contents(T& _destinate, text _path) noexcept
{
	return	file_get_contents(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path);
}


template<class T>
T file_get_contents(text _path) noexcept
{
	T			dest;
	errorval	r	= file_get_contents(dest, _path);
	return		dest;
}

// ******************************************************************
// Read file data.
// ------------------------------------------------------------------
//	_use_except
//		true:	If not read a file throw exception.
//		false:	If not read a file return empty.
template<class T>
T read_file(text _path, bool _use_except = true)
{
	T			r;
	errorval	e	= file_get_contents<T>(r, _path);
	if (!e.good() && _use_except)
	{
		throw	std::exception();
	}
	return	r;
}

AMTRS_FILESYSTEM_NAMESPACE_END
#endif
