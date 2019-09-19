/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__variant__hpp
#define	__libamtrs__variant__hpp
#include <map>
#include <vector>


#if		!__has_include(<variant>)
#	define	AMTRS_USE_COMPATI_VARIANT		1
#elif	(AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS)
#	define	AMTRS_USE_COMPATI_VARIANT		1
#endif


#ifndef	AMTRS_USE_COMPATI_VARIANT
#include <variant>
#else
#include "inc/compati/variant.hpp"
#endif
#include "amtrs.hpp"
AMTRS_NAMESPACE_BEGIN


//! 配列とハッシュをサポートする variant の拡張です。
template<class... T>
struct	variantobj;


//! variantobjで使用する配列。
template<class... T>
struct	vararray;


//! variantobjで使用するハッシュ。
template<class... T>
struct	varassoc;



template<class... T>
struct	variantobj : public std::variant<T..., vararray<T...>, varassoc<T...>>
{
	using	_base_type	= std::variant<T..., vararray<T...>, varassoc<T...>>;

	using	array_type		= vararray<T...>;
	using	assoc_type		= varassoc<T...>;
	using	variant_type	= _base_type;

	using	_base_type::_base_type;
	using	_base_type::operator = ;

	variantobj(){}
	variantobj(const array_type& _rhs) : _base_type(variant_type(_rhs)) {}
	variantobj(const assoc_type& _rhs) : _base_type(variant_type(_rhs)) {}
	variantobj(array_type&& _rhs) : _base_type(variant_type(std::move(_rhs))) {}
	variantobj(assoc_type&& _rhs) : _base_type(variant_type(std::move(_rhs))) {}
};



template<class... T>
struct	vararray : public std::vector<variantobj<T...>>
{
	using	_base_type	= std::vector<variantobj<T...>>;
	using	_base_type::_base_type;
	using	_base_type::operator =;
};


template<class... T>
struct	varassoc : public std::map<std::string, variantobj<T...>>
{
	using	_base_type	= std::map<std::string, variantobj<T...>>;
	using	_base_type::_base_type;
	using	_base_type::operator =;

	// ===========================================================================
	//! 指定するキーが存在するか調べ、存在するなら値に対するポインタを返します。
	// ---------------------------------------------------------------------------
	//! 存在しない場合は nullptr を返します。
	// ---------------------------------------------------------------------------
	template<class NameT>
	auto has(NameT&& _name) -> variantobj<T...>*;

	// ===========================================================================
	//! 指定するキーが存在するか調べ、存在するなら値に対するポインタを返します。
	// ---------------------------------------------------------------------------
	//! 存在しない場合は nullptr を返します。
	// ---------------------------------------------------------------------------
	template<class NameT>
	auto has(NameT&& _name) const -> const variantobj<T...>*;

	// ===========================================================================
	//! 指定するキーが存在するか調べ、存在するなら値に対するポインタを返します。
	// ---------------------------------------------------------------------------
	//! 存在しない場合は nullptr を返します。
	// ---------------------------------------------------------------------------
	template<class Type, class NameT>
	auto has_if(NameT&& _name) -> Type*;

	// ===========================================================================
	//! 指定するキーが存在するか調べ、存在するなら値に対するポインタを返します。
	// ---------------------------------------------------------------------------
	//! 存在しない場合は nullptr を返します。
	// ---------------------------------------------------------------------------
	template<class Type, class NameT>
	auto has_if(NameT&& _name) const -> const Type*;
};


template<class... T>
template<class NameT>
auto varassoc<T...>::has(NameT&& _name) -> variantobj<T...>*
{
	auto	it	= _base_type::find(std::forward<NameT>(_name));
	auto	e	= _base_type::end();
	return	it != e
			? &it->second
			: nullptr;
}


template<class... T>
template<class NameT>
auto varassoc<T...>::has(NameT&& _name) const -> const variantobj<T...>*
{
	auto	it	= _base_type::find(std::forward<NameT>(_name));
	auto	e	= _base_type::end();
	return	it != e
			? &it->second
			: nullptr;
}


template<class... T>
template<class Type, class NameT>
auto varassoc<T...>::has_if(NameT&& _name) -> Type*
{
	auto	v	= has(std::forward<NameT>(_name));
	if (!v)
	{
		return	nullptr;
	}
	return	std::get_if<Type>(v);
}


template<class... T>
template<class Type, class NameT>
auto varassoc<T...>::has_if(NameT&& _name) const -> const Type*
{
	auto	v	= has(std::forward<NameT>(_name));
	if (!v)
	{
		return	nullptr;
	}
	return	std::get_if<Type>(v);
}

AMTRS_NAMESPACE_END
#endif
