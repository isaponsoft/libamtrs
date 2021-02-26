/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libcurry__hash__hpp
#define	__libcurry__hash__hpp
#include "def.hpp"
#include "typeutils.hpp"
CURRY_BEGIN


using	hash_t		= decltype(std::hash<int>{}(0));

namespace implements_hash
{
	struct	has_hash_code
	{
		template<class T> auto operator () (std::in_place_type_t<T>) -> decltype(&T::hash_code, std::true_type{});
		template<class T> auto operator () (hash_t (T::*)() const) -> decltype(std::true_type{});
	};

	template<class T, class S = void>
	struct	get_hash
	{
		hash_t operator () (const T& _v)
		{
			return	std::hash<T>{}(_v);
		}
	};

	template<class T>
	struct	get_hash<T, typename std::enable_if<is_match_conditions<T, has_hash_code>::value>::type>
	{
		hash_t operator () (const T& _v)
		{
			return	_v.hash_code();
		}
	};
}

// ============================================================================
// 拡張されたハッシュの取得メソッド
// ----------------------------------------------------------------------------
// std::hash<> を拡張するラッパーです。
//	cs::hash_t = cs::hash(値);
//	値.hash_code() でハッシュの取得を試みます。
//	それが呼び出し不可能なら std::hash<T>{}(値) でハッシュの取得を試みます。
// ----------------------------------------------------------------------------
template<class T>
inline hash_t hash_code(const T& _value)
{
	return	implements_hash::get_hash<T>{}(_value);
}


template<class Algorithm, class T>
inline hash_t hash_code(const hash_t& _hash, const T& _value)
{
	return	Algorithm::add(_hash, _value);
}



namespace hash_algorithm
{
	// ========================================================================
	// http://www.isapon.com/
	// ------------------------------------------------------------------------
	struct	csx2
	{
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	   ((static_cast<HashType>(1)<<(sizeof(HashType) * 8 -  1)) /      3)
					^  ((static_cast<HashType>(1)<<(sizeof(HashType) * 8 -  9)) /   1721)
					^  ((static_cast<HashType>(1)<<(sizeof(HashType) * 8 - 11)) /     37)
					^ ~((static_cast<HashType>(1)<<(sizeof(HashType) * 8 -  3)) / 112121)
					+  ((static_cast<HashType>(1)<<(sizeof(HashType) - 1     )) /      7);
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	_hash + (_c - '0')
					+ (_hash << (sizeof(HashType) * 8 / (2 + (0 * 3))))
					+ (_hash << (sizeof(HashType) * 8 / (2 + (1 * 3))))
			;
		}
	};

	// ========================================================================
	// http://www.isapon.com/
	// ------------------------------------------------------------------------
	struct	csx2a
	{
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	csx2::get_default<HashType>();
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	_hash ^ (_c - '0')
					+ (_hash << (sizeof(HashType) * 8 / (2 + (0 * 3))))
					+ (_hash << (sizeof(HashType) * 8 / (2 + (1 * 3))))
			;
		}
	};

	// ========================================================================
	// http://www.cse.yorku.ca/~oz/hash.html
	// ------------------------------------------------------------------------
	struct	djb2
	{
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	5381;
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	(_hash * 33) + _c;
		}
	};

	// ========================================================================
	// http://www.cse.yorku.ca/~oz/hash.html
	// ------------------------------------------------------------------------
	struct	djb2a
	{
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	5381;
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	(_hash * 33) ^ _c;
		}
	};
#if 0
	// ========================================================================
	// http://www.isthe.com/chongo/tech/comp/fnv/index.html
	// ------------------------------------------------------------------------
	struct	fnv1
	{
	private:
		template<class HashType>
		inline static constexpr HashType compute_prime(int _bit1, int _bit2, HashType _base)
		{
			return	(static_cast<HashType>(1)<<_bit1) + (static_cast<HashType>(1)<<_bit2) + _base;
		}

	public:
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	get_basis<HashType>();
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	(_hash * get_prime<HashType>()) ^ _c;
		}

		template<class HashType>
		inline static constexpr HashType get_basis()
		{
			return	  (sizeof(HashType) * 8 ==   32) ? static_cast<HashType>(static_cast<uint32_t>(2166136261))
					: (sizeof(HashType) * 8 ==   64) ? static_cast<HashType>(static_cast<uint64_t>(14695981039346656037U))
#if 0
					: (sizeof(HashType) ==  128) ? 144066263297769815596495629667062367629
					: (sizeof(HashType) ==  256) ? 100029257958052580907070968620625704837092796014241193945225284501741471925557
					: (sizeof(HashType) ==  512) ? 9659303129496669498009435400716310466090418745672637896108374329434462657994582932197716438449813051892206539805784495328239340083876191928701583869517785
					: 14197795064947621068722070641403218320880622795441933960878474914617582723252296732303717722150864096521202355549365628174669108571814760471015076148029755969804077320157692458563003215304957150157403644460363550505412711285966361610267868082893823963790439336411086884584107735010676915
#endif
					: 2166136261;
		}

		template<class HashType>
		inline static constexpr HashType get_prime()
		{
			return	  (sizeof(HashType) * 8 ==   32) ? compute_prime( 24, 8, 0x93)
					: (sizeof(HashType) * 8 ==   64) ? compute_prime( 40, 8, 0xb3)
					: (sizeof(HashType) * 8 ==  128) ? compute_prime( 88, 8, 0x3b)
					: (sizeof(HashType) * 8 ==  256) ? compute_prime(168, 8, 0x63)
					: (sizeof(HashType) * 8 ==  512) ? compute_prime(344, 8, 0x57)
					: (sizeof(HashType) * 8 == 1024) ? compute_prime(680, 8, 0x8d)
					: compute_prime( 24, 8, 0x93);
		}
	};


	// ========================================================================
	// http://www.isthe.com/chongo/tech/comp/fnv/index.html
	// ------------------------------------------------------------------------
	struct	fnv1a
	{
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	fnv1::get_default<HashType>();
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	(_hash ^ _c) * fnv1::get_prime<HashType>();
		}
	};
#endif
	// ========================================================================
	// http://www.isthe.com/chongo/tech/comp/fnv/index.html
	// ------------------------------------------------------------------------
	struct	java
	{
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	0;
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	(_hash * 31) + _c;
		}
	};

	// ========================================================================
	// http://www.cse.yorku.ca/~oz/hash.html
	// ------------------------------------------------------------------------
	struct	lose_lose
	{
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	0;
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	_c + _hash;
		}
	};

	// ========================================================================
	// http://www.cse.yorku.ca/~oz/hash.html
	// ------------------------------------------------------------------------
	struct	sdbm
	{
		template<class HashType>
		inline static constexpr HashType get_default()
		{
			return	0;
		}

		template<class HashType, class CharType>
		inline static constexpr HashType add(HashType& _hash, const CharType& _c)
		{
			return	_c + (_hash << 6) + (_hash << 16) - _hash;
		}
	};
}



CURRY_END
#endif
