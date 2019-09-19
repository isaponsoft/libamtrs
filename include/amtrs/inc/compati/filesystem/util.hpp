/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__filesystem__util__hpp
#define	__libamtrs__filesystem__util__hpp
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "../string_view.hpp"
#include "types.hpp"
#include "path.hpp"
#include "filesystem_error.hpp"
AMTRS_NAMESPACE_FILESYSTEM_BEGIN
namespace file_get_contents_impl
{
	template<class T, class GetPtr>
	void file_get_contents(T& _destinate, const path& _path, bool _zero_add, GetPtr _getptr)
	{
		auto	fsize	= static_cast<std::size_t>(file_size(_path));
		_destinate.resize(fsize + (_zero_add ? 1 : 0));
		std::ifstream	f(_path, std::ios_base::binary);
		f.read(_getptr(_destinate), fsize);
		f.close();
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
void file_get_contents(T& _destinate, const path& _path, bool _zero_add = true)
{
	file_get_contents_impl::file_get_contents
	(
		_destinate, _path, _zero_add,
		[](T& _data) -> decltype(_data.data())
		{
			return	_data.data();
		}
	);
}

template<class CharT, class TraitsT, class Allocator>
void file_get_contents(std::basic_string<CharT, TraitsT, Allocator>& _destinate, const path& _path, bool _zero_add = true)
{
	file_get_contents_impl::file_get_contents
	(
		_destinate, _path, _zero_add,
		[](std::basic_string<CharT, TraitsT, Allocator>& _data) -> CharT*
		{
			return	const_cast<CharT*>(_data.data());
		}
	);
}


//!	@brief	指定したデータ型のコンテナにファイルの内容を読み込んで返します。
inline std::string file_get_contents(const path& _path)
{
	std::string	ret;
	file_get_contents(ret, _path, true);
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


AMTRS_NAMESPACE_FILESYSTEM_END
#endif
