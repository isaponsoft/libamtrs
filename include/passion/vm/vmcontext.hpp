#ifndef	__passion__vmcontext__hpp
#define	__passion__vmcontext__hpp
#include "def.hpp"
#include "log.hpp"
#include "vmobject.hpp"
#include "vmbuffer.hpp"
PASSION_NAMESPACE_BEGIN
struct	vmcontext;
PASSION_NAMESPACE_IMPLIMENTS_BEGIN(vmcontext)

template<class T>
T read(vmcontext&, const uint8_t*);

PASSION_NAMESPACE_IMPLIMENTS_END



struct	vmcontext
		: passion_context
{
	vmbuffer<vmvalue>			_stack;
	vmbuffer<char>				variablefields;
	std::size_t					stackSize;

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




	static vmcontext create(std::size_t _stackSize = 128, std::size_t = 64)
	{
		vmcontext	thiz;
		thiz._stack			= decltype(thiz._stack)(_stackSize);
		thiz.variablefields	= vmbuffer<char>(8192);
		thiz.ss		= thiz._stack.data();
		thiz.ds		= thiz.variablefields.data();
		std::memset(thiz.variablefields.data(), 0, sizeof(char) * thiz.variablefields.size());
		return	thiz;
	}

	static vmcontext create(vmobj* _object, vmclass* _clz, const vmstring& _function, std::size_t _argc = 0, const vmvalue* _argv = nullptr, std::size_t _stackSize = 128, std::size_t _exceptionSize = 64)
	{
		return	create(_object, _clz->find_function(_function, _argc), _argc, _argv, _stackSize, _exceptionSize);
	}

	static vmcontext create(vmobj* _object, vmfunction* _function, std::size_t _argc = 0, const vmvalue* _argv = nullptr, std::size_t _stackSize = 128, std::size_t _exceptionSize = 64);

	void destroy_global_variable(vmclass_manager* _vm);


	vmcontext() { passion_context_initialize(this); }
	vmcontext(const vmcontext&) = delete;
	vmcontext(      vmcontext&&) = default;
	vmcontext& operator = (const vmcontext&) = delete;
	vmcontext& operator = (      vmcontext&&) = default;
private:

	static vmcontext** _context()
	{
		thread_local vmcontext*	sCurrent	= nullptr;
		return	&sCurrent;
	}
};



inline vmcontext vmcontext::create(vmobj* _object, vmfunction* _function, std::size_t _argc, const vmvalue* _argv, std::size_t _stackSize, std::size_t _exceptionSize)
{
	vmcontext	thiz	= create(_stackSize, _exceptionSize);
	thiz.stackSize	= _stackSize - 1;
	thiz.sp			= _stackSize - 1;
	thiz.ip			= 0;
	thiz.cs			= _function->function();
	thiz.push(vmvalue());				// 戻り値の格納先
	for (std::size_t i = 0; i < _argc; ++i)
	{
		thiz.push(_argv[i]);
	}
	thiz.lp	=  thiz.sp;
	thiz.ap	=  thiz.sp + _argc;
	return	thiz;
}



PASSION_NAMESPACE_END
#endif
