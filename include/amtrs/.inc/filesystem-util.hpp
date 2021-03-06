/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__util__hpp
#define	__libamtrs__filesystem__util__hpp
#include "memory-shared_buffer.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


bool file_get_contents(amtrs_bufferif_one_init _destinate, path_type _path, vfs* _loader = nullptr);


bool file_put_contents(path_type _path, void const* _data, size_t _size);


/*
 * 
 * 転送先の型に合わせてamtrs_bufferif_one_initを生成します。
 */
template<class T>
bool file_get_contents(T& _destinate, path_type _path, vfs* _loader = nullptr)
{
	return	amtrs::filesystem::file_get_contents(amtrs_bufferif_one_init
	{
		.object		= &_destinate,
		.allocate	= [](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path);
}



//!	@brief	指定したデータ型のコンテナにファイルの内容を読み込んで返します。
template<class T>
inline T file_get_contents(path_type _path, vfs* _loader = nullptr)
{
	T	ret;
	file_get_contents(ret, _path, _loader);
	return	ret;
}




//!	@brief	_source は data() と size() を持っている必要があります。
template<class T>
bool file_put_contents(path_type _path, T const& _source)
{
	return	file_put_contents(_path, _source.data(), _source.size());
}



AMTRS_FILESYSTEM_NAMESPACE_END
#endif
