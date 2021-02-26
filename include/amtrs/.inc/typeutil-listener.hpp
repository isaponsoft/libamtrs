/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_traits__listener__hpp
#define __libamtrs__type_traits__listener__hpp
AMTRS_NAMESPACE_BEGIN

template<class...>	struct	listener;
template<class...>	struct	ref_listener;
template<class...>	struct	listener_traits;


#define	AMTRS_DEFINE_LISTENER(_event_tag, ...)	\
	template<> struct ::amtrs::listener<_event_tag> : ::amtrs::listener<__VA_ARGS__> {};


AMTRS_IMPLEMENTS_BEGIN(listener)
template<class...>	struct	listener_lambdable;
template<class...>	struct	listener_override;
template<class T>	struct	__wapping              { using type	= listener<T>; };
template<class T>	struct	__wapping<listener<T>> { using type = listener<T>; };
AMTRS_IMPLEMENTS_END(listener)



template<class... Listener, class... Lambda>
auto make_listener(Lambda&&... _lambda)
{
	using namespace AMTRS_IMPLEMENTS(listener);

	using	listener_type	= typename __wapping<Listener...>::type;
	using	lambda_type		= listener_lambdable<listener_type, Lambda...>;
	return	listener_override<lambda_type, listener_type>(std::move(_lambda)...);
}


template<class... Listener, class... Lambda>
auto make_ref_listener(Lambda&&... _lambda) -> ref<ref_listener<Listener...>>
{
	using namespace AMTRS_IMPLEMENTS(listener);

	using	listener_type	= typename __wapping<Listener...>::type;
	using	lambda_type		= listener_lambdable<ref_listener<Listener...>, Lambda...>;
	return	new listener_override<lambda_type, listener_type>(std::move(_lambda)...);
}


template<class... T>
struct	ref_listener
		: ref_object
		, AMTRS_IMPLEMENTS(listener)::__wapping<T...>::type
{
	virtual ~ref_listener() {}
};


template<class... T>
struct	ref_listener<listener<T...>>
		: ref_listener<T...>
{
};


// 関数型で指定されたリスナ
template<class R, class... Args>
struct	listener<R(Args...)>
{
	virtual R operator () (Args... _args)
	{
		if constexpr (!std::is_same<R,void>::value)
		{
			return {};
		}
	}
};

// 関数型で指定されたリスナ
template<class R, class... Args, class... Other>
struct	listener<R(Args...), Other...>
		: listener<Other...>
{
	using	super_type	= listener<Other...>;
	using	super_type::operator ();
	virtual R operator () (Args... _args)
	{
		if constexpr (!std::is_same<R,void>::value)
		{
			return {};
		}
	}
};


// 複数のリスナが存在する場合は先頭とそれ以外で分離して解析する
template<class F, class... Other>
struct	listener<F, Other...>
		: listener_traits<listener<F>, Other...>::type
{
	using	super_type	= typename listener_traits<listener<F>, Other...>::type;
	using	super_type::operator ();
};

template<class T>
struct	listener<listener<listener<T>>>
		: listener<T>
{
};

template<class T>
struct	listener<T>
		: listener_traits<T>::type
{
};



template<class T, class... Other>
struct	listener_traits<T, Other...>
{
private:
#if	__MSVC_VER
	template<class R, class... Args, class... O> static constexpr auto __extract(listener<R(Args...), O...>*) -> listener<R(Args...), O..., Other...>*;
	template<class T1, class T2, class... O> static constexpr auto __extract(listener<T1, T2, O...>*) -> listener<T1, T2, O..., Other...>*;
#else
	template<class R, class... Args> static constexpr auto __extract(listener<R(Args...)>*) -> listener<R(Args...), Other...>*;
	template<class R, class... Args, class T2> static constexpr auto __extract(listener<R(Args...), T2>*) -> listener<R(Args...), T2, Other...>*;
	template<class R, class... Args, class T2, class T3> static constexpr auto __extract(listener<R(Args...), T2, T3>*) -> listener<R(Args...), T2, T3, Other...>*;
	template<class R, class... Args, class T2, class T3, class T4> static constexpr auto __extract(listener<R(Args...), T2, T3, T4>*) -> listener<R(Args...), T2, T3, T4, Other...>*;
	template<class R, class... Args, class T2, class T3, class T4, class T5> static constexpr auto __extract(listener<R(Args...), T2, T3, T4, T5>*) -> listener<R(Args...), T2, T3, T4, T5, Other...>*;
	template<class R, class... Args, class T2, class T3, class T4, class T5, class T6> static constexpr auto __extract(listener<R(Args...), T2, T3, T4, T5, T6>*) -> listener<R(Args...), T2, T3, T4, T5, T6, Other...>*;
	template<class R, class... Args, class T2, class T3, class T4, class T5, class T6, class T7> static constexpr auto __extract(listener<R(Args...), T2, T3, T4, T5, T6, T7>*) -> listener<R(Args...), T2, T3, T4, T5, T6, T7, Other...>*;
	template<class R, class... Args, class T2, class T3, class T4, class T5, class T6, class T7, class T8> static constexpr auto __extract(listener<R(Args...), T2, T3, T4, T5, T6, T7, T8>*) -> listener<R(Args...), T2, T3, T4, T5, T6, T7, T8, Other...>*;
	template<class R, class... Args, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> static constexpr auto __extract(listener<R(Args...), T2, T3, T4, T5, T6, T7, T8, T9>*) -> listener<R(Args...), T2, T3, T4, T5, T6, T7, T8, T9, Other...>*;
	template<class R, class... Args, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> static constexpr auto __extract(listener<R(Args...), T2, T3, T4, T5, T6, T7, T8, T9, T10>*) -> listener<R(Args...), T2, T3, T4, T5, T6, T7, T8, T9, T10, Other...>*;

