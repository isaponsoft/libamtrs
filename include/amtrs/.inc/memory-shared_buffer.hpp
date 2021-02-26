/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__memory__shared_buffer__hpp
#define	__libamtrs__memory__shared_buffer__hpp
#include <string_view>
#include <tuple>
#include "memory-view.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(shared_buffer)
struct	instancebase
		: public ref_object
{
	static char* allocate(size_t _size)
	{
		auto	data	= new char[_size + 1];
		data[_size]	= 0;
		return	data;

	}

	virtual void deallocate() = 0;
	void dec_ref()
	{
		if (_dec_ref())
		{
			this->~instancebase();
			delete[]	reinterpret_cast<char*>(this);
		}
	}
	size_t	elmsize	= 0;
	size_t	bytesize	= 0;
};

template<class T>
struct	noheader
		: instancebase
{
	using	super_type	= instancebase;
	using	value_type	= T;

	static noheader* create(size_t _len)
	{
		auto*	m	= allocate(sizeof(noheader) + sizeof(value_type) * _len);
		auto	t	= new ((void*)m) noheader();
		t->bytesize	= sizeof(value_type) * _len;
		return	t;
	}

	virtual void deallocate() override
	{
		value_type*		data	= reinterpret_cast<value_type*>(this + 1);
		for (int i = 0; i < super_type::elmsize; ++i)
		{
			data[i].~value_type();
		}
	}
};

template<class T, class H>
struct	headerable
		: instancebase
{
	using	super_type	= instancebase;
	using	value_type	= T;
	using	header_type	= H;

	template<class... Args>
	static headerable* create(size_t _len, Args&&... _args)
	{
		auto*	m	= allocate(sizeof(headerable) + sizeof(value_type) * _len);
		auto	t	= new ((void*)m) headerable(std::forward<Args>(_args)...);
		t->bytesize	= sizeof(value_type) * _len;
		return	t;
	}

	template<class... Args>
	headerable(Args&&... _args)
		: header(std::forward<Args>(_args)...)
	{}


	virtual void deallocate() override
	{
		value_type*		data	= reinterpret_cast<value_type*>(this + 1);
		for (int i = 0; i < super_type::elmsize; ++i)
		{
			data[i].~value_type();
		}
	}

	header_type	header;
};


struct	shared_buffer_iterator_incrementor
{
	template<class V> void inc(V& _r) { ++_r; }
	template<class V> void dec(V& _r) { --_r; }
};

struct	shared_buffer_iterator_decrementor
{
	template<class V> void inc(V& _r) { --_r; }
	template<class V> void dec(V& _r) { ++_r; }
};


AMTRS_IMPLEMENTS_END(shared_buffer)
template<class T, class Containor, class Increment>
class	shared_buffer_iterator
{
public:
	using	value_type			= T;
	using	pointer				= T*;
	using	const_pointer		= typename std::add_const<T>::type*;
	using	reference			= T&;
	using	const_reference		= typename std::add_const<T>::type&;
	using	iterator_category	= std::random_access_iterator_tag;
	using	difference_type		= typename Containor::difference_type;

	shared_buffer_iterator() = default;
	shared_buffer_iterator(const shared_buffer_iterator&) = default;
	shared_buffer_iterator(shared_buffer_iterator&&) = default;
	shared_buffer_iterator& operator = (const shared_buffer_iterator&) = default;
	shared_buffer_iterator& operator = (shared_buffer_iterator&&) = default;
	shared_buffer_iterator(pointer _p) : mCursor(_p) {}

	pointer   operator -> () const noexcept { return  mCursor; }
	reference operator *  () const noexcept { return *mCursor; }

	shared_buffer_iterator& operator ++ () noexcept { Increment{}.inc(mCursor); return *this; }
	shared_buffer_iterator  operator ++ (int) noexcept { shared_buffer_iterator retval(*this); ++(*this); return retval; }
	shared_buffer_iterator& operator -- () noexcept { Increment{}.dec(mCursor); return *this; }
	shared_buffer_iterator  operator -- (int) noexcept { shared_buffer_iterator retval(*this); --(*this); return retval; }

	bool operator == (shared_buffer_iterator const& _r) const noexcept { return mCursor == _r.mCursor; }
	bool operator != (shared_buffer_iterator const& _r) const noexcept { return mCursor != _r.mCursor; }

