/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__vm_datatype__hpp
#define	__libamtrs__lang__passion__vm_datatype__hpp
PSNVM_NAMESPACE_BEGIN
using namespace AMTRS_NAMESPACE;

using	vmint		= intmax_t;
using	vmptr		= void*;

union	vmreg
{
	intmax_t	i;
	double		d;
	void*		p;


	vmreg() = default;
	vmreg(vmreg const&) = default;
	vmreg& operator = (vmreg const&) = default;
	vmreg(vmint _n) : i(_n) {}
	vmreg(void* _p) : p(_p) {}
	vmreg& operator = (vmint _n){ i = _n; return *this; }
	vmreg& operator = (void* _p){ p = _p; return *this; }
	operator vmint () const noexcept { return i; }
	operator void* () const noexcept { return p; }
};


// call を行った時にスタックに退避される戻り位置
struct	abi_returnpoint
{
	intmax_t	ip;			// function内のoffsetになる
	vmptr		function;
};


/*
 * 可変長引数の型を示しているか調べます。
 * "..." もしくは "..." で終わっている型の時 true です。
 */
bool is_vaargs(text _datatype);

/*
 * 可変長引数の型の場合は ... を取り除きます。
 */
text trim_vaargs(text _datatype);



bool is_scalar(text _datatype);
bool is_pointer(text _datatype);
bool is_reference(text _datatype);
bool is_pointer_reference(text _datatype);
text remove_pointer(text _datatype);
text remove_pointer_reference(text _datatype);
text remove_reference(text _datatype);

/*
 * 名前空間を連結します。
 * _"a::_b" を返します。
 */
sstring namespace_cat(text _a, text _b);


PSNVM_NAMESPACE_END
#endif
