/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__basic__ref_object__hpp
#define	__libamtrs__basic__ref_object__hpp
#include <atomic>
AMTRS_NAMESPACE_BEGIN

template<class...>
struct	ref_traits;

class	ref_object
{
public:
	ref_object() = default;
	virtual ~ref_object() {}

	// カウンターは移動もコピーもしない
	ref_object(ref_object&& _r) {}
	ref_object(const ref_object& _r) {}
	ref_object& operator = (ref_object&& _r) { return *this; }
	ref_object& operator = (const ref_object& _r) { return *this; }

	void add_ref() const noexcept
	{
		++mUsingCount;
	}

	void dec_ref() const noexcept
	{
		if (_dec_ref())
		{
			delete this;
		}
	}

	std::size_t use_count() const noexcept
	{
		return	mUsingCount.load();
	}


protected:
	bool _dec_ref() const noexcept
	{
		return	(--mUsingCount) == 0;
	}

private:
	mutable std::atomic<std::size_t>	mUsingCount	= 0;
};




template<class Ptr, class... Dummy>
struct	ref_traits<Ptr, Dummy...>
{
	using	object_type	= Ptr;
	using	pointer		= Ptr;
	using	reference	= typename std::add_lvalue_reference<decltype(*std::declval<Ptr>())>::type;

protected:
	static constexpr object_type construct(ref_traits const& _traits)
	{
		return	nullptr;
	}


	template<class RTraits, class RPtr>
	static constexpr object_type copy(ref_traits const& _traits, RTraits const& _rtaits, RPtr const& _ptr)
	{
		_ptr->add_ref();
		return	_ptr;
	}


	template<class RTraits, class RPointer>
	static constexpr object_type move(ref_traits const& _traits, RTraits const&, RPointer _s)
	{
		object_type	r(_s);
		_s	= nullptr;
		return	r;
	}

	constexpr void destruct(pointer _ptr)
	{
		_ptr->dec_ref();
	}

	static constexpr bool empty(object_type const& _o)
	{
		return	_o == nullptr;
	}
};


