/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__memory__ref_object__hpp
#define	__libamtrs__memory__ref_object__hpp
#include <atomic>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


template<class T, class CounterT = std::atomic<T>>
class	basic_ref_object
{
	using	counter			= CounterT;
public:
	using	counter_type	= T;

	basic_ref_object() = default;

	// カウンターは移動もコピーもしない
	basic_ref_object(basic_ref_object&& _r) {}
	basic_ref_object(const basic_ref_object& _r) {}
	basic_ref_object& operator = (basic_ref_object&& _r) { return *this; }
	basic_ref_object& operator = (const basic_ref_object& _r) { return *this; }

	virtual ~basic_ref_object() = default;

	void add_ref() const
	{
		++mUsingCount;
	}


	void dec_ref() const
	{
		if ((--mUsingCount) == 0)
		{
			delete	this;
		}
	}

	counter_type use_count() const noexcept
	{
		return	mUsingCount.load();
	}

protected:
	mutable counter			mUsingCount	= 0;

};




class	ref_object
		: public basic_ref_object<std::size_t>
{
	using	_base_ref_type		= basic_ref_object<std::size_t>;
public:
	using	_base_ref_type::_base_ref_type;


private:
	using	_base_ref_type::mUsingCount;
};



template<class P>
class	basic_ref_container
{
public:
	using	value_type	= typename std::remove_pointer<P>::type;
	using	pointer		= P;
	using	reference	= value_type&;

	constexpr basic_ref_container()
		: p(nullptr)
	{}


	constexpr basic_ref_container(pointer _ptr)
		: p(_ptr)
	{
		if (p)
		{
			p->add_ref();
		}
	}


	constexpr basic_ref_container(const basic_ref_container& _r)
		: p(_r.p)
	{
		if (p)
		{
			p->add_ref();
		}
	}


	constexpr basic_ref_container(basic_ref_container&& _r)
		: p(_r.p)
	{
		_r.p	= nullptr;
	}


	~basic_ref_container()
	{
		if (p)
		{
			p->dec_ref();
			p	= nullptr;
		}
	}

	pointer get() const noexcept
	{
		return	p;
	}

	void set(pointer _ptr) const noexcept
	{
		p	= _ptr;
	}

	pointer release() noexcept
	{
		pointer	retval(p);
		p	= nullptr;
		return	retval;
	}

	bool empty() const noexcept
	{
		return	p == nullptr;
	}

	void swap(basic_ref_container& _b)
	{
		std::swap(p, _b.p);
	}

	bool operator == (pointer _ptr) const noexcept
	{
		return	p == _ptr;
	}

	bool operator == (const basic_ref_container& _r) const noexcept
	{
		return	p == _r.p;
	}

private:
	pointer	p;
};


template<class T, class Container = basic_ref_container<T>>
class	basic_ref
{
	using	container	= Container;
public:
	using	value_type	= typename container::value_type;
	using	pointer		= typename container::pointer;
	using	reference	= typename container::reference;


	constexpr basic_ref() = default;

	//! assign
	constexpr basic_ref(pointer _ptr)
		: mRef(_ptr)
	{}

	//! copy
	constexpr basic_ref(const basic_ref& _r)
		: mRef(_r.mRef)
	{}

	//! move
	constexpr basic_ref(basic_ref&& _r)
		: mRef(std::move(_r.mRef))
	{}

	//! assign
	template<class ST, class SC>
	constexpr basic_ref(const basic_ref<ST, SC>& _r)
		: mRef(_r.get())
	{}

	//! assign-move
	template<class ST, class SC>
	constexpr basic_ref(basic_ref<ST, SC>&& _r)
		: mRef(_r.get())
	{
		_r.clear();
	}


	//! 参照を増やさずにリファレンスを生成します。
	static basic_ref make(pointer _ptr)
	{
		basic_ref retval;
		retval.mRef.set(_ptr);
		return retval;
	}


	constexpr reference operator * () const { return *mRef.get(); }
	constexpr pointer operator -> () const noexcept { return mRef.get(); }
	constexpr operator pointer () const noexcept { return mRef.get(); }
	constexpr bool empty() const noexcept { return mRef.empty(); }
	constexpr void clear() noexcept { mRef.~container(); }
	constexpr pointer release() noexcept { return mRef.release(); }
	constexpr pointer get() const noexcept { return mRef.get(); }


	constexpr basic_ref& operator = (const std::nullptr_t)
	{
		clear();
		return	*this;
	}


	constexpr basic_ref& operator = (pointer _ptr)
	{
		if (*this != _ptr)
		{
			container	tmp(std::move(mRef));
			new (&mRef) container(_ptr);
		}
		return	*this;
	}

	constexpr basic_ref& operator = (const basic_ref& _right)
	{
		if (*this != _right)
		{
			container	tmp(std::move(mRef));
			new (&mRef) container(_right.mRef);
		}
		return	*this;
	}

	constexpr basic_ref& operator = (basic_ref&& _right)
	{
		if (*this == _right)
		{
			_right.clear();
		}
		else
		{
			container	tmp(std::move(mRef));
			new (&mRef) container(std::move(_right.mRef));
		}
		return	*this;
	}


	template<typename _ST, class SC>
	constexpr basic_ref& operator = (const basic_ref<_ST, SC>& _right)
	{
		if (mRef.get() != _right.get())
		{
			container	tmp(std::move(mRef));
			new (&mRef) container(_right.mRef.get());
		}
		return	*this;
	}

	template<typename _ST, class SC>
	constexpr basic_ref& operator = (basic_ref<_ST, SC>&& _right)
	{
		if (mRef.get() != _right.get())
		{
			container	tmp(std::move(mRef));
			new (&mRef) container(_right.mRef.get());
		}
		_right.clear();
		return	*this;
	}

	void swap(basic_ref& _b)
	{
		std::swap(mRef, _b.mRef);
	}

	constexpr bool operator == (pointer _ptr) const noexcept { return mRef == _ptr; }
	constexpr bool operator != (pointer _ptr) const noexcept { return !(mRef == _ptr); }
	constexpr bool operator == (const basic_ref& _right) const noexcept { return mRef == _right.mRef; }
	constexpr bool operator != (const basic_ref& _right) const noexcept { return !(mRef == _right.mRef); }

private:
	container	mRef;

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
