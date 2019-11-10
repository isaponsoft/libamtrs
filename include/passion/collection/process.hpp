#ifndef	__passion__collection__process__hpp
#define	__passion__collection__process__hpp
#include <iostream>
#include <string>
#include <cstddef>
#include <thread>
#include <amtrs/process/@>
#include "../psnvm.hpp"
PASSION_NAMESPACE_BEGIN namespace collection {



inline void register_process(vmclass_manager* _vm)
{
	auto	clazz	= _vm->create_class("process");

	clazz->add_function(native::create_vfunc(_vm, clazz->name(), "run", [](passion_context& _context, vmobject* _thiz, vmstring _command) -> vmint_t
	{
		return	process::run(_command.c_str());
	}));


	clazz->add_function(native::create_vfunc(_vm, clazz->name(), "exec", [](passion_context& _context, vmobject* _thiz, vmstring _command) -> vmstring
	{
		return	process::exec(_command.c_str());
	}));

	clazz->build();
}

} PASSION_NAMESPACE_END
#endif
