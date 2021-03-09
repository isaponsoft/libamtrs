/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <amtrs/archive.hpp>
#include <amtrs/console.hpp>
#include <amtrs/io.hpp>
#include <amtrs/net.hpp>
#include <amtrs/system.hpp>
#include <amtrs/scriptutil.hpp>
#include <thread>
AMTRS_SCRIPTUTIL_NAMESPACE_BEGIN



std::string array_shift(std::deque<std::string>& _array)
{
	std::string	c;
	if (!_array.empty())
	{
		c	= _array.front();
		_array.pop_front();
	}
	return	c;
}


update_result update(std::string_view _targetfile, std::string_view _text, bool _noupdate)
{
	if (filesystem::exists(_targetfile))
	{
		auto	now	= amtrs::filesystem::file_get_contents<std::string>(_targetfile);
		if (now == _text)
		{
			return	update_skip;
		}
	}
	if (_noupdate)
	{
		return	update_modify;
	}

	if (auto dir = amtrs::filesystem::parent_path(_targetfile); !dir.empty())
	{
		std::filesystem::create_directories(dir);
	}
	return	amtrs::filesystem::file_put_contents(_targetfile, _text)
			? update_modify
			: update_err;
}


std::string add_exe(std::string_view _filename)
{
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	return	std::string(_filename) + ".exe";
#else
	return	std::string(_filename);
#endif
}


std::string add_bat(std::string_view _filename)
{
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	return	std::string(_filename) + ".bat";
#else
	return	std::string(_filename) + ".sh";
#endif
}


std::string slashdir(std::string_view _filename)
{
	return	regex_replace_callback<std::string>(std::string(_filename), R"(\\)", [&](auto&)
	{
		return	"/";
	});
}


std::string backslashdir(std::string_view _filename, bool _escape)
{
	return	regex_replace_callback<std::string>(std::string(_filename), "/", [&](auto&)
	{
		return	_escape ? "\\\\" : "\\";
	});
}



bool enumenvs(std::vector<std::pair<std::string, std::string>>& _dest, std::string const& _envscript, std::string const& _workfile)
{
	std::vector<std::pair<std::string, std::string>>	envs;
char const	s[] = {
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
R"(@ECHO OFF
CALL ##SCRIPT##
SET
)"
#else
R"(#!env sh
source ##SCRIPT##
env
)"
#endif
};
	std::string	envset(s);
	envset	= regex_replace_callback(envset, "##SCRIPT##", [&](auto&)
	{
		return	std::string(_envscript);
	});
	ssu::update(_workfile, envset);
	std::string	out;
	std::string	err;
	std::string	cmd;
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	cmd	= _workfile;
#else
	cmd	= std::string("env sh ") + _workfile;
#endif
	ssu::exec(out, err, cmd.c_str());
	for (auto line : split_chars(out, "\n"))
	{
		line	= trim(line, "\r\n\\t ");

		std::string_view	key;
		std::string_view	value;
		auto	r = wcmatch(std::string_view(line), std::string_view("(+)=(+)"), [&](auto& m)
		{
			if (m.index == 1)
			{
				key		= m.match;
			}
			if (m.index == 2)
			{
				value	= m.match;
			}
		});
		if (r)
		{
			_dest.push_back({std::string(key), std::string(value)});
		}
	}
	return	true;
}


int exec(std::string& _out, std::string& _err, std::string const& _command)
{
	int		r	= -1;
	auto	progname	= _command;
	if (auto f = _command.find_first_of(" "); f != std::string::npos)
	{
		progname	= progname.substr(0, f);
	}
	progname	= std::string(filesystem::filename(progname));
	console::progress_start(console::progresstype::subprocess, 0, 0, progname.c_str());
	std::thread			process;
	process	= std::thread([&]()
	{
		try
		{
			r	= system::exec(_command.c_str(), [&](auto type, auto* data, auto* size) -> int
			{
				if (!*data)
				{
					std::string&	str(type ==  system::streamtype::cout ? _out : _err);
					auto	s	= str.size();
					str.resize(s + *size);
					*data	= str.data() + s;
				}
				return	0;
			});
		}
		catch (...)
		{
		}
		process.detach();
	});

	while (process.joinable())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	console::progress_end();
	return	r;
}


