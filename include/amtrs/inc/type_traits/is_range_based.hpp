/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__type_traits__range_based__hpp
#define	__libamtrs__type_traits__range_based__hpp
#include "void_t.hpp"
AMTRS_NAMESPACE_BEGIN

// ==================================================================
//!	@brief	range-based for loop が可能かどうか調べます。
// ------------------------------------------------------------------
template<class T, class = void_t<>>
struct	is_range_based
		: std::false_type
{};


template<class T>
struct	is_range_based<T,
			void_t<decltype(
				*std::declval<T>().begin(),										// oprator * ()
				++std::begin(std::declval<T>()) == std::end(std::declval<T>())	// begin() && end() && operator ++ ()
				, std::true_type{}
			)>>
		: std::true_type
{};

AMTRS_NAMESPACE_END
#endif
