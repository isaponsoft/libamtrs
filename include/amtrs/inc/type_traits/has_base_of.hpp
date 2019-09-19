/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__type_utils__has_base_of__hpp
#define __libamtrs__type_utils__has_base_of__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN



// ****************************************************************************
//! Checkerで指定するベースクラスを継承しているか調査します。
// ----------------------------------------------------------------------------
//! example)
//!		struct	check_my_base_class
//!		{
//!			template<class... B>
//!			using base_type		= my_base_class<B...>;
//!		};
//!		
//!
//!		template<class... B>
//!		class	my_class : my_base_class<B...>
//!		{};
//!
//!		static_assert(has_base_of<my_class<int>, check_my_base_class>::value);
// ============================================================================
template<class T, class Checker>
struct	has_base_of
{
private:
	template<class... B>
	static constexpr std::true_type chk(const typename Checker::template base_type<B...>&) noexcept;
	static constexpr std::false_type chk(...) noexcept;
public:
	using	type	= decltype( chk(std::declval<T>()) );
	static constexpr type	value;
};



#if	0
EXAMPLE)

struct	base{};

template<class T>	struct	base2 : T {};
template<class T>	struct	base3 : T {};
template<class T>	struct	base4 : T {};


struct	__has_base2 { template<class... B> using base_type = base2<B...>; };
struct	__has_base4 { template<class... B> using base_type = base4<B...>; };



template<class T>	using	has_base2	= has_base_of<T, __has_base2>;
template<class T>	using	has_base4	= has_base_of<T, __has_base4>;


int main()
{
	using	cls	= base3<base2<base<int>>>;
	static constexpr auto	v1	= has_base2<cls>::value;
	std::cout << v1 << std::endl;

	static constexpr auto	v2	= has_base4<cls>::value;
	std::cout << v2 << std::endl;

	return	0;
}
#endif


AMTRS_NAMESPACE_END
#endif
