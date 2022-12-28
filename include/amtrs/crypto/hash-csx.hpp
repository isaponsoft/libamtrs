/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__crypto__hash_csx__hpp
#define	__libamtrs__crypto__hash_csx__hpp
#include "../amtrs.hpp"
AMTRS_CRYPTO_NAMESPACE_BEGIN

template<class T>
struct	csx
{
	using	hash_type	= T;

	template<class It>
	constexpr auto compute(It _first, It _last) const noexcept -> hash_type
	{
		hash_type	h	= get_default();
		for (auto it = _first; it != _last; ++it)
		{
			h	= update(h, *it);
		}
		return	h;
	}


	constexpr T get_default() const noexcept
	{
		return	   ((static_cast<T>(1)<<(sizeof(T) * 8 -  1)) /      3)
				^  ((static_cast<T>(1)<<(sizeof(T) * 8 -  9)) /   1721)
				^  ((static_cast<T>(1)<<(sizeof(T) * 8 - 11)) /     37)
				^ ~((static_cast<T>(1)<<(sizeof(T) * 8 -  3)) / 112121)
				+  ((static_cast<T>(1)<<(sizeof(T) - 1     )) /      7);
	}

	template<class C>
	constexpr T update(T& _hash, C const& _c) const noexcept
	{
		return	
				((_hash + _c - 113    ) << shift1)
			^	((_hash               ) >> shift2)
			^	((_hash + _c + (_c<<3))          )
		;
	}

	static constexpr int	shift1	= sizeof(hash_type) * 8 - 8;
	static constexpr int	shift2	= sizeof(hash_type) * 8 / 13;
};

AMTRS_CRYPTO_NAMESPACE_END
#endif