int exec(std::string& _out, std::string const& _command)
{
	int	r	= -1;
	console::progress_start(console::progresstype::subprocess, 0, 0, _command.c_str());
	try
	{
		r	= system::exec(_command.c_str(), [&](auto type, auto* data, auto* size) -> int
		{
			console::progress_inc();
			if (!*data)
			{
				if (type == system::streamtype::cout)
				{
					auto	s	= _out.size();
					_out.resize(s + *size);
					*data	= _out.data() + s;
				}
				if (type ==  system::streamtype::cerr)
				{
					std::cerr << std::string_view((char*)*data, *size);
				}
			}
			return	0;
		});
	}
	catch (...)
	{
	}
	console::progress_end();
	return	-1;
}


int exec(std::string const& _command)
{
	try
	{
		return	system::exec(_command.c_str());
	}
	catch (...)
	{
		return	-1;
	}
}


bool download(std::string const& _url, std::string const& _savename, std::string* _savedname)
{
	using namespace amtrs;
	using namespace amtrs::net;

	std::string	url	= _url;
	for (;;)
	{
		mini_http					m(url);
		http::http_header_analyzer	hha;


		std::string		fname	= _savename;
		if (_savename.empty() || filesystem::is_directory(_savename))
		{
			fname = std::string(filesystem::filename((std::string_view)m.uri_info().path));
			if (fname == "/")
			{
				fname	= std::string(m.uri_info().host);
			}
			if (filesystem::is_directory(_savename))
			{
				fname	= _savename + fname;
			}
			fname	= filesystem::normalize_path(fname);
		}

		if (auto dir = filesystem::parent_path(fname); !dir.empty() && !filesystem::is_directory(dir))
		{
			try
			{
				std::filesystem::create_directories(dir);
			}
			catch (...)
			{
				std::cerr << "Can't create directory \"" << dir << "\"." << std::endl;
				return	1;
			}
		}

		std::ofstream	out(fname, std::ios::trunc|std::ios::binary);

		// リスナを作成
		size_t		fls	= 0;
		size_t		ttl	= 0;
		auto		l	= make_listener<mini_http>
		(
				// ヘッダ受信イベント(ヘッダを１行単位で返す)
				[&](typename listener<mini_http>::http_header_recieve_event e)
				{
					hha.add(trim(e.data, " \r\n"));
				},
				// ヘッダ終了。200ならプログレスバー開始
				[&](typename listener<mini_http>::http_header_complite_event e)
				{
					if (hha.response_code() == 200)
					{
						console::progress_start(console::progresstype::download, 0, 0, std::string(filesystem::filename(_savename)).c_str());
					}
				},
				// 本体受信イベント(本体部分の受信のみ)
				[&](typename listener<mini_http>::http_body_recieve_event e)
				{
					auto	len	= m.contents_length();
					console::progress_maxsize(len != -1 ? len : 0);
					console::progress_inc(e.data.size());

					out.write(e.data.data(), e.data.size());
					if ((fls += e.data.size()) > 1024 * 1024)
					{
						out.flush();
						fls = 0;
					}
					ttl += e.data.size();
				},
				// 上記以外のイベント（すべて無視する）
				[](auto e) {}
		);
		// リスナを設定
		m.set_listener(&l);

		// リクエストを発行

		m.request();
		if (m.contents_length() == -1)
		{
			console::progress_maxsize(ttl);
		}
		console::progress_end();

		if (hha.response_code() == 301 || hha.response_code() == 302)
		{
			url	= hha.find("Location")->second;
			continue;
		}

		if (_savedname)
		{
			*_savedname	= fname;
		}
		return	hha.response_code() == 200;
	}
}


