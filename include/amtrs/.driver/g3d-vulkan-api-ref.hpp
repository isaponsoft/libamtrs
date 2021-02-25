/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__ref__hpp
#define	__libamtrs__g3d__vulkan__ref__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN
template<class...>	class	vkref;
template<class...>	class	vkhandle;
template<class...>	struct	vkhandle_traits;
AMTRS_G3D_VKUTIL_NAMESPACE_END
namespace std {
	template<class VK, class T>
	AMTRS_G3D_VKUTIL_NAMESPACE::vkref<VK> get(AMTRS_G3D_VKUTIL_NAMESPACE::vkref<T> const& v)
	{
		if (v.empty())
		{
			return	AMTRS_G3D_VKUTIL_NAMESPACE::vkref<VK>();
		}
		return	std::get<AMTRS_G3D_VKUTIL_NAMESPACE::vkref<VK>>(v.mRef->mDeps);
	}
}



AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN
template<class T>
class	vkhandle<T>
		: public ref_object
		, public vkhandle_traits<T>
{
public:
	using	value_type	= T;
	using	traits_type	= vkhandle_traits<T>;

	vkhandle(value_type _)
		: mHandle(_)
	{
	}

	~vkhandle()
	{
		destroy();
	}

	bool empty() const noexcept { return mHandle == VK_NULL_HANDLE; }
	value_type release() noexcept { value_type r(mHandle); mHandle = VK_NULL_HANDLE; return r; }
	void destroy() noexcept
	{
		if (!empty())
		{
//			std::cout << "[" << (void*)this << "] destroy:" << typeid(*this).name() << ":" << (void*)mHandle << std::endl;
			traits_type::destroy(mHandle);
			mHandle	= VK_NULL_HANDLE;
		}
	}

	value_type get() const noexcept { return mHandle; }
	operator value_type () const noexcept { return get(); }

protected:
	value_type	mHandle = VK_NULL_HANDLE;
};



template<class T, class... Deps>
class	vkhandle<T, Deps...>
		: public ref_object
		, public vkhandle_traits<T>
{
public:
	using	value_type	= T;
	using	traits_type	= vkhandle_traits<T>;

	template<class... Args>
	vkhandle(value_type _, vkref<Deps> const&... _deps)
		: mDeps(_deps...)
		, mHandle(_)
	{
	}


	~vkhandle()
	{
//		std::cout << "[" << (void*)this << "] destroy:" << typeid(*this).name() << ":" << (void*)mHandle << std::endl;
		destroy();
	}

	bool empty() const noexcept { return mHandle == VK_NULL_HANDLE; }
	value_type release() noexcept { value_type r(mHandle); mHandle = VK_NULL_HANDLE; return r; }
	void destroy() noexcept
	{
		if (!empty())
		{
			destroy(std::make_index_sequence<sizeof...(Deps)>());
		}
	}

	value_type get() const noexcept { return mHandle; }
	operator value_type () const noexcept { return get(); }

private:
	template <class I, I... Seq>
	void destroy(std::integer_sequence<I, Seq...> _index)
	{
		traits_type::destroy(std::get<Seq>(mDeps)..., mHandle);
		mHandle	= VK_NULL_HANDLE;
		mDeps	= {};
	}

protected:
	std::tuple<vkref<Deps>...>	mDeps;
	value_type					mHandle = VK_NULL_HANDLE;


	template<class R, class S>
	friend	vkref<R> std::get(vkref<S> const&);

};


template<class T, class... Deps>
class	vkref_impl
{
public:
	using	handle_type	= vkhandle<T, Deps...>;
	using	value_type	= typename handle_type::value_type;
	using	traits_type	= typename handle_type::traits_type;

	vkref_impl() = default;
	vkref_impl(vkref_impl const&) = default;
	vkref_impl(vkref_impl&&) = default;
	vkref_impl& operator = (vkref_impl const&) = default;
	vkref_impl& operator = (vkref_impl&&) = default;
	vkref_impl(vkref<T> const& _) : vkref_impl(static_cast<vkref_impl const&>(_)) {}
	vkref_impl(vkref<T>&& _) : vkref_impl(static_cast<vkref_impl&&>(_)) {}
	vkref<T>& operator = (vkref<T> const& _) { operator = (static_cast<vkref_impl const&>(_)); return static_cast<vkref<T>&>(*this); }
	vkref<T>& operator = (vkref<T>&& _) { operator = (static_cast<vkref_impl&&>(_)); return static_cast<vkref<T>&>(*this); }


	static vkref<T> _make(T _handle, vkref<Deps> const&... _deps)
	{
		return	_handle != VK_NULL_HANDLE
				? vkref<T>(_handle, _deps...)
				: vkref<T>();
	}

	explicit vkref_impl(T _handle, vkref<Deps> const&... _values)
		: mRef(new handle_type(_handle, _values...))
	{
	}

	value_type get() const noexcept
	{
		return	mRef.empty() ? VK_NULL_HANDLE : mRef->get();
	}

	void swap(vkref_impl& _r) noexcept
	{
		std::swap(mRef, _r.mRef);
	}

	void clear()
	{
		mRef	= nullptr;
	}

	// ==============================================================
	//! 強制的にハンドルを破棄します。
	// --------------------------------------------------------------
	//! 他から参照が残っていようと強制的にハンドルを破棄します。
	//! 主に試験目的での使用を想定しています。
	// --------------------------------------------------------------
	void force_destroy()
	{
		mRef->destroy();
		mRef	= nullptr;
	}

	bool empty() const noexcept
	{
		return	mRef.empty() || mRef->empty();
	}

	operator value_type () const noexcept
	{
		return	get();
	}

	ref<handle_type> handle() const noexcept
	{
		return	mRef;
	}

protected:
	traits_type& traits() noexcept { return *mRef; }
	traits_type const& traits() const noexcept { return *mRef; }

	ref<handle_type>	mRef;

	template<class R, class S>
	friend	vkref<R> std::get(vkref<S> const&);
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
