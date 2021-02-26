/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/string/starts_with.hpp>
#include <psnvm/.compiler-codereader.hpp>
#include <psnvm/.compiler-token.hpp>
#include <iostream>
PASSION_NAMESPACE_BEGIN


bool codereader::skip_blunk()
{
	while (!empty())
	{
		if (front() == ' ' || front() == '\t' || front() == '\r')
		{
			next();
			continue;
		}
		if (front() == '\n')
		{
			next();
			continue;
		}
		if ((size() >= 3)
		 && ((((uint8_t const*)data())[0]) == 0xFE)
		 && ((((uint8_t const*)data())[1]) == 0xBB)
		 && ((((uint8_t const*)data())[2]) == 0xBF)
		)
		{
			next(3);
			continue;
		}

		auto	pos	= position();
		auto	m	= match_starts({ "//", "/*" });
		if (m == "//")
		{
			// 改行コードが見つかるまでスキップ
			while (!empty())
			{
				auto	c	= front();
				next();
				if (c == '\n'|| c == '\r')
				{
					break;
				}
			}
			continue;
		}
		if (m == "/*")
		{
			//  */ が見つかるまでスキップ
			while (match_starts({"*/"}) != "*/")
			{
				if (empty())
				{
					// エラー
					position(pos);
					return	!empty();
				}
				next();
			}
			continue;
		}
		break;
	}
	return	!empty();
}


bool codereader::skip_space()
{
	while (!empty())
	{
		if (front() == ' ' || front() == '\t')
		{
			next();
			continue;
		}
		if ((size() >= 3)
		 && ((((uint8_t const*)data())[0]) == 0xFE)
		 && ((((uint8_t const*)data())[1]) == 0xBB)
		 && ((((uint8_t const*)data())[2]) == 0xBF)
		)
		{
			next(3);
			continue;
		}

		auto	pos	= position();
		auto	m	= match_starts({ "//", "/*" });
		if (m == "//")
		{
			// 改行コードが見つかるまでスキップ
			while (!empty())
			{
				auto	c	= front();
				next();
				if (c == '\n'|| c == '\r')
				{
					break;
				}
			}
			continue;
		}
		if (m == "/*")
		{
			//  */ が見つかるまでスキップ
			while (match_starts({"*/"}) != "*/")
			{
				if (empty())
				{
					// エラー
					position(pos);
					return	!empty();
				}
				next();
			}
			continue;
		}

		break;
	}
	return	!empty();
}



auto codereader::read_raw_string() noexcept -> std::pair<std::string_view, std::string_view>
{
	static constexpr std::string_view	none{nullptr, 0};
	if (empty())
	{
		return	{ none, none };
	}

	auto		pos		= position();
	char const*	begin	= data();
	if (front() != 'R')
	{
		return	{ none, none };
	}
	next();
	if (empty() || front() != '"')
	{
		position(pos);
		return	{ none, none };
	}
	next();

	// delimiterを検出
	const char*	delms	= data();
	for (;;)
	{
		if (empty())
		{
			position(pos);
			return	{ none, none };
		}
		if (front() == '(')
		{
			break;
		}
		next();
	}
	std::string_view	delm{delms, (size_t)(data() - delms)};
	next();


	// 終端までスキップ
	for (;;)
	{
		if (empty())
		{
			position(pos);
			return	{ none, none };
		}
		if (front() == ')')
		{
			next();
			if (empty())
			{
				position(pos);
				return	{ none, none };
			}
			if (!match_starts({delm}).empty() && !empty() && front() == '"')
			{
				break;
			}
		}
		next();
	}

	auto	all	= std::string_view(begin, (size_t)(data() - begin));
	auto	txt	= std::string_view(begin, (size_t)(data() - begin)).substr(delm.size() + 3, all.size() - (4 + delm.size()*2));
	return	{ all, txt };
}


auto codereader::read_hiredociment() noexcept -> std::string_view
{
	static constexpr std::string_view	none{nullptr, 0};
	if (empty())
	{
		return	none;
	}

	auto		pos		= position();
	char const*	begin	= data();
	if (match_starts({"DOC\""}).empty())
	{
		return	none;
	}

	// delimiterを検出
	const char*	delms	= data();
	for (;;)
	{
		if (empty())
		{
			position(pos);
			return	none;
		}
		if (front() == '(')
		{
			break;
		}
		next();
	}
	std::string_view	delm{delms, (size_t)(data() - delms)};
	next();


	// 終端までスキップ
	for (;;)
	{
		if (empty())
		{
			position(pos);
			return	none;
		}
		if (front() == ')')
		{
			next();
			if (empty())
			{
				position(pos);
				return	none;
			}
			if (!match_starts({delm}).empty() && !empty() && front() == '"')
			{
				break;
			}
		}
		next();
	}

	return	{begin, (size_t)(data() - begin)};
}


