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

template<class...>
struct	ref_traits;

class	ref_count
{
public:
	ref_count() = default;

	// カウンターは移動もコピーもしない
	ref_count(ref_count&& _r) {}
	ref_count(const ref_count& _r) {}
	ref_count& operator = (ref_count&& _r) { return *this; }
	ref_count& operator = (const ref_count& _r) { return *this; }

	void add_ref()
	{
		++mUsingCount;
	}

	bool dec_ref()
	{
		return	dec_ref(0);
	}

	bool dec_ref(int)
	{
		return	(--mUsingCount) == 0;
	}


	std::size_t use_count() const noexcept
	{
		return	mUsingCount.load();
	}


private:
	mutable std::atomic<std::size_t>	mUsingCount	= 0;
};



//! 標準の ref_traits に対応する参照カウンタ
class	ref_object : public ref_count
{
public:
	virtual ~ref_object() = default;


	void dec_ref()
	{
		if (dec_ref(0))
		{
			delete	this;
		}
	}

	bool dec_ref(int)
	{
		return	ref_count::dec_ref(0);
	}

private:
	template<class...>
	friend	struct	ref_traits;
};




template<class T>
struct	ref_traits<T*>
{
	using	object_type	= T*;
	using	pointer		= T*;
	using	reference	= T&;

	constexpr void construct(object_type& _o)
	{
		_o	= nullptr;
	}

	constexpr void construct(object_type& _o, object_type const& _s)
	{
		_o	= _s;
		if (_o)
		{
			_o->add_ref();
		}
	}

	constexpr void construct(object_type& _o, object_type&& _s)
	{
		_o	= _s;
		_s	= nullptr;
	}

	constexpr void destruct(object_type& _o)
	{
		if (_o)
		{
			_o->dec_ref();
			_o	= nullptr;
		}
	}

	constexpr object_type release(object_type& _o)
	{
		object_type	r(_o);
		_o	= nullptr;
		return	r;
	}

	constexpr bool empty(object_type const& _o)
	{
		return	_o == nullptr;
	}
};


template<class T, class Traits = ref_traits<T>>
class	basic_ref
{
	using	traits_type	= Traits;
public:
	using	object_type	= typename traits_type::object_type;
	using	pointer		= typename traits_type::pointer;
	using	reference	= typename traits_type::reference;


	constexpr basic_ref()
	{
		traits_type{}.construct(mRef);
	}

	//! assign
	constexpr basic_ref(pointer _ptr)
	{
		object_type	tmp(_ptr);
		traits_type{}.construct(mRef, tmp);
	}

	//! copy
	constexpr basic_ref(const basic_ref& _r)
	{
		traits_type{}.construct(mRef, _r.mRef);
	}

	//! move
	constexpr basic_ref(basic_ref&& _r)
	{
		traits_type{}.construct(mRef, std::move(_r.mRef));
	}

	//! assign
	template<class ST, class SC>
	constexpr basic_ref(const basic_ref<ST, SC>& _r)
	{
		object_type	w(_r.mRef);
		traits_type{}.construct(mRef, w);
	}

	//! assign-move
	template<class ST, class SC>
	constexpr basic_ref(basic_ref<ST, SC>&& _r)
	{
		object_type	w(_r.release());
		traits_type{}.construct(mRef, std::move(w));
	}

	~basic_ref()
	{
		clear();
	}


	constexpr reference operator * () const { return *mRef; }
	constexpr pointer operator -> () const noexcept { return mRef; }
	constexpr operator pointer () const noexcept { return mRef; }
	constexpr bool empty() const noexcept { return traits_type{}.empty(mRef); }
	constexpr void clear() noexcept { traits_type{}.destruct(mRef); }
	constexpr pointer release() noexcept { return traits_type{}.release(mRef); }
	constexpr pointer get() const noexcept { return mRef; }


	constexpr basic_ref& operator = (const std::nullptr_t)
	{
		clear();
		return	*this;
	}


	constexpr basic_ref& operator = (pointer _ptr)
	{
		if (*this != _ptr)
		{
			basic_ref	tmp(std::move(*this));
			object_type	w(_ptr);
			traits_type{}.construct(mRef, w);
		}
		return	*this;
	}

	constexpr basic_ref& operator = (const basic_ref& _r)
	{
		if (*this != _r)
		{
			basic_ref	tmp(std::move(*this));
			traits_type{}.construct(mRef, _r.mRef);
		}
		return	*this;
	}

	constexpr basic_ref& operator = (basic_ref&& _r)
	{
		if (*this == _r)
		{
			_r.clear();
		}
		else
		{
			basic_ref	tmp(std::move(*this));
			traits_type{}.construct(mRef, std::move(_r.mRef));
		}
		return	*this;
	}


	template<typename _ST, class SC>
	constexpr basic_ref& operator = (const basic_ref<_ST, SC>& _r)
	{
		if (mRef != _r.get())
		{
			basic_ref	tmp(std::move(*this));
			object_type	w(_r.mRef);
			traits_type{}.construct(mRef, w);
		}
		return	*this;
	}

	template<typename _ST, class SC>
	constexpr basic_ref& operator = (basic_ref<_ST, SC>&& _r)
	{
		if (mRef != _r.get())
		{
			basic_ref	tmp(std::move(*this));
			object_type	w(_r.release());
			traits_type{}.construct(mRef, std::move(w));
		}
		_r.clear();
		return	*this;
	}

	void swap(basic_ref& _b)
	{
		std::swap(mRef, _b.mRef);
	}

	constexpr bool operator == (pointer _ptr) const noexcept { return mRef == _ptr; }
	constexpr bool operator != (pointer _ptr) const noexcept { return !(mRef == _ptr); }
	constexpr bool operator == (const basic_ref& _r) const noexcept { return mRef == _r.mRef; }
	constexpr bool operator != (const basic_ref& _r) const noexcept { return !(mRef == _r.mRef); }

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
