/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__type_traits__find_first_function_of__hpp
#define __libamtrs__type_traits__find_first_function_of__hpp
#include "def.hpp"
#include "void_type.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//!	@brief	指定する呼び出し可能な型を可変テンプレート引数の中から探します。
// ----------------------------------------------------------------------------
//!	find_first_function_of<目的の型, 型の一覧...>
//!
//!	using	ftype	= amtrs::find_first_function_of<FILE*(const char*, const char*), decltype(fclose), decltype(fopen)>;
//!	static_assert(ftype::value == 1, "no index");
//!	static_assert(std::is_same<FILE*(const char*, const char*), typename ftype::type>::value, "no type");
//!
//!	using	ftype_err	= amtrs::find_first_function_of<float, int, long>;
//!	static_assert(ftype_err::value == ~0, "no match test");
//!
// ----------------------------------------------------------------------------
template<class F, class... Types>
struct	find_first_function_of;



namespace find_first_function_of_impl
{
	template<class F, std::size_t I, class...>
	struct	_find_first_function_of;


	template<class F, std::size_t I, class Type>
	struct	_find_first_function_of<F, I, Type>
	{
	private:
		static constexpr bool	_same	= std::is_same<F, Type>::value;
	public:
		using							type	= typename std::conditional<_same, Type, void_type<F>>::type;
		static constexpr std::size_t	value	= _same ? I : ~0;
	};


	template<class F, std::size_t I, class Type, class... Types>
	struct	_find_first_function_of<F, I, Type, Types...>
	{
	private:
		static constexpr bool	_same	= std::is_same<F, Type>::value;
		using					_next	= _find_first_function_of<F, I+1, Types...>;
	public:
		using							type	= typename std::conditional<_same, Type, typename _next::type>::type;
		static constexpr std::size_t	value	= _same ? I : _next::value;
	};
} // namespace find_first_function_of_impl


template<class F, class... Types>
struct	find_first_function_of
		: find_first_function_of_impl::_find_first_function_of<F, 0, Types...>
{
};

AMTRS_NAMESPACE_END
#endif
