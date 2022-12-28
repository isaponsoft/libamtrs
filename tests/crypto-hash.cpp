/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <chrono>
#include <iomanip>
#include <amtrs/tests.hpp>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/string/split_iterator.hpp>
#include <amtrs/string/text.hpp>
#include <amtrs/system/log.hpp>
#include <amtrs/crypto/hash-csx.hpp>
#include <amtrs/crypto/hash-sth.hpp>
#include <amtrs/crypto/.inc-crypto/crypto-md5.hpp>
#include <amtrs/crypto/.inc-crypto/crypto-sha256.hpp>
using namespace amtrs;

#define	_PRFX(_name)	crypto_hash_##_name


#define	DUMP			0

AMTRS_TEST_F(_PRFX(1_csx))
{
	text	t1("Hello world");
	text	t2("Hello worle");
	text	t3("Iello world");
	auto	h32_1	= crypto::csx<uint32_t>{}.compute(t1.begin(), t1.end());
	auto	h32_2	= crypto::csx<uint32_t>{}.compute(t2.begin(), t2.end());
	auto	h32_3	= crypto::csx<uint32_t>{}.compute(t3.begin(), t3.end());
#if	DUMP
	// Dump
	std::cout << std::endl;
	std::cout << std::hex << h32_1 << std::endl;
	std::cout << std::hex << h32_2 << std::endl;
	std::cout << std::hex << h32_3 << std::endl;
#endif

	auto	h64_1	= crypto::csx<uint64_t>{}.compute(t1.begin(), t1.end());
	auto	h64_2	= crypto::csx<uint64_t>{}.compute(t2.begin(), t2.end());
	auto	h64_3	= crypto::csx<uint64_t>{}.compute(t3.begin(), t3.end());
#if	DUMP
	// Dump
	std::cout << std::endl;
	std::cout << std::hex << h64_1 << std::endl;
	std::cout << std::hex << h64_2 << std::endl;
	std::cout << std::hex << h64_3 << std::endl;
#endif
}


AMTRS_TEST_F(_PRFX(2_uint))
{
	text	t1("Hello world");
	text	t2("Hello worle");
	text	t3("Iello world");
	text	t4("Hello world!!!!!");
	crypto::sth<uint32_t>{}.compute(t1.begin(), t1.end());
	crypto::sth<uint64_t>{}.compute(t1.begin(), t1.end());

	auto	ha11	= crypto::sth<uint64_t, uint8_t[16]>{}.compute(t1.begin(), t1.end());
	auto	ha12	= crypto::sth<uint64_t, uint8_t[16]>{}.compute(t2.begin(), t2.end());
	auto	ha13	= crypto::sth<uint64_t, uint8_t[16]>{}.compute(t3.begin(), t3.end());
	auto	ha14	= crypto::sth<uint64_t, uint8_t[16]>{}.compute(t1.begin(), t1.end());
	auto	ha15	= crypto::sth<uint64_t, uint8_t[16]>{}.compute(t4.data(), t4.data() + t1.size());

	auto	ha21	= crypto::sth<uint64_t, uint8_t[32]>{}.compute(t1.begin(), t1.end());
	auto	ha22	= crypto::sth<uint64_t, uint8_t[32]>{}.compute(t2.begin(), t2.end());
	auto	ha23	= crypto::sth<uint64_t, uint8_t[32]>{}.compute(t3.begin(), t3.end());

	AMTRS_TEST_FALSE(ha11 == ha12);
	AMTRS_TEST_FALSE(ha11 == ha13);
	AMTRS_TEST_FALSE(ha12 == ha13);
	AMTRS_TEST_TRUE(ha11 == ha14);
	AMTRS_TEST_TRUE(ha11 == ha15);

	AMTRS_TEST_TRUE(ha11 != ha12);
	AMTRS_TEST_TRUE(ha11 != ha13);
	AMTRS_TEST_TRUE(ha12 != ha13);
	AMTRS_TEST_FALSE(ha11 != ha14);
	AMTRS_TEST_FALSE(ha11 != ha15);

#if	DUMP
	// Dump
	std::cout << std::endl;
	std::cout << t1 << " : (128) "; for (auto& c : ha11.hash) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
	std::cout << t2 << " : (128) "; for (auto& c : ha12.hash) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
	std::cout << t3 << " : (128) "; for (auto& c : ha13.hash) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;

	std::cout << t1 << " : (256) "; for (auto& c : ha21.hash) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
	std::cout << t2 << " : (256) "; for (auto& c : ha22.hash) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
	std::cout << t3 << " : (256) "; for (auto& c : ha23.hash) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
#endif
}

