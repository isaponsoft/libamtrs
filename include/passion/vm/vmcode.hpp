#ifndef	__passion__vm__vmcode__hpp
#define	__passion__vm__vmcode__hpp
#include <amtrs/logging.hpp>
#include "def.hpp"
#include "vmclass.hpp"
#include "vmobject.hpp"
#include "vmaryobj.hpp"
#include "vmarygcobj.hpp"
#include "vmstrobj.hpp"
PASSION_NAMESPACE_BEGIN





inline vmclass_manager::vmclass_manager()
{
	mSharedData	= vmshared_data::get_instance();

	auto	regist	= [this](const char* _name, std::size_t) -> vmclass*
	{
		auto	c	= new vmclass(_name);
		add_class(c);
		return	c;
	};

	// グローバル関数を扱うための特殊なクラスを登録
	mClsGlobal		= regist("",		sizeof(void*));
	mClsInt			= regist("int",		sizeof(int));

	regist("void",		0);
	regist("bool",		sizeof(int8_t));
	regist("char",		sizeof(char));
	regist("int8_t",	sizeof(int8_t));
	regist("int16_t",	sizeof(int16_t));
	regist("int32_t",	sizeof(int32_t));
	regist("int64_t",	sizeof(int64_t));
	regist("float",		sizeof(float));
	regist("double",	sizeof(double));
	regist("nullptr_t",	0);

	mClsInt->add_function(native::create_vfunc(this, "int", "to_string", [](passion_context& _context, vmobject* _thiz) -> vmstring
	{
		(void)_context;
		return	vmstring(std::to_string((int)(std::intptr_t)_thiz));
	}));
	mClsInt->build();

	// string operator+(char,string)
	mClsGlobal->add_function(native::create_static_function(this, "", "operator+", [](passion_context& _context, char _a, vmstring _text) -> vmstring
	{
		(void)_context;
		return	vmstring(std::to_string(_a)) + _text;
	}));

	// string operator+(int64_t,string)
	mClsGlobal->add_function(native::create_static_function(this, "", "operator+", [](passion_context& _context, int64_t _a, vmstring _text) -> vmstring
	{
		(void)_context;
		return	vmstring(std::to_string(_a)) + _text;
	}));

	// string operator+(string,int64_t)
	mClsGlobal->add_function(native::create_static_function(this, "", "operator+", [](passion_context& _context, vmstring _a, int64_t _b) -> vmstring
	{
		(void)_context;
		return	_a + vmstring(std::to_string(_b));
	}));

	// string operator+(string,string)
	mClsGlobal->add_function(native::create_static_function(this, "", "operator+", [](passion_context& _context, vmstring _a, vmstring _b) -> vmstring
	{
		(void)_context;
		return	_a + _b;
	}));
	mClsGlobal->build();

	// void yield
	{
		struct	yield_function : vmfunction
		{
			yield_function() : vmfunction(amtrs_string_view(""), amtrs_string_view("yield"), amtrs_string_view("void"), {})
			{
				add_attribute(attribute_flag::static_attribute);
				mFunctionInfo->data		= nullptr;
				mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
								{
									(void)_data;
									switch (_context->ip++)
									{
										case 0 :
											return	context_result::cr_yield;
										case 1 :
											_context->pop_context();
											return	context_result::cr_continue;
									}
									return	context_result::cr_continue;
								};
			}
		};
		mClsGlobal->add_function(new yield_function());
	}

	mClsGlobal->build();


	{
		auto	c	= mSharedData->clazz(vmshared_data::string_clz);
		mClsString	= c;
		add_class(c);
		c->build();
	}
	{
		auto	c	= mSharedData->clazz(vmshared_data::array_clz);
		add_class(c);
		c->build();
	}
	{
		auto	c	= mSharedData->clazz(vmshared_data::arraygc_clz);
		add_class(c);
		c->build();
	}
}



template<class NTC>
vmclass* vmclass_manager::create_native_class()
{
	vmclass*	clz	= this->create_class(NTC::signature().c_str());
	clz->set_native_class<NTC>(this);
	clz->build();
	return	clz;
}



