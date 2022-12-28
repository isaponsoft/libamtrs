/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <amtrs/filesystem-lar.hpp>
#include <amtrs/optparse.hpp>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <stack>

#define	DEBUGTRACE	1

#include <regex>

using namespace amtrs;
using namespace amtrs::filesystem;

struct	license
{
	char const*	name;
	char const*	text;
	license(char const* _n, char const* _t)
		: name(_n)
		, text(_t)
	{}
};
#include <amtrs/amtrs-license.hpp>


static optinfo	sOptions[] = {
	{ 'h', "help",			opttype::none,		"-h is short help, --help is long help."				},
	{ 'l', "licenses",		opttype::none,		"Licenses print."										},
	{ 'c', "create",		opttype::required,	"larfile"												},
	{ 'a', "append",		opttype::required,	"larfile"												},
	{ 'e', "extract",		opttype::required,	"larfile"												},
	{ 'd', "search-dir",	opttype::required,	"--search-dir path. File search directories."			},
	{ 't', "compress-type",	opttype::required,	"compression type. zlib(default), raw"					},
};

enum class	modetype
{
	none,
	create,
	append,
	extract
};



struct	setting
{
	std::string					name;
	modetype					mode				= modetype::none;
	compresstype				compress			= compresstype::lar_raw;
	bool						createSubdir		= false;
	bool						extractToSourceDir	= false;				// larのあった場所に展開する
	std::string					archiveFile;
	std::vector<std::string>	searchdirs;
	std::vector<std::string>	sources;
};

bool argument(setting& _out, int _argc, char** _args);
int exec_create_append(setting& conf);
int exec_extract(setting& conf);
void usage(std::string _name, bool _long);


int main(int _argc, char** _args)
{
	std::setlocale(LC_ALL, "");
	setting		conf;

#if	DEBUGTRACE
	for (int i = 1; i < _argc; ++i)
	{
		std::cerr << i << ":" << _args[i] << std::endl;
	}
#endif


	if (!argument(conf, _argc, _args))
	{
		return	1;
	}

	// 検索パスが無いならカレントを追加しておく
	if (conf.searchdirs.empty())
	{
		conf.searchdirs.push_back(".");
	}


	try
	{
		int	r	= 0;
		if (conf.mode == modetype::create || conf.mode == modetype::append)
		{
			r	= exec_create_append(conf);
		}
		if (conf.mode == modetype::extract)
		{
			r	= exec_extract(conf);
		}
		std::cout << "OK" << std::endl;
		return	r;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return	1;
	}
}

int exec_one(filesystem::lar& ar, std::string basedir, std::string path)
{
	std::string	filename	= std::string(filesystem::trim_first_separator(std::string_view(path).substr(basedir.size())));
	auto		fsize		= filesystem::file_size(path);
	auto		ae			= ar.add_entry(filename);
	ae->compress_size	= fsize;
	ae->original_size	= fsize;
	ae->data_alloc();
	io::fdesc	f(path.c_str(), io::openmode::open, io::accessmode::read_only);
	if (!f.good())
	{
		std::cerr << "Can't open source \"" << filename << "\"." << std::endl;
		return	1;
	}
	f.read(ae->data(), fsize).gcount();
	return	0;
}


int exec_create_append(setting& conf)
{
	// 作成／追記モード
	std::cerr << "Output target : " << conf.archiveFile << std::endl;

	filesystem::lar	ar;

	if (conf.mode == modetype::append)
	{
		auto	fd	= io::fdesc(conf.archiveFile.c_str(), io::openmode::open, io::accessmode::read_only);
		if (!fd.good())
		{
			std::cerr << "Can't open arhive \"" << conf.archiveFile << "\"." << std::endl;
			return	1;
		}
		auto	i	= io::make_read_seekable(std::move(fd));
		if (!io::read(ar, i))
		{
			std::cerr << "Read error arhive \"" << conf.archiveFile << "\"." << std::endl;
			return	1;
		}
	}

	for (auto& dir : conf.searchdirs)
	{
		for (auto& src : conf.sources)
		{
			for (auto& e : findfiles(filesystem::normalize_path(dir + "/" + src)))
			{
				std::string	path		= e.path().string();
				if (filesystem::is_directory(path))
				{
					for (auto& e : std::filesystem::recursive_directory_iterator(path))
					{
						if (e.is_regular_file())
						{
							if (exec_one(ar, dir, e.path().string()))
							{
								return	1;
							}
						}
					}
				}
				else
				{
					if (exec_one(ar, dir, path))
					{
						return	1;
					}
				}
			}
		}
	}

	for (auto& e : ar)
	{
		std::cerr << e.name() << std::endl;
		e.compress_to(lar_deflate);
//		std::cerr << std::fixed << std::setprecision(2) << (double(e.compress_size) * 100 / double(e.original_size))  << "%" << std::endl;
	}

	io::write(io::fdesc(conf.archiveFile.c_str(), io::openmode::create_always, io::accessmode::write_only), ar);

	return	0;
}


