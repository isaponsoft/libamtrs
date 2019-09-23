/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__context__h
#define	__passion__context__h
#include "function.h"
#include "variant.h"
PASSION_EXTERN_C_BEGIN

enum	context_result
{
	cr_finish,
	cr_continue,
	cr_yield,
};


/*
 * passion virtual machine context.
 */
struct	passion_context
{
	/*
	 * Virtual machine registers.
	 */
	passion_register			ip;			/* instruction pointer */
	passion_register			sp;			/* stack pointer */
	passion_register			ap;			/* argument pointer */
	passion_register			lp;			/* local pointer .. base pointer */
	passion_register			ep;			/* exception pointer */

	/*
	 * Virtual machine segment register.
	 */
	struct passion_function*	cs;			/* code segment */
	void*						ds;			/* data segment = global data. */
	union passion_variant*		ss;			/* stack segment */




#ifdef	__cplusplus
	void* variables(std::intmax_t _offset = 0) noexcept { return reinterpret_cast<void*>(reinterpret_cast<std::intmax_t>(ds) + _offset); }

	void push(std::intmax_t _number)			{ push(passion_variant(_number)); }
	void push(psn::vmint_t _number)				{ push(passion_variant(_number)); }
	void push(psn::vmobj* _obj)					{ push(passion_variant(_obj));    }
	void push(struct passion_function* _func)	{ push(passion_variant(_func));	  }
	void push(passion_variant _value)			{ new (&ss[--sp]) passion_variant(std::move(_value)); }


	const passion_variant& top() const			{ return ss[sp]; }
	psn::vmint_t pop_integer()					{ return ss[sp++].i; }
	passion_variant pop()						{ return std::move(ss[sp++]); }
	psn::vmobj* pop_object()					{ return reinterpret_cast<psn::vmobj*>(pop().p); }
	struct passion_function* pop_function()		{ return reinterpret_cast<struct passion_function*>(pop().p); }
	passion_variant pop_value()					{ return pop(); }

	bool pop_context()
	{
		#ifdef	AMTRS_TRACE_LOG
		if (lp > sp)
		{
			// ローカル変数の削除忘れチェック
			AMTRS_TRACE_LOG("compler error. local variable deallocation error. lp = %zd > sp = %zd", lp, sp);
		}
		#endif
		cs		= pop_function();
		lp		= pop_integer();
		ap		= pop_integer();
		ip		= pop_integer();
		return	true;
	}



	context_result run()
	{
		for (;;)
		{
			auto			func		= cs;
			if (!func)
			{
				break;
			}
			#ifdef	AMTRS_TRACE_LOG
			PSNVMBC_TRACE_LOG("run: %s ; sp=%p:%zd, ap=%p:%zd, lp=%p:%zd", func->symbol.txt, &top(), sp, arguments(), ap, locals(), lp);
			#endif
			auto	r	= (context_result)func->function(this, func->data);
			if (r == cr_yield)
			{
				return	r;
			}
			if (r == cr_finish)
			{
				return	r;
			}
		}
		return	cr_finish;
	}


	// ========================================================================
	//! 呼び出しを行う。
	// ------------------------------------------------------------------------
	void call(struct passion_function* _function, std::size_t _argumentSize)
	{
		auto	nextAp	= sp + _argumentSize;

		// 現在の状態をスタックへ退避
		push(ip);
		push(ap);
		push(lp);
		push(cs);

		// 呼び出し先の状態をコンテキストへ設定
		ip		= 0;
		ap		= nextAp;
		lp		= sp;
		cs		= _function;
	}


	// **************************************************************
	// 引数
	// **************************************************************
	passion_variant& argument_value(int _n) noexcept { return *argument_address(_n); }
	passion_variant* arguments() noexcept { return &ss[ap - 1]; }
	passion_variant* argument_address(std::size_t _n) noexcept { return arguments() + _n; }

	// **************************************************************
	// ローカル変数
	// **************************************************************
	passion_variant* locals() noexcept { return &ss[lp - 1]; }
	passion_variant& local_value(std::intmax_t _n) noexcept { return locals()[-_n]; }
	void* local_address(std::intmax_t _n) noexcept { return &locals()[-_n]; }

	void* stack_address(std::intmax_t _n) noexcept { return &ss[sp - _n - 1]; }



	// **************************************************************
	// バイトコード読み取り
	// **************************************************************
	int8_t read_int8(const uint8_t* _bytecodes) { return static_cast<int8_t>(_bytecodes[ip++]); }
	uint8_t read_uint8(const uint8_t* _bytecodes) { return _bytecodes[ip++]; }