AMTRS_TEST_F(_PRFX(3_char))
{
#if	DUMP
	std::cout << std::endl;
#endif

	text	t("Hello world");
	{
		char	buff[8] = {0};
		crypto::sth<uint32_t>{}.compute(buff, sizeof(buff), t.begin(), t.end());
#if	DUMP
		for (auto& c : buff) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
#endif
	}
	{
		char	buff[8] = {0};
		crypto::sth<uint64_t>{}.compute(buff, sizeof(buff), t.begin(), t.end());
#if	DUMP
		for (auto& c : buff) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
#endif
	}
	{
		char	buff[32] = {0};
		crypto::sth<uint32_t>{}.compute(buff, sizeof(buff), t.begin(), t.end());
#if	DUMP
		for (auto& c : buff) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
#endif
	}
	{
		char	buff[32] = {0};
		crypto::sth<uint64_t>{}.compute(buff, sizeof(buff), t.begin(), t.end());
#if	DUMP
		for (auto& c : buff) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
#endif
	}

	{
		char	buff1[32] = {0};
		char	buff2[32] = {0};	for (auto& c : buff2) { c = (char)0xCC; }
		crypto::sth<uint64_t>{}.compute(buff1, sizeof(buff1), t.begin(), t.end());
		crypto::sth<uint64_t>{}.compute(buff2, sizeof(buff2), t.begin(), t.end());

#if	DUMP
		for (auto& c : buff1) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
		for (auto& c : buff2) std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << ((int)c & 0xff); std::cout << std::endl;
#endif
		AMTRS_TEST_TRUE(std::equal(std::begin(buff1), std::end(buff1), std::begin(buff2)));
	}
}



// ********************************************************
// benchmark examples
#if	0

AMTRS_TEST_F(_PRFX(x_benchmark))
{
	static constexpr int	count	= 10000000;
	text	t("Hello world");
	
	std::cout << std::endl;

#if	0
	// MD5
	{
		uint8_t	buff[16];
		std::cout << "MD5(128)   : " << std::flush;
		auto	start	= std::chrono::system_clock::now();
		for (int i = 0; i < count; ++i)
		{
			crypto::md5::compute(buff, (uint8_t const*)t.data(), t.size());
		}
		auto	end		= std::chrono::system_clock::now();
		double	time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		std::cout << (time/1000.0) << "sec" << std::endl;
	}
#endif
#if	0
	// SHA-256
	{
		uint8_t	buff2[32];
		std::cout << "SHA256     : " << std::flush;
		auto	start	= std::chrono::system_clock::now();
		for (int i = 0; i < count; ++i)
		{
			crypto::sha256(buff2, (uint8_t const*)t.data(), t.size(), true);
		}
		auto	end		= std::chrono::system_clock::now();
		double	time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		std::cout << (time/1000.0) << "sec" << std::endl;
	}
#endif

	// STH32
	{
		std::cout << "STH32      : " << std::flush;
		auto	start	= std::chrono::system_clock::now();
		for (int i = 0; i < count; ++i)
		{
			crypto::sth<uint32_t>{}.compute(t.begin(), t.end());
		}
		auto	end		= std::chrono::system_clock::now();
		double	time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		std::cout << (time/1000.0) << "sec" << std::endl;
	}

	// STH64
	{
		std::cout << "STH64      : " << std::flush;
		auto	start	= std::chrono::system_clock::now();
		for (int i = 0; i < count; ++i)
		{
			crypto::sth<uint64_t>{}.compute(t.begin(), t.end());
		}
		auto	end		= std::chrono::system_clock::now();
		double	time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		std::cout << (time/1000.0) << "sec" << std::endl;
	}

	// STH64-16
	{
		std::cout << "STH64-128  : " << std::flush;
		char	buff[128 / 8];
		auto	start	= std::chrono::system_clock::now();
		for (int i = 0; i < count; ++i)
		{
			crypto::sth<uint64_t>{}.compute(buff, sizeof(buff), t.begin(), t.end());
		}
		auto	end		= std::chrono::system_clock::now();
		double	time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		std::cout << (time/1000.0) << "sec" << std::endl;
	}
	
	// STH64-256
	{
		std::cout << "STH64-256  : " << std::flush;
		char	buff[256 / 8];
		auto	start	= std::chrono::system_clock::now();
		for (int i = 0; i < count; ++i)
		{
			crypto::sth<uint64_t>{}.compute(buff, sizeof(buff), t.begin(), t.end());
		}
		auto	end		= std::chrono::system_clock::now();
		double	time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		std::cout << (time/1000.0) << "sec" << std::endl;
	}

	// STH32-2048
	{
		std::cout << "STH32-2048 : " << std::flush;
		char	buff[2048 / 8];
		auto	start	= std::chrono::system_clock::now();
		for (int i = 0; i < count; ++i)
		{
			crypto::sth<uint32_t>{}.compute(buff, sizeof(buff), t.begin(), t.end());
		}
		auto	end		= std::chrono::system_clock::now();
		double	time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		std::cout << (time/1000.0) << "sec" << std::endl;
	}

	// STH64-2048
	{
		std::cout << "STH64-2048 : " << std::flush;
		char	buff[2048 / 8];
		auto	start	= std::chrono::system_clock::now();
		for (int i = 0; i < count; ++i)
		{
			crypto::sth<uint64_t>{}.compute(buff, sizeof(buff), t.begin(), t.end());
		}
		auto	end		= std::chrono::system_clock::now();
		double	time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		std::cout << (time/1000.0) << "sec" << std::endl;
	}
}
#endif



