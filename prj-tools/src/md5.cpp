/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#include <amtrs/crypto/md5.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
	bool	strmode	= false;
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
		while (cmd[0])
		{
			switch (*cmd++)
			{
				case 'q' :
					sQuickPrint	= true;
					break;
				case 'r' :
					sRevesePrint	= true;
					break;
				case 's' :
					strmode	= true;
					break;
			}
		}
	}
	if (!strmode && index < _argc && (std::string_view(_args[index]) == "-s"))
	{
		strmode	= true;
		++index;
	}

	if (strmode)
	{
		if (index >= _argc)
		{
			usage();
			return	0;
		}

		auto	string	= _args[index++];
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
		int		fd	= open(filename, O_RDONLY);
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
			auto	rs	= read(fd, buff, sizeof(buff));
			if (rs == 0)
			{
				break;
			}
			e.update((uint8_t*)buff, rs);
		}
		e.finish();
		close(fd);

		uint8_t	hash[16];
		char	str[33];
		e.get(hash);
		print_md5(hash, filename);
	}

	return	0;
}
