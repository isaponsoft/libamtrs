#ifndef	__libamtrs__net__simple_http_win32__hpp
#define	__libamtrs__net__simple_http_win32__hpp
#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "../../os/win32/windows.hpp"
#include "../../os/win32/error_code.hpp"
#include <winhttp.h>
#include "../../string.hpp"
#pragma comment(lib, "Winhttp.lib")
AMTRS_NET_NAMESPACE_BEGIN



// ========================================================================
//! HTTPヘッダーを分離します。
// ------------------------------------------------------------------------
//! 返されるヘッダの一覧は、ヘッダに書かれていたデータの順番を保持します。
// ------------------------------------------------------------------------
template<class CharT = char, class Traits = std::char_traits<CharT>>
auto split_http_headers(std::basic_string_view<CharT, Traits> _headers) -> std::vector<std::pair<std::basic_string<CharT, Traits>, std::basic_string<CharT, Traits>>>
{
	using	string_type	= std::basic_string<CharT, Traits>;
	using	value_type	= std::pair<string_type, string_type>;
	using	result_type	= std::vector<value_type>;

	result_type	retval;
	for (auto l : split_chars(_headers, "\n"))
	{
		auto		line	= trim(l, "\r");
		if (!line.empty())
		{
			auto		sepPos	= line.find(':');
			value_type	pair;
			if (sepPos == string_type::npos)
			{
				pair.second	= string_type(line);
			}
			else
			{
				auto	key		= std::string_view(line.data(), sepPos);
				auto	value	= trim(std::string_view(line.data() + sepPos + 1, line.size() - (sepPos + 1)), " ");
				pair.first	= string_type(key);
				pair.second	= string_type(value);
			}
			if (pair.first.empty() && pair.second.empty())
			{
				break;
			}
			retval.push_back(std::move(pair));
		}
	}
	return	retval;
}



template<class CharT, class Traits = std::char_traits<CharT>>
struct	url_traits
{
	using	view_type	= std::basic_string_view<CharT, Traits>;
	using	string_type	= std::basic_string<CharT, Traits>;

	// ========================================================================
	//! URLを解析し、スキーム、ホスト名、ポート名、パスを取得します。
	// ------------------------------------------------------------------------
	//! ユーザー名、ポート名、パスは存在しない場合は empty() == true を返します。
	// ------------------------------------------------------------------------
	static bool split(view_type& _scheme, view_type& _username, view_type& _hostname, view_type& _port, view_type& _path, const string_type& _url)
	{
		typename string_type::size_type	cur	= 0;
		typename string_type::size_type	end	= _url.size();

		_scheme		=
		_username	=
		_hostname	=
		_port		=
		_path		= view_type(_url.data(), 0);

		// スキームの抜出
		auto	schemeEnd	= _url.find("://", cur);
		if (schemeEnd == string_type::npos)
		{
			return	false;
		}
		_scheme	= view_type(_url.data() + cur, schemeEnd - cur);
		cur		= schemeEnd + 3;

		auto	firstSlashEnd	= _url.find("/", cur);

		// ユーザー名の抜出
		auto	atmarkEnd		= _url.find("@", cur);
		if (atmarkEnd != string_type::npos)
		{
			// @が有る
			if ((firstSlashEnd == string_type::npos) || (firstSlashEnd != string_type::npos && atmarkEnd < firstSlashEnd))
			{
				// ユーザー名っぽい
				_username		= view_type(_url.data() + cur, atmarkEnd - cur);
				cur				= atmarkEnd + 1;
			}
		}

		// ポートの抜出
		auto	portSep		= _url.find(":", cur);
		if (portSep != string_type::npos)
		{
			// @が有る
			if ((firstSlashEnd == string_type::npos) || (firstSlashEnd != string_type::npos && portSep < firstSlashEnd))
			{
				// ポート番号っぽい
				_username		= view_type(_url.data() + cur, portSep - cur);
				cur				= portSep + 1;
			}
		}

		// ホスト名の抜き出し
		if (firstSlashEnd == string_type::npos)
		{
			_hostname	= view_type(_url.data() + cur, end - cur);
			return	true;
		}

		_hostname	= view_type(_url.data() + cur, firstSlashEnd - cur);
		cur			= firstSlashEnd + 1;
		_path		= view_type(_url.data() + cur, end - cur);

		return	true;
	}

	// ポート名からポートに変換します。
	static int port(view_type _portName)
	{
		if (_portName.empty())
		{
			return	0;
		}
		if (is_match(make_constring("http"), _portName))
		{
			return	80;
		}
		if (is_match(make_constring("https"), _portName))
		{
			return	433;
		}
		return	::atoi(_portName.data());
	}


protected:
	template<std::size_t N>
	static bool is_match(constring<N> _a, view_type _b) noexcept
	{
		return	_a.size() == _b.size() && std::memcmp(_a.data(), _b.data(), N);
	}
};


