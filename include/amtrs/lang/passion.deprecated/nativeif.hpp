/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__pvmni__hpp
#define	__libamtrs__passion__pvmni__hpp
#include "../../string/format.hpp"
#include "vm.hpp"
/*
 * Virtual Machine Native Interface.
 */
PSNVM_NAMESPACE_BEGIN namespace vmni {


// 型指定可変引数をネイティブ関数で扱うためのインターフェース
template<class T>
struct	valist;

/*
 * モジュールのインターフェースのバージョンを示す。
 */
struct	pv_module_ver
{
	static constexpr int	version	= 1;
	pv_module_ver()
		: vercode(reinterpret_cast<void*>(static_cast<size_t>(version)))
	{}
	void*	vercode;
};
#define	PSNVM_MODULE_INIT		pv_module_ver()


/*
 * 継承元やメンバのアクセス識別子。
 */
enum	pv_access
{
	pvma_public		= 0,
	pvma_protected	= 1,
	pvma_private	= 2,
};


/*
 * クラスの継承情報を示す。
 */
struct	pv_basecls
{
	char const*		name;
	pv_access		access	= pv_access::pvma_private;
	pv_basecls(char const* _name)
		: name(_name)
	{}

	pv_basecls(pv_access _access, char const* _name)
		: name(_name)
		, access(_access)
	{}
};


/*
 * メンバ変数の情報。
 */
struct	pv_memberval
{
	pv_access		access	= pv_access::pvma_private;
	char const*		type;
	char const*		name;
};


/*
 * メンバ変数の情報を纏める。
 */
struct	pv_member
{
	pv_member(pv_memberval const* _list) : list(_list) {}
	pv_member(size_t _size)     		 : size(_size) {}

	pv_memberval const*		list	= nullptr;
	size_t					size	= static_cast<size_t>(-1);
};

struct	native_function
{
	void*	buff[2];

	native_function(native_function const&) = default;
	native_function& operator = (native_function const&) = default;

	native_function(void* v)
		: buff{v, nullptr}
	{}

	template<class R, class... Args>
	native_function(R(*v)(Args...))
	{
		static_assert(sizeof(buff) >= sizeof(v), "buffer low");
		using	func_type	= R(*)(Args...);
		*reinterpret_cast<func_type*>(buff) = v;
	}

	template<class R, class C, class... Args>
	native_function(R(C::*v)(Args...))
	{
		static_assert(sizeof(buff) >= sizeof(v), "buffer low");
		using	func_type	= R(C::*)(Args...);
		*reinterpret_cast<func_type*>(buff) = v;
	}

	template<class R, class... Args>
	auto get_f() noexcept
	{
		using	func_type	= R(Args...);
		return	*reinterpret_cast<func_type**>(buff);
	}

	template<class R, class C, class... Args>
	auto get_mf() noexcept
	{
		using	func_type	= R(C::*)(Args...);
		return	*reinterpret_cast<func_type*>(buff);
	}
};


/*
 * メンバ関数の情報。
 */
struct	pv_function
{
	using	bridge_type	= int(vmstate* _vm, native_function _func);

	pv_access			access	= pv_access::pvma_private;
	char const*			name;
	char const*			signature;
	bridge_type*		bridge;
	native_function		native;

	pv_function()
		: name(nullptr)
		, signature(nullptr)
		, native(nullptr)
		, bridge(nullptr)
	{}

	template<class R, class... Args>
	pv_function(char const* _name, char const* _signature, R(*_native)(Args...));

	template<class R, class C, class... Args>
	pv_function(char const* _name, char const* _signature, R(C::*_native)(Args...));
};


/*
 * Class declare.
 */
struct	pv_class
{
	char const*			name		= nullptr;
	pv_basecls const*	extends		= nullptr;
	pv_member			vars		= {(size_t)-1};
	pv_function const*	functions	= nullptr;
};


/*
 * Module info.
 */
struct	pv_module
{
	using	module_init_type	= int(vmstate*);

	pv_module_ver		ver;
	char const*			name;
	char const*			version;
	char const*			docs;
	pv_class const*		classes;
};

#define	__psnvm_module_name(_a, _b)		_a##_b
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#define	PSNVM_MODULE_EXTERN				extern "C"
#else
#define	PSNVM_MODULE_EXTERN
#endif
#define	PSNVM_MODULE(_name)				PSNVM_MODULE_EXTERN AMTRS_FORCE_EXPORT ::psnvm::vmni::pv_module __psnvm_module_name(_name, _module)




/*
 * Create constractor
 */
template<class T, class... Args>
auto pv_ctor() -> void(*)(T*, Args...)
{
	return	[](T* _thiz, Args... _args)
	{
		new (_thiz) T(std::forward<Args>(_args)...);
	};
}

/*
 * Create destractor
 */
template<class T>
auto pv_dtor() -> void(*)(T*)
{
	return	[](T* _thiz)
	{
		_thiz->~T();
	};
}


template<class R, class... Args>
function* makefunction(sstring _name, sstring _signature, R(*_f)(Args...));

template<class R, class... Args>
function* make_member_function(sstring _name, sstring _signature, R(*_f)(Args...));



template<class T>
struct	valist : vaargs<T>
{
	using	super_type	= vaargs<T>;
	using	type		= T;
	static constexpr intmax_t	value_size		= sizeof(T);
	static constexpr intmax_t	aligned_size	= (value_size + sizeof(vmint) - 1) & ~(sizeof(vmint)-1);

	valist(intmax_t _size, T* _values)
		: super_type(_size, _values)
	{}

	template<class P>
	class	iterator_base
	{
	public:
		iterator_base() = default;
		iterator_base(iterator_base const&) = default;
		iterator_base(iterator_base&&) = default;
		iterator_base(P _p) : _ptr(reinterpret_cast<intptr_t>(_p)) {}
		T& operator * () { return *reinterpret_cast<T*>(_ptr); }
		P operator -> () { return reinterpret_cast<T*>(_ptr); }
		bool operator == (iterator_base const& _r) const noexcept { return _ptr == _r._ptr; }
		bool operator != (iterator_base const& _r) const noexcept { return _ptr != _r._ptr; }
		iterator_base& operator ++ () noexcept {_ptr -= aligned_size; return *this; }
		iterator_base& operator ++ (int) noexcept { iterator_base r(*this); _ptr -= aligned_size; return r; }
	protected:
		intptr_t	_ptr;
	};

	using	iterator		= iterator_base<T*>;
	using	iterator_const	= iterator_base<T const*>;

	iterator begin() noexcept { return reinterpret_cast<T*>(ptr(-aligned_size)); }
	iterator end() noexcept { return reinterpret_cast<T*>(ptr((1+super_type::size()) * -aligned_size)); }
	iterator_const begin() const noexcept { return reinterpret_cast<T*>(ptr(-aligned_size)); }
	iterator_const end() const noexcept { return reinterpret_cast<T*>(ptr((1+super_type::size()) * -aligned_size)); }
	T& operator [] (size_t _i) noexcept { return *reinterpret_cast<T*>(ptr((1+_i) * -aligned_size)); }
	T const& operator [] (size_t _i) const noexcept { return *reinterpret_cast<T*>(ptr((1+_i) * -aligned_size)); }
private:
	intptr_t ptr(size_t _offset) { return reinterpret_cast<intptr_t>(super_type::data()) + _offset; }
	using	super_type::data;
};


namespace __
{
	template<class...>	struct	getarg;
	template<class...>	struct	nativeinv;
	template<class...>	struct	nativeinv_noneva;
	template<class...>	struct	nativeinv_va;
	template<class...>	struct	result_invoker;
	template<class...>	struct	store_result;
	template<class...>	struct	lasttype;

	template<>
	struct	lasttype<>
	{
		using	type	= void;
	};

	template<class F>
	struct	lasttype<F>
	{
		using	type	= F;
	};

	template<class F, class... Args>
	struct	lasttype<F, Args...>
	{
		using	type	= typename lasttype<Args...>::type;
	};

	template<class VA>
	constexpr auto __is_vmvaargs__(valist<VA>) noexcept -> std::true_type;
	constexpr auto __is_vmvaargs__(...) noexcept -> std::false_type;

	template<class...>
	struct	__is_vmvaargs;

	template<>
	struct	__is_vmvaargs<>
	{
		using	type	= std::false_type;
	};

	template<class... Args>
	struct	__is_vmvaargs
	{
		using	type	= decltype(__is_vmvaargs__(std::declval<typename lasttype<Args...>::type>()));
	};

	template<class... Args>
	using	is_vmvaargs_t	= typename __is_vmvaargs<Args...>::type;


	template<class... Args>
	struct	argtypes
	{};

	template<size_t N, class...>
	struct	__gettype;

	template<size_t N, class F>
	struct	__gettype<N, F>
	{
		using	type	= F;
	};


	template<size_t N, class F, class... Args>
	struct	__gettype<N, F, Args...>
	{
		using	type	= typename std::conditional<N==0, F, typename __gettype<N-1, Args...>::type>::type;
	};


	template<size_t N, class... Args>
	class	subtuple;

	// tuple の先頭から指定する数だけ抜き出したタプルを生成する
	template<size_t N, class... Args>
	class	subtuple
	{
	private:
		template<int... Idx>	static constexpr auto __mktpl(std::integer_sequence<int, Idx...>) -> argtypes<typename __gettype<Idx, Args...>::type...>;
		static constexpr auto tpl() -> decltype(__mktpl(std::make_integer_sequence<int, N>{}));
	public:
		using	type	= decltype(tpl());
	};

	// スカラ並びに構造体
	template<class T>
	struct	getarg<T>
	{
		static constexpr size_t	value_size			= sizeof(T);
		static constexpr size_t	alined_value_size	= (value_size + sizeof(vmint) - 1) & ~(sizeof(vmint)-1);

		static auto get(void*& p) -> T&
		{
			p = reinterpret_cast<void*>(reinterpret_cast<intmax_t>(p) - alined_value_size);
			return	*reinterpret_cast<T*>(p);
		}

	};

	// ネイティブ側の引数が参照になっている場合、スタックにはポインタが詰まれているので
	// そのあたりの型の変換を行う
	template<class T>
	struct	getarg<T&>
	{
		static auto get(void*& p) -> T&
		{
			using	stack_type	= typename std::remove_reference<typename std::remove_cv<T>>::type*;		// 参照をポインタに置き換えたものがスタックに入っている
			p = reinterpret_cast<void*>(reinterpret_cast<intmax_t>(p) - sizeof(stack_type));
			// ポインタのポインタにすることで書式上、ごまかす
			return	**reinterpret_cast<typename std::remove_reference<T>::type**>(p);
		}
	};



	template<class R>
	struct	result_invoker<R>
	{
		template<class... Args, class... Params>
		static void func(vmstate& _vm, R(*_f)(Args...), Params&&... _args)
		{
			new (*reinterpret_cast<R**>(_vm.reg.rbx.p)) R (_f(std::forward<Params>(_args)...));
		}
	};

	template<class R>
	struct	result_invoker<R&>
	{
		template<class... Args, class... Params>
		static void func(vmstate& _vm, R&(*_f)(Args...), Params&&... _args)
		{
			_vm.reg.rax	= &_f(std::forward<Params>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member(vmstate& _vm, R&(C::*_f)(Args...), C* _this, Params&&... _args)
		{
			_vm.reg.rax	= (_this->*_f)(std::forward<Params>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member_vm(vmstate& _vm, R&(C::*_f)(Args...), C* _this, Params&&... _args)
		{
			_vm.reg.rax	= (_this->*_f)(&_vm, std::forward<Params>(_args)...);
		}
	};

	template<>
	struct	result_invoker<void>
	{
		template<class... Args, class... Params>
		static void func(vmstate& _vm, void(*_f)(Args...), Params&&... _args)
		{
			_f(std::forward<Params>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member(vmstate& _vm, C* _this, void(C::*_f)(Args...), Params&&... _args)
		{
			(_this->*_f)(std::forward<Params>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member_vm(vmstate& _vm, C* _this, void(C::*_f)(Args...), Params&&... _args)
		{
			(_this->*_f)(&_vm, std::forward<Params>(_args)...);
		}
	};

	template<>
	struct	result_invoker<vmint>
	{
		template<class... Args, class... Params>
		static void func(vmstate& _vm, vmint(*_f)(Args...), Params&&... _args)
		{
			_vm.reg.rax	= _f(std::forward<Params>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member(vmstate& _vm, C* _this, vmint(C::*_f)(Args...), Params&&... _args)
		{
			_vm.reg.rax	= (_this->*_f)(std::forward<Params>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member_vm(vmstate& _vm, C* _this, vmint(C::*_f)(Args...), Params&&... _args)
		{
			_vm.reg.rax	= (_this->*_f)(&_vm, std::forward<Params>(_args)...);
		}
	};

	template<>
	struct	result_invoker<int>
	{
		template<class... Args>
		static void func(vmstate& _vm, int(*_f)(Args...), Args&&... _args)
		{
			_vm.reg.rax	= _f(std::forward<Args>(_args)...);
		}

		template<class C, class... Args>
		static void member(vmstate& _vm, C* _this, int(C::*_f)(Args...), Args&&... _args)
		{
			_vm.reg.rax	= (_this->*_f)(std::forward<Args>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member_vm(vmstate& _vm, C* _this, int(C::*_f)(Args...), Params&&... _args)
		{
			_vm.reg.rax	= (_this->*_f)(&_vm, std::forward<Params>(_args)...);
		}
	};

	template<class R>
	struct	result_invoker<R*>
	{
		template<class... Args, class... Params>
		static void func(vmstate& _vm, R*(*_f)(Args...), Params&&... _args)
		{
			_vm.reg.rax	= _f(std::forward<Params>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member(vmstate& _vm, C* _this, R*(C::*_f)(Args...), Params&&... _args)
		{
			_vm.reg.rax	= (_this->*_f)(std::forward<Params>(_args)...);
		}

		template<class C, class... Args, class... Params>
		static void member_vm(vmstate& _vm, C* _this, R*(C::*_f)(vmstate*, Args...), Params&&... _args)
		{
			_vm.reg.rax	= (_this->*_f)(&_vm, std::forward<Params>(_args)...);
		}
	};


	// 可変長引数のない関数
	template<class R, class... Args>
	struct	nativeinv_noneva<R(Args...)>
	{
		void native_invoker(vmstate* _vm, R(*_f)(Args...))
		{
			_native_invoker(_vm, _f, std::make_index_sequence<sizeof...(Args)>{});
		}


		// メンバ関数
		template<class C>
		void native_invoker_m(vmstate* _vm, R(C::*_f)(Args...))
		{
			_native_invoker_m<C>(_vm, _f, std::make_index_sequence<sizeof...(Args)>{});
		}

		// メンバ関数（VM受け取り）
		template<class C, class... A>
		void native_invoker_m(vmstate* _vm, R(C::*_f)(vmstate*, A...))
		{
			_native_invoker_vmm<decltype(_f), C, A...>(_vm, _f, std::make_index_sequence<sizeof...(A)>{});
		}

	private:
		template<std::size_t... I> 
		void _native_invoker(vmstate* _vm, R(*_f)(Args...), std::index_sequence<I...>)
		{
			auto	sp		= _vm->reg.rbx.p;
			auto	args	= std::tuple<Args...>{getarg<Args>::get(sp)...};
			result_invoker<R>::func(*_vm, _f, std::get<I>(args)...);
		}

		template<class C, class F, std::size_t... I>
		void _native_invoker_m(vmstate* _vm, F _f, std::index_sequence<I...>)
		{
			auto	sp		= _vm->reg.rbx.p;
			C*		thiz	= getarg<C*>::get(sp);
			auto	args	= std::tuple<Args...>{getarg<Args>::get(sp)...};
			result_invoker<R>::member(*_vm, thiz, _f, std::get<I>(args)...);
		}

		template<class F, class C, class... TT, std::size_t... I>
		void _native_invoker_vmm(vmstate* _vm, F _f, std::index_sequence<I...>)
		{
			auto	sp		= _vm->reg.rbx.p;
			auto	args	= std::tuple<TT...>{getarg<TT>::get(sp)...};
			result_invoker<R>::template member_vm<C>(*_vm, _f, std::get<I>(args)...);
		}
	};



	// 可変長引数のある関数
	template<class R, class... Args>
	struct	nativeinv_va<R(Args...)>
	{
		void native_invoker(vmstate* _vm, R(*_f)(Args...))
		{
			using	last_type	= typename lasttype<Args...>::type;
			using	va_type		= typename last_type::type;
			using	subtpl		= typename subtuple<sizeof...(Args)-1, Args...>::type;
			_native_invoker<va_type>(_vm, _f, subtpl{});
		}

		// メンバ関数
		template<class C>
		void native_invoker_m(vmstate* _vm, R(C::*_f)(Args...))
		{
			using	last_type	= typename lasttype<Args...>::type;
			using	va_type		= typename last_type::type;
			using	subtpl		= typename subtuple<sizeof...(Args)+1-1, C*, Args...>::type;
			_native_invoker_m<va_type, decltype(_f), C>(_vm, _f, subtpl{}, std::make_index_sequence<sizeof...(Args)>{});
		}

	private:
		template<class VA, class... Arg>
		void _native_invoker(vmstate* _vm, R(*_f)(Args...), argtypes<Arg...>)
		{
			auto	sp		= _vm->reg.rbx.p;
			auto	args	= std::tuple<Arg...>{getarg<Arg>::get(sp)...};	// 引数の呼び出し順序がc++17以前では不定なので一度tupleに入れる
			vmint	vaCount	= getarg<vmint>::get(sp);
			result_invoker<R>::func(*_vm, _f, std::get<Arg>(args)..., valist<VA>(vaCount, reinterpret_cast<VA*>(sp)));
		}

		template<class VA, class F, class C, class... TT, std::size_t... I>
		void _native_invoker_m(vmstate* _vm, F _f, argtypes<TT...>, std::index_sequence<I...>)
		{
			auto	sp		= _vm->reg.rbx.p;
			auto	args	= std::tuple<TT...>{getarg<TT>::get(sp)...};
			vmint	vaCount	= getarg<vmint>::get(sp);
			result_invoker<R>::template member<C>(*_vm, _f, std::get<I>(args)..., valist<VA>(vaCount, reinterpret_cast<VA*>(sp)));
		}
	};



	template<class R, class... Args>
	struct	nativeinv<R(Args...)>
			: std::conditional<is_vmvaargs_t<Args...>::value,
				nativeinv_va<R(Args...)>,
				nativeinv_noneva<R(Args...)>
			>::type
	{};


	template<class R>
	struct	store_result<R>
	{
		static void store(void* rax, R val)
		{
			new (reinterpret_cast<R*>(rax)) R(std::move(val));
		}
	};

	template<class R>
	struct	store_result<R&>
	{
		static void store(void* rax, R& val)
		{
			*reinterpret_cast<R**>(rax)	= &val;
		}
	};

} // namespace __


template<class R, class... Args>
pv_function::pv_function(char const* _name, char const* _signature, R(*_native)(Args...))
	: name(_name)
	, signature(_signature)
	, native(_native)
	, bridge([](vmstate* _vm, native_function _native) -> int
	{
		using	callback_type	= R(Args...);
		__::nativeinv<R(Args...)>{}.native_invoker(_vm, _native.get_f<R, Args...>());
		return	(int)function_result::fr_return;
	})
{}


template<class R, class C, class... Args>
pv_function::pv_function(char const* _name, char const* _signature, R(C::*_native)(Args...))
	: name(_name)
	, signature(_signature)
	, native(_native)
	, bridge([](vmstate* _vm, native_function _native) -> int
	{
		using	callback_type	= R(C::*)(Args...);
		__::nativeinv<R(Args...)>{}.native_invoker_m(_vm, _native.get_mf<R, C, Args...>());
		return	(int)function_result::fr_return;
	})
{}


template<class R, class... Args>
function* makefunction(sstring _name, sstring _signature, R(*_f)(Args...))
{
	using	callback_type	= R(Args...);
	struct	wrapper : function
	{
		callback_type*	f;
		wrapper(sstring _name, sstring _sig, R(*_f)(Args...))
			: function(std::move(_name), std::move(_sig))
		{
			f			= _f;
			func		= [](vmstate* _vm, function* _param) -> function_result
						{
							__::nativeinv<R(Args...)>{}.native_invoker(_vm, static_cast<wrapper*>(_param)->f);
							return	result_type::fr_return;
						};
		}
	};
	return	new wrapper(std::move(_name), std::move(_signature), _f);
}



template<class R, class... Args>
function* make_member_function(sstring _name, sstring _signature, R(*_f)(Args...))
{
	using	callback_type	= R(Args...);
	struct	wrapper : function
	{
		callback_type*	f;
		wrapper(sstring _name, sstring _sig, R(*_f)(Args...))
		{
			name		= std::move(_name);
			signature	= std::move(_sig);
			f			= _f;
			func		= [](vmstate* _vm, function* _param) -> function_result
						{
							__::nativeinv<R(Args...)>{}.native_invoker_m(_vm, static_cast<wrapper*>(_param)->f);
							return	result_type::fr_return;
						};
		}
	};
	return	new wrapper(std::move(_name), std::move(_signature), _f);
}

} PSNVM_NAMESPACE_END
#endif
