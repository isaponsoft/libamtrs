/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__filesystem__filesystem_win32__hpp
#define	__libamtrs__filesystem__filesystem_win32__hpp
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wchar.h>
#include "types.hpp"
AMTRS_NAMESPACE_FILESYSTEM_BEGIN


// ============================================================================
//!	@brief	ファイルステータスを取得します。
//!			標準ライブラリの status をジェネリックにしたものです。
// ----------------------------------------------------------------------------
//!	@param	_path
//!			ファイルの状態を調べたいパス。
//!	@param	_error
//!			amtrs::filesystem::error::make([std::error_code&]) で生成される
//!			エラーハンドリングヘルパ。
// ----------------------------------------------------------------------------
//!	http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4100.pdf, 61
// ----------------------------------------------------------------------------
template<class PathTraits, class ErrorHelper>
file_status status(const basic_path<PathTraits>& _path, ErrorHelper _error)
{
	struct _stat64i32	st;
	if (::_wstat(_path.native().data(), &st))
	{
		// ファイルが見つからなかった
		_error	= errno;		// エラーコードをセット
		if (errno == ENOENT)
		{
			return	file_status(file_type::not_found);
		}
		// エラーコードがセットされているなら例外
		_error.throw_if("status()");
		return	file_status(file_type::none);
	}


	// file type.
	file_status		retval;
	retval.type
	(
		(st.st_mode & _S_IFREG) ? file_type::regular   :
		(st.st_mode & _S_IFDIR) ? file_type::directory :
// Unsupported
//		(st.st_mode) ? file_type::symlink   :
//		(st.st_mode) ? file_type::block     :
//		(st.st_mode) ? file_type::character :
//		(st.st_mode) ? file_type::fifo      :
//		(st.st_mode) ? file_type::socket    :
		               file_type::unknown
	);
	return	retval;
}



template<class PathTraits, class ErrorHelper>
basic_path<PathTraits> current_path(ErrorHelper _error)
{
	DWORD					dwSize	= GetCurrentDirectoryW(0, nullptr);
	std::vector<wchar_t>	buff(dwSize);
	if (!GetCurrentDirectoryW(dwSize, buff.data()))
	{
		_error(::GetLastError(), "current_path()");
	}
	return	{buff.data()};
}



template<class PathTraits, class ErrorHelper>
void current_path(const basic_path<PathTraits>& _path, ErrorHelper _error)
{
	if (!SetCurrentDirectoryW(_path.c_str()))
	{
		_error(::GetLastError(), "current_path()");
	}
}

template<class PathTraits, class ErrorHelper>
std::uintmax_t file_size(const basic_path<PathTraits>& _path, ErrorHelper _error)
{
	LARGE_INTEGER		sz;
	HANDLE				handle	= CreateFileW(_path.c_str(), 0, FILE_SHARE_READ|FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (handle == INVALID_HANDLE_VALUE)
	{
		_error(::GetLastError(), "file_size()");
	}
	::GetFileSizeEx(handle, &sz);
	::CloseHandle(handle);
	return	sz.QuadPart;
}

AMTRS_NAMESPACE_FILESYSTEM_END
#endif
