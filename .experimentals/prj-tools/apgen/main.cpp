/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <map>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <amtrs/filesystem/@>
#include <amtrs/system/env.hpp>
#include <amtrs/string/regex/@>
#include "apgen.hpp"
#include "functions/functions.hpp"

namespace fs	= amtrs::filesystem;



int main(int _argc, char* _args[])
{
	if (amtrs::getenv<std::string>("AMTRS_HOME").empty())
	{
		apgen::error("Set enveroment AMTRS_HOME.");
		return	1;
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
				apgen::error("-M need module");
				return	1;
			}
			opt.genType	= _args[i];
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
			apgen::error("Duplicate gen directory.");
			return	1;
		}

		opt.genFile	=  _args[i];
		if (opt.genFile != "-")
		{
			opt.genFile	+= "/APGen.txt";
			if (amtrs::filesystem::is_relative(opt.genFile))
			{
				opt.genFile	=  amtrs::filesystem::normalize_path(amtrs::filesystem::absolute_path(opt.genFile));
			}
		}
	}

	if (opt.genFile.empty())
	{
		apgen::error("Nothing APGen.txt directory.");
		return	1;
	}


	for (auto al = apgen::autolink::_link(); al; al = al->next)
	{
		if (al->name == opt.genType)
		{
			auto	loader	= amtrs::filesystem::fileloader().get_instance();
			loader->push_back(amtrs::filesystem::stdvfs::get_instance());

			apgen::report(amtrs::format<std::string>("Module : %s", al->name.data()));

			apgen::status	st;

			std::string	templatename	= amtrs::getenv<std::string>("AMTRS_HOME");
			templatename += "/tools/template/apgen/";
			templatename += al->name;
			templatename += "/";
			st.values["APGEN_TEMPLATE_DIR"]	= amtrs::filesystem::normalize_path(templatename);

			// APGenConf.txt を読み込む
			if (opt.genFile != "-")
			{
				bool											ec	= false;

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

				apgen::load_conf(st, opt.genFile, ec);
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
					apgen::error(amtrs::format<std::string>("Unkown parameter \"%s\".", p.first.c_str()));
					return	1;
				}
			}

			int		r	= g->execute(st, opt);
			delete	g;
			return	r;
		}
	}


	apgen::report("Modules");
	for (auto al = apgen::autolink::_link(); al; al = al->next)
	{
		apgen::report(amtrs::format<std::string>("  %s", al->name.data()));
	}

	return	0;
}


void apgen::error(std::string const& _msg)
{
	std::cerr << _msg << std::endl;
}

using	codeiterator	= decltype(amtrs::split_chars(std::string_view(), std::string_view()));

enum class	codetype
{
	cb_none,
	cb_if,
	cb_else,
	cb_for,
};


