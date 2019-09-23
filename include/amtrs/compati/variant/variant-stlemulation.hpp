/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__variant__variant__stlemulation__hpp
#define	__libamtrs__variant__variant__stlemulation__hpp
namespace std
{
	template<class... Types>
	using	variant					= ::amtrs::variant<Types...>;

	template<class T>
	using	variant_size			= ::amtrs::variant<T>;

	template<size_t I, class T>
	using	variant_alternative		= ::amtrs::variant_alternative<I, T>;

	template<size_t I, class T>
	using	variant_alternative_t	= ::amtrs::variant_alternative_t<I, T>;

	template <class T, class... Types>
	constexpr bool holds_alternative(const variant<Types...>& _v) noexcept { return ::amtrs::holds_alternative<T, Types...>(_v); }

	using	bad_variant_access		= ::amtrs::bad_variant_access;

	constexpr std::size_t	variant_npos = ::amtrs::variant_npos;
}
#endif
