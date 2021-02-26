/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__functions__hpp
#define	__libamtrs__filesystem__functions__hpp
AMTRS_FILESYSTEM_NAMESPACE_BEGIN



inline bool exists(path_type _path, vfs* _loader = nullptr)
{
	ref<fileloader>	def;
	if (!_loader)
	{
		def		= fileloader::get_instance();
		def->push_back(stdvfs::get_instance());
		_loader	= def.get();
	}
	return	_loader->exists(_path);
}


inline uintmax_t file_size(path_type _path, vfs* _loader = nullptr)
{
	ref<fileloader>	def;
	if (!_loader)
	{
		def		= fileloader::get_instance();
		def->push_back(stdvfs::get_instance());
		_loader	= def.get();
	}
	return	static_cast<uintmax_t>(_loader->file_size(_path));
}


inline file_status status(path_type _path, vfs* _loader = nullptr)
{
	ref<fileloader>	def;
	if (!_loader)
	{
		def		= fileloader::get_instance();
		def->push_back(stdvfs::get_instance());
		_loader	= def.get();
	}
	return	_loader->status(_path);
}


inline bool is_regular_file(path_type _path, vfs* _loader = nullptr)
{
	return	status(_path, _loader).type() == file_type::regular;
}


inline bool is_directory(path_type _path, vfs* _loader = nullptr)
{
	return	status(_path, _loader).type() == file_type::directory;
}


inline bool rename(path_type _old_p, path_type _new_p, std::error_code& _ec)
{
	std::string	o(_old_p);
	std::string	n(_new_p);
	auto	r	= ::rename(o.c_str(), n.c_str());
	return	r == 0;
}


inline bool rename(path_type _old_p, path_type _new_p)
{
	std::string	o(_old_p);
	std::string	n(_new_p);
	auto	r	= ::rename(o.c_str(), n.c_str());
	return	r == 0;
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
#include AMTRS_DRIVER_INCLUDE(filesystem-functions.hpp)
#endif
