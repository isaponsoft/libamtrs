/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <vector>
#include <amtrs/locale/locale.hpp>
#include <amtrs/filesystem/@>
#include <amtrs/string/@>
#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

using namespace amtrs;


static	std::map<std::string, std::string>		mTexts;


namespace locale {


//! ロケールシステムを初期化します。
void initialize()
{
	mTexts = {};
}

//! ロケールシステムにロケールファイルを追加します。
void add_locale_text(std::string _localefilename)
{
	// 現在の言語
	std::string	language	= amtrs::locale().get_language();

	// デフォルト言語に併せて言語の検索順序を変更する
	std::vector<std::string>	langs;
	langs.push_back(language);

	// 中国語（繁体字）以外の場合は中国語（繁体字）を検索候補に加える
	if (language == "zh_SG" || language == "zh_HK" || language == "zh_TW")
	{
		langs.push_back("zh_CN");
	}

	// 英語（米国）以外はアメリカ英語を候補に加える
	if (language != "en_US")
	{
		langs.push_back("en_US");
	}

	// 日本語以外は日本語を候補に加える(デフォルトロケール)
	if (language != "ja_JP")
	{
		langs.push_back("ja_JP");
	}


	auto		fl		= filesystem::fileloader::get_instance();
	std::string	filename;
	for (auto& lang : langs)
	{
		std::string	testname	= format<std::string>(_localefilename.c_str(), lang.c_str());
		if (fl->exists(testname))
		{
			filename	= std::move(testname);
			break;
		}
	}

	if (filename.empty())
	{
		return;
	}

	auto		txfile	= filesystem::file_get_contents<std::string>(filename, fl);
	for (auto line : split_chars(txfile, "\n"))
	{
		line	= trim(line, "\n\r");
		if (starts_with(line, "#")) continue;

		auto	pos	= line.find_first_of("\t ");
		auto	tag	= trim(line.substr(0, pos), "\t ");
		auto	txt	= trim(line.substr(pos+1), "\t ");
		mTexts.insert({std::string(tag), std::string(txt)});

		//AMTRS_DEBUG_LOG("%s : %s", std::string(tag).c_str(), std::string(txt).c_str());
	}
}


//! 対応する文字列を取得します。
char const* gettext(char const* _tag, char const* _default)
{
	auto	i	= mTexts.find(std::string(_tag));
	if (i == mTexts.end())
	{
		return	_default ? _default : _tag;
	}
	return	(*i).second.c_str();
}





} // namespace locale
