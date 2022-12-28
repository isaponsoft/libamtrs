/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include "apgen.hpp"



//! 指定したバッファへURLのデータをダウンロードします。
bool apgen::download(std::string& _dest, std::string const& _url)
{
	using namespace amtrs;
	using namespace amtrs::net;

	std::string	url	= _url;
	for (;;)
	{
		std::cout << "Downloading: " << url << std::endl;
		mini_http					m(url);
		http::http_header_analyzer	hha;

		_dest	= {};

		// リスナを作成
		auto		l	= make_listener<mini_http>
		(
				// ヘッダ受信イベント(ヘッダを１行単位で返す)
				[&](typename listener<mini_http>::http_header_recieve_event e)
				{
					hha.add(trim(e.data, " \r\n"));
				},
				// 本体受信イベント(本体部分の受信のみ)
				[&](typename listener<mini_http>::http_body_recieve_event e)
				{
					_dest += e.data;
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
			url	= hha.find("Location")->second;
			//std::cout << "Move location" << std::endl;
			continue;
		}
		break;
	}

	return	true;
}


bool apgen::download(std::string const& _url)
{
	using namespace amtrs;
	using namespace amtrs::net;

	std::string	url	= _url;
	for (;;)
	{
		mini_http					m(url);
		http::http_header_analyzer	hha;


		auto			fname = std::string(filesystem::filename((std::string_view)m.uri_info().path));

		if (fname == "/")
		{
			fname	= std::string(m.uri_info().host);
		}

		std::cout << "Downloading: " << url << " to " << fname << std::endl;

		std::ofstream	out(fname, std::ios::trunc|std::ios::binary);

		// リスナを作成
		auto		l	= make_listener<mini_http>
		(
				// ヘッダ受信イベント(ヘッダを１行単位で返す)
				[&](typename listener<mini_http>::http_header_recieve_event e)
				{
					std::cout << trim(e.data, " \r\n") << std::endl;
					hha.add(trim(e.data, " \r\n"));
				},
				// 本体受信イベント(本体部分の受信のみ)
				[&](typename listener<mini_http>::http_body_recieve_event e)
				{
					out.write(e.data.data(), e.data.size());
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
			url	= hha.find("Location")->second;
			//std::cout << "Move location" << std::endl;
			continue;
		}

		break;
	}

	return	true;
}

