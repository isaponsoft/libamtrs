/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__crypto__hash_djb2__hpp
#define	__libamtrs__crypto__hash_djb2__hpp
#include "../amtrs.hpp"
AMTRS_CRYPTO_NAMESPACE_BEGIN

// ========================================================================
// http://www.cse.yorku.ca/~oz/hash.html
// ------------------------------------------------------------------------
template<class T>
struct	djb2
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


	constexpr hash_type get_default() const noexcept
	{
		return	5381;
	}

	template<class C>
	constexpr hash_type update(hash_type& _hash, C const& _c) const noexcept
	{
		return	(_hash * 33) + _c;
	}
};


AMTRS_CRYPTO_NAMESPACE_END
#endif
