/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__vmarygcobj__hpp
#define	__passion__vmarygcobj__hpp
#include "vmobject.hpp"
#include "vmaryobj.hpp"
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! 実行時に使用する配列オブジェクト
// ----------------------------------------------------------------------------
class	vmarygcobj
		: public vmaryobj
{
	using	_base_type	= vmaryobj;
public:
	static constexpr auto signature() { return make_constring("^[]"); }

	static void set_array_class(vmclass* _class) { mClass = _class; }

	
	static ref<vmobj> create(std::size_t _stride, std::size_t _size)
	{
		vmvariant	arg[2]	= {
			vmvariant(vmvalue(_stride)),
			vmvariant(vmvalue(_size))
		};
		return	vmobject::create(mClass, arg, 2);
	}

private:
	static void native_functions(vmclass_manager* _vm, vmclass* _clazz);

	static inline	vmclass*	mClass;

	friend	struct vmclass;
};




PASSION_NAMESPACE_END
#endif
