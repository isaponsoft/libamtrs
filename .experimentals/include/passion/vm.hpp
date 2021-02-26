/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__vm__hpp
#define	__libamtrs__passion__vm__hpp
#include "passion.hpp"
PASSION_NAMESPACE_BEGIN
using namespace amtrs;

class	vm;
struct	vmcontext;


template<class T>
struct	vm_traits
{
	using	string_type	= std::string;

	template<class Elm>
	using	array_type	= std::vector<Elm>;

	using	offset_type	= intptr_t; 
};

class	function_entry
{
public:
	virtual ~function_entry() {}
	virtual void run(vmcontext* _ctx) = 0;
};


struct	parameter_data : vm_traits<vm>
{
	string_type					name;		//!< 名称
	string_type					type;		//!< 型
};


//! 識別子を解析したときの情報
struct	identify_info : vm_traits<vm>
{
	string_type						name;		//!< 名称
	array_type<parameter_data>		paramters;	//!< 仮引数
};


struct	function : vm_traits<vm>
{
	using	destractor_type	= void();

	string_type			identify;
	function_entry*		function	= nullptr;
	void*				value		= nullptr;
	destractor_type*	destroy;
};


class	vmcontext : vm_traits<vm>
{
public:
	char*			stack		= nullptr;			//!< stack memory
	size_t			stackSize	= 0;
	offset_type		sp			= 0;				//!< stack pointer
	function_entry*	func		= nullptr;			//!< code segment
	offset_type		ip			= 0;				//!< instruction pointer(code segment offset)

	vmcontext() = default;
	vmcontext(vmcontext const&) = delete;
	vmcontext(vmcontext&& _r)
		: stack(_r.stack)
		, stackSize(_r.stackSize)
		, sp(_r.sp)
		, func(_r.func)
		, ip(_r.ip)
		{
		_r.stack		= nullptr;
		_r.stackSize	= 0;
		_r.sp			= sp;
		_r.func			= func;
		_r.ip			= ip;
	}

	~vmcontext()
	{
		clear();
	}

	void clear()
	{
		if (stack)
		{
			delete[]	stack;
		}
	}
};

PASSION_NAMESPACE_END
#endif
