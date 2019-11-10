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
#ifndef	__libamtrs__filesystem__directory_iterator__posix__hpp
#define	__libamtrs__filesystem__directory_iterator__posix__hpp
#include <memory>
#include <dirent.h>
#include "path.hpp"
#include "directory_entry.hpp"
AMTRS_STD_FILESYSTEM__NAMESPACE_BEGIN

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

AMTRS_STD_FILESYSTEM__NAMESPACE_END
#endif