template<class T, class Traits = ref_traits<T>>
class	basic_ref
		: public Traits
{
public:
	using	traits_type	= Traits;
	using	object_type	= typename traits_type::object_type;
	using	pointer		= typename traits_type::pointer;
	using	reference	= typename traits_type::reference;

	using	traits_type::traits_type;

	//! initialize
	constexpr basic_ref()
		: mRef(traits_type::construct(traits_type()))
	{}

	//! assign
	constexpr basic_ref(pointer _ptr)
		: mRef(traits_type::empty(_ptr)
			? traits_type::construct(traits_type())
			: traits_type::copy(traits_type(), traits_type(), _ptr)
		)
	{}

	//! copy
	constexpr basic_ref(basic_ref const& _r)
		: mRef(_r.empty()
			? traits_type::construct(traits_type())
			: traits_type::copy(traits_type(), traits_type(), _r.mRef)
		)
	{}

	//! cast
	template<class ST, class SC>
	constexpr basic_ref(basic_ref<ST, SC> const& _r)
		: mRef(_r.empty()
			? traits_type::construct(traits_type())
			: traits_type::copy(traits_type(), typename basic_ref<ST, SC>::traits_type(), _r.mRef)
		)
	{}

	//! move
	constexpr basic_ref(basic_ref&& _r)
		: mRef(_r.empty()
			? traits_type::construct(traits_type())
			: traits_type::move(traits_type(), traits_type(), _r.release())
		)
	{}

	//! cast & move
	template<class ST, class SC>
	constexpr basic_ref(basic_ref<ST, SC>&& _r)
		: mRef(_r.empty()
			? traits_type::construct(traits_type())
			: traits_type::move(traits_type(), typename basic_ref<ST, SC>::traits_type(), _r.release())
		)
	{}

	~basic_ref()
	{
		clear();
	}


	//! 参照を増やさずにリファレンスを生成します。
	static basic_ref make(pointer _ptr)
	{
		basic_ref	retval;
		retval.mRef	= _ptr;
		return	retval;
	}


	constexpr basic_ref& operator = (std::nullptr_t)
	{
		clear();
		return	*this;
	}


	constexpr basic_ref& operator = (pointer _ptr)
	{
		if (*this != _ptr)
		{
			if (traits_type::empty(_ptr))
			{
				clear();
			}
			else
			{
				basic_ref	tmp(std::move(*this));
				mRef	= traits_type::copy(traits_type(), traits_type(), _ptr);
			}
		}
		return	*this;
	}

	//! assin
	constexpr basic_ref& operator = (basic_ref const& _r)
	{
		if (*this != _r)
		{
			if (_r.empty())
			{
				clear();
			}
			else
			{
				basic_ref	tmp(std::move(*this));
				mRef	= traits_type::copy(traits_type(), traits_type(), _r.mRef);
			}
		}
		return	*this;
	}

	//! assin - cast
	template<typename ST, class SC>
	constexpr basic_ref& operator = (basic_ref<ST, SC> const& _r)
	{
		if (mRef != _r.get())
		{
			if (_r.empty())
			{
				clear();
			}
			else
			{
				basic_ref	tmp(std::move(*this));
				mRef	= traits_type::copy(traits_type(), typename basic_ref<ST, SC>::traits_type(), _r.mRef);
			}
		}
		return	*this;
	}

	//! move
	constexpr basic_ref& operator = (basic_ref&& _r)
	{
		if (_r.empty())
		{
			clear();
		}
		else if (mRef == _r.get())
		{
			_r.clear();
		}
		else
		{
			basic_ref	tmp(std::move(*this));
			mRef	= traits_type::move(traits_type(), traits_type(), _r.release());
		}
		return	*this;
	}

	//! move - cast
	template<typename ST, class SC>
	constexpr basic_ref& operator = (basic_ref<ST, SC>&& _r)
	{
		if (_r.empty())
		{
			clear();
		}
		else if (mRef == _r.get())
		{
			_r.clear();
		}
		else
		{
			basic_ref	tmp(std::move(*this));
			mRef	= traits_type::move(traits_type(), typename basic_ref<ST, SC>::traits_type(), _r.release());
		}
		return	*this;
	}


	constexpr bool operator == (pointer _ptr) const noexcept { return mRef == _ptr; }
	constexpr bool operator != (pointer _ptr) const noexcept { return !(mRef == _ptr); }
	constexpr bool operator == (const basic_ref& _r) const noexcept { return mRef == _r.mRef; }
	constexpr bool operator != (const basic_ref& _r) const noexcept { return !(mRef == _r.mRef); }


	constexpr reference operator * () const { return *mRef; }
	constexpr pointer operator -> () const noexcept { return mRef; }
	constexpr operator pointer () const noexcept { return mRef; }
	constexpr bool empty() const noexcept { return traits_type::empty(mRef); }
	constexpr void clear() noexcept { if (!empty()) { traits_type::destruct(release()); } }
	constexpr pointer release() noexcept { pointer r(mRef); mRef = traits_type::construct(traits_type()); return r; }
	constexpr pointer get() const noexcept { return mRef; }
	void swap(basic_ref& _b) { std::swap(mRef, _b.mRef); }


private:
	object_type	mRef;

	template<class, class>
	friend	class	basic_ref;
};


template<class _T>
using	ref	= basic_ref<_T*>;



template<class T>
class	ref_singleton
		: public ref_object
{
public:
	virtual ~ref_singleton()
	{
		instance()	= nullptr;
	}

	static ref<T> get_instance()
	{
		if (!instance())
		{
			instance()	= new T();
		}
		return	instance();
	}

protected:
	static T*& instance()
	{
		static T*	_instance = nullptr;
		return	_instance;
	}
};



template<class Type, class... Args>
ref<Type> make_ref(Args&&... _args)
{
	return	new Type(std::forward<Args>(_args)...);
}

AMTRS_NAMESPACE_END
#endif
