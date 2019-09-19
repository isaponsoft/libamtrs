/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__type_utils__is_concept__hpp
#define __libamtrs__type_utils__is_concept__hpp
#include "is_match_conditions.hpp"
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//!	@brief	c++1z draft の候補に挙がった concept に似た機能を提供します。
// ----------------------------------------------------------------------------
//!	ConceptType には template<class T> operator (T) を持つ関数クラスを
//!	指定します。
//!	関数メソッドの中で調べたいプログラムを書くことでテストできます。
//
//!	例)
//!	struct	is_containor
//!	{
//!		template<class T>
//!		void operator () (T value)
//!		{
//!			for (auto i : value){}
//!		}
//!	};
//!	static_assert(sa::is_concept<std::vector<char>, is_containor>::value, "");	// ok
//!	static_assert(sa::is_concept<int              , is_containor>::value, "");	// assert
// ----------------------------------------------------------------------------
template<class Type, class ConceptType>
struct	is_concept
{
private:
	struct	conditions
	{
		template<class T>
		std::true_type operator () (std::in_place_type_t<T> i)
		{
			return	is_concept<ConceptType, Type>::check(i);
		}
	};

	template<class T>
	static std::true_type check(std::in_place_type_t<T>)
	{
		std::declval<ConceptType>()(std::declval<T>());
		return	std::true_type{};
	}

public:
	using	type	= typename is_match_conditions<Type, conditions>::type;

	static const bool	value	= type::value;
};


AMTRS_NAMESPACE_END
#endif
