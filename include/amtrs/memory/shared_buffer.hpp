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
#ifndef	__libamtrs__memory__shared_buffer__hpp
#define	__libamtrs__memory__shared_buffer__hpp
#include <string_view>
#include "../type_traits/pair_base.hpp"
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


// *****************************************************************************
//! 共有バッファ。
// ----------------------------------------------------------------------------
//! バッファをポインタで管理するため、複数の場所で同じインスタンスを参照できます。
//! 参照カウンタで寿命管理された配列で、std::shared_ptr<T[]> に近いものになって
//! います。shared_ptrとの違いとして、
//!		・サイズを持っている
//!		・begin(), end() のイテレータが使える
//!	というメリットがあります。
//! また、確保されたバッファの最後尾には必ず char ひとつ分の未使用領域が確保
//! され、0 でクリアされます。つまり、shared_buffer<char>::data() を使った場合
//! には、必ず 0 で終わる文字列のバッファとして利用できることを意味します。
// ----------------------------------------------------------------------------

template<class T, class H = void, class Alloc = std::allocator<T>>
class	shared_buffer;

namespace __ {

template<class T, class H, class Alloc>
struct	shared_buffer_traits
{
	using	size_type			= size_t;
	using	difference_type		= ptrdiff_t;
	using	value_type			= T;
	using	const_value_type	= typename std::add_const<T>::type;
	using	pointer				= T*;
	using	reference			= T&;
	using	const_pointer		= typename std::add_const<T>::type*;
	using	const_reference		= typename std::add_const<T>::type&;
	using	allocator_type		= Alloc;
	using	header_type			= H;
};


template<bool IsClass, class Header>
struct	shared_buffer_instance_base;

template<class Header>
struct	shared_buffer_instance_base<true, Header>
{
	using	type	= pair_base_t<ref_count, Header>;
};

template<class Header>
struct	shared_buffer_instance_base<false, Header>
{
	struct	type
			: ref_count
	{
		template<class... Args>
		type(ref_count _r, Args&&... _args)
			: ref_count(_r)
			, headerValue(std::forward<Args>(_args)...)
		{}

		Header	headerValue;

		operator Header* () noexcept { return &headerValue; }
		operator Header const* () const noexcept { return &headerValue; }
	};
};

template<>
struct	shared_buffer_instance_base<false, void>
{
	using	type	= ref_count;
};



template<class T, class H, class Alloc>
struct	shared_buffer_instance
		: shared_buffer_instance_base<std::is_class<H>::value, H>::type
		, Alloc
{
private:
	using	__base_type			= typename shared_buffer_instance_base<std::is_class<H>::value, H>::type;
public:
	using	traits_type			= shared_buffer_traits<T, H, Alloc>;
	using	size_type			= typename traits_type::size_type;
	using	difference_type		= typename traits_type::difference_type;
	using	value_type			= typename traits_type::value_type;
	using	const_value_type	= typename traits_type::const_value_type;
	using	pointer				= typename traits_type::pointer;
	using	reference			= typename traits_type::reference;
	using	const_pointer		= typename traits_type::const_pointer;
	using	const_reference		= typename traits_type::const_reference;
	using	allocator_type		= typename traits_type::allocator_type;

	shared_buffer_instance(size_type _len, allocator_type const& _alloc)
		: allocator_type(_alloc)
	{
		init_array(_len);
	}

	template<class... Args>
	shared_buffer_instance(size_type _len, allocator_type const& _alloc, Args&&... _args)
		: __base_type(ref_count(), std::forward<Args>(_args)...)
		, allocator_type(_alloc)
	{
		init_array(_len);
	}

	virtual ~shared_buffer_instance()
	{
		value_type*		val		= data();
		for (size_type i = 0; i < length; ++i)
		{
			std::allocator_traits<allocator_type>::destroy(*this, &val[i]);
		}
	}

	bool dec_ref()
	{
		if (ref_count::dec_ref(0))
		{
			std::allocator_traits<allocator_type>::destroy(*this, this);

			size_type		len		= compute_size(length);
			std::allocator_traits<allocator_type>::deallocate(*this, reinterpret_cast<T*>(this), len);
		}
		return	false;
	}


	static constexpr size_type compute_size(size_type _length) noexcept
	{
		size_type	total	= (sizeof(shared_buffer_instance) + sizeof(T) * (1 + _length) - 1);
		return	(total / sizeof(T)) * sizeof(T);
	}

	size_type	length	= 0;

	pointer data() noexcept { return reinterpret_cast<pointer>(this + 1); }
	const_pointer data() const noexcept { return reinterpret_cast<const_pointer>(this + 1); }

private:
	void init_array(size_type _len)
	{
		value_type*		val		= data();
		for (length = 0; length < _len; ++length)
		{
			std::allocator_traits<allocator_type>::construct(*this, &val[length]);
		}
	}
};

struct	shared_buffer_iterator_incrementor
{
	template<class T> void inc(T& _r) { ++_r; }
	template<class T> void dec(T& _r) { --_r; }
};

struct	shared_buffer_iterator_decrementor
{
	template<class T> void inc(T& _r) { --_r; }
	template<class T> void dec(T& _r) { ++_r; }
};

template<class T, class Containor, class Increment>
class	basic_shared_buffer_iterator
{
public:
	using	value_type			= T;
	using	pointer				= T*;
	using	const_pointer		= typename std::add_const<T>::type*;
	using	reference			= T&;
	using	const_reference		= typename std::add_const<T>::type&;
	using	iterator_category	= std::random_access_iterator_tag;
	using	difference_type		= typename Containor::traits_type::difference_type;

