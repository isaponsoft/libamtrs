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
#ifndef	__libamtrs__filesystem__functions__hpp
#define	__libamtrs__filesystem__functions__hpp
#include "types.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


inline bool exists(path_type _path, vfs* _loader = nullptr)
{
	ref<vfs>	def;
	if (!_loader)
	{
		def		= fileloader::get_instance();
		_loader	= def.get();
	}
	return	_loader->exists(_path);
}


inline uintmax_t file_size(path_type _path, vfs* _loader = nullptr)
{
	ref<vfs>	def;
	if (!_loader)
	{
		def		= fileloader::get_instance();
		_loader	= def.get();
	}
	return	static_cast<uintmax_t>(_loader->file_size(_path));
}


inline file_status status(path_type _path, vfs* _loader = nullptr)
{
	ref<vfs>	def;
	if (!_loader)
	{
		def		= fileloader::get_instance();
		_loader	= def.get();
	}
	return	_loader->status(_path);
}


inline bool is_directory(path_type _path, vfs* _loader = nullptr)
{
	return	status(_path, _loader).type() == file_type::directory;
}


inline void rename(path_type _old_p, path_type _new_p, std::error_code& _ec)
{
	std::string	o(_old_p);
	std::string	n(_new_p);
	::rename(o.c_str(), n.c_str());
}


inline void rename(path_type _old_p, path_type _new_p)
{
	std::string	o(_old_p);
	std::string	n(_new_p);
	::rename(o.c_str(), n.c_str());
}


inline bool remove(path_type _path, std::error_code& _ec)
{
	std::string	p(_path);
	_ec.clear();
	auto	r	= ::remove(p.c_str());
	return	r == 0;
}


inline bool remove(path_type _path)
{
	std::error_code	e;
	auto			r	= remove(_path, e);
	if (e)
	{
	//	throw	filesystem_error(e.message(), e);
	}
	return	r;
}



AMTRS_FILESYSTEM_NAMESPACE_END
#endif
