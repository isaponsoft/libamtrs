#ifndef	__passion__collection__refrection__hpp
#define	__passion__collection__refrection__hpp
#include <iostream>
#include <string>
#include <cstddef>
#include <amtrs/filesystem.hpp>
#include "../psnvm.hpp"
PASSION_NAMESPACE_BEGIN namespace collection {


inline void register_vm(vmclass_manager* _vm)
{
#if	0
	auto	clazz	= _vm->create_class("vmclass_manager");
	{
	auto	f = native::create_function("get_class_names", [=]() -> vmarray_ptr
	{
		auto&		classes	= vmclass_manager->classes();
		vmarray_ptr	retval	= vmarray::create(classes.size());

		std::size_t	i = 0;
		for (auto& clz : classes)
		{
			(*retval)[i++] = vmvariant{std::in_place_type_t<vmstring>(), clz.second->name()};
		}
		return	retval;
	});
	f->return_type	= "string[]";
	clazz->add_function(f);
	}


	{
	auto	f = native::create_function("get_function_names", [=](vmstring _class_name) -> vmarray_ptr
	{
		auto		clz	= vmclass_manager->get_class(_class_name);
		if (!clz)
		{
			return	{};
		}

		vmarray_ptr	retval	= vmarray::create(clz->methods().size());

		std::size_t	i = 0;
		for (auto& m : clz->methods())
		{
			vmstring	name	= m->name + "(";
			std::size_t	c = 0;
			for (const auto& p : m->parameters)
			{
				if (c++ > 0)
				{
					name += ", ";
				}
				name += p;
			}
			name += ") -> ";
			name += m->return_type;

			(*retval)[i++] = vmvariant{std::in_place_type_t<vmstring>(), name};
		}
		return	retval;
	});
	f->return_type	= "string[]";
	clazz->add_function(f);
	}
#endif
}

} PASSION_NAMESPACE_END
#endif