auto codereader::read_line() -> std::string_view
{
	char const*	lineBegin	= data();
	while (!empty())
	{
		if (front() == '\n')
		{
			next();
			break;
		}
		auto	[token, token_type] = read(~0);
		if (token_type != codereader::unkown_type)
		{
			continue;
		}
		next();
	}
	return	{lineBegin, (size_t)(data() - lineBegin)};
}


auto codereader::read(unsigned int _types) -> std::pair<std::string_view, read_types>
{
	constexpr std::pair<std::string_view, read_types>	none({nullptr, 0}, unkown_type);

	char const*	begin	= data();
	auto		pos		= position();

	// 文字列を読み取る
	if (_types & string_type)
	{
		// ヒアドキュメント
		auto	doc	= read_hiredociment();
		if (!doc.empty())
		{
			return	{ doc, string_type };
		}


		// c++11/RAW文字列
		auto	rs	= read_raw_string();
		if (!rs.first.empty())
		{
			return	{ rs.first, string_type };
		}


		if (front() == '"')
		{
			next();
			for (;;)
			{
				if (empty())
				{
					position(pos);
					return	none;
				}
				auto	c	= front();
				next();
				if (c == '"')
				{
					break;
				}
			}
			return	{ {begin, (size_t)(data() - begin)}, string_type };
		}
	}

	// マクロ名を読み取る
	if (_types & macroname_type)
	{
		if (auto s = is_identify_first(data(), size()); s > 0)
		{
			do
			{
				next(s);
				s	= (!empty() && front() == '.')
					? 1
					: is_identify_secondary(data(), size());
			} while (s > 0);
			return	{ {begin, (size_t)(data() - begin)}, macroname_type };
		}
	}


	// 識別子を読み取る
	if (_types & identify_type)
	{
		if (auto s = is_identify_first(data(), size()); s > 0)
		{
			do
			{
				next(s);
			} while ((s = is_identify_secondary(data(), size())) > 0);
			return	{ {begin, (size_t)(data() - begin)}, identify_type };
		}
	}


	// 数値を読み取る
	if (_types & numeric_type)
	{
		auto	c = front();
		if (c >= '1' && c <= '9')
		{
			// 整数(or実数)
			next();
			while (!empty())
			{
				c = front();
				if (c >= '0' && c <= '9')
				{
					next();
					continue;
				}
				break;
			}
			if (!empty() && front() == '.')
			{
				// 実数
				next();
				while (!empty())
				{
					c = front();
					if (c >= '0' && c <= '9')
					{
						next();
						continue;
					}
					break;
				}
				if (!empty() && front() == 'f')
				{
					next();
				}
			}
			else if (_types & integer_type)
			{
				return	{ {begin, (size_t)(data() - begin)}, integer_type };
			}

			return	{ {begin, (size_t)(data() - begin)}, numeric_type };
		}
	}

	if (_types & integer_type)
	{
		auto	c = front();
		if (c == '0')
		{
			next();
			if (!empty() && c >= '1' && c <= '9')
			{
				position(pos);
			}
			else
			{
				return	{ {begin, (size_t)(data() - begin)}, integer_type };
			}
		}
		else if (c >= '1' && c <= '9')
		{
			next();
			while (!empty())
			{
				c = front();
				if (c >= '0' && c <= '9')
				{
					next();
					continue;
				}
				break;
			}
			return	{ {begin, (size_t)(data() - begin)}, integer_type };
		}
	}

	return	none;
}


std::string_view codereader::match_starts(std::initializer_list<std::string_view> _tokens)
{
	std::string_view	retval(nullptr, 0);
	for (auto& t : _tokens)
	{
		if (t.size() > retval.size() && size() >= t.size())
		{
			if (amtrs::starts_with(std::string_view(*this), t))
			{
				retval	= {data(), t.size()};
			}
		}
	}
	if (retval.size() > 0)
	{
		next(retval.size());
	}
	return	retval;
}



PASSION_NAMESPACE_END