	int16_t read_int16(const uint8_t* _bytecodes)
	{
		uint16_t	retval	= _bytecodes[ip++];
		retval |= _bytecodes[ip++] <<  8;
		return	static_cast<int16_t>(retval);
	}


	uint16_t read_uint16(const uint8_t* _bytecodes)
	{
		uint16_t	retval	= _bytecodes[ip++];
		retval |= _bytecodes[ip++] <<  8;
		return	static_cast<uint16_t>(retval);
	}


	int32_t read_int32(const uint8_t* _bytecodes)
	{
		uint32_t	retval	= _bytecodes[ip++];
		retval |= _bytecodes[ip++] <<  8;
		retval |= _bytecodes[ip++] << 16;
		retval |= _bytecodes[ip++] << 24;
		return	static_cast<int32_t>(retval);
	}


	uint32_t read_uint32(const uint8_t* _bytecodes)
	{
		uint32_t	retval	= _bytecodes[ip++];
		retval |= _bytecodes[ip++] <<  8;
		retval |= _bytecodes[ip++] << 16;
		retval |= _bytecodes[ip++] << 24;
		return	static_cast<uint32_t>(retval);
	}

#endif
};


/*
 * initialize context.
 */
inline void passion_context_initialize(struct passion_context* _context)
{
	_context->ip	= 0;
	_context->sp	= 0;
	_context->ap	= 0;
	_context->lp	= 0;
	_context->ep	= 0;

	_context->cs	= passion_null;
	_context->ds	= passion_null;
	_context->ss	= passion_null;
}


/*
 * read bytecode value.
 */
inline uint8_t passion_bc_read_u8(struct passion_context* _context, const void* _bytecode)
{
	return	((const uint8_t*)_bytecode)[_context->ip++];
}

inline uint16_t passion_bc_read_u16(struct passion_context* _context, const void* _bytecode)
{
	_context->ip += 2;
	return	((uint16_t)((const uint8_t*)_bytecode)[_context->ip-2]     )
		|	((uint16_t)((const uint8_t*)_bytecode)[_context->ip-1] << 8)
	;
}

inline uint32_t passion_bc_read_u32(struct passion_context* _context, const void* _bytecode)
{
	_context->ip += 4;
	return	((uint32_t)((const uint8_t*)_bytecode)[_context->ip-4]     )
		|	((uint32_t)((const uint8_t*)_bytecode)[_context->ip-3] << 8)
		|	((uint32_t)((const uint8_t*)_bytecode)[_context->ip-2] <<16)
		|	((uint32_t)((const uint8_t*)_bytecode)[_context->ip-1] <<24)
	;
}

inline uint64_t passion_bc_read_u64(struct passion_context* _context, const void* _bytecode)
{
	_context->ip += 8;
	return	((uint64_t)((const uint8_t*)_bytecode)[_context->ip-8]     )
		|	((uint64_t)((const uint8_t*)_bytecode)[_context->ip-7] << 8)
		|	((uint64_t)((const uint8_t*)_bytecode)[_context->ip-6] <<16)
		|	((uint64_t)((const uint8_t*)_bytecode)[_context->ip-5] <<24)
		|	((uint64_t)((const uint8_t*)_bytecode)[_context->ip-4] <<32)
		|	((uint64_t)((const uint8_t*)_bytecode)[_context->ip-3] <<40)
		|	((uint64_t)((const uint8_t*)_bytecode)[_context->ip-2] <<48)
		|	((uint64_t)((const uint8_t*)_bytecode)[_context->ip-1] <<56)
	;
}

inline int8_t passion_bc_read_i8(struct passion_context* _context, const void* _bytecode)
{
	return	(int8_t)passion_bc_read_u8(_context, _bytecode);
}

inline int16_t passion_bc_read_i16(struct passion_context* _context, const void* _bytecode)
{
	return	(int16_t)passion_bc_read_u16(_context, _bytecode);
}

inline int32_t passion_bc_read_i32(struct passion_context* _context, const void* _bytecode)
{
	return	(int32_t)passion_bc_read_u32(_context, _bytecode);
}

inline int64_t passion_bc_read_i64(struct passion_context* _context, const void* _bytecode)
{
	return	(int64_t)passion_bc_read_u64(_context, _bytecode);
}

inline float passion_bc_read_float(struct passion_context* _context, const void* _bytecode)
{
	union
	{
		uint32_t	i;
		float		f;
	}	v;
	v.i	= passion_bc_read_u32(_context, _bytecode);
	return	v.f;
}

inline double passion_bc_read_double(struct passion_context* _context, const void* _bytecode)
{
	union
	{
		uint64_t	i;
		double		d;
	}	v;
	v.i	= passion_bc_read_u64(_context, _bytecode);
	return	v.d;
}

PASSION_EXTERN_C_END
#endif