	difference_type operator - (shared_buffer_iterator const& _r) const noexcept { return mCursor - _r.mCursor; }

protected:
	pointer	mCursor;
};




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
template<class T, class H = void>
class	shared_buffer
{
protected:
	using	instance_type	= typename std::conditional<std::is_same<H, void>::value
									, AMTRS_IMPLEMENTS(shared_buffer)::noheader<T>
									, AMTRS_IMPLEMENTS(shared_buffer)::headerable<T, H>
							>::type;
public:
	using	size_type					= size_t;
	using	difference_type				= ptrdiff_t;
	using	value_type					= T;
	using	const_value_type			= typename std::add_const<T>::type;
	using	pointer						= T*;
	using	reference					= T&;
	using	const_pointer				= typename std::add_const<T>::type*;
	using	const_reference				= typename std::add_const<T>::type&;
	using	header_type					= typename std::conditional<std::is_same<void, H>::value, int, H>::type;
	using	iterator					= shared_buffer_iterator<value_type, shared_buffer, AMTRS_IMPLEMENTS(shared_buffer)::shared_buffer_iterator_incrementor>;
	using	const_iterator				= shared_buffer_iterator<const_value_type, shared_buffer, AMTRS_IMPLEMENTS(shared_buffer)::shared_buffer_iterator_incrementor>;
	using	reverse_iterator			= shared_buffer_iterator<value_type, shared_buffer, AMTRS_IMPLEMENTS(shared_buffer)::shared_buffer_iterator_decrementor>;
	using	const_reverse_iterator		= shared_buffer_iterator<const_value_type, shared_buffer, AMTRS_IMPLEMENTS(shared_buffer)::shared_buffer_iterator_decrementor>;

	shared_buffer() = default;
	shared_buffer(const shared_buffer&) = default;
	shared_buffer(shared_buffer&&) = default;
	shared_buffer& operator = (const shared_buffer&) = default;
	shared_buffer& operator = (shared_buffer&&) = default;

	shared_buffer(size_type _size)
		: shared_buffer(_size, std::make_tuple(), std::make_index_sequence<0>()) {}

	template<class TS>
	shared_buffer(view<TS> _in)
	{
		auto			inst	= instance_type::create(_in.size());
		auto			src		= _in.data();
		for (value_type* init = reinterpret_cast<value_type*>(inst + 1); inst->elmsize < _in.size(); ++(inst->elmsize))
		{
			new ((void*)&init[inst->elmsize]) value_type(*src++);
		}
		mInstance	= inst;
	}

	template<class... HArgs>
	shared_buffer(size_type _size, std::tuple<HArgs...> _hargs)
		: shared_buffer(_size, _hargs, std::make_index_sequence<sizeof...(HArgs)>()) {}


	template<class HArg>
	shared_buffer(size_type _size, HArg&& _hargs)
		: shared_buffer(_size, std::make_tuple(std::move(_hargs)), std::make_index_sequence<1>()) {}

	template<class... HeaderArgs, std::size_t... Indecies>
	shared_buffer(size_type _length, std::tuple<HeaderArgs...> _hargs, std::index_sequence<Indecies...>)
	{
		auto			inst	= instance_type::create(_length, std::get<Indecies>(_hargs)...);
		for (value_type* init = reinterpret_cast<value_type*>(inst + 1); inst->elmsize < _length; ++(inst->elmsize))
		{
			new ((void*)&init[inst->elmsize]) value_type();
		}
		mInstance	= inst;
	}

	template<class HArg>
	shared_buffer(std::initializer_list<HArg> _hargs)
	{
		auto	inst	= instance_type::create(_hargs.size());
		auto	in		= _hargs.begin();
		for (value_type* init = reinterpret_cast<value_type*>(inst + 1); inst->elmsize < _hargs.size(); ++(inst->elmsize))
		{
			new ((void*)&init[inst->elmsize]) value_type(std::move(*in));
			++in;
		}
		mInstance	= inst;
	}


	template<class It>
	shared_buffer(It _first, It _last)
	{
		size_type		siz		= std::distance(_first, _last);
		auto			inst	= instance_type::create(siz);
		for (value_type* init = reinterpret_cast<value_type*>(inst + 1); inst->elmsize < siz; ++(inst->elmsize))
		{
			new ((void*)&init[inst->elmsize]) value_type(*_first);
			++_first;
		}
		mInstance	= inst;
	}


	template<class It>
	shared_buffer(It _first, size_t _size)
	{
		auto			inst	= instance_type::create(_size);
		for (value_type* init = reinterpret_cast<value_type*>(inst + 1); inst->elmsize < _size; ++(inst->elmsize))
		{
			new ((void*)&init[inst->elmsize]) value_type(*_first);
			++_first;
		}
		mInstance	= inst;
	}

