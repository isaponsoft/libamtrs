/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__vmachine__hxx
#define	__libamtrs__lang__passion__vmachine__hxx
#include "pvlnode.hxx"
#include "vmmemorymap.hxx"
PSNVM_LANG_NAMESPACE_BEGIN

union	vmreg
{
	int8_t			v[sizeof(void*)];
	intmax_t		i;
	uintmax_t		u;
	void*			p;	// 実データのアドレス
	static_assert(sizeof(intmax_t) == sizeof(void*));
};


struct	vmvariable
{
	vdatatype		datatype;
	vmreg			value;

	vmvariable() = default;
	vmvariable(vmvariable const&) = default;
	vmvariable(vmvariable&&) = default;
	vmvariable& operator = (vmvariable const&) = default;
	vmvariable& operator = (vmvariable&&) = default;
	vmvariable(vdatatype _datatype)
		: datatype(std::move(_datatype))
	{}

	vmvariable(vdatatype _datatype, intmax_t _n)
		: datatype(std::move(_datatype))
	{
		value.i	= _n;
	}

	vmvariable(vdatatype _datatype, uintmax_t _n)
		: datatype(std::move(_datatype))
	{
		value.u	= _n;
	}

	bool operator == (vmvariable const& _r) const noexcept { return datatype == _r.datatype && value.i == _r.value.i; }
	bool operator != (vmvariable const& _r) const noexcept { return !(*this == _r); }
};


struct	vmachine
{
	using	memory_error_callback	= typeutil::callback<bool(void* mem, text location, bool write)>;

	vmachine(size_t _stacksize = 4096, size_t _stackdepth = 1024);
	void reset();

	static vmachine* current();

	void local_alloc(vdatatype _datatype);
	void local_free();

	auto run(pvlnode& _node) -> vmvariable { vmvariable r; run(r, _node); return r; }
	void run(vmvariable& _retval, pvlnode& _node);

	auto run(vdecl_function* _func, view<vmreg> _arguments) -> vmvariable { vmvariable r; run(r, _func, _arguments); return r; }
	void run(vmvariable& _retval, vdecl_function* _func, view<vmreg> _arguments);

	auto local(size_t _index) -> void*;

	void add_memory(void* m, size_t s);
	bool remove_memory(void* m);
	bool in_memory(void* m, size_t s);

	text bounds_error() const noexcept { return mBoundsError; }

	memory_error_callback	memory_error;

private:
	vmmemorymap			mMemory;
	view<vmreg>			mArguments;
	stackbuffer			mLocalvars;
	stackbuffer			mLocalSizes;
	bool				mReturn			= false;
	bool				mBreak			= false;
	text				mBoundsError	= nullptr;
};


PSNVM_LANG_NAMESPACE_END
#endif
