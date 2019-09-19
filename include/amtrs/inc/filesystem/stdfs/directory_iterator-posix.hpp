/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__filesystem__directory_iterator__posix__hpp
#define	__libamtrs__filesystem__directory_iterator__posix__hpp
#include <memory>
#include <dirent.h>
#include "path.hpp"
#include "directory_entry.hpp"
AMTRS_FILESYSTEM_STDFS_NAMESPACE_BEGIN

template<class DirEntryT>
class	basic_directory_iterator<DirEntryT>::instance
{
public:
	instance(const path_type& _path)
		: m_path(_path)
		, m_dir(nullptr)
		, m_entry(nullptr)
	{}

	~instance()
	{
		close();
	}

	void close()
	{
		if (m_dir)
		{
			::closedir(m_dir);
			m_dir = nullptr;
		}
	}

	template<class ErrorCode>
	bool open(ErrorCode _error)
	{
		m_dir	= opendir(m_path.native().c_str());
		if (!m_dir)
		{
			_error(errno, "directory_iterator()");
			return	false;
		}
		return	next();
	}

	bool next()
	{
		do
		{
			m_entry = readdir(m_dir);
			if (m_entry == nullptr)
			{
				close();
				return	false;
			}
		} while (!strcmp(m_entry->d_name, ".") || !strcmp(m_entry->d_name, ".."));
		return	true;
	}


	void set(value_type& _entry)
	{
		if (!m_entry)
		{
			_entry.assign(path_type());
			return;
		}
		auto	filepath	= m_path.native();
		filepath	+= "/";
		filepath	+= m_entry->d_name;
		_entry.assign(filepath);
	}

private:
	path_type			m_path;
	DIR*				m_dir;
	struct dirent*		m_entry;
};

AMTRS_FILESYSTEM_STDFS_NAMESPACE_END
#endif