	bool operator == (const shared_buffer& _r) const noexcept { return mInstance == _r.mInstance; }
	bool operator != (const shared_buffer& _r) const noexcept { return mInstance != _r.mInstance; }

	size_type size() const noexcept { return mInstance ? mInstance->elmsize : 0; }
	bool empty() const noexcept { return size() == 0; }

	void clear() { mInstance.clear(); }

	      pointer data()       noexcept { return mInstance ? reinterpret_cast<      pointer>(mInstance.get()+1) : nullptr; }
	const_pointer data() const noexcept { return mInstance ? reinterpret_cast<const_pointer>(mInstance.get()+1) : nullptr; }

	      iterator  begin()       noexcept { return data(); }
	const_iterator  begin() const noexcept { return cbegin(); }
	const_iterator cbegin() const noexcept { return data(); }
	      iterator  end()       noexcept { return data() + size(); }
	const_iterator  end() const noexcept { return cend(); }
	const_iterator cend() const noexcept { return data() + size(); }


	      reverse_iterator  rbegin()       noexcept { return data() + size()-1; }
	const_reverse_iterator  rbegin() const noexcept { return crbegin(); }
	const_reverse_iterator crbegin() const noexcept { return data() + size()-1; }
	      reverse_iterator  rend()       noexcept { return data() - 1; }
	const_reverse_iterator  rend() const noexcept { return crend(); }
	const_reverse_iterator crend() const noexcept { return data() - 1; }

	template<class I> reference operator [] (I _index) noexcept { return data()[_index]; }
	template<class I> const_reference operator [] (I _index) const noexcept { return data()[_index]; }


	//! ヘッダを取得します
	template<class D = H*, typename = typename std::enable_if_t<std::is_same<void, D>::value == 0>>
	auto header(D = {}) noexcept -> header_type& { return mInstance->header; }

	//! ヘッダを取得します
	template<class D = H*, typename = typename std::enable_if_t<std::is_same<void, D>::value == 0>>
	auto header(D = {}) const noexcept -> header_type const& { return mInstance->header; }

protected:
	ref<instance_type>	mInstance;

	template<class,class>
	friend	class	shared_buffer;
};


//! 特殊なバッファ
//! void型のバッファは直接作成することは出来ず、何らかの型を抽象化して
//! 保持します。このタイプはリニアなメモリをデバイスに渡すときなどに
//! 使用されます。
template<class H>
class	shared_buffer<void, H>
{
public:
	using	size_type					= size_t;
	using	difference_type				= ptrdiff_t;
	using	value_type					= void;
	using	const_value_type			= typename std::add_const<void>::type;
	using	pointer						= void*;
	using	const_pointer				= typename std::add_const<void>::type*;

	shared_buffer() = default;
	shared_buffer(shared_buffer const&) = default;
	shared_buffer(shared_buffer&&) = default;
	shared_buffer& operator = (shared_buffer const&) = default;
	shared_buffer& operator = (shared_buffer&&) = default;

	template<class T2, class H2>
	shared_buffer(shared_buffer<T2, H2> const& _src)
	{
		mInstance	= new instance_type();
		mInstance->origin	= _src.mInstance;
		mInstance->bytesize	= sizeof(T2) * _src.size();
		mInstance->data		= _src.data();
	}

	template<class It>
	shared_buffer(It _first, It _last)
		: shared_buffer(shared_buffer<std::remove_const_t<std::remove_reference_t<decltype(*_first)>>>(_first, _last))
	{}

	template<class It>
	shared_buffer(It _first, size_t _size)
		: shared_buffer(shared_buffer<std::remove_const_t<std::remove_reference_t<decltype(*_first)>>>(_first, _size))
	{}

	bool operator == (const shared_buffer& _r) const noexcept { return mInstance == _r.mInstance; }
	bool operator != (const shared_buffer& _r) const noexcept { return mInstance != _r.mInstance; }

	size_type size() const noexcept { return mInstance ? mInstance->bytesize : 0; }
	bool empty() const noexcept { return size() == 0; }

	void clear() { mInstance.clear(); }

//	      pointer data()       noexcept { return mInstance ? reinterpret_cast<      pointer>(mInstance->data) : nullptr; }
	const_pointer data() const noexcept { return mInstance ? reinterpret_cast<const_pointer>(mInstance->data) : nullptr; }

protected:
	struct	instance_type
			: public ref_object
	{
		ref<ref_object>	origin;
		size_t			bytesize	= 0;
		void const*		data;
	};

	ref<instance_type>	mInstance;
};



AMTRS_NAMESPACE_END
#endif
