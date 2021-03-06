/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN
/*
bool tmpname(amtrs_bufferif_one_init const& _out)
{
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	char	buff[L_tmpnam_s];
	if (tmpnam_s(buff, sizeof(buff)))
	{
		return	false;
	}
	bool	r	= false;
	size_t	len		= ::strlen(buff);
	auto	dest	= _out.allocate(_out.object, len);
	if (dest)
	{
		std::copy_n(reinterpret_cast<char const*>(&buff[0]), len, reinterpret_cast<char*>(dest));
		r	= true;
	}
	return	r;
#else
	char*	p	= tmpnam(nullptr);
	if (!p)
	{
		return	false;
	}
	std::string	s(p);
	free(p);
	auto	dest	= _out.allocate(_out.object, s.size());
	if (dest)
	{
		std::copy_n(reinterpret_cast<char const*>(s.data()), s.size(), reinterpret_cast<char*>(dest));
		return	true;
	}
	return	false;
#endif
}
*/

bool file_get_contents(amtrs_bufferif_one_init _destinate, path_type _path, vfs* _loader)
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
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, fsize));
	if (!data)
	{
		return	false;
	}
	if (fsize > 0)
	{
		auto	in		= _loader->open(_path);
		if (in.good())
		{
			in.read(data, fsize);
		}
	}
	return	true;
}


bool file_put_contents(path_type _path, void const* _data, size_t _size)
{
	std::ofstream	f((std::string)_path, std::ios_base::binary);
	f.write(reinterpret_cast<char const*>(_data), _size);
	f.close();
	return	f.good();
}

AMTRS_FILESYSTEM_NAMESPACE_END