	template<class T1, class T2> static constexpr auto __extract(listener<T1, T2>*) -> listener<T1, T2, Other...>*;
	template<class T1, class T2, class T3> static constexpr auto __extract(listener<T1, T2,  T3>*) -> listener<T1, T2, T3, Other...>*;
	template<class T1, class T2, class T3, class T4> static constexpr auto __extract(listener<T1, T2,  T3, T4>*) -> listener<T1, T2, T3, T4, Other...>*;
	template<class T1, class T2, class T3, class T4, class T5> static constexpr auto __extract(listener<T1, T2,  T3, T4, T5>*) -> listener<T1, T2, T3, T4, T5, Other...>*;
	template<class T1, class T2, class T3, class T4, class T5, class T6> static constexpr auto __extract(listener<T1, T2,  T3, T4, T5, T6>*) -> listener<T1, T2, T3, T4, T5, T6, Other...>*;
	template<class T1, class T2, class T3, class T4, class T5, class T6, class T7> static constexpr auto __extract(listener<T1, T2,  T3, T4, T5, T6, T7>*) -> listener<T1, T2, T3, T4, T5, T6, T7, Other...>*;
	template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> static constexpr auto __extract(listener<T1, T2,  T3, T4, T5, T6, T7, T8>*) -> listener<T1, T2, T3, T4, T5, T6, T7, T8, Other...>*;
	template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> static constexpr auto __extract(listener<T1, T2,  T3, T4, T5, T6, T7, T8, T9>*) -> listener<T1, T2, T3, T4, T5, T6, T7, T8, T9, Other...>*;
	template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> static constexpr auto __extract(listener<T1, T2,  T3, T4, T5, T6, T7, T8, T9, T10>*) -> listener<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, Other...>*;
#endif

public:	
	using	type	= typename std::remove_pointer<decltype( __extract((T*)nullptr) )>::type;
};







AMTRS_IMPLEMENTS_BEGIN(listener)

template<class...>
struct	listener_lambdable;

template<class Listener, class Lambda>
struct	listener_lambdable<Listener, Lambda>
		: Listener
{
	listener_lambdable(const listener_lambdable&) = default;
	listener_lambdable(listener_lambdable&&) = default;
	listener_lambdable(Lambda&& _lambda) : mLambda(std::move(_lambda)) {}

	template<class... Args, class = typename std::enable_if<std::is_invocable<Lambda, Args&&...>::value>::type>
	auto invoke(Args&&... _args)
	{
		if constexpr (std::is_same<decltype(mLambda(std::forward<Args>(_args)...)), void>::value)
		{
			mLambda(std::forward<Args>(_args)...);
		}
		else
		{
			return	mLambda(std::forward<Args>(_args)...);
		}
	}

private:
	Lambda	mLambda;
};


template<class Listener, class Lambda, class... Other>
struct	listener_lambdable<Listener, Lambda, Other...>
		: listener_lambdable<Listener, Other...>
{
	listener_lambdable(const listener_lambdable&) = default;
	listener_lambdable(listener_lambdable&&) = default;
	listener_lambdable(Lambda&& _lambda, Other&&... _other) : listener_lambdable<Listener, Other...>(std::move(_other)...), mLambda(std::move(_lambda)) {}

	template<class... Args>
	auto invoke(Args&&... _args)
	{
		if constexpr (std::is_invocable<Lambda, Args&&...>::value)
		{
			return	mLambda(std::forward<Args>(_args)...);
		}
		else
		{
			static_assert(sizeof...(Other) > 0, " candidate template ignored: requirement");
			return	listener_lambdable<Listener, Other...>::invoke(std::forward<Args>(_args)...);
		}
	}

private:
	Lambda	mLambda;
};




template<class Lambdable, class R, class... Args>
struct	listener_override<Lambdable, listener<R(Args...)>>
		: Lambdable
{
	using	Lambdable::Lambdable;
	listener_override(const listener_override&) = default;
	listener_override(listener_override&&) = default;
	virtual R operator () (Args... _args) override
	{
		if constexpr (std::is_same<R, void>::value)
		{
			Lambdable::invoke(_args...);
		}
		else
		{
			return	Lambdable::invoke(_args...);
		}
	}
};


template<class Lambdable, class R, class... Args, class... Other>
struct	listener_override<Lambdable, listener<R(Args...), Other...>>
		: listener_override<Lambdable, listener<Other...>>
{
	using	super_type	= listener_override<Lambdable, listener<Other...>>;
	using	super_type::super_type;
	using	super_type::operator ();
	listener_override(const listener_override&) = default;
	listener_override(listener_override&&) = default;
	virtual R operator () (Args... _args) override
	{
		if constexpr (std::is_same<R, void>::value)
		{
			Lambdable::invoke(_args...);
		}
		else
		{
			return	Lambdable::invoke(_args...);
		}
	}
};


template<class Lambdable, class F, class... Other>
struct	listener_override<Lambdable, listener<F, Other...>>
		: listener_override<Lambdable, typename listener_traits<listener<F>, Other...>::type>
{
	using	super_type	= listener_override<Lambdable, typename listener_traits<listener<F>, Other...>::type>;
	using	super_type::super_type;
	using	super_type::operator ();
	listener_override(const listener_override&) = default;
	listener_override(listener_override&&) = default;
};

AMTRS_IMPLEMENTS_END(listener)



AMTRS_NAMESPACE_END
#endif
