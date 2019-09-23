/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__vmcontext__hpp
#define	__passion__vmcontext__hpp
#include "context.hpp"
#include "function.h"
PASSION_NAMESPACE_BEGIN

class	vmclass_manager;

struct	vmcontext
		: passion_context
{
	bool empty() const noexcept { return !mStackBuffer || !mGlobalValues; }

	// ========================================================================
	//! 現在のコンテキストを返す。
	// ------------------------------------------------------------------------
	//! コンテキストはスレッド毎に独立して設定する必要があります。
	// ------------------------------------------------------------------------
	static vmcontext* current_context() noexcept { return *_context(); }


	// ========================================================================
	//! 現在のコンテキストを設定する。
	// ------------------------------------------------------------------------
	//! コンテキストはスレッド毎に独立して設定する必要があります。
	// ------------------------------------------------------------------------
	static vmcontext* set_context(vmcontext* _cntx) noexcept { vmcontext* r = *_context(); *_context() = _cntx; return r; }


	static vmcontext create(std::size_t _stackSize = 512, std::size_t = 64)
	{
		vmcontext	thiz;
		thiz.mStackSize			= _stackSize;
		thiz.mStackBuffer				= new (std::nothrow) vmvalue[thiz.mStackSize];
		thiz.mGlobalSize	= 8192;
		thiz.mGlobalValues		= new (std::nothrow) char[thiz.mGlobalSize];
		if (!thiz.empty())
		{
			thiz.ss		= thiz.mStackBuffer;
			thiz.ds		= thiz.mGlobalValues;
			std::memset(thiz.mGlobalValues, 0, sizeof(char) * thiz.mGlobalSize);
			return	thiz;
		}
		return	{};
	}

	static vmcontext create(passion_function* _function, std::size_t _argc = 0, const vmvalue* _argv = nullptr, std::size_t _stackSize = 512, std::size_t _exceptionSize = 64)
	{
		vmcontext	thiz	= create(_stackSize, _exceptionSize);
		thiz.mStackSize	= _stackSize - 1;
		thiz.sp			= _stackSize - 1;
		thiz.ip			= 0;
		thiz.cs			= _function;
		thiz.push(vmvalue());				// 戻り値の格納先
		for (std::size_t i = 0; i < _argc; ++i)
		{
			thiz.push(_argv[i]);
		}
		thiz.lp	=  thiz.sp;
		thiz.ap	=  thiz.sp + _argc;
		return	thiz;
	}



	void destroy_global_variable(vmclass_manager* _vm);


	vmcontext()
	{
		passion_context_initialize(this);
	}


	vmcontext(vmcontext&& _r)
		: mStackBuffer(_r.mStackBuffer)
		, mStackSize(_r.mStackSize)
		, mGlobalValues(_r.mGlobalValues)
		, mGlobalSize(_r.mGlobalSize)
	{
		*static_cast<passion_context*>(this)	= *static_cast<passion_context*>(&_r);
		_r.mStackBuffer				= nullptr;
		_r.mStackSize			= 0;
		_r.mGlobalValues		= nullptr;
		_r.mGlobalSize	= 0;
	}

	vmcontext& operator = (vmcontext&& _r)
	{
		*static_cast<passion_context*>(this)	= *static_cast<passion_context*>(&_r);
		if (mStackBuffer)
		{
			delete[] mStackBuffer;
		}
		if (mGlobalValues)
		{
			delete[] mGlobalValues;
		}
		mStackBuffer			= _r.mStackBuffer;
		mStackSize				= _r.mStackSize;
		mGlobalValues			= _r.mGlobalValues;
		mGlobalSize				= _r.mGlobalSize;
		_r.mStackBuffer			= nullptr;
		_r.mStackSize			= 0;
		_r.mGlobalValues		= nullptr;
		_r.mGlobalSize	= 0;
		return	*this;
	}

	vmcontext(const vmcontext&) = delete;
	vmcontext& operator = (const vmcontext&) = delete;

	~vmcontext()
	{
		if (mStackBuffer)
		{
			delete[] mStackBuffer;
		}
		if (mGlobalValues)
		{
			delete[] mGlobalValues;
		}
	}


private:
	static vmcontext** _context()
	{
		thread_local vmcontext*	sCurrent	= nullptr;
		return	&sCurrent;
	}

	vmvalue*				mStackBuffer		= nullptr;
	std::size_t				mStackSize			= 0;
	char*					mGlobalValues		= nullptr;
	std::size_t				mGlobalSize			= 0;
};


PASSION_NAMESPACE_END
#endif