inline ref<vmobj> vmobject::create(vmclass* _clazz, vmvariant* _args, std::size_t _size)
{
	// まずは空のオブジェクトを作成
	std::size_t		varSize		= sizeof(vmobject) + passion_variable_size_compute(_clazz->struct_size());
	std::size_t		nativeSize	= 0;
	if (_clazz->native().sizer)
	{
		nativeSize	= _clazz->native().sizer(_args, _size);
		nativeSize	= (nativeSize + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
	}
	std::size_t		thisSize	= sizeof(vmobject) + varSize + nativeSize;
	auto*			mem			= ::operator new (thisSize);
	ref<vmobject>	thiz		= new (mem) vmobject();
	std::memset(thiz->member(), 0, varSize);

	// vmobjectの先頭へのポインタを追加
	_clazz->initvtable(thiz.get());

	// ネイティブ領域を初期化
	std::intptr_t	ntv			= reinterpret_cast<std::intmax_t>(thiz->member()) + varSize;
	if (_clazz->native().init)
	{
		thiz->mNative	= _clazz->native().init(thiz->top() + 1, reinterpret_cast<void*>(ntv), _args, _size);
	}
	//AMTRS_TRACE_LOG("vmobject::create(%s) %p", _clazz->name().c_str(), thiz.get());

	return	thiz->top();
}


inline vmclass* vmclass_manager::create_class(const vmstring& _class_name, std::size_t _base_class_count, vmclass* _base_classes[])
{
	ref<vmclass>	clz	= prototype_class(_class_name);
	if (!clz->prototype)
	{
		AMTRS_TRACE_LOG("Duplicate class %s", _class_name.c_str());
		throw	std::runtime_error("Duplicate class");
	}
	if (_base_class_count > 0)
	{
		for (decltype(_base_class_count) i = 0; i < _base_class_count; ++i)
		{
			clz->add_base_class(_base_classes[i]);
		}
	}
	return	clz;
}


inline vmclass* vmclass_manager::prototype_class(const vmstring& _class_name)
{
	ref<vmclass>	clz	= get_class(_class_name);
	if (clz.empty())
	{
		clz				= new vmclass(_class_name);
		clz->prototype	= true;
		mClasses[_class_name.c_str()] = clz;
	}
	return	clz;
}

inline void vmstrobj::native_functions(vmclass_manager* _vm, vmclass* _clazz)
{
	// 文字列同士の比較
	_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "operator==", [](passion_context& _context, vmstrobj* _thiz, vmstrobj* _r) -> vmint_t
	{
		(void)_context;
		return	_thiz == _r || _thiz->view() == _r->view();
	}));

	// 文字列同士の比較
	_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "operator!=", [](passion_context& _context, vmstrobj* _thiz, vmstrobj* _r) -> vmint_t
	{
		(void)_context;
		return	(_thiz != _r) && (_thiz->view() != _r->view());
	}));

	// 文字列のサイズを返す。
	_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "size", [](passion_context& _context, vmstrobj* _thiz) -> vmint_t
	{
		(void)_context;
		return	_thiz->size();
	}));

	// 文字列が指定した文字列で始まっているか調べる。
	_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "starts_with", [](passion_context& _context, vmstrobj* _thiz, vmstrobj* _match) -> vmint_t
	{
		(void)_context;
		std::string_view	t(_thiz->c_str(), _thiz->size());
		std::string_view	m(_match->c_str(), _match->size());
		return	starts_with(t, m) ? 1 : 0;

	}));

	// 文字列の一部を切り抜く。
	_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "substr", [](passion_context& _context, vmstrobj* _thiz, vmint_t _pos, vmint_t _size) -> vmstring
	{
		(void)_context;
		auto	sz	= _thiz->size();
		auto	b	= static_cast<std::size_t>(_pos);
		auto	e	= static_cast<std::size_t>(_pos + _size);
		if (b > sz)
		{
			b	= sz;
		}
		if (e > sz)
		{
			e	= sz;
		}
		return	vmstring(std::string_view(_thiz->c_str() + b, (std::size_t)(e - b)));
	}));

	// 文字列のポインタを返す。
	_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "c_str", [](passion_context& _context, vmstrobj* _thiz) -> char const*
	{
		return	_thiz->c_str();
	}));
}


inline vmfunction* vmclass::get_destractor()
{
	if (mDestractor.empty())
	{
		struct	vmdestractor
				: public vmfunction
		{
			vmdestractor(amtrs_string_view _class, amtrs_string_view _name)
				: vmfunction(_class, _name, "", {})
			{
				mFunctionInfo->data		= nullptr;
				mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
										{
											(void)_data;
											_context->pop_context();
											return	context_result::cr_continue;
										};
			}
		};

		mDestractor	= new vmdestractor(mClassName, vmstring("~") + mClassName);
	}
	return	mDestractor.get();
}


inline vmshared_data::~vmshared_data()
{
	vmstrobj::set_string_class(nullptr);
	vmaryobj::set_array_class(nullptr);
	vmarygcobj::set_array_class(nullptr);
	mInstance	= nullptr;
}


