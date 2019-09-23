#ifndef	__passion__vm__vmnative__hpp
#define	__passion__vm__vmnative__hpp
#include <amtrs/type_traits.hpp>
#include "vmfunction.hpp"
#include "vmvariable_trits.hpp"
PASSION_NAMESPACE_BEGIN


namespace native {


//! 仮想ネイティブ関数のシグネチャを生成するためのラッパー
template<class...>
struct	__virtual_function_traits;


template<class F>
struct	__virtual_function_traits<F>
		: __virtual_function_traits<typename function_type_of<F>::type>
{};


//! 関数（引数なし）
template<class R, class C>
struct	__virtual_function_traits<R(passion_context&, C*)>
{
	using	vmtype	= ref<vmfunction>;
	static constexpr auto signature() noexcept
	{
		return	variable_traits<R()>::signature();
	}

	// vmfunction::parameters を生成するための関数
	template<class VMC>
	static void make_parameters(VMC& /*_vmc*/)
	{}
};


//! 関数（引数１つあり）
template<class R, class C, class F>
struct	__virtual_function_traits<R(passion_context&, C*, F)>
{
	using	vmtype	= ref<vmfunction>;
	static constexpr auto signature() noexcept
	{
		return	variable_traits<R(F)>::signature();
	}

	// vmfunction::parameters を生成するための関数
	template<class PARAMS>
	static void make_parameters(PARAMS& _params)
	{
		_params.push_back(vmstring(variable_traits<F>::signature().c_str()));
	}
};


//! 関数（引数あり）
template<class R, class C, class F, class... Args>
struct	__virtual_function_traits<R(passion_context&, C*, F, Args...)>
{
	using	vmtype			= ref<vmfunction>;
	static constexpr auto signature() noexcept
	{
		return	variable_traits<R(F, Args...)>::signature();
	}

	// vmfunction::parameters を生成するための関数
	template<class PARAMS>
	static void make_parameters(PARAMS& _params)
	{
		_params.push_back(vmstring(variable_traits<F>::signature().c_str()));
		__virtual_function_traits<R(passion_context&, C*, Args...)>::make_parameters(_params);
	}
};

template<class>
struct	__native_func_argument;


//! T* で指定する型へ変換します。
template<class T>
struct	__native_func_argument<T*>
{
	T* get(vmobject* _o)
	{
		return	_o->native<T>();
	}
};



//! vmint_t へ変換します。
template<>
struct	__native_func_argument<vmint_t>
{
	vmint_t get(vmint_t _v)
	{
		return	_v;
	}

	vmint_t get(const vmvalue& _v)
	{
		return	_v.i;
	}

	vmvalue to_variant(vmint_t _v)
	{
		return	vmvalue(_v);
	}
};

//! int64_t へ変換します。
template<>
struct	__native_func_argument<char>
{
	char get(vmint_t _v)
	{
		return	(char)_v;
	}

	char get(const vmvalue& _v)
	{
		return	(char)_v.i;
	}

	vmvalue to_variant(char _v)
	{
		return	vmvalue(_v);
	}
};



//! int64_t へ変換します。
template<>
struct	__native_func_argument<int64_t>
{
	int64_t get(vmint_t _v)
	{
		return	_v;
	}

	int64_t get(const vmvalue& _v)
	{
		return	_v.i;
	}

	vmvalue to_variant(int64_t _v)
	{
		return	vmvalue(_v);
	}
};


//! vmstring の相互変換
template<>
struct	__native_func_argument<vmstring>
{
	vmstring get(vmstrobj* _o)
	{
		return	_o ? vmstring(_o->c_str()) : vmstring();
	}

	vmstring get(vmobj* _o)
	{
		return	get(_o ? reinterpret_cast<vmstrobj*>(_o + 1) : nullptr);
	}

	vmstring get(vmvalue _v)
	{
		return	get(_v.o);
	}

	vmvalue to_variant(vmstring _v)
	{
		return	vmvalue(vmstrobj::create(_v).release());
	}
};



//! vmstring の相互変換
template<>
struct	__native_func_argument<std::string>
{
	std::string get(vmstrobj* _o)
	{
		return	_o ? std::string(_o->c_str()) : std::string();
	}

	vmstring get(vmobj* _o)
	{
		return	get(_o ? reinterpret_cast<vmstrobj*>(_o + 1) : nullptr);
	}

	std::string get(vmvalue _v)
	{
		return	get(_v.o);
	}

	vmvalue to_variant(const std::string& _text)
	{
		return	vmvalue(vmstrobj::create(_text).release());
	}
};


//! char const* の相互変換
template<>
struct	__native_func_argument<char const*>
{
	char const* get(vmstrobj* _o)
	{
		return	_o ? _o->c_str() : nullptr;
	}

