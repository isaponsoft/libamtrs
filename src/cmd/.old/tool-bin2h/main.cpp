/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <fstream>

using namespace AMTRS_NAMESPACE;


static char hex(int n)
{
	return	n < 10
			? '0' + n
			: 'a' + n - 10;
}


int main(int _argc, char** _args)
{
	if (_argc != 2)
	{
		return	1;
	}

	std::string	filename	= _args[1];

	std::string	bin;
	if (!filesystem::file_get_contents(bin, filename))
	{
		std::cerr << "Can't read " << filename << std::endl;
		return	1;
	}

	int		count	= 0;
	std::cout << "/* " << filename << " size = " << bin.size() << " */\n";
	for (auto& b : bin)
	{
		std::cout << "0x";
		std::cout << hex((b >> 4) & 0xf);
		std::cout << hex((b     ) & 0xf);
		std::cout << ", ";
		if (++count == 16)
		{
			std::cout << "\n";
			count = 0;
		}
	}
	if (count)
	{
		std::cout << "\n";
	}
	return	0;
}
