/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__util__hpp
#define	__libamtrs__filesystem__util__hpp
#include "memory-shared_buffer.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN
namespace file_get_contents_impl
{
	template<class T, class GetPtr>
	bool file_get_contents(T& _destinate, path_type _path, bool _zero_add, GetPtr _getptr, vfs* _loader)
	{
		ref<fileloader>	def;
		if (!_loader)
		{
			def		= fileloader::get_instance();
			def->push_back(stdvfs::get_instance());
			_loader	= def.get();
		}

		auto	fsize	= static_cast<std::size_t>(_loader->file_size(_path));
		AMTRS_SYSTRACE_LOG("%s is %zdbytes", _path.data(), (size_t)fsize);
		if (fsize == -1)
		{
			return	false;
		}
		_destinate.resize(fsize + (_zero_add ? 1 : 0));
		if (fsize > 0)
		{
			auto	in		= _loader->open(_path);
			if (in.good())
			{
				in.read(_getptr(_destinate), fsize);
			}
			else
			{
				_destinate	= {};
			}
		}
		if (_zero_add)
		{
			_destinate[fsize]	= 0;
			_destinate.resize(fsize);
		}
		return	true;
	}

	template<class D, class GetPtr>
	bool file_get_contents(shared_buffer<void, D>& _destinate, path_type _path, bool _zero_add, GetPtr _getptr, vfs* _loader)
	{
		ref<fileloader>	def;
		if (!_loader)
		{
			def		= fileloader::get_instance();
			def->push_back(stdvfs::get_instance());
			_loader	= def.get();
		}

		auto	fsize	= static_cast<std::size_t>(_loader->file_size(_path));
		AMTRS_SYSTRACE_LOG("%s is %zdbytes", _path.data(), (size_t)fsize);
		if (fsize == -1)
		{
			return	false;
		}
		_destinate	= shared_buffer<void, D>(fsize);
		if (fsize > 0)
		{
			auto	in		= _loader->open(_path);
			in.read(reinterpret_cast<char*>(_getptr(_destinate)), fsize);
		}
		return	true;
	}
}


//!	@brief	指定したデータ型のコンテナにファイルの内容を読み込んで返します。
//!	_destinate で指定したバッファは resize() され、先頭から書き込まれます。
template<class T>
bool file_get_contents(T& _destinate, path_type _path, bool _zero_add = true, vfs* _loader = nullptr)
{
	return	file_get_contents_impl::file_get_contents
	(
		_destinate, _path, _zero_add,
		[](T& _data) -> decltype(_data.data())
		{
			return	_data.data();
		}, _loader
	);
}


//!	@brief	指定したデータ型のコンテナにファイルの内容を読み込んで返します。
template<class T>
inline T file_get_contents(path_type _path, vfs* _loader = nullptr)
{
	T	ret;
	file_get_contents(ret, _path, true, _loader);
	return	ret;
}


//!	@brief	_source は data() と size() を持っている必要があります。
template<class T>
void file_put_contents(path_type _path, const T& _source)
{
	std::ofstream	f((std::string)_path, std::ios_base::binary);
	f.write(_source.data(), _source.size());
	f.close();
}


//!	@brief	_source は data() と size() を持っている必要があります。
inline void file_put_contents(path_type _path, std::string_view _source)
{
	std::ofstream	f((std::string)_path, std::ios_base::binary);
	f.write(_source.data(), _source.size());
	f.close();
}


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
