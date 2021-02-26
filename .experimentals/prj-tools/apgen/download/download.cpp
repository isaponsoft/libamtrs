/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <regex>
#include <unordered_map>
#include <amtrs/amtrs.hpp>
#include <amtrs/filesystem/@>
#include <amtrs/filesystem/zip/@>
#include <amtrs/string/@>
#include <amtrs/string/regex/@>
#include <amtrs/process/@>
#include <amtrs/opt/io/zlib-stream_in.hpp>
#include <amtrs/net/@>
#include <amtrs/net/http/@>
#include <apgen.hpp>

using namespace apgen;



class	DownloadVirtualGenerator
		: public generator_base
{
public:
	// 引数の整合性チェック
	virtual bool arguments_check(std::string const& _key, std::string const& _value, option const& _opt) override
	{
		if (_key == "URL")
		{
			mURL				= _value;
			return	true;
		}
		if (_key == "HEADER_NO_PRINT")
		{
			mHeaderNoPrint		= true;
			return	true;
		}
		if (_key == "CONTENTS_NO_SAVE")
		{
			mContentsNoSave		= true;
			return	true;
		}
		if (_key == "MOVE_LOCATION")
		{
			mMoveLocation		= true;
			return	true;
		}

		return	false;
	}


	virtual int execute(status const& _state, option const& _opt) override
	{
		using namespace amtrs;
		using namespace amtrs::net;

		if (mURL.empty())
		{
			std::cerr << "URL nothing." << std::endl;
		}

		std::string	url	= mURL;
		for (;;)
		{
			mini_http					m(url);
			http::http_header_analyzer	hha;

			auto			fname = std::string(filesystem::filename((std::string_view)m.uri_info().path));

			if (fname == "/")
			{
				fname	= std::string(m.uri_info().host);
			}

			std::ofstream	out;
			if (!mContentsNoSave)
			{
				out.open(fname, std::ios::trunc|std::ios::binary);
			}

			// リスナを作成
			auto		l	= make_listener<mini_http>
			(
					// ヘッダ受信イベント(ヘッダを１行単位で返す)
					[&](typename listener<mini_http>::http_header_recieve_event e)
					{
						if (!mHeaderNoPrint)
						{
							std::cout << trim(e.data, " \r\n") << std::endl;
						}
						hha.add(trim(e.data, " \r\n"));
					},
					// 本体受信イベント(本体部分の受信のみ)
					[&](typename listener<mini_http>::http_body_recieve_event e)
					{
						if (!mContentsNoSave)
						{
							out.write(e.data.data(), e.data.size());
						}
					},
					// 上記以外のイベント（すべて無視する）
					[](auto e) {}
			);

			// リスナを設定
			m.set_listener(&l);

			// リクエストを発行
			m.request();

			if (hha.response_code() == 301)
			{
				if (mMoveLocation)
				{
					url	= hha.find("Location")->second;
					continue;
				}
			}

			break;
		}
		return	0;
	}


	std::string							mURL;
	bool								mHeaderNoPrint	= false;
	bool								mContentsNoSave	= false;
	bool								mMoveLocation	= false;
};


apgen::autolink	_download({"download", [](auto& opt) -> generator_base*
{
	return	new DownloadVirtualGenerator();
}});

