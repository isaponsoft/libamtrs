/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__directory_iterator__win32__hpp
#define	__libamtrs__directory_iterator__win32__hpp
#include <windows.h>
#include <memory>
#include "path.hpp"
#include "directory_entry.hpp"
AMTRS_STD_FILESYSTEM__NAMESPACE_BEGIN

template<class DirEntryT>
class	basic_directory_iterator<DirEntryT>::instance
{
public:
	instance(const path_type& _path)
		: m_findHandle(INVALID_HANDLE_VALUE)
		, m_path(_path)
	{}

	~instance()
	{
		close();
	}

	void close()
	{
		if (m_findHandle != INVALID_HANDLE_VALUE)
		{
			FindClose(m_findHandle);
			m_findHandle	= INVALID_HANDLE_VALUE;
		}
	}

	template<class ErrorCode>
	bool open(ErrorCode _ec)
	{
		auto	findpath	= m_path.native() + L"\\*";
		m_findHandle	= FindFirstFileW(findpath.data(), &m_findData);
		if (m_findHandle == INVALID_HANDLE_VALUE)
		{
			_ec(::GetLastError(), "amtrs::filesystem::directory_iterator", m_path);
			return	false;
		}
		while (!std::wcscmp(m_findData.cFileName, L".") || !std::wcscmp(m_findData.cFileName, L".."))
		{
			if (!FindNextFileW(m_findHandle, &m_findData))
			{
				close();
				return	false;
			}
		}
		return	true;
	}

	bool next()
	{
		if (m_findHandle != INVALID_HANDLE_VALUE)
		{
			if (FindNextFileW(m_findHandle, &m_findData))
			{
				return	true;
			}
			close();
		}
		return	false;
	}

	void set(value_type& _entry)
	{
		if (m_findHandle == INVALID_HANDLE_VALUE)
		{
			_entry.assign(path_type());
			return;
		}
		auto	filepath	= m_path.native();
		filepath	+= L"\\";
		filepath	+= m_findData.cFileName;
		_entry.assign(filepath);
	}

private:
	path_type			m_path;
	WIN32_FIND_DATAW	m_findData;
	HANDLE				m_findHandle;
};


AMTRS_STD_FILESYSTEM__NAMESPACE_END
#endif
