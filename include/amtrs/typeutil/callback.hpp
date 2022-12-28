/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__typeutil__callback__hpp
#define	__libamtrs__typeutil__callback__hpp
#include <type_traits>
#include "../amtrs.hpp"
AMTRS_TYPEUTIL_NAMESPACE_BEGIN

template<class...>
struct	callback;

/*
 * コールバック関数を定義、引数として渡すためのラッパーです。
 * std::function よりも軽量ですが、機能は多くありません。
 */
template<class R, class... Args>
struct	callback<R(Args...)>
{
	using	callback_type	= R(void* _userparam, Args...);

	bool empty() const noexcept { return mProc == nullptr; }

	// 引数で指定する呼び出し可能なオブジェクトを登録します。
	template<class Func, class = typename std::enable_if<0 == std::is_same<typename std::remove_reference<Func>::type, callback<R(Args...)>>::value>::type**>
	callback(Func&& _func)
	{
		mProc	= [](void* _param, Args... _args) -> R
				{
					Func&	f	= *reinterpret_cast<Func*>(_param);
					return	f(std::forward<Args>(_args)...);
				};
		mParam	= reinterpret_cast<void*>(&_func);
	}


	callback(callback_type _proc, void* _userparam)
		: mProc(_proc)
		, mParam(_userparam)
	{}


	callback() = default;
	callback(callback const&) = default;
	callback& operator = (callback const&) = default;

	callback(std::nullptr_t)
		: mProc(nullptr)
		, mParam(nullptr)
	{}

	callback& operator = (std::nullptr_t)
	{
		mProc	= nullptr;
		mParam	= nullptr;
		return	*this;
	}


	template<class... Params>
	R operator () (Params&&... _params) const
	{
		return	mProc(mParam, std::forward<Params>(_params)...);
	}

protected:
	callback_type*	mProc	= nullptr;
	void*			mParam	= nullptr;
};

AMTRS_TYPEUTIL_NAMESPACE_END
#endif