bool extract(io::vstreamif _in, std::string const& _saveto, bool _skiprootdir)
{
	using namespace amtrs;
	if (!_in.good())
	{
		return	false;
	}

	bool		first	= true;
	std::string	skipdir;

	if (_skiprootdir)
	{
		auto				pos	= _in.tell();

		// ルートディレクトリを調査
		archive_enumrator	a(&_in);
		if (a.empty())
		{
			return	false;
		}

		for (auto& e : a)
		{
			auto	name	= e.name();
			auto	dir		= filesystem::parent_path(name);
			if (first)
			{
				skipdir	= std::string(dir);
			}
			else
			{
				if (skipdir != dir)
				{
					// 複数のディレクトリがあったのでスキップしない
					skipdir.clear();
					break;
				}
			}
		}
		_in.clear();
		_in.seek(pos);
	}

	size_t		skiplen	= skipdir.size();
	if (skipdir[skiplen] == '/' || skipdir[skiplen] == '\\')
	{
		++skiplen;
	}


	std::string	cd;
	archive_enumrate(&_in, [&](auto& e)
	{
		auto			name	= e.name();
		if (skiplen > 0)
		{
			name	= name.substr(skiplen);
		}
		std::string		dest;
		if (_saveto.empty())
		{
			dest	= std::string(name);
		}
		else
		{
			dest	= _saveto;
			dest.append("/");
			dest.append(name);
		}
		dest	= filesystem::normalize_path(dest);

		auto			dir		= filesystem::parent_path(dest);
		if (cd != dir)
		{
			if (!dir.empty() && !filesystem::is_directory(dir))
			{
				std::filesystem::create_directories(dir);
			}
			cd	= dir;
		}
		std::ofstream	o(dest, std::ios_base::trunc|std::ios_base::binary);
		auto			i = io::make_streamif(e);
		io::copy_to_eof(o, i);
	});

	return	true;
}

bool extract(std::string const& _file, std::string const& _saveto, bool _skiprootdir)
{
	std::ifstream	in(_file, std::ios::binary);
	return	extract(io::make_vstreamif(io::make_streamif(std::move(in))), _saveto, _skiprootdir);
}


bool extract(std::string const& _file, std::string const& _saveto, extract_params _info)
{
	std::ifstream	in(_file, std::ios::binary);
	_info.sourcename	= _file;
	return	extract(io::make_vstreamif(io::make_streamif(std::move(in))), _saveto, std::move(_info));
}


static std::string_view first_dir(std::string_view _name)
{
	std::string_view	r(_name.substr(0, 0));
	if (auto pos = _name.find_first_of("/\\"); pos != std::string_view::npos)
	{
		r	= _name.substr(0, pos);
	}
	return	r;
}


