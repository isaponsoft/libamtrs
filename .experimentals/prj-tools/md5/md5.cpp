/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/crypto/md5.hpp>
#include <amtrs/io/functions.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string_view>


// constexpr に MD5 を求め、結果が正しいか検証する。
constexpr bool test() noexcept
{
	uint8_t		in[10]		= { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd' };
	uint8_t		hw_md5[16]	= { 0x68, 0xe1, 0x09, 0xf0, 0xf4, 0x0c, 0xa7, 0x2a, 0x15, 0xe0, 0x5c, 0xc2, 0x27, 0x86, 0xf8, 0xe6 };
	uint8_t		md5[16]		= {0};
	amtrs::crypto::md5::compute(md5, in);
	for (int i = 0; i < 16; ++i)
	{
		if (hw_md5[i] != md5[i])
		{
			return	false;
		}
	}
	return	true;
}
static_assert(test() == true, "MD5 Algorithm error.");


static bool	sRevesePrint	= false;
static bool sQuickPrint		= false;

void usage()
{
	fprintf(stderr, "usage: md5 [-q] [-s string] [files ...]\n");
}


void print_md5(uint8_t (&_md5)[16], const char* _name, bool _quote = false)
{
	char	str[33];
	char	c[16]	= { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' }; 
	for (int i = 0; i < 16; ++i)
	{

		str[i*2 + 0] = c[(_md5[i]>>4)&0x0f];
		str[i*2 + 1] = c[(_md5[i]  )&0x0f];
	}
	str[32]	= 0x00;
	if (!sQuickPrint && !sRevesePrint)
	{
		if (_quote)
		{
			printf("MD5 (\"%s\") = ", _name);
		}
		else
		{
			printf("MD5 (%s) = ", _name);
		}
	}
	printf("%s", str);
	if (!sQuickPrint && sRevesePrint)
	{
		if (_quote)
		{
			printf(" \"%s\"", _name);
		}
		else
		{
			printf(" %s", _name);
		}
	}
	printf("\n");
}


int main(int _argc, char* _args[])
{
	bool		strmode	= false;
	char const*	string	= nullptr;
	if (_argc == 1)
	{
		usage();
		return	0;
	}

	int			index	= 1;
	char const*	cmd		= _args[index];
	if (cmd[0] == '-')
	{
		++index;
		++cmd;
		while (cmd[0])
		{
			if (cmd[0] == 's')
			{
				strmode	= true;
				if (_args[index])
				{
					string	= _args[index];
					++index;
				}
				break;
			}
			switch (*cmd++)
			{
				case 'q' :
					sQuickPrint	= true;
					break;
				case 'r' :
					sRevesePrint	= true;
					break;
			}
		}
	}

	if (!strmode && index < _argc && (std::string_view(_args[index]) == "-s"))
	{
		strmode	= true;
		++index;
		if (index >= _argc)
		{
			usage();
			return	0;
		}
		string	= _args[index++];
	}

	if (strmode)
	{
		uint8_t	md5[16];
		auto	in	= std::string_view(string);
		amtrs::crypto::md5::compute(md5, (uint8_t const*)in.data(), in.size());
		print_md5(md5, string, true);

		if (index >= _argc)
		{
			return	0;
		}
	}

	while (index < _argc)
	{
		auto*		filename	= _args[index++];
		struct stat	fst;
		int		fd	= amtrs::io::open(filename, amtrs::io::_RDONLY);
		if (fd == -1)
		{
			fprintf(stderr, "md5: %s: No such file or directory\n", filename);
			continue;
		}
		fstat(fd, &fst);
		
		amtrs::crypto::md5	e;
		for (;;)
		{
			char	buff[1024];
			auto	rs	= amtrs::io::read(fd, buff, sizeof(buff));
			if (rs == 0)
			{
				break;
			}
			e.update((uint8_t*)buff, rs);
		}
		e.finish();
		amtrs::io::close(fd);

		uint8_t	hash[16];
	//	char	str[33];
		e.get(hash);
		print_md5(hash, filename);
	}

	return	0;
}