	char const* get(vmobj* _o)
	{
		return	get(_o ? reinterpret_cast<vmstrobj*>(_o + 1) : nullptr);
	}

	char const* get(vmvalue _v)
	{
		return	get(_v.o);
	}

	vmvalue to_variant(std::string_view _text)
	{
		return	vmvalue(vmstrobj::create(_text).release());
	}
};


//! vmstring の相互変換
template<>
struct	__native_func_argument<vmstrobj*>
{
	vmstrobj* get(vmstrobj* _o)
	{
		return	_o;
	}

	vmstrobj* get(vmobj* _o)
	{
		return	get(reinterpret_cast<vmstrobj*>(_o + 1));
	}

	vmstrobj* get(vmobject* _o)
	{
		return	_o->native<vmstrobj>();
	}

	vmstrobj* get(vmvalue _v)
	{
		return	get(_v.o);
	}
};

//! vmstring の相互変換
template<class T>
struct	__native_func_argument<ref<T>>
{
	T* get(T* _o)
	{
		return	_o;
	}

	T* get(vmobj* _o)
	{
		return	reinterpret_cast<T*>(_o + 1);
	}

	T* get(vmvalue _v)
	{
		return	get(_v.o);
	}

	vmvalue to_variant(T* _v)
	{
		return	vmvalue(_v->base);
	}

	vmvalue to_variant(ref<T>& _v)
	{
		return	vmvalue(static_cast<vmobj*>(_v.release()->obj()));
	}

	vmvalue to_variant(ref<vmobj>& _v)
	{
		return	to_variant(_v.release());
	}
};





template<class...>
struct	__native_vfunc_invoke;


template<class R, class This>
struct	__native_vfunc_invoke<R(passion_context&, This*)>
{
	template<class F>
	auto operator () (F& _function, passion_context& _context, vmobject* _scope)
	{
		auto	r	= _function(_context, __native_func_argument<This*>{}.get(_scope));
		return	r;
	}
};


template<class R, class This, class... Args>
struct	__native_vfunc_invoke<R(passion_context&, This*, Args...)>
{
	template<class F, std::size_t... Indecies>
	auto _invoke(F& _function, passion_context& _context, vmobject* _scope, std::index_sequence<Indecies...>)
	{
		auto*		ap		= _context.arguments();
		return	_function(_context, __native_func_argument<This*>{}.get(_scope), __native_func_argument<Args>{}.get(ap[-static_cast<int>(Indecies)-1])...);
	}


	template<class F>
	auto operator () (F& _function, passion_context& _context, vmobject* _scope)
	{
		return	_invoke<F>(_function, _context, _scope, std::make_index_sequence<sizeof...(Args)>());
	}

};


template<class F>
class	native_vfunc
		: public vmfunction
{
public:
	using	function_type	= typename amtrs::function_type_of<F>::type;
	using	invoker_type	= __native_vfunc_invoke<function_type>;

	template<class It>
	native_vfunc(amtrs_string_view _class_name, amtrs_string_view _name, F&& _function, It _argFirst, It _argLast)
		: vmfunction(
				std::move(_class_name),
				std::move(_name),
				vmstring(variable_traits<typename amtrs::callable_result_of<F>::type>::signature().c_str()),
				_argFirst, _argLast
		)
		, mFunction(std::move(_function))
	{
		mFunctionInfo->data		= &mFunction;
		mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
								{
									F&		func	= *reinterpret_cast<F*>(_data);
									using	rt		= decltype(invoker_type{}(func, *_context, std::declval<vmobject*>()));
									vmobject*	scope	= _context->argument_value(0).o->base();
									if constexpr (std::is_same<void, rt>::value)
									{
										invoker_type{}(func, *_context, scope);
									}
									else
									{
										auto		retval	= invoker_type{}(func, *_context, scope);
										_context->argument_value(1) = __native_func_argument<decltype(retval)>{}.to_variant(retval);
									}
									_context->pop_context();
									return	context_result::cr_continue;
								};
	}

private:
	F	mFunction;
};



// ============================================================================
//!	仮想関数を作成します。
// ----------------------------------------------------------------------------
template<class F>
ref<vmfunction> create_vfunc(vmclass_manager* /*_vm*/, const vmstring& _class_name, const vmstring& _func_name, F&& _f)
{
	//using	result_type		= typename amtrs::callable_result_of<F>::type;
	std::vector<vmstring>	args;
	__virtual_function_traits<F>::make_parameters(args);
	auto	retval		= new native_vfunc<F>(_class_name, _func_name, std::move(_f), args.begin(), args.end());
	return	retval;
}


} PASSION_NAMESPACE_END
#endif
