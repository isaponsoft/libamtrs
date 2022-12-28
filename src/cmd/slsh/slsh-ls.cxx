/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/console/escape.hpp>
#include <amtrs/filesystem/enumfiles.hpp>
PSNVM_SLSH_NAMESPACE_BEGIN

int f_ls(sstring& e)
{
	using namespace amtrs::console;
	sstring	path;
	if (e.empty())
	{
		path	= filesystem::current_path<sstring>();
	}
	else
	{
		path	= e;
	}

	simplearray<sstring>	fl;
	size_t					maxlen	= 0;
	filesystem::enumfiles(path, {}, [&](filesystem::enumentry& stat) -> bool
	{
		if (stat.name == "." || stat.name == "..")
		{
			return	true;
		}
		bool	isDir	= stat.type & filesystem::enumentry::fst_directory ? true : false;
		if (stat.type & filesystem::enumentry::fst_symlink)
		{
			if (amtrs::filesystem::is_directory(amtrs::filesystem::readlink<sstring>(path+"/"+stat.name)))
			{
				isDir	= true;
			}
		}
		if (isDir)
		{
			stat.name.append("/");
		}
		fl.push_back(stat.name);
		if (maxlen < stat.name.size())
		{
			maxlen	= stat.name.size();
		}
		return	true;
	});

	auto	winsz		= curent_editor()->size();
	size_t	xpos	= 0;
	constexpr int	padd	= 2;
	for (auto& f : fl)
	{
		auto	len	= f.size();
		if (xpos + maxlen + padd >= winsz.width)
		{
			xpos	= 0;
			fwrite(new_line, 1, sizeof(new_line), stdout);
		}
		if (xpos) { io::fdesc::stdo() << "  "; }
		io::fdesc::stdo() << f;
		for (size_t dif = maxlen - len; dif > 0; --dif)
		{
			io::fdesc::stdo() << " ";
		}
		xpos	+= maxlen + padd;
	}
	io::fdesc::stdo() << new_line;
	return	0;
}

PSNVM_SLSH_NAMESPACE_END