bool extract(io::vstreamif _in, std::string const& _saveto, extract_params _info)
{
	using namespace amtrs;
	if (!_in.good())
	{
		return	false;
	}

	bool		first	= true;
	size_t		ftotal	= 0;
	std::string	skipdir;

	if (_info.skiproot)
	{
		auto				pos	= _in.tell();

		// ルートディレクトリを調査
		archive_enumrator	a(&_in);
		if (a.empty())
		{
			return	false;
		}

		for (auto& e : a)
		{
			auto	name	= e.name();
			auto	dir		= first_dir(name);
			if (first)
			{
				skipdir	= std::string(dir);
				first	= false;
			}
			else
			{
				if (skipdir != dir)
				{
					// 複数のディレクトリがあったのでスキップしない
					skipdir.clear();
				}
			}
			++ftotal;
		}
		_in.clear();
		_in.seek(pos);
	}

	size_t		skiplen	= skipdir.size();
	if (skipdir[skiplen] == '/' || skipdir[skiplen] == '\\')
	{
		++skiplen;
	}

	console::progress_start(console::progresstype::extract, 0, ftotal, _info.sourcename.c_str());

	std::string			cd;
	extract_message		msg;
	try
	{
		for (auto& e : archive_enumrator(&_in))
		{
			console::progress_inc(1);

			auto			name	= e.name();
			if (skiplen > 0)
			{
				name	= name.substr(skiplen);
			}
			if (_saveto.empty())
			{
				msg.name	= std::string(name);
			}
			else
			{
				msg.name	= _saveto;
				msg.name.append("/");
				msg.name.append(name);
			}
			msg.msgid	= 0;
			bool		isDir	= msg.name.empty() ? false
								: (msg.name.back() == '/' || msg.name.back() == '\\') ? true
								: false;

			msg.name	= filesystem::normalize_path(msg.name);
			if (_info.msgproc)
			{
				int	r	= _info.msgproc(msg);
				if (r < 0)
				{
					break;
				}
			}

			auto			dir		= msg.name;
			if (!isDir)
			{
				dir	= filesystem::parent_path(msg.name);
			}
			if (cd != dir)
			{
				if (!dir.empty() && !filesystem::is_directory(dir))
				{
					try
					{
						std::filesystem::create_directories(dir);
					}
					catch (...)
					{
						console::progress_end();
						std::cerr << "Cat't create directory \"" << dir << "\"" << std::endl;
						return	false;
					}
				}
				cd	= dir;
			}
			if (isDir)
			{
				continue;
			}


			std::ofstream	o(msg.name, std::ios_base::trunc|std::ios_base::binary);
			if (!o.good())
			{
				std::cerr << "Can&t create " << msg.name << std::endl;
				throw	1;
			}
			auto			i = io::make_streamif(e);
			io::copy_to_eof(o, i);
		}
	}
	catch (...)
	{
		std::cout << "Extrak exception" << std::endl;
		console::progress_end();
		return	false;
	}

	console::progress_end();
	return	true;
}



bool visualstudio_setup()
{
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	// Where Visual Studio Install Direcotry
	std::string	vswhere	= "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe";
	std::string	out;
	std::string	err;
	exec(out, err, vswhere);

	std::string	vspath;
	
	for (auto line : split_chars(out, "\n"))
	{
		line	= trim(line, "\r\n\t ");
		if (starts_with(line, "installationPath:"))
		{
			// vcvarsall run.
			auto	vsp	= trim(line.substr(::strlen("installationPath:")), "\t ");
			vspath	= std::string(vsp);
			vspath.append("/VC/Auxiliary/Build/vcvarsall.bat x64");

			std::vector<std::pair<std::string, std::string>>	envs;
			ssu::enumenvs(envs, filesystem::normalize_path(vspath) + " x64", ssu::add_bat("tmp"));
			for (auto& e : envs)
			{
				SetEnvironmentVariableA(e.first.c_str(), e.second.c_str());
			}
			return	true;
		}
	}

#endif
	return	true;
}


bool cpp_setup()
{
	return	visualstudio_setup();
}


bool ls(std::string const& _patturn)
{
	struct	local
	{
		static bool listup(std::string const& _path, std::string const& _patturn)
		{
			for (auto& e : std::filesystem::directory_iterator(_path))
			{
				std::cout << e.path().string() << std::endl;
			}
			return	true;
		}
	};

	std::string	path	= ".";
	if (!_patturn.empty())
	{
		path	= _patturn;
	}
	return	local::listup(path, _patturn);
}

bool cat(std::string_view _file)
{
	return	cat({_file});
}

bool cat(std::initializer_list<std::string_view> _files)
{
	for (auto& f : _files)
	{
		std::string	s;
		filesystem::file_get_contents(s, f);
		std::cout << s;
	}
	return	true;
}


AMTRS_SCRIPTUTIL_NAMESPACE_END
