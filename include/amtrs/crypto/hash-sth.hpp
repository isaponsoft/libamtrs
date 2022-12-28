/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__crypto__hash_sth__hpp
#define	__libamtrs__crypto__hash_sth__hpp
#include "../amtrs.hpp"
AMTRS_CRYPTO_NAMESPACE_BEGIN
template<class...>
struct	sth;


// ******************************************************************
// STring Hash
// ------------------------------------------------------------------
// 1. non-cryptographic hash function.
// 2. Can generate variable length hashes.
// 3. It's reasonably fast.
template<class T>
struct	sth<T>
{
	int		year;		// seed1
	int		date;		// seed2

	// year & date is hash seed.
	sth(int _year = 0x1977, int _date = 0x0823) : year(_year), date(_date) {}

	sth(sth const&) = default;
	sth& operator = (sth const&) = default;

	// 整数型のハッシュ値を計算します。
	//	compute<uint64_t>(str.begin(), str.end())
	template<class It>
	auto compute(It _first, It _end) noexcept -> T
	{
		return	 hasher(std::distance(_first, _end), sizeof(T), year, date).update(_first, _end);
	}

	// 指定する長さのバイナリ列としてハッシュ値を書き出します
	// ハッシュ値は T で指定する型で計算されます。
	//	std::string		str("Hello world.");
	//	char			buff[100];
	//	compute<uint64_t>(buff, sizeof(buff), str.begin(), str.end())
	template<class It>
	void compute(void* _dest, size_t _dstlen, It _src, It _srcend) noexcept
	{
		if (_dstlen == 0)
		{
			return;
		}

		auto	h		= hasher(std::distance(_src, _srcend), _dstlen, year, date);
		auto	dst		= reinterpret_cast<uint8_t*>(_dest);
		auto	dstend	= dst + _dstlen;

		// Generation base hash code with all characters.
		h.update(_src, _srcend);

		// Generation and store hash.
		auto	s		= _src;
		*dst	= 0;
		for (;;)
		{
			size_t	dstlen	= sizeof(T) < (dstend - dst) ? sizeof(T) : (dstend - dst);
			size_t	srclen	= dstlen <= (size_t)(_srcend-s) ? dstlen : (size_t)(_srcend-s);
			T		hash	= h.update(s, s + srclen);

			// First byte is addition to previous hash.
			dst[0] += (hash&0xff);
			hash >>= 8;

			// After secondary byte is direct store.
			for (size_t i = 1; i < dstlen; ++i)
			{
				dst[i] = (hash&0xff);
				hash >>= 8;
			}

			// Next
			if ((dst += sizeof(T) - 1) >=  dstend)	return;
			if ((s   += sizeof(T) - 1) >= _srcend)	s	= _src;
		}
	}

	struct	hasher
	{
		static constexpr uint16_t	tbl[8] = { 0x317e, 0xe2c5, 0x1e12, 0x2821, 0x35f2, 0x1813, 0x24c2, 0x1e76 };
		static constexpr auto baseval(uint16_t _n, size_t _len) -> T { return ((T)tbl[_len & 0x07] * (T)_n) + (_len > 0 ? baseval(_n, _len - 1) : 0); }

		static constexpr T	base1	= baseval(0x0d35, sizeof(T)*8);
		static constexpr T	base2	= baseval(0x1a93, sizeof(T)*8);
		static constexpr T	base3	= baseval(0x17cd, sizeof(T)*8);
		static constexpr T	base4	= baseval(0x2109, sizeof(T)*8);

		hasher(size_t _inSize, size_t _outSize, int _y, int _d) noexcept
			: year(_y)
			, date(_d)
			, h1((T)((T)base1 * (T)base2 * (T)base3 + _inSize + _outSize            ))
			, h2((T)((T)base2 * (T)base3 * (T)base4 +           _outSize + sizeof(T)))
			, h3((T)((T)base3                       + _inSize + _outSize + sizeof(T)))
		{}

		template<class It>
		auto update(It _first, It _end) noexcept -> T
		{
			for (auto it = _first; it != _end; ++it)
			{
				h1	= (h1 << 3) - h1 + *it;
				h2	+= h1 + h3 + year;
				h3	*= h1 + h2 - date;
			}
			return	 h1 + h2 + h3 + h2 * h3;
		}

		int		year;
		int		date;
		T		h1;
		T		h2;
		T		h3;
	};
};


template<class T, class C, size_t N>
struct	sth<T, C[N]> : sth<T>
{
	using	super_type	= sth<T>;
	using	super_type::super_type;

	struct	hash_type
	{
		C	hash[N];
		auto size() const noexcept -> size_t { return N; }
		auto compare(hash_type const& _r) const noexcept -> int;
		auto begin() const noexcept { return std::begin(hash); }
		auto end() const noexcept { return std::end(hash); }
		auto operator [] (intmax_t i) const noexcept -> C const& { return hash[i]; }
		auto operator == (hash_type const& _r) const noexcept { return compare(_r) == 0; }
		auto operator != (hash_type const& _r) const noexcept { return compare(_r) != 0; }
		auto operator <  (hash_type const& _r) const noexcept { return compare(_r) <  0; }
		auto operator <= (hash_type const& _r) const noexcept { return compare(_r) <= 0; }
		auto operator >  (hash_type const& _r) const noexcept { return compare(_r) >  0; }
		auto operator >= (hash_type const& _r) const noexcept { return compare(_r) >= 0; }
	};

	// 整数型のハッシュ値を計算します。
	//	compute<uint64_t>(str.begin(), str.end())
	template<class It>
	auto compute(It _first, It _end) noexcept -> hash_type
	{
		hash_type	h;
		super_type::compute(h.hash, h.size(), _first, _end);
		return		h;
	}
};


template<class T, class C, size_t N>
auto sth<T, C[N]>::hash_type::compare(hash_type const& _r) const noexcept -> int
{
	for (int i = 0; i < size(); ++i)
	{
		int r((int)hash[i] - (int)_r.hash[i]);
		if (r != 0)
		{
			return r;
		}
	}
	return	0;
}

AMTRS_CRYPTO_NAMESPACE_END
#endif
