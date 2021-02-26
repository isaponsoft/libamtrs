/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__vmstrobj__hpp
#define	__passion__vmstrobj__hpp
#include "vmobject.hpp"
#include "vmshared_data.hpp"
PASSION_NAMESPACE_BEGIN

// ****************************************************************************
//! 実行時に使用する文字列オブジェクト
// ----------------------------------------------------------------------------
class	vmstrobj
		: public vmobject::vmnative
{
	using	_base_type	= vmobject::vmnative;
public:
	static constexpr auto signature() { return make_constring("psn::string"); }

	const char* c_str() const noexcept
	{
		return	reinterpret_cast<const char*>(this + 1);
	}

	std::size_t size() const noexcept { return mSize; }
	
	std::size_t hash() const noexcept { return mHash; }

	std::string_view view() const noexcept { return {c_str(), size()}; }

	auto begin() const noexcept { return c_str(); }

	auto end() const noexcept { return c_str() + size(); }

	static ref<vmobj> create(std::string_view _text)
	{
		vmvariant	arg[2]	= {
			vmvariant(vmvalue((void*)_text.data())),
			vmvariant((vmvalue)_text.size())
		};
		return	vmobject::create(mClazz, arg, 2);
	}

	static void set_string_class(vmclass* _clz) { mClazz = _clz; }

private:
	static std::size_t native_size(vmclass::argument_type* _argv, std::size_t _size)
	{
		(void)_size;
		return	sizeof(vmstrobj) + static_cast<std::size_t>(std::get<vmvalue>(_argv[1]).i + 1);
	}


	static vmnative* native_new(void* _native, void* _extra, vmclass::argument_type* _argv, std::size_t _size)
	{
		(void)_size;
		vmstrobj*	thiz	= new (_native) vmstrobj();


		const char*		str		= (const char*)std::get<vmvalue>(_argv[0]).p;
		int				len		= (int)std::get<vmvalue>(_argv[1]).i;

		thiz->mString	= reinterpret_cast<char*>(_extra);
		std::copy(str, str + len, thiz->mString);
		thiz->mString[len]	= 0;
		thiz->mSize	= len;
		thiz->mHash	= std::hash<std::string_view>{}(std::string_view{ thiz->c_str(), thiz->size() });
		return	thiz;
	}


	static void native_functions(vmclass_manager* _vm, vmclass* _clazz);

	static inline vmclass*	mClazz;

	std::size_t	mSize	= 0;
	std::size_t	mHash	= 0;
	char*		mString	= nullptr;

	friend	struct vmclass;
};




PASSION_NAMESPACE_END
#endif
