#ifndef	__passion__vm__vmfunc_static__hpp
#define	__passion__vm__vmfunc_static__hpp
#include <amtrs/type_traits/@>
#include "vmnative.hpp"
PASSION_NAMESPACE_BEGIN namespace native {



//! 仮想ネイティブ関数のシグネチャを生成するためのラッパー
template<class...>
struct	__static_function_traits;


template<class F>
struct	__static_function_traits<F>
		: __static_function_traits<typename function_type_of<F>::type>
{};


//! 関数（引数なし）
template<class R>
struct	__static_function_traits<R(passion_context&)>
{
	using	vmtype	= ref<vmfunction>;
	static constexpr auto signature() noexcept
	{
		return	variable_traits<R()>::signature();
	}

	// vmfunction::parameters を生成するための関数
	template<class VMC>
	static void make_parameters(VMC& _vmc)
	{}
};


//! 関数（引数１つあり）
template<class R, class F>
struct	__static_function_traits<R(passion_context&, F)>
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
template<class R, class F, class... Args>
struct	__static_function_traits<R(passion_context&, F, Args...)>
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
		__static_function_traits<R(passion_context&, Args...)>::make_parameters(_params);
	}
};


template<class...>
struct	__native_invoke_staticfunction;


template<class R>
struct	__native_invoke_staticfunction<R(passion_context&)>
{
	template<class F>
	auto operator () (F& _function, passion_context& _context)
	{
		auto	r	= _function(_context);
		return	r;
	}
};


template<class R, class... Args>
struct	__native_invoke_staticfunction<R(passion_context&, Args...)>
{
	template<class F, std::size_t... Indecies>
	auto _invoke(F& _function, passion_context& _context, std::index_sequence<Indecies...>)
	{
		auto*		ap		= _context.arguments();
		return	_function(_context, __native_func_argument<Args>{}.get(ap[-static_cast<int>(Indecies)])...);
	}


	template<class F>
	auto operator () (F& _function, passion_context& _context)
	{
		return	_invoke<F>(_function, _context, std::make_index_sequence<sizeof...(Args)>());
	}

};


template<class F>
class	native_sfunc
		: public vmfunction
{
public:
	using	function_type	= typename amtrs::function_type_of<F>::type;
	using	invoker_type	= __native_invoke_staticfunction<function_type>;

	template<class It>
	native_sfunc(amtrs_string_view _class_name, amtrs_string_view _name, F&& _function, It _argFirst, It _argLast)
		: vmfunction(
				std::move(_class_name),
				std::move(_name),
				vmstring(variable_traits<typename amtrs::callable_result_of<F>::type>::signature().c_str()),
				_argFirst, _argLast
		)
		, mFunction(std::move(_function))
	{
		add_attribute(attribute_flag::static_attribute);
		mFunctionInfo->data		= &mFunction;
		mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
						{
							F&		func	= *reinterpret_cast<F*>(_data);
							using	rt		= decltype(invoker_type{}(func, *_context));
							if constexpr (std::is_same<void, rt>::value)
							{
								invoker_type{}(func, *_context);
							}
							else
							{
								auto		retval	= invoker_type{}(func, *_context);
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
//!	静的関数を作成します。
// ----------------------------------------------------------------------------
template<class F>
ref<vmfunction> create_static_function(vmclass_manager* /*_vm*/, const vmstring& _class_name, const vmstring& _func_name, F&& _f)
{
	//using	result_type		= typename amtrs::callable_result_of<F>::type;
	std::vector<vmstring>	args;
	__static_function_traits<F>::make_parameters(args);
	auto	retval		= new native_sfunc<F>(_class_name, _func_name, std::move(_f), args.begin(), args.end());
	return	retval;
}



} PASSION_NAMESPACE_END
#endif
