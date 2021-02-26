/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <psnvm/psnvm.hpp>
#include <amtrs/filesystem/@>
#include <amtrs/string/join.hpp>

using namespace amtrs;
namespace fs	= amtrs::filesystem;
namespace psn	= PASSION_NAMESPACE;





int main(int _argc, char** _args)
{
	if (_argc == 1)
	{
		return	0;
	}

	psn::preprocessor::setting		setting;
	std::vector<psnvm::codereader>	presrcs;
	std::vector<char const*>		srcfiles;

	for (int i = 1; i < _argc; ++i)
	{
		psnvm::vmstring	argv(_args[i]);
		if (argv.empty())
		{
			continue;
		}
		if (amtrs::starts_with(argv, "-I"))
		{
			auto	name	= ((std::string_view)argv).substr(2);
			if (name.empty() && (i + 1 < _argc))
			{
				++i;
				name	= std::string_view(_args[i]);
			}

			setting.include_paths.push_back(name);
			continue;
		}
		else if (amtrs::starts_with(argv, "-D"))
		{
			auto	text	= ((std::string_view)argv).substr(2);
			auto	pos		= text.find_first_of('=');
			if (pos == std::string_view::npos)
			{
				setting.define(text, "");
			}
			else
			{
				setting.define(text.substr(0, pos), text.substr(pos+1));
			}
			continue;
		}
		else if (amtrs::starts_with(argv, "-p"))
		{
			// -p ファイル
			// ソースコードにの前に暗黙的に include させたいファイルを
			// 指定できます。
			auto	srcname	= ((std::string_view)argv).substr(2);
			if (srcname.empty() && (i + 1 < _argc))
			{
				++i;
				srcname	= std::string_view(_args[i]);
			}

			presrcs.push_back({srcname, fs::file_get_contents<std::string>(srcname)});
			continue;
		}

		srcfiles.push_back(_args[i]);
	}


	for (auto srcname : srcfiles)
	{
		if (!fs::exists(srcname))
		{
			std::cerr << "can't open " << srcname << std::endl;
			return	1;
		}



		// エラーレポート
		psn::reportor		report;

		setting.sources		= presrcs;
		setting.sources.push_back({srcname, fs::file_get_contents<std::string>(srcname)});

		// プリプロセッサ呼び出し
		psn::vmstring		src	= psn::preprocessor::preprocess(&report, setting);
std::cout << src << std::endl;

		if (!report.empty())
		{
			for (auto& e : report)
			{
				if (e.code != psn::ok)
				{
					std::cerr << (std::string_view)e.filename << "(" << e.line << ") : " << std::abs(e.code) << " : " << e.whats() << std::endl;
				}
			}
			return	1;
		}
		psn::vm				vm;
		psn::compiler		cmp(&vm);
	}


	return	0;
}
