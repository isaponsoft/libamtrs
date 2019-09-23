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
#ifndef	__libamtrs__crypto__base64__hpp
#define	__libamtrs__crypto__base64__hpp
#include "def.hpp"
AMTRS_CRYPTO_NAMESPACE_BEGIN

// ==================================================================
//!	@brief	base64 の符号化を行った際に必要なバッファのサイズを返します。
// ------------------------------------------------------------------
constexpr std::size_t base64_encode_size(std::size_t _n)
{
	return	((_n + 2) / 3) * 4;
}

// ==================================================================
//!	@brief	base64 の符号化を行います。
// ------------------------------------------------------------------
template<class OutCursor, class InCursor>
amtrs_constexpr14 std::size_t base64_encode(OutCursor _dest, InCursor _s, std::size_t _n)
{
	auto	dest	= cursor::make_put(_dest);
	auto	src		= cursor::make_get(_s);
	auto	length	= base64_encode_size(_n);

	constexpr typename decltype(dest)::value_type	table[]	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

	// ３文字単位で処理
	const std::size_t	loopCount	= _n / 3;
	for (std::size_t i = 0; i < loopCount; ++i)
	{
		// Original character.
		auto	o1	= src.get();
		auto	o2	= src.get();
		auto	o3	= src.get();
		// Encoded character.
		auto	e1	=  (o1>>2) & 0x3f;
		auto	e2	= ((o1<<4) & 0x30) | ((o2>>4) & 0x0f);
		auto	e3	= ((o2<<2) & 0x3c) | ((o3>>6) & 0x03);
		auto	e4	=  (o3     & 0x3f);
		dest.put(table[e1]);
		dest.put(table[e2]);
		dest.put(table[e3]);
		dest.put(table[e4]);
	}

	// ３文字に満たない端数
	const std::size_t	modCount	= _n % 3;
	if (modCount >= 1)
	{
		// Original character.
		auto	o1	=                 src.get();
		auto	o2	= modCount >= 2 ? src.get() : 0;
		auto	o3	= modCount >= 3 ? src.get() : 0;
		// Encoded character.
		auto	e1	=  (o1>>2) & 0x3f;
		auto	e2	= ((o1<<4) & 0x30)
					| (modCount >= 2 ? ((o2>>4) & 0x0f) : (0));
		auto	e3	= modCount >= 2
					? (((o2<<2) & 0x3c) | ((o3>>6) & 0x03))
					: 64;
		auto	e4	= modCount >= 3
					? (o3     & 0x3f)
					: 64;
		dest.put(table[e1]);
		dest.put(table[e2]);
		dest.put(table[e3]);
		dest.put(table[e4]);
	}

	return	length;
}



AMTRS_CRYPTO_NAMESPACE_END
#endif