	basic_shared_buffer_iterator() = default;
	basic_shared_buffer_iterator(const basic_shared_buffer_iterator&) = default;
	basic_shared_buffer_iterator(basic_shared_buffer_iterator&&) = default;
	basic_shared_buffer_iterator& operator = (const basic_shared_buffer_iterator&) = default;
	basic_shared_buffer_iterator& operator = (basic_shared_buffer_iterator&&) = default;
	basic_shared_buffer_iterator(pointer _p) : mCursor(_p) {}

	pointer   operator -> () const noexcept { return  mCursor; }
	reference operator *  () const noexcept { return *mCursor; }

	basic_shared_buffer_iterator& operator ++ () noexcept { Increment{}.inc(mCursor); return *this; }
	basic_shared_buffer_iterator  operator ++ (int) noexcept { basic_shared_buffer_iterator retval(*this); ++(*this); return retval; }
	basic_shared_buffer_iterator& operator -- () noexcept { Increment{}.dec(mCursor); return *this; }
	basic_shared_buffer_iterator  operator -- (int) noexcept { basic_shared_buffer_iterator retval(*this); --(*this); return retval; }

	bool operator == (basic_shared_buffer_iterator const& _r) const noexcept { return mCursor == _r.mCursor; }
	bool operator != (basic_shared_buffer_iterator const& _r) const noexcept { return mCursor != _r.mCursor; }

	difference_type operator - (basic_shared_buffer_iterator const& _r) const noexcept { return mCursor - _r.mCursor; }

protected:
	pointer	mCursor;
};


template<class T, class Containor>
using	shared_buffer_iterator			= basic_shared_buffer_iterator<T, Containor, shared_buffer_iterator_incrementor>;

template<class T, class Containor>
using	shared_buffer_reverse_iterator	= basic_shared_buffer_iterator<T, Containor, shared_buffer_iterator_decrementor>;




template<class T, class H, class Alloc>
struct	shared_buffer_base
{
	using	traits_type					= shared_buffer_traits<T, H, Alloc>;
	using	instance_type				= shared_buffer_instance<T, H, Alloc>;
	using	size_type					= typename instance_type::size_type;
	using	allocator_type				= typename instance_type::allocator_type;
	using	value_type					= typename instance_type::value_type;
	using	const_value_type			= typename instance_type::const_value_type;
	using	pointer						= typename instance_type::pointer;
	using	const_pointer				= typename instance_type::const_pointer;
	using	reference					= typename instance_type::reference;
	using	const_reference				= typename instance_type::const_reference;
	using	iterator					= __::shared_buffer_iterator<value_type, shared_buffer_base>;
	using	const_iterator				= __::shared_buffer_iterator<const_value_type, shared_buffer_base>;
	using	reverse_iterator			= __::shared_buffer_reverse_iterator<value_type, shared_buffer_base>;
	using	const_reverse_iterator		= __::shared_buffer_reverse_iterator<const_value_type, shared_buffer_base>;

	shared_buffer_base() = default;
	shared_buffer_base(const shared_buffer_base&) = default;
	shared_buffer_base(shared_buffer_base&&) = default;
	shared_buffer_base& operator = (const shared_buffer_base&) = default;
	shared_buffer_base& operator = (shared_buffer_base&&) = default;
	~shared_buffer_base() = default;

	
	bool operator == (const shared_buffer_base& _r) const noexcept { return mInstance == _r.mInstance; }
	bool operator != (const shared_buffer_base& _r) const noexcept { return mInstance != _r.mInstance; }


	size_type size() const noexcept
	{
		return	mInstance ? mInstance->length : 0;
	}


	bool empty() const noexcept
	{
		return	size() == 0;
	}

	pointer data() noexcept
	{
		return	mInstance->data();
	}

	const_pointer data() const noexcept
	{
		return	mInstance->data();
	}

	iterator begin() noexcept { return data(); }
	const_iterator cbegin() const noexcept { return data(); }
	const_iterator begin() const noexcept { return cbegin(); }
	iterator end() noexcept { return data() + size(); }
	const_iterator cend() const noexcept { return data() + size(); }
	const_iterator end() const noexcept { return cend(); }


	reverse_iterator rbegin() noexcept { return data() + size()-1; }
	const_reverse_iterator crbegin() const noexcept { return data() + size()-1; }
	const_reverse_iterator rbegin() const noexcept { return crbegin(); }
	reverse_iterator rend() noexcept { return data() - 1; }
	const_reverse_iterator crend() const noexcept { return data() - 1; }
	const_reverse_iterator rend() const noexcept { return crend(); }