int exec_extract(setting& conf)
{
	for (auto& archive : conf.sources)
	{
		filesystem::lar	ar;
		auto	fd		= io::fdesc(archive.c_str(), io::openmode::open, io::accessmode::read_only);
		if (!fd.good())
		{
			std::cerr << "Can't open " << archive << std::endl;
			return	1;
		}
		if (!io::read(ar, std::move(fd)))
		{
			std::cerr << "Read error " << archive << std::endl;
			return	1;
		}

		std::string	fileprefix;
		std::string	subdir;
		if (conf.extractToSourceDir)
		{
			fileprefix	= std::string(parent_path(archive)) + "/";
		}
		if (conf.createSubdir)
		{
			subdir		= std::string(stem(archive)) + "/";

			// subdir が二重に出来る場合はサブディレクトリを作らないようにする
			bool	unmatch	= false;
			for (auto& e : ar)
			{
				if (!starts_with(e.name(), subdir))
				{
					unmatch	= true;
					break;
				}
			}
			if (!unmatch)
			{
				fileprefix	= fileprefix + subdir;
			}
		}


		for (auto& e : ar)
		{
			std::string	name	= fileprefix + std::string(e.name());
			std::cerr << name << " ... " << e.original_size << std::endl;
			std::string	dir		= std::string(filesystem::parent_path(name));
			if (!dir.empty())
			{
				std::filesystem::create_directories(dir);
			}
			auto	fd	= io::fdesc(name.c_str(), io::openmode::create_always, io::accessmode::write_only);
			if (!fd.good())
			{
				std::cerr << "Can't create " << e.name() << std::endl;
				return	1;
			}
			e.compress_to(lar_raw);
			fd.write(e.data(), e.compress_size);
		}
	}
	return	0;
}


bool argument(setting& _out, int _argc, char** _args)
{
	_out.name		= _args[0];
	--_argc;
	++_args;

	optstate	o;
	while (optparse(o, _args, _argc, sOptions))
	{
		switch (o.shortval)
		{
			case (int)opterr::none_argumrnt :
				std::cerr << o.opt << " has not argument." << std::endl;
				return	1;

			case 'h' :
			{
				usage(_out.name, o.longmode);
				return	1;
			}

			case 'a' :
			{
				_out.mode			= modetype::append;
				_out.archiveFile	= std::string(o.arg);
				break;
			}

			case 'c' :
			{
				_out.mode			= modetype::create;
				_out.archiveFile	= std::string(o.arg);
				break;
			}

			case 'e' :
			{
				_out.mode			= modetype::extract;
				_out.sources.push_back(std::string(o.arg));
				break;
			}

			case 'd' :
			{
				_out.searchdirs.push_back(o.arg);
				break;
			}

			case 't' :
			{
				if (o.arg == "zlib")
				{
					_out.compress	= compresstype::lar_deflate;
				}
				else if (o.arg == "raw")
				{
					_out.compress	= compresstype::lar_raw;
				}
				else
				{
					_out.compress	= compresstype::lar_deflate;
				}
				break;
			}

			default:
				return	1;
		}
	}


	std::string_view	opt;
	if (!readopt(opt, _argc, _args))
	{
		if (_out.mode == modetype::none || _out.mode == modetype::append || _out.mode == modetype::create)
		{
			usage(_out.name, true);
			return	false;
		}
	}

	_out.sources.push_back(std::string(opt));
	while (readopt(opt, _argc, _args))
	{
		_out.sources.push_back(std::string(opt));
	}

	// 全てのワイルドカードを展開
	std::vector<std::string>	sources;
	if (_out.searchdirs.empty())
	{
		// 検索ディレクトリの指定が無いのでカレントディレクトリから検索
		for (auto& src : _out.sources)
		{
			for (auto& e : findfiles(filesystem::normalize_path(src)))
			{
				sources.push_back(e.path().string());
			}
		}
	}
	else
	{
		// 検索ディレクトリの指定があるので指定ディレクトリから検索
		for (auto& dir : _out.searchdirs)
		{
			for (auto& src : _out.sources)
			{
				for (auto& e : findfiles(filesystem::normalize_path(dir + "/" + src)))
				{
					sources.push_back(e.path().string());
				}
			}
		}
	}
	_out.sources	= std::move(sources);

	// 自動モード選択
	if (_out.mode == modetype::none)
	{
		// 全てのファイルが .lar なら展開モード
		bool	no_lar	= false;
		for (auto& src : _out.sources)
		{
			if (filesystem::extension(src) != ".lar")
			{
				no_lar	= true;
				break;
			}
		}

		if (no_lar)
		{
			// 圧縮モード
			_out.mode				= modetype::create;

			// 出力ファイル名を計算する
			std::string	retval;
			auto		pathsize	= common_parent_path(_out.sources.begin(), _out.sources.end());
			if (pathsize > 0)
			{
				_out.archiveFile	= std::string(trim_root_path(stem(trim_last_separator(std::string_view(_out.sources.front()).substr(0, pathsize)))));
			}
			if (_out.archiveFile.empty())
			{
				_out.archiveFile	= std::string(stem(_out.sources.front()));
			}
			_out.archiveFile	+= ".lar";
		}
		else
		{
			// lar展開モード
			_out.mode				= modetype::extract;
			_out.createSubdir		= true;
			_out.extractToSourceDir	= true;			
		}
	}
	return	true;
}





void usage(std::string _name, bool _long)
{
	if (_long)
			std::cout << usage_long (sOptions, _name, "filename...", "Lite ARchive.") << std::endl;
	else	std::cout << usage_short(sOptions, _name, "[filename...]") << std::endl;
}