class	simple_http_win32
		: public simple_http
{
public:
	struct	hinet_closer
	{
		using	pointer	= HINTERNET;
		void operator () (pointer _handle)
		{
			::WinHttpCloseHandle(_handle);
		}
	};
	using	hinternet	= std::unique_ptr<HINTERNET, hinet_closer>;


	//! 指定したURLへ GET のリクエストを行います。
	virtual void request_get(std::string_view _url) override
	{
		if (!::WinHttpCheckPlatform())
		{
			os::win32::throw_system_error("simple_http");
		}

		hinternet			hSession;
		hinternet			hConnect;
		hinternet			hRequest;

		std::string			url(_url);
		std::string_view	view_scheme;
		std::string_view	view_username;
		std::string_view	view_hostname;
		std::string_view	view_port;
		std::string_view	view_path;
		url_traits<char>{}.split(view_scheme, view_hostname, view_hostname, view_port, view_path, url);

		auto		hostname	= u8_to_wstring(view_hostname);
		auto		path		= u8_to_wstring(view_path);
		int			portNum		= url_traits<char>{}.port(view_scheme);
		if (!view_port.empty())
		{
			portNum	= url_traits<char>{}.port(view_port);
		}

		// セッション
		hSession	= hinternet(WinHttpOpen(
						L"libamtrs simple http/1.0",			// UserAgent
						WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
						WINHTTP_NO_PROXY_NAME,
						WINHTTP_NO_PROXY_BYPASS,
						0
					));
		if (hSession == nullptr)
		{
			os::win32::throw_system_error("WinHttpOpen");
		}

		// 接続相手
		hConnect	= hinternet(WinHttpConnect( 
						hSession.get(),
						hostname.c_str(),
						portNum,
						0
					));
		if (hSession == nullptr)
		{
			os::win32::throw_system_error("WinHttpConnect");
		}

		// リクエスト情報
		hRequest	=  hinternet(WinHttpOpenRequest( 
						hConnect.get(),
						L"GET",
						path.c_str(),
						nullptr,
						WINHTTP_NO_REFERER,
						WINHTTP_DEFAULT_ACCEPT_TYPES,
						0
					));
		if (hRequest == nullptr)
		{
			os::win32::throw_system_error("WinHttpSendRequest");
		}


		if (!WinHttpSendRequest( 
				hRequest.get(),
				WINHTTP_NO_ADDITIONAL_HEADERS,
				0,
				WINHTTP_NO_REQUEST_DATA,
				0, 0, 0
		))
		{
			os::win32::throw_system_error("WinHttpSendRequest");
		}
		if (!WinHttpReceiveResponse(hRequest.get(), nullptr))
		{
			os::win32::throw_system_error("WinHttpReceiveResponse");
		}

		DWORD 		dwBufferSize	= 0;

		// レスポンスコード
		DWORD		dwStatusCode = 0;
		dwBufferSize	= sizeof(dwStatusCode);
		WinHttpQueryHeaders(hRequest.get(),
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
			WINHTTP_HEADER_NAME_BY_INDEX, 
			&dwStatusCode, &dwBufferSize, WINHTTP_NO_HEADER_INDEX);
		mResponseCode	= (int)dwStatusCode;

		// Content-Length
		DWORD		dwContentLength = 0;
		dwBufferSize	= sizeof(dwContentLength);
		WinHttpQueryHeaders(hRequest.get(),
			WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, 
			WINHTTP_HEADER_NAME_BY_INDEX, 
			&dwContentLength, &dwBufferSize, WINHTTP_NO_HEADER_INDEX);
		mContentLength	= (std::intmax_t)dwContentLength;



		// ヘッダのサイズを取得する
		WinHttpQueryHeaders( 
			hRequest.get(),
			WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX, 
			nullptr, 
			&dwBufferSize,
			WINHTTP_NO_HEADER_INDEX
		);

		std::wstring	header;
		header.resize(std::size_t(dwBufferSize) / sizeof(typename std::wstring::value_type));
		WinHttpQueryHeaders(hRequest.get(),
			WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX,
			header.data(), &dwBufferSize,
			WINHTTP_NO_HEADER_INDEX
		);
		header.resize(header.size() - 1);	// remove \0

		auto			u8Header	= to_u8string<std::string>(header);
		mHeaders	= split_http_headers({u8Header.data(), u8Header.size()});

		std::string		body;
		do
		{
			DWORD	dwDownloaded;
			dwBufferSize = 0;
			WinHttpQueryDataAvailable(hRequest.get(), &dwBufferSize);
			if (dwBufferSize)
			{
				auto	s	= body.size();
				body.resize(s + std::size_t(dwBufferSize));
				WinHttpReadData(hRequest.get(), body.data() + s, dwBufferSize, &dwDownloaded);
				if (!dwDownloaded)
				{
					break;
				}
				body.resize(s + dwDownloaded);
			}
		} while (dwBufferSize > 0);

		mContentBody	= std::move(body);
	}

	friend	class simple_http;
};

inline ref<simple_http> simple_http::create()
{
	ref<simple_http_win32>	thiz	= new simple_http_win32();
	return	thiz;
}


AMTRS_NET_NAMESPACE_END
#endif
