/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__string__hpp
#define	__passion__string__hpp
#ifdef	__cplusplus
#include <algorithm>
#include <unordered_set>
#include <ostream>
#include <string_view>
#include "typename_utils.h"
PASSION_NAMESPACE_BEGIN


template<class T>
T passion_remove_array(const T& _type)
{
	if (passion_is_array(_type))
	{
		return	T(_type.substr(0, _type.size() - PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE));
	}
	return	_type;
}


template<class T>
T passion_remove_reference(const T& _type)
{
	if (passion_is_reference(_type))
	{
		return	T(_type.substr(0, _type.size() - 1));
	}
	return	_type;
}


//! pointer, gc pointer を除去します。
template<class T>
T passion_remove_pointerable(const T& _type)
{
	if (passion_is_pointerable(_type))
	{
		return	T(_type.substr(0, _type.size() - 1));
	}
	return	_type;
}


template<class T>
T passion_add_pointer(const T& _type)
{
	return	passion_is_pointer(_type)
			? T(_type)
			: T(passion_remove_reference(_type) + "*");
}



template<class T>
T passion_add_reference(const T& _type)
{
	return	passion_is_reference(_type)
			? T(_type)
			: T(_type + "&");
}

// ============================================================================
//!	amtrs_string の c++ ラッパー。
// ----------------------------------------------------------------------------
class	vmstring : public amtrs::string_wrapper<vmstring>
{
public:
	using	super_type	= amtrs::string_wrapper<vmstring>;
	using	super_type::super_type;
	using	super_type::operator =;
	using	super_type::operator ==;
	using	super_type::operator !=;
	using	super_type::operator +;
	using	super_type::operator +=;
	using	super_type::operator amtrs_string_view;
	using	super_type::operator std::string_view;


	//! プリミティブの場合、プリミティブのサイズを取得します。
	//! オブジェクト型の場合は 0 を返します。
	std::size_t primitive_size() const noexcept { return passion_size_of_primitive({this->data(), this->size()}); }

	// ========================================================================
	//! ^ * をひとつ取り除きます。
	// ------------------------------------------------------------------------
	//! ポインタではない場合は empty() を返します。
	// ------------------------------------------------------------------------
	vmstring remove_pointer() const { return passion_remove_pointerable(*this); }
	vmstring remove_pointer_if() const
	{
		if (passion_is_pointer(*this))
		{
			return	vmstring(this->substr(0, this->size() - 1));
		}
		return	*this;
	}



	// ========================================================================
	//! 配列、ポインタ、参照などすべて取り除きます。
	// ------------------------------------------------------------------------
	vmstring remove_all() const
	{
		vmstring	retval	= passion_remove_reference(*this);
		for (;;)
		{
			vmstring	c	= retval;
			retval	= passion_remove_array(retval);
			retval	= retval.remove_pointer();
			if (c == retval) { break; }
		}
		return	retval;
	}

};

PASSION_NAMESPACE_END
namespace std {



template<>
struct	hash<::PASSION_NAMESPACE::vmstring>
{
	std::size_t operator () (::PASSION_NAMESPACE::vmstring const& _s) const noexcept
	{
		return	_s.hash();
	}
};



template<>
struct	hash<::PASSION_NAMESPACE::vmstring const>
{
	std::size_t operator () (::PASSION_NAMESPACE::vmstring const& _s) const noexcept
	{
		return	_s.hash();
	}
};

}
#endif	// __cplusplus
#endif
