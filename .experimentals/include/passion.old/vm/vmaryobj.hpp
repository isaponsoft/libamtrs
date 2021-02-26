/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__vmaryobj__hpp
#define	__passion__vmaryobj__hpp
#include "vmobject.hpp"
#include "vmvariable_trits.hpp"
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! 実行時に使用する配列オブジェクト
// ----------------------------------------------------------------------------
class	vmaryobj
		: public vmobject::vmnative
{
	using	_base_type	= vmobject::vmnative;
public:
	static constexpr auto signature() { return make_constring("[]"); }

	std::size_t size() const noexcept { return mSize; }
	std::size_t stride() const noexcept { return mStride; }
	
	static ref<vmobj> create(std::size_t _stride, std::size_t _size)
	{
		vmvariant	arg[2]	= {
			vmvariant(vmvalue(_stride)),
			vmvariant(vmvalue(_size))
		};
		return	vmobject::create(mClass, arg, 2);
	}

	void* data() noexcept { return mArray; }
	const void* data() const noexcept { return mArray; }

	static void set_array_class(vmclass* _class) { mClass = _class; }

protected:
	template<class Derived>
	static std::size_t _native_size(vmclass::argument_type* _argv, std::size_t /*_size*/)
	{
		return	sizeof(Derived) + (size_t)std::get<vmvalue>(_argv[0]).i * (size_t)std::get<vmvalue>(_argv[1]).i;
	}

	template<class Derived>
	static Derived* _native_new(void* _native, void* _extra, vmclass::argument_type* _argv, std::size_t /*_size*/)
	{
		Derived*		thiz		= new (_native) Derived();
		thiz->mStride	= (size_t)std::get<vmvalue>(_argv[0]).i;;
		thiz->mSize		= (size_t)std::get<vmvalue>(_argv[1]).i;
		thiz->mArray	= _extra;
		return	thiz;
	}


protected:
	static std::size_t native_size(vmclass::argument_type* _argv, std::size_t _size)
	{
		return	_native_size<vmaryobj>(_argv, _size);
	}

	static vmobject::vmnative* native_new(void* _native, void* _extra, vmclass::argument_type* _argv, std::size_t _size)
	{
		return	_native_new<vmaryobj>(_native, _extra, _argv, _size);
	}


	static void native_functions(vmclass_manager* _vm, vmclass* _clazz);


	static inline	vmclass*	mClass;

	std::size_t	mSize	= 0;
	std::size_t	mStride	= 0;
	void*		mArray	= nullptr;

	friend	struct vmclass;
};




PASSION_NAMESPACE_END
#endif
