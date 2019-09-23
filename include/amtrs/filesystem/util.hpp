/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__filesystem__util__hpp
#define	__libamtrs__filesystem__util__hpp
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "types.hpp"
#include "path.hpp"
#include "fileloader.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN
namespace file_get_contents_impl
{
	template<class T, class GetPtr>
	void file_get_contents(T& _destinate, const path& _path, bool _zero_add, GetPtr _getptr, vfs* _loader)
	{
		ref<vfs>	def;
		if (!_loader)
		{
			AMTRS_TRACE_LOG("use default filesystem");
			def		= fileloader::get_instance();
			_loader	= def.get();
		}

		auto	fsize	= static_cast<std::size_t>(_loader->file_size(_path));
		AMTRS_TRACE_LOG("%s is %zdbytes", _path.string().c_str(), (size_t)fsize);
		_destinate.resize(fsize + (_zero_add ? 1 : 0));
		auto	in		= _loader->open(_path);
		in.read(_getptr(_destinate), fsize);
		if (_zero_add)
		{
			_destinate[fsize]	= 0;
			_destinate.resize(fsize);
		}
	}
}


//!	@brief	指定したデータ型のコンテナにファイルの内容を読み込んで返します。
//!	_destinate で指定したバッファは resize() され、先頭から書き込まれます。
template<class T>
void file_get_contents(T& _destinate, const path& _path, bool _zero_add = true, vfs* _loader = nullptr)
{
	file_get_contents_impl::file_get_contents
	(
		_destinate, _path, _zero_add,
		[](T& _data) -> decltype(_data.data())
		{
			return	_data.data();
		}, _loader
	);
}

template<class CharT, class TraitsT, class Allocator>
void file_get_contents(std::basic_string<CharT, TraitsT, Allocator>& _destinate, const path& _path, bool _zero_add = true, vfs* _loader = nullptr)
{
	file_get_contents_impl::file_get_contents
	(
		_destinate, _path, _zero_add,
		[](std::basic_string<CharT, TraitsT, Allocator>& _data) -> CharT*
		{
			return	const_cast<CharT*>(_data.data());
		}, _loader
	);
}


//!	@brief	指定したデータ型のコンテナにファイルの内容を読み込んで返します。
inline std::string file_get_contents(const path& _path, vfs* _loader = nullptr)
{
	std::string	ret;
	file_get_contents(ret, _path, true, _loader);
	return	ret;
}


//!	@brief	_source は data() と size() を持っている必要があります。
template<class T>
void file_put_contents(const path& _path, const T& _source)
{
	std::ofstream	f(_path, std::ios_base::binary);
	f.write(_source.data(), _source.size());
	f.close();
}


//!	@brief	_source は data() と size() を持っている必要があります。
inline void file_put_contents(const path& _path, std::string_view _source)
{
	std::ofstream	f(_path, std::ios_base::binary);
	f.write(_source.data(), _source.size());
	f.close();
}


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
