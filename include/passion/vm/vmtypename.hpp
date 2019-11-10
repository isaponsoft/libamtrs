#ifndef	__passion__vm__vmtypename__hpp
#define	__passion__vm__vmtypename__hpp
#include "vmstring.hpp"
PASSION_NAMESPACE_BEGIN
struct	vmclass;




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


// ========================================================================
//!	型名を扱います。
// ------------------------------------------------------------------------
//!	vmstringに型に関する拡張機能を持たせ、型名であることを明確にするため
//! 強い型付けを行います。
//!
//! 型名にはポインタ記号や配列などの修飾子が入ります。
// ------------------------------------------------------------------------
struct	vmtypename : public vmstring
{
	//! プリミティブの場合、プリミティブのサイズを取得します。
	//! オブジェクト型の場合は 0 を返します。
	std::size_t primitive_size() const noexcept { return passion_size_of_primitive({this->data(), this->size()}); }

	// ========================================================================
	//! ^ * をひとつ取り除きます。
	// ------------------------------------------------------------------------
	//! ポインタではない場合は empty() を返します。
	// ------------------------------------------------------------------------
	vmtypename remove_pointer() const { return passion_remove_pointerable(*this); }
	vmtypename remove_pointer_if() const
	{
		if (passion_is_pointer(*this))
		{
			return	vmtypename(this->substr(0, this->size() - 1));
		}
		return	*this;
	}



	// ========================================================================
	//! 配列、ポインタ、参照などすべて取り除きます。
	// ------------------------------------------------------------------------
	vmtypename remove_all() const
	{
		vmtypename	retval	= passion_remove_reference(*this);
		for (;;)
		{
			vmtypename	c	= retval;
			retval	= passion_remove_array(retval);
			retval	= retval.remove_pointer();
			if (c == retval) { break; }
		}
		return	retval;
	}

	using	vmstring::vmstring;
	using	vmstring::operator =;
	using	vmstring::operator ==;
	using	vmstring::operator !=;
};


//! いずれ削除、プログラムの修正のための互換クラス
class	vmtype : public vmtypename
{
public:
	vmtype() = default;
	vmtype(const vmtype&) = default;
	vmtype& operator = (const vmtype&) = default;

	vmtype(vmtypename _name, vmclass* _clz)
		: vmtypename(std::move(_name))
		, mClass(_clz)
	{
		mTypename	= *this;
	}

	//! native class作成用
	static vmtype make_nativetype(vmtypename _name, std::size_t _size)
	{
		vmtype	ret(std::move(_name), nullptr);
		ret.mSize	= _size;
		return	ret;
	}

	vmclass* clazz() const noexcept { return mClass; }

	std::size_t size() const noexcept;


private:
	vmtypename	mTypename;
	vmclass*	mClass	= nullptr;
	std::size_t	mSize	= 0;
};






PASSION_NAMESPACE_END
#endif