inline void vmshared_data::initialize()
{
	struct	vmdeallocator : public vmfunction
	{
		vmdeallocator()
			: vmfunction("", "operator delete", "", {})
		{
			add_attribute(attribute_flag::static_attribute);
			mFunctionInfo->data		= nullptr;
			mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
							{
								(void)_data;
								vmobj*	obj	= _context->argument_value(0).o;
								switch (_context->ip++)
								{
									case 0 :
										_context->push(obj->base()->top());
										_context->call(obj->vtbl->functions[0], 1);
										break;
									case 1 :
										_context->pop();
										_context->pop_context();
										_context->pop();
										obj->dec_ref();
										break;
								}
								return	context_result::cr_continue;
							};
		}
	};
	mDeallocator	= new vmdeallocator();

	{
		auto	c	= new vmclass(vmstrobj::signature().c_str());
		mClasses[string_clz]	= c;
		c->set_native_class<vmstrobj>(nullptr);
		vmstrobj::set_string_class(c);
	}

	{
		auto	c	= new vmclass("[]");
		mClasses[array_clz]		= c;
		c->set_native_class<vmaryobj>(nullptr);
		vmaryobj::set_array_class(c);
	}

	{
		auto	c	= new vmclass("^[]");
		mClasses[arraygc_clz]		= c;
		c->set_native_class<vmarygcobj>(nullptr);
		vmarygcobj::set_array_class(c);
	}
}


inline void vmcontext::destroy_global_variable(vmclass_manager* _vm)
{
	auto	clz	= _vm->get_global_scope();
	for (auto& vi : clz->static_variables())
	{
		auto	ti	= clz->get_static_variable_info(vi.name);
		if (ti->type == "psn::string" || passion_is_gcpointer(ti->type) || passion_is_array(ti->type))
		{
			auto	address	= (vmvalue*)variables(vi.position);
			auto	obj		= address->o;
			if (obj && obj->use_count() == 1)
			{
				struct	mainfunc : vmfunction
				{
					mainfunc()
						: vmfunction("__startup", "__startup", "", {})
					{
						mFunctionInfo->data		= this;
						mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
												{
													auto	thiz	= reinterpret_cast<mainfunc*>(_data);
													switch (_context->ip++)
													{
														case 0 :
															_context->push(thiz->obj);
															_context->call(vmshared_data::get_instance()->deallocator()->function(), 1);
															break;
														case 1 :
															thiz->finished	= true;
															return	context_result::cr_finish;
													}
													return	context_result::cr_continue;
												};
					}

					bool					finished	= false;
					vmobj*					obj			= nullptr;
				};
				mainfunc	mf;
				mf.obj	= obj;
				this->call(mf.function(), 0);
				while (!mf.finished)
				{
					this->run();
				}
			}
		}
	}
}


namespace native {
	template<>
	struct	__native_func_argument<vmaryobj*>
	{
		vmaryobj* get(vmaryobj* _o)
		{
			return	_o;
		}

		vmaryobj* get(vmobject* _o)
		{
			return	_o->native<vmaryobj>();
		}

		vmaryobj* get(vmvariant& _v)
		{
			auto	o	= std::get<ref<vmobj>>(_v);
			return	get(o->base());
		}
	};

	template<>
	struct	__native_func_argument<vmarygcobj*>
	{
		vmarygcobj* get(vmarygcobj* _o)
		{
			return	_o;
		}

		vmarygcobj* get(vmobject* _o)
		{
			return	_o->native<vmarygcobj>();
		}

		vmarygcobj* get(vmvariant& _v)
		{
			auto	o	= std::get<ref<vmobj>>(_v);
			return	get(o->base());
		}
	};
}

inline void vmaryobj::native_functions(vmclass_manager* _vm, vmclass* _clazz)
{
	_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "size", [](passion_context& _context, vmaryobj* _thiz) -> vmint_t
	{
		(void)_context;
		return	_thiz->mSize;
	}));
}

inline void vmarygcobj::native_functions(vmclass_manager* _vm, vmclass* _clazz)
{
	vmaryobj::native_functions(_vm, _clazz);

	struct	vmdestractor
			: public vmfunction
	{
		vmdestractor(amtrs_string_view _class, amtrs_string_view _name)
			: vmfunction(_class, _name, "", {})
		{
			mFunctionInfo->data		= nullptr;
			mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
							{
								(void)_data;
								vmarygcobj*	thiz	= _context->argument_value(0).o->native<vmarygcobj>();
								while (_context->ip < (intmax_t)thiz->size())
								{
									auto	top		= reinterpret_cast<ref<vmobj>*>(thiz->data());
									auto	obj		= (top + _context->ip++)->get();
									if (obj)
									{
										if (obj->use_count() == 1)
										{
											_context->push(obj);
											_context->call(vmshared_data::get_instance()->deallocator()->function(), 1);
											return	context_result::cr_continue;
										}
										else
										{
											obj->down_ref();
										}
									}
								}
								_context->pop_context();
								return	context_result::cr_continue;
							};
		}
	};
	_clazz->set_destractor(new vmdestractor(_clazz->name(), vmstring("~") + _clazz->name()));

}

PASSION_NAMESPACE_END
#endif
