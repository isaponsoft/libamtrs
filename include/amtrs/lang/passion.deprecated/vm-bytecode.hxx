/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__vm_bytecode__hpp
#define	__libamtrs__lang__passion__vm_bytecode__hpp
#include "types.hxx"
#include "vm.hpp"
PSNVM_NAMESPACE_BEGIN


struct	operand_nothing
{
	sstring		_nouse1;
	sstring		_nouse2;
	vmreg		_nouse3;
};

struct	operand_label
{
	sstring		name;
	sstring		_nouse1;
	vmreg		_nouse2;
};

struct	operand_alloc
{
	sstring		datatype;
	sstring		name;
	vmreg		_nouse2;
};

// メンバへのオフセットを示す整数に変換される
struct	opecode_moff
{
	sstring		datatype;
	sstring		name;
	vmreg		_nouse2;
};

struct	opecode_reg_push
{
	sstring		name;
	sstring		_nouse2;
	vmreg		regidx;
};

struct	opecode_reg_pop
{
	sstring		_nouse1;
	sstring		_nouse2;
	vmreg		regidx;
};

struct	operand_lea
{
	sstring		datatype;
	sstring		name;
	vmreg		_nouse1;
};

struct	operand_lea_r
{
	sstring		reg;
	sstring		name;
	vmreg		off;
};

struct	operand_load
{
	sstring		datatype;
	sstring		_nouse1;
	vmreg		_nouse2;
};

struct	operand_push_int
{
	sstring		datatype;
	sstring		_nouse1;
	intmax_t	imm;
};

struct	operand_push_str
{
	sstring		datatype;
	sstring		imm;
	vmreg		_nouse1;
};

struct	operand_sizeof
{
	sstring		datatype;
	sstring		_nouse1;
	intmax_t	_nouse2;
};

struct	operand_operator_bin
{
	sstring		datatype;
	sstring		_nouse1;
	vmreg		_nouse2;
};

struct	operand_call
{
	sstring		name;
	sstring		signature;
	void*		func;			// 関数のキャッシュ
};

struct	operand_jmp
{
	sstring		label;
	sstring		_nouse1;
	intmax_t	offset;
};

struct	bytecode
{
	opecode		cmd;
	union
	{
		struct
		{
			sstring		operand1, operand2;
			vmreg		operand3;
		}						_;
		operand_label			label;
		operand_alloc			alloc_local;
		operand_alloc			free_local;
		operand_lea				lea;
		operand_lea_r			lea_r;
		operand_lea_r			load_r;
		opecode_moff			moff;
		opecode_reg_push		reg_push;
		opecode_reg_push		reg_pop;
		operand_load			load;
		operand_load			store;
		operand_push_int		push_int;
		operand_push_str		push_str;
		operand_sizeof			size_of;
		operand_operator_bin	operator_bin;
		operand_call			call;
		operand_nothing			ret;
		operand_jmp				jmp;
	};

	bytecode()
	{
		new (&_.operand1) sstring();
		new (&_.operand2) sstring();
		_.operand3.p	= nullptr;
	}
	bytecode(bytecode const& _r)
	{
		cmd	= _r.cmd;
		new (&_.operand1) sstring(_r._.operand1);
		new (&_.operand2) sstring(_r._.operand2);
		_.operand3.p	= _r._.operand3.p;
	}
	bytecode(bytecode&& _r)
	{
		cmd	= _r.cmd;
		new (&_.operand1) sstring(std::move(_r._.operand1));
		new (&_.operand2) sstring(std::move(_r._.operand2));
		_.operand3.p	= _r._.operand3.p;
	}
	bytecode& operator = (bytecode const& _r)
	{
		clear();
		cmd	= _r.cmd;
		new (&_.operand1) sstring(_r._.operand1);
		new (&_.operand2) sstring(_r._.operand2);
		_.operand3.p	= _r._.operand3.p;
		return	*this;
	}
	bytecode& operator = (bytecode&& _r)
	{
		clear();
		cmd	= _r.cmd;
		new (&_.operand1) sstring(std::move(_r._.operand1));
		new (&_.operand2) sstring(std::move(_r._.operand2));
		_.operand3.p	= _r._.operand3.p;
		return	*this;
	}
	~bytecode() { clear(); }

	void clear()
	{
		_.operand1.clear();
		_.operand1.clear();
	}
};

PSNVM_NAMESPACE_END
#endif