// ********************************************************
// sth vs md5 vs sha256 hash test.
#if	0
static int	counts_sth[16] = {0};
static int	counts_md5[16] = {0};
static int	counts_sha[16] = {0};

void t(text _t)
{
	#define	MINLEN	0
	#define	MAXLEN	8
	union
	{
		uint64_t	n;
		uint8_t		buff1[8];
	};
	n	= crypto::sth<uint64_t>{}.compute(_t.begin(), _t.end());
	for (int i = MINLEN; i < MAXLEN; ++i)
	{
		counts_sth[(buff1[i]>>4) & 0xf]++;
		counts_sth[ buff1[i]     & 0xf]++;
	}

	uint8_t	buff[16];
	crypto::md5::compute(buff, (uint8_t const*)_t.data(), _t.size());
	for (int i = 0; i < 16; ++i)
	{
		counts_md5[(buff[i]     ) & 0xf]++;
		counts_md5[(buff[i] >> 4) & 0xf]++;
	}

	uint8_t	buff2[32];
	crypto::sha256(buff2, (uint8_t const*)_t.data(), _t.size(), true);
	for (int i = 0; i < 32; ++i)
	{
		counts_sha[(buff2[i]     ) & 0xf]++;
		counts_sha[(buff2[i] >> 4) & 0xf]++;
	}


}

AMTRS_TEST_F(_PRFX(x_check))
{
	std::cout << std::endl;
	for (int i = 0; i < 100; ++i)
	{
		auto	s	= format("{x}", {i});
		std::cout << std::hex << crypto::sth<uint64_t>{}.compute(s.begin(), s.end()) << " : " << (text)s << std::endl;
	}


#if	0
	for (int i = 0; i < 1000000; ++i)
	{
		t(format("{x}", {i}));
	}
	for (int i = 0; i < 1000000; ++i)
	{
		t(format("{X}", {i}));
	}
	for (int i = 0; i < 1000000; ++i)
	{
		t(format("{d}", {i}));
	}
#endif
#if	1
	// random binary
	for (int i = 0; i < 1000000; ++i)
	{
		uint8_t	buff[16];
		auto	s	= format("{d}", {i});
		crypto::md5::compute(buff, (uint8_t const*)s.data(), s.size());
		t(text((char*)buff, (char*)buff+16));
	}
#endif
#if	1
	{
		auto	txt	= filesystem::file_get_contents<sstring>("lower.lst");
		for (auto tx : split_chars(txt, "\n"))
		{
			t(tx);
		}
	}
	{
		auto	txt	= filesystem::file_get_contents<sstring>("mixed.lst");
		for (auto tx : split_chars(txt, "\n"))
		{
			t(tx);
		}
	}
	{
		auto	txt	= filesystem::file_get_contents<sstring>("lower-j.lst");
		for (auto tx : split_chars(txt, "\n"))
		{
			t(tx);
		}
	}
#endif
	std::cout << "==============" << std::endl;
	unsigned int	min_sth	= ~0;
	unsigned int	max_sth	= 0;
	unsigned int	min_md5	= ~0;
	unsigned int	max_md5	= 0;
	unsigned int	min_sha	= ~0;
	unsigned int	max_sha	= 0;
	for (int i = 0; i < 16; ++i)
	{
		auto	ch	= (unsigned)counts_sth[i];
		min_sth	= min_sth < ch ? min_sth : ch;
		max_sth	= max_sth > ch ? max_sth : ch;

		auto	cm	= (unsigned)counts_md5[i];
		min_md5	= min_md5 < cm ? min_md5 : cm;
		max_md5	= max_md5 > cm ? max_md5 : cm;

		auto	cs	= (unsigned)counts_sha[i];
		min_sha	= min_sha < cs ? min_sha : cs;
		max_sha	= max_sha > cs ? max_sha : cs;

		std::cout << std::hex << i << ":" << std::dec << ch << ":" << (cm/2) << ":" << (cs/4) << std::endl;
	}
	std::cout << "STH:" << std::fixed << std::setprecision(2) << (100.0 * (double)min_sth / (double)max_sth) << "%" << std::endl;
	std::cout << "MD5:" << std::fixed << std::setprecision(2) << (100.0 * (double)min_md5 / (double)max_md5) << "%" << std::endl;
	std::cout << "SHA:" << std::fixed << std::setprecision(2) << (100.0 * (double)min_sha / (double)max_sha) << "%" << std::endl;
}
#endif
