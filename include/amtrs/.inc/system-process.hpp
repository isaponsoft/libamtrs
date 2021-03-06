/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system_process__hpp
#define	__libamtrs__system_process__hpp
AMTRS_EXTERN_C_BEGIN
/*
 * recv, send interface
 */
typedef	struct
{
	//! キャプチャしたいストリームには nullptr を渡します。
	void*	object_stdin;
	void*	object_stdout;
	void*	object_stderr;

	int		(*conntact)(void* obj, void** data, size_t* size);

}	amtrs_system_process_pipe;
AMTRS_EXTERN_C_END



AMTRS_NAMESPACE_SYSTEM_BEGIN

enum class streamtype
{
	cin,
	cout,
	cerr,
};


/*
 *
 */
int exec(amtrs_system_process_pipe const& _pipe, char const* _command);

int exec(char const* _command);


template<class Callback>
inline int exec(char const* _command, Callback _callback)
{
	struct	callback
	{
		streamtype	type;
		Callback&	callback;
	};
	callback					cb_cin  { streamtype::cin,  _callback };
	callback					cb_cout { streamtype::cout, _callback };
	callback					cb_cerr { streamtype::cerr, _callback };

	amtrs_system_process_pipe	p{};
	p.object_stdout	= &cb_cout,
	p.object_stderr	= &cb_cerr,
	p.conntact		= [](void* obj, void** data, size_t* size) -> int
	{
		auto	cb	= reinterpret_cast<callback*>(obj);
		return	cb->callback(cb->type, data, size);
	};
	return	exec(p, _command);
}



AMTRS_NAMESPACE_SYSTEM_END
#endif
