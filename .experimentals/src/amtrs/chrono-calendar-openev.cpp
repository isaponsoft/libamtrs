/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/chrono.hpp>
#include <amtrs/net/http/@>
#include <amtrs/string.hpp>
AMTRS_CHRONO_NAMESPACE_BEGIN

// ==================================================================
//! OpenEVカレンダー
// ------------------------------------------------------------------
//! iCalendarよりシンプルで機能の少ないカレンダー配信のための
//! プロトコル
// ------------------------------------------------------------------
class	openev_calendar
		: public chrono::calendar
{
public:
	std::string			mUrl;
	std::string			mTitle;
	net::mini_http		mHttp;
	std::vector<event>	mEvents;

	openev_calendar(std::string _url, std::string _title)
		: mUrl(std::move(_url))
		, mTitle(std::move(_title))
	{}

	virtual std::string identify() const override
	{
		return	mUrl;
	}

	virtual std::string name() const override
	{
		return	mTitle;
	}

	virtual std::string account() const override
	{
		return	{};
	}

	virtual std::string owner_account() const override
	{
		return	{};
	}

	virtual rgb<uint8_t> color() const override
	{
		return	{0, 0, 0};
	}

	virtual void events(std::function<void(event const&)> _callback) override
	{
		for (auto& e : mEvents)
		{
			_callback(e);
		}
	}

	virtual std::string insert(event const& _e) override
	{
		return	{};
	}

	virtual bool update(event const& _e) override
	{
		return	false;
	}

	virtual bool erase(std::string const& _identify) override
	{
		return	false;
	}

	virtual bool writable() const override
	{
		return	false;
	}


	bool connect()
	{
		std::string	s;
		auto		l	= make_listener<net::mini_http>([&](auto e)
					{
						if constexpr (std::is_same<decltype(e), net::mini_http::http_body_recieve_event>::value)
						{
							s += e.data;
						}
					});

		mHttp	= net::mini_http(mUrl);
		if (mHttp.empty())
		{
			return	false;
		}

		mHttp.set_listener(&l);
		mHttp.request();
		if (mHttp.empty())
		{
			return	false;
		}

		mEvents	= {};

		event	e;
		bool	commentMode	= false;
		for (auto line : split_chars(trim(std::string_view(s), " \t"), "\n"))
		{
			line	= trim(line, "\r\n\t");
			if (line.empty() || line[0] == '#') { continue; }

			auto [ cmd, value ] = parse_command_and_value(line);
			if (cmd.empty())
			{
				if (commentMode)
				{
					e.description	+= std::string("\n");
					e.description	+= line;
				}
				continue;
			}
			if (cmd == "@ITEM")
			{
				e	= {};
				continue;
			}

			if (cmd == "@ID")
			{
				e.identify	= value;
				continue;
			}

			if (cmd == "@TITLE")
			{
				e.title	= value;
				continue;
			}

			if (cmd == "@URL")
			{
				e.url	= value;
				continue;
			}

			if (cmd == "@DATETIME")
			{
				std::time_t	v;
				chrono::bedatetime(&v, value);
				e.start	=  chrono::timev(v);
				continue;
			}

			if (cmd == "@DETAIL")
			{
				commentMode		=  true;
				e.description	+= value;
				continue;
			}

			if (cmd == "@END")
			{
				mEvents.push_back(e);
				commentMode	= false;
				e			= {};
				continue;
			}
		}
		return	true;
	}

	static std::pair<std::string_view, std::string_view> parse_command_and_value(std::string_view _line)
	{
		using namespace amtrs;

		auto	line	= trim(_line, "\n\r\t ");
		if (line.empty())
		{
			return	{};
		}
		if (line[0] != '@')
		{
			return	{};
		}
		auto	pos		= line.find_first_of(" \t");
		if (pos == line.npos)
		{
			return	{ line, {} };
		}
		std::string_view	cmd	=      line.substr(0,     pos);
		std::string_view	val	= trim(line.substr(pos+1      ), " \t");
		return	{ cmd, val };
	}
};


ref<calendar> calendar::openev_with_url(std::string _url, std::string _title)
{
	ref<openev_calendar>	thiz	= new openev_calendar(std::move(_url), std::move(_title));
	thiz->connect();
	return	thiz;
}


AMTRS_CHRONO_NAMESPACE_END
