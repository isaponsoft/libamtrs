/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__util__callback__hpp
#define	__libamtrs__util__callback__hpp
#include <memory>
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
template<class...> class callback;


// std::function に似た機能を提供しますが std::bind などをサポートしない分軽量です。
template<class R, class... Args>
class	callback<R(Args...)>
{
protected:
	struct	invoker
	{
		virtual ~invoker() {}
		virtual R invoke(Args&&...) = 0;
	};

public:
	callback() = default;
	callback(callback&& _r) = default;
	callback& operator = (callback&& _r) = default;

	template<class Func>
	callback(Func _func)
	{
		struct	impl : invoker
		{
			Func	func;
			impl(Func _func) : func(std::move(_func)) {}
			virtual R invoke(Args&&... _params) override { return func(std::forward<Args>(_params)...); }
		};
		mInvoker	= std::unique_ptr<invoker>(new impl(std::move(_func)));
	}

	R operator () (Args... _params) const
	{
		return	mInvoker->invoke(std::forward<Args>(_params)...);
	}

	void swap(callback& _r) noexcept { std::swap(mInvoker, _r.mInvoker); }
	void clear() { mInvoker.reset(); }
	bool empty() const noexcept { return mInvoker == nullptr; }

	callback(const callback&) = delete;
	callback& operator = (const callback&) = delete;
private:
	std::unique_ptr<invoker>	mInvoker;
};

AMTRS_NAMESPACE_END
#endif
