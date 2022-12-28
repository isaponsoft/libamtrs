/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <map>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <amtrs/console.hpp>
#include <amtrs/optparse.hpp>
#include <amtrs/filesystem.hpp>
#include "apgen.hpp"
#include "functions/functions.hpp"

using namespace amtrs;
namespace	fs	= amtrs::filesystem;
using		ll	= console::loglevel;

std::string	gAmtrsHome;


int main(int _argc, char* _args[])
{
	amtrs::console::set_loglevel((amtrs::console::loglevel)2);

	gAmtrsHome	= fs::normalize_path((fs::readlink(fs::parent_path(_args[0])) / "../..").string());
	if (!amtrs::getenv<std::string>("AMTRS_HOME").empty())
	{
		gAmtrsHome	= amtrs::getenv<std::string>("AMTRS_HOME");
	}

	apgen::option	opt;
	for (int i = 1; i < _argc; ++i)
	{
		std::string_view	arg(_args[i]);

		if (arg == std::string_view("-M"))
		{
			++i;
			if (i == _argc)
			{
				console::log(ll::ll_error, "-M need module");
				return	1;
			}
			opt.genType	= _args[i];
			continue;
		}

		if (arg == std::string_view("--loglevel"))
		{
			++i;
			if (i == _argc)
			{
				console::log(ll::ll_error, "--loglevel LOGLEVEL(0-6 default=3)");
				return	1;
			}
			opt.loglevel	= atoi(_args[i]);
			amtrs::console::set_loglevel((amtrs::console::loglevel)opt.loglevel);
			continue;
		}


		if (amtrs::starts_with(arg, (std::string_view)"-D"))
		{
			auto	param	= arg.substr(2);
			opt.args.push_back({std::string(), std::string(param)});
			continue;
		}

		if (arg[0] == '-' && arg.find_first_of(':') != std::string_view::npos)
		{
			auto	pos		= arg.find_first_of(':');
			auto	genname	= arg.substr(1, pos - 1);
			auto	param	= arg.substr(pos + 1);
			opt.args.push_back({std::string(genname), std::string(param)});
			continue;
		}

		if (!opt.genFile.empty())
		{
			console::log(ll::ll_error, "Duplicate gen directory.");
			return	1;
		}

		opt.genFile	=  _args[i];
		if (opt.genFile != "-")
		{
			if (std::filesystem::is_directory(opt.genFile))
			{
				opt.genFile	+= "/APGen.txt";
			}
			if (amtrs::filesystem::is_relative(opt.genFile))
			{
				opt.genFile	=  amtrs::filesystem::normalize_path(amtrs::filesystem::absolute_path(opt.genFile));
			}
		}
	}

	if (opt.genFile.empty())
	{
		console::log(ll::ll_error, "Nothing APGen.txt directory.");
		return	1;
	}



	for (auto al = apgen::autolink::_link(); al; al = al->next)
	{
		if (al->name == opt.genType)
		{
			auto	loader	= amtrs::filesystem::fileloader::get_instance();
			loader->push_back(amtrs::filesystem::stdvfs::get_instance());

			console::log(ll::ll_debug, "Module : %s", al->name.data());

			apgen::status	st;

			std::string	templatename	= gAmtrsHome;
			templatename += "/apgen/";
			templatename += al->name;
			templatename += "/";
			st.values["APGEN_TEMPLATE_DIR"]	= amtrs::filesystem::normalize_path(templatename);
			st.values["AMTRS_HOME"]			= gAmtrsHome;
			st.values["APGEN"]				= amtrs::filesystem::normalize_path(std::string_view(_args[0]));
			st.values["APGEN_RERAL"]		= fs::readlink(std::string_view(_args[0])).string();

			// APGenConf.txt を読み込む
			if (opt.genFile != "-")
			{
				bool	ec	= false;

				st.values["APGEN_STARTUP_DIR"]	= amtrs::filesystem::normalize_path(amtrs::filesystem::parent_path(opt.genFile));

				for (auto& p : opt.args)
				{
					auto	pos	= p.second.find_first_of('=');
					if (pos == std::string_view::npos)
					{
						continue;
					}
					std::string	key	= std::string(amtrs::trim(std::string_view(p.second.substr(0, pos)), " \t"));
					std::string	val	= std::string(amtrs::trim(std::string_view(p.second.substr(pos+1)), " \t"));
					key	= replace_macro(key, st);
					val	= replace_macro(val, st);
					st.values.insert({key, val});
				}

//				apgen::load_conf(st, opt.genFile, ec);
				for (auto& p : st.values)
				{
					if (p.first != al->name) { continue; }

					// ジェネレータに一致する設定だけ読み込む
					auto	pos	= p.second.find_first_of('=');
					if (pos == std::string_view::npos)
					{
						continue;
					}
					std::string	key	= std::string(amtrs::trim(std::string_view(p.second.substr(0, pos)), " \t"));
					std::string	val	= std::string(amtrs::trim(std::string_view(p.second.substr(pos+1)), " \t"));
					key	= replace_macro(key, st);
					val	= replace_macro(val, st);
					st.values.insert({key, val});
				}
				if (ec)
				{
					return	1;
				}
			}

			// 引数で上書きする
			for (auto& p : opt.args)
			{
				auto	pos	= p.second.find_first_of('=');
				if (pos == std::string_view::npos)
				{
					continue;
				}
				std::string	key	= std::string(amtrs::trim(std::string_view(p.second.substr(0, pos)), " \t"));
				std::string	val	= std::string(amtrs::trim(std::string_view(p.second.substr(pos+1)), " \t"));
				key	= replace_macro(key, st);
				val	= replace_macro(val, st);
				st.values.insert({key, val});
			}

			auto*	g		= al->proc(opt);

			// 不明なジェネレータ引数を調べる
			for (auto& p : st.values)
			{
				if (!g->arguments_check(p.first, p.second, opt))
				{
					console::log(ll::ll_error, "Unkown parameter \"%s\".", p.first.c_str());
					return	1;
				}
			}
			int		r	= g->execute(st, opt);
			delete	g;
			return	r;
		}
	}


	console::log(ll::ll_main, "Modules");
	for (auto al = apgen::autolink::_link(); al; al = al->next)
	{
		console::log(ll::ll_main, "  %s", al->name.data());
	}

	return	0;
}


void apgen::error(std::string const& _msg)
{
	std::cerr << _msg << std::endl;
}



void apgen::report(std::string const& _report)
{
	std::cout << _report << std::endl;
}