namespace apgen {

codeiterator process(status& _status, std::string const& _apgenconf, bool& _error, codeiterator _begin, codeiterator _end, codetype _type = codetype::cb_none, bool _skip = false)
{
	for (auto it = _begin; it != _end; )
	{
		auto				itDef	= it;
		std::string_view	ln(*it++);
		std::string			line	= std::string(amtrs::trim(ln, "\r\n \t"));
		if (line.empty() || amtrs::starts_with(line, (std::string_view)"#")) { continue; }

		while (amtrs::ends_with(line, "\\") && it != _end)
		{
			std::string_view	ln2(*it++);
			std::string			l2	= std::string(amtrs::trim(ln2, "\r\n \t"));
			line = line.substr(0, line.size() - 1);
			line += " ";
			line += l2;
		}

		if (amtrs::starts_with(line, ".for"))
		{
			std::smatch	m;
			if (!std::regex_match(line, m, std::regex(R"(\.for[ \t]*\((.*?)\))")))
			{
				error(".for stntax error");
				_error	= true;
				return	_end;
			}

			std::string		params	= m[1].str();
			params	= apgen::replace_macro(params, _status);
			auto			prmsc	= amtrs::split_chars((std::string_view)params, (std::string_view)" ");
			auto			pit		= prmsc.begin();
			auto			pend	= prmsc.end();

			std::string		iname	= (std::string)*pit++;
			codeiterator	beg		= it;
			int				fc		= 0;
			for (; pit != pend; ++pit)
			{
				std::string	v(*pit);
				if (!v.empty())
				{
					_status.values[iname]			= v;
					_status.values["__for_count"]	= amtrs::format<std::string>("%d", fc);
					it	= process(_status, _apgenconf, _error, beg, _end, codetype::cb_none, _skip);
					++fc;
				}
			}
			ln		= std::string_view(*it);
			line	= std::string(amtrs::trim(ln, "\r\n \t"));
			if (!amtrs::starts_with(line, ".end"))
			{
				error(".for stntax error");
				_error	= true;
				return	_end;
			}
			++it;
			continue;
		}

		if (amtrs::starts_with(line, ".if"))
		{
			std::smatch	m;
			if (!std::regex_match(line, m, std::regex(R"(\.if[ \t]*\((.*?)\))")))
			{
				error(".if stntax error");
				_error	= true;
				return	_end;
			}
			bool		exec	= false;
			bool		skip	= _skip;
			if (!skip)
			{
				bool		notFlg	= false;
				std::string	cond	= m[1].str();
				if (auto pos = cond.find_first_of(' '); pos != std::string_view::npos)
				{
					if (cond.substr(0, pos) == "NOT")
					{
						notFlg	= true;
						cond	= cond.substr(pos+1);
					}
				}
				cond	= apgen::replace_macro(cond, _status);
				exec	= (!cond.empty() && cond != "0");
				skip	= !exec;
				if (notFlg)
				{
					exec	= !exec;
					skip	= !skip;
				}
			}
			it		= process(_status, _apgenconf, _error, it, _end, codetype::cb_none, skip);
			ln		= std::string_view(*it);
			line	= std::string(amtrs::trim(ln, "\r\n \t"));
			if (amtrs::starts_with(line, ".else"))
			{
				++it;
				it	= process(_status, _apgenconf, _error, it, _end, codetype::cb_none, _skip || exec);
				ln		= std::string_view(*it);
				line	= std::string(amtrs::trim(ln, "\r\n \t"));
			}
			if (amtrs::starts_with(line, ".end"))
			{
				++it;
				continue;
			}


			continue;
		}

		if (amtrs::starts_with(line, ".else") || amtrs::starts_with(line, ".elif") || amtrs::starts_with(line, ".end"))
		{
			return	itDef;
		}

		if (_skip)
		{
			continue;
		}

		// マクロの置換
		line	= replace_macro(line, _status);

		// 関数を処理する
		if (apgen::process_function(_status, line))
		{
		//	continue;
		}


		line	= amtrs::regex_replace_callback(line, R"(\$\\\{)", [&](auto m) -> std::string
				{
					return	"${";
				});


		// includeを処理する
		bool	incproc	= false;
		amtrs::regex_match_callback(line, R"(@include\((.*)\))", [&](auto m)
		{
			std::string	inc	= m[1];
			if (amtrs::filesystem::is_relative(inc))
			{
				inc	= std::string(amtrs::filesystem::parent_path(_apgenconf)) + "/" + inc;
			}
			load_conf(_status, amtrs::filesystem::normalize_path(inc), _error);
			incproc	= true;
		});
		if (_error)
		{
			return	it;
		}
		if (incproc)
		{
			continue;
		}

		if (amtrs::trim(line, "\r\n \t").empty())
		{
			continue;
		}

		// 代入処理
		auto	pos	= line.find_first_of('=');
		if (pos == std::string_view::npos)
		{
			error(amtrs::format<std::string>("unkown line \"%s\".", line.c_str()));
			continue;
		}
		auto	key	= std::string(amtrs::trim(line.substr(0, pos), " \t"));
		auto	val	= std::string(amtrs::trim(line.substr(pos+1), " \t"));
		if (!_status.values.insert({key, val}).second)
		{
			_status.values[key] = val;
		}
		std::cout << "set " << key.c_str() << " <= " <<  val << std::endl;
	}
	return	_end;
}



void load_conf(status& _status, std::string const& _apgenconf, bool& _error)
{
	_error	= false;

	// コンフィグファイルの有無をチェック
	if (!amtrs::filesystem::exists(_apgenconf))
	{
		error(amtrs::format<std::string>("\"%s\" not found.", _apgenconf.c_str()));
		_error	= true;
		return;
	}
	apgen::report(amtrs::format<std::string>("Load %s", _apgenconf.c_str()));


	auto	selfPrv	= _status.values["APGEN_SELF_DIR"];

	_status.values["APGEN_SELF_DIR"]	= fs::normalize_path((std::string)fs::parent_path(_apgenconf));

	auto	gp		= amtrs::filesystem::file_get_contents<std::string>(_apgenconf);
	auto	script	= std::string_view(gp);

	// BOMから始まっていればスキップ
	auto	bom		= "\xEF\xBB\xBF";
	if (amtrs::starts_with(script, std::string_view(bom)))
	{
		script	= script.substr(3);
	}

	// 行単位でコードブロックを生成
	auto		c	= amtrs::split_chars(script, "\n");
	process(_status, _apgenconf, _error, c.begin(), c.end());
	_status.values["APGEN_SELF_DIR"]	= selfPrv;
	return;
}


void report(std::string const& _report)
{
	std::cout << _report << std::endl;
}


codeiterator update_script(std::string& _txt, status& _status, codeiterator _begin, codeiterator _end, bool _skip = false)
{
	for (auto it = _begin; it != _end; )
	{
		auto				itDef	= it;
		std::string_view	ln(*it++);
		std::string			line(ln);
		line += "\n";


		if (amtrs::starts_with(amtrs::trim(line, "\r\n \t"), ".for"))
		{
			while (amtrs::ends_with(amtrs::trim(line, "\r\n \t"), "\\") && it != _end)
			{
				std::string_view	ln2(*it++);
				std::string			l2	= std::string(amtrs::trim(ln2, "\r\n \t"));
				line = std::string(amtrs::trim(line, "\r\n \t"));
				line = line.substr(0, line.size() - 1);
				line += " ";
				line += l2;
			}
			line	= std::string(amtrs::trim(line, "\r\n \t"));

			std::smatch	m;
			if (!std::regex_match(line, m, std::regex(R"(^\.for[ \t]*\((.*?)\)$)")))
			{
				if (!_skip)
				{
					_txt	+= std::string(ln);
					_txt	+= "\n";
				}
				continue;
			}

			std::string		params	= m[1].str();
			params	= apgen::replace_macro(params, _status);
			auto			prmsc	= amtrs::split_chars((std::string_view)params, (std::string_view)" ");
			auto			pit		= prmsc.begin();
			auto			pend	= prmsc.end();

			std::string		iname	= (std::string)*pit++;
			codeiterator	beg		= it;
			int				fc		= 0;
			for (; pit != pend; ++pit)
			{
				std::string	v(*pit);
				if (!v.empty())
				{
					_status.values[iname]			= v;
					_status.values["__for_count"]	= amtrs::format<std::string>("%d", fc);
					it	= update_script(_txt, _status, beg, _end, _skip);
					++fc;
				}
			}
			ln		= std::string_view(*it);
			line	= std::string(amtrs::trim(ln, "\r\n \t"));
			if (!amtrs::starts_with(line, ".end"))
			{
				continue;
			}
			++it;
			continue;
		}

		if (amtrs::starts_with(amtrs::trim(line, "\r\n \t"), ".if"))
		{
			while (amtrs::ends_with(amtrs::trim(line, "\r\n \t"), "\\") && it != _end)
			{
				std::string_view	ln2(*it++);
				std::string			l2	= std::string(amtrs::trim(ln2, "\r\n \t"));
				line = std::string(amtrs::trim(line, "\r\n \t"));
				line = line.substr(0, line.size() - 1);
				line += " ";
				line += l2;
			}
			line	= std::string(amtrs::trim(line, "\r\n \t"));

			std::smatch	m;
			if (!std::regex_match(line, m, std::regex(R"(\.if[ \t]*\((.*?)\))")))
			{
				if (!_skip)
				{
					_txt	+= std::string(ln);
					_txt	+= "\n";
				}
				continue;
			}
			bool		exec	= false;
			bool		skip	= _skip;
			if (!skip)
			{
				bool		notFlg	= false;
				std::string	cond	= m[1].str();
				if (auto pos = cond.find_first_of(' '); pos != std::string_view::npos)
				{
					if (cond.substr(0, pos) == "NOT")
					{
						notFlg	= true;
						cond	= cond.substr(pos+1);
					}
				}
				cond	= apgen::replace_macro(cond, _status);
				exec	= (!cond.empty() && cond != "0");
				skip	= !exec;
				if (notFlg)
				{
					exec	= !exec;
					skip	= !skip;
				}
			}
			it		= update_script(_txt, _status, it, _end, skip);
			ln		= std::string_view(*it);
			line	= std::string(amtrs::trim(ln, "\r\n \t"));
			if (amtrs::starts_with(line, ".else"))
			{
				++it;
				it		= update_script(_txt, _status, it, _end, _skip || exec);
				ln		= std::string_view(*it);
				line	= std::string(amtrs::trim(ln, "\r\n \t"));
			}
			if (amtrs::starts_with(line, ".end"))
			{
				++it;
				continue;
			}


			continue;
		}

		if (amtrs::starts_with(amtrs::trim(line, "\r\n \t"), ".else") || amtrs::starts_with(amtrs::trim(line, "\r\n \t"), ".elif") || amtrs::starts_with(amtrs::trim(line, "\r\n \t"), ".end"))
		{
			return	itDef;
		}

		if (_skip)
		{
			continue;
		}

		// マクロの置換
		line	= std::string(ln) + "\n";
		line	= replace_macro(line, _status);

		line	= amtrs::regex_replace_callback(line, R"(\$\\\{)", [&](auto m) -> std::string
				{
					return	"${";
				});

		// 関数を処理する
		apgen::process_function(_status, line);

		_txt	+= line;
	}
	return	_end;
}



std::string update_script(std::string const& _txt, apgen::status& _status)
{
	std::string	txt;
	auto		c	= amtrs::split_chars(_txt, "\n");
	update_script(txt, _status, c.begin(), c.end(), false);
	return	txt;
}


std::string replace_macro(std::string const& _txt, apgen::status const& _status)
{
	std::string	txt	= _txt;
	for (;;)
	{
		bool	replaced	= false;
		txt	= amtrs::regex_replace_callback(txt, R"(\$\{(.*?)(:(.+))*\})", [&](auto m) -> std::string
			{
				std::string	vname	= m[1].str();
				replaced			= true;
				for (auto name : amtrs::split_chars((std::string_view)vname, (std::string_view)"|"))
				{
					for (auto& d : _status.values)
					{
						if (d.first == name)
						{
							std::string	val	= d.second;
							if (m[3] == "backslash-escape")
							{
								val	= amtrs::filesystem::normalize_path(val, true);
							}
							if (!val.empty())
							{
								return	val;
							}
						}
					}
				}
				if (std::string def = m[3].str(); !def.empty())
				{
					return	def;
				}
				return	"";
			});
		if (!replaced)
		{
			break;
		}
	}
	return	txt;
}

}