	template<class I> reference operator [] (I _index) noexcept { return data()[_index]; }
	template<class I> const_reference operator [] (I _index) const noexcept { return data()[_index]; }
	
protected:
	template<class... HArgs>
	void create(size_type _length, allocator_type _alloc, std::tuple<HArgs...> _hargs)
	{
		size_type		len		= instance_type::compute_size(_length);

		T*				buf		= std::allocator_traits<allocator_type>::allocate(_alloc, len);
		instance_type*	inst	= reinterpret_cast<instance_type*>(buf);
		construct(_alloc, inst, _length, _hargs, std::make_index_sequence<sizeof...(HArgs)>());
		mInstance	= inst;
	}


	template<class A, class... HArgs, std::size_t... Indecies>
	void construct(A& _alloc, instance_type* _inst, size_type _length, std::tuple<HArgs...> _hargs, std::index_sequence<Indecies...>)
	{
		std::allocator_traits<allocator_type>::construct(_alloc, _inst, _length, _alloc, std::get<Indecies>(_hargs)...);
	}



	template<class HArg>
	void create(size_type _length, allocator_type _alloc,  HArg&& _hargs)
	{
		size_type		len		= instance_type::compute_size(_length);

		T*				buf		= std::allocator_traits<allocator_type>::allocate(_alloc, len);
		instance_type*	inst	= reinterpret_cast<instance_type*>(buf);
		std::allocator_traits<allocator_type>::construct(_alloc, inst, _length, _alloc, std::forward<HArg>(_hargs));
		mInstance	= inst;
	}


	ref<instance_type>	mInstance;
};


}


template<class T, class H, class Alloc>
class	shared_buffer
		: public __::shared_buffer_base<T, H, Alloc>
{
	using	__super_type	= __::shared_buffer_base<T, H, Alloc>;
	using	__traits_type	= __::shared_buffer_traits<T, H, Alloc>;
public:
	using	size_type					= typename __traits_type::size_type;
	using	value_type					= typename __traits_type::value_type;
	using	const_value_type			= typename std::add_const<value_type>::type;
	using	pointer						= typename __traits_type::pointer;
	using	reference					= typename __traits_type::reference;
	using	const_pointer				= typename __traits_type::const_pointer;
	using	const_reference				= typename __traits_type::const_reference;
	using	allocator_type				= typename __traits_type::allocator_type;
	using	header_type					= H;


	shared_buffer() = default;
	shared_buffer(const shared_buffer&) = default;
	shared_buffer(shared_buffer&&) = default;
	shared_buffer& operator = (const shared_buffer&) = default;
	shared_buffer& operator = (shared_buffer&&) = default;


	shared_buffer(size_type _size, allocator_type const& _alloc = allocator_type())
	{
		__super_type::create(_size, _alloc, header_type());
	}

	template<class... HArgs>
	shared_buffer(size_type _size, std::tuple<HArgs...> _hargs, allocator_type const& _alloc = allocator_type())
	{
		__super_type::create(_size, _alloc, _hargs);
	}


	template<class HArg>
	shared_buffer(size_type _size, HArg&& _harg, allocator_type const& _alloc = allocator_type())
	{
		__super_type::create(_size, _alloc, std::forward<HArg>(_harg));
	}

	template<class HArg>
	shared_buffer(size_type _size, std::initializer_list<HArg> _harg, allocator_type const& _alloc = allocator_type())
	{
		__super_type::create(_size, _alloc, _harg);
	}


	header_type& header() noexcept
	{
		auto*	instance	= __super_type::mInstance.get();
		return	*(header_type*)instance;
	}


	header_type const& header() const noexcept
	{
		auto*	instance	= __super_type::mInstance.get();
		return	*(header_type const*)instance;
	}

	using	__super_type::operator ==;
	using	__super_type::operator !=;


protected:


	template<class, class, class>
	friend	struct	__::shared_buffer_base;
};





template<class T, class Alloc>
class	shared_buffer<T, void, Alloc>
		: public __::shared_buffer_base<T, void, Alloc>
{
	using	__super_type	= __::shared_buffer_base<T, void, Alloc>;
	using	__traits_type	= __::shared_buffer_traits<T, void, Alloc>;



public:
	using	size_type		= typename __traits_type::size_type;
	using	value_type		= typename __traits_type::value_type;
	using	pointer			= typename __traits_type::pointer;
	using	reference		= typename __traits_type::reference;
	using	const_pointer	= typename __traits_type::const_pointer;
	using	const_reference	= typename __traits_type::const_reference;
	using	allocator_type	= typename __traits_type::allocator_type;


	shared_buffer() = default;
	shared_buffer(const shared_buffer&) = default;
	shared_buffer(shared_buffer&&) = default;
	shared_buffer& operator = (const shared_buffer&) = default;
	shared_buffer& operator = (shared_buffer&&) = default;


	shared_buffer(size_type _size, allocator_type const& _alloc = allocator_type())
	{
		__super_type::create(_size, _alloc, std::make_tuple());
	}

	using	__super_type::operator ==;
	using	__super_type::operator !=;

protected:
	template<class, class, class>
	friend	struct	__::shared_buffer_base;
};


AMTRS_NAMESPACE_END
#endif
