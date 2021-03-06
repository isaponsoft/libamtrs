/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/crypto.hpp>
#include <amtrs/tests.hpp>
using namespace amtrs;

#define	_PRFX(_name)	crypto_##_name


static char hex(int n)
{
	return	n < 10
			? '0' + n
			: 'a' + n - 10;
}

static std::string hash_md5(char const* _txt)
{
	uint8_t	m[16];
	crypto::md5::compute(m, reinterpret_cast<uint8_t const*>(_txt), strlen(_txt));
	std::string	r;
	r.resize(32);
	for (int i = 0; i < 16; ++i)
	{
		r[i * 2    ]	= hex((m[i] >> 4) & 0x0f);
		r[i * 2 + 1]	= hex((m[i]     ) & 0x0f);
	}
	return	r;
}

AMTRS_TEST_F(_PRFX(md5))
{
	AMTRS_TEST_EQ("d41d8cd98f00b204e9800998ecf8427e", hash_md5(""));
	AMTRS_TEST_EQ("b96fc993d65b87059ed8de9a54e6b6f2", hash_md5("libamtrs"));
	AMTRS_TEST_EQ("5cd95d64cef16b2c8dde9c98a08ee78d", hash_md5("digest string"));
	AMTRS_TEST_EQ("764569e58f53ea8b6404f6fa7fc0247f", hash_md5("Hello world."));
	AMTRS_TEST_EQ("c3fcd3d76192e4007dfb496cca67e13b", hash_md5("abcdefghijklmnopqrstuvwxyz"));
	AMTRS_TEST_EQ("55032b1ba8bc84b3755818c8a48ea031", hash_md5("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"));
}

