/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__crypto__base64__hpp
#define	__libamtrs__crypto__base64__hpp
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
constexpr std::size_t base64_encode(OutCursor _dest, InCursor _s, std::size_t _n)
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
