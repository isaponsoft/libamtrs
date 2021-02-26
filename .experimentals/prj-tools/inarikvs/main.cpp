/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <amtrs/logging/@>
#include <inarikvs/inarikvs.hpp>
#include <inarikvs/storage/lowio.hpp>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>



std::string_view match_command(std::string_view _line, std::string_view _cmd)
{
	if (_line.substr(0, _cmd.size()) == _cmd)
	{
		return	_line.substr(0, _cmd.size());
	}
	return	_line.substr(0, 0);
}


std::string_view skip_space(std::string_view _line)
{
	auto*	cur	= _line.data();
	auto*	end	= cur + _line.size();
	while (cur != end && *cur == ' ')
	{
		++cur;
	}
	return	std::string_view(cur, end - cur);
}

bool is_key(char c)
{
	return	(c != '\r' && c != '\n' && c != ' ' && c != '\t' && c != 0);
}


std::string_view get_key(std::string_view _line)
{
	auto*	cur	= _line.data();
	auto*	end	= cur + _line.size();
	while (cur != end && is_key(*cur))
	{
		++cur;
	}
	return	_line.substr(0, cur - _line.data());
}


std::string_view is_return(std::string_view _line)
{
	auto*	cur	= _line.data();
	auto*	end	= cur + _line.size();
	if (cur == end || *cur++ != '\r')
	{
		return	_line.substr(0, 0);
	}
	if (cur == end || *cur++ != '\n')
	{
		return	_line.substr(0, 0);
	}
	return	_line.substr(0, 2);
}


int main()
{
	inarikvs::database<inarikvs::lowio>	db;
	db.load("testdb");

	for (;;)
	{
		std::cout << "> ";

		std::string	tmp;
		std::getline(std::cin, tmp);
		std::string_view	line(tmp);

		if (line == "exit")
		{
			db.flush();
			break;
		}

		if (line == "keys")
		{
			db.keys([](auto kv)
			{
				std::cout << kv->key() << std::endl;
				return	false;
			});
			continue;
		}

		if (line == "start")
		{
			db.start();
			continue;
		}

		if (line == "flush")
		{
			db.flush();
			continue;
		}


		if (std::string_view cmd = match_command(line, "get"); !cmd.empty())
		{
			line	= skip_space(line.substr(cmd.size()));
			if (cmd = is_return(line); cmd.empty())
			{
				// キーの指定がある
				line	= skip_space(line.substr(cmd.size()));
				auto	key	= get_key(line);
				if (key.empty())
				{
					std::cout << "(get command syntax error)" << std::endl;
					continue;
				}
				auto	v	= db.get(key);
				if (v)
				{
					std::cout << std::string(v->value()) << std::endl;
				}
				else
				{
					std::cout << "(nothing)" << std::endl;
				}
			}
			else
			{
				// キーの指定がないのでコマンド入力を受け付ける
				std::string	key;
				std::cout << "key> ";
				std::getline(std::cin, key);
				auto	v	= db.get(key);
				if (v)
				{
					std::cout << std::string(v->value()) << std::endl;
				}
				else
				{
					std::cout << "(nothing)" << std::endl;
				}
			}
			continue;
		}


		if (std::string_view cmd = match_command(line, "set"); !cmd.empty())
		{
			line	= skip_space(line.substr(cmd.size()));
			if (cmd = is_return(line); cmd.empty())
			{
				// キーの指定がある
				line	= skip_space(line.substr(cmd.size()));
				auto	key	= get_key(line);
				if (key.empty())
				{
					std::cout << "(set command syntax error)" << std::endl;
					continue;
				}
				line	= skip_space(line.substr(key.size()));
				auto	val	= get_key(line);
				if (val.empty())
				{
					std::cout << "(set command syntax error)" << std::endl;
					continue;
				}

				db.set(key, val);
				std::cout << "OK" << std::endl;
			}
			else
			{
				// キーの指定がないのでコマンド入力を受け付ける
				std::string	key;
				std::string	value;

				std::cout << "key> ";
				std::getline(std::cin, key);

				std::cout << "value> ";
				std::getline(std::cin, value);

				db.set(key, value);
				std::cout << "OK" << std::endl;
			}
			continue;
		}
	}

	return	0;
}
