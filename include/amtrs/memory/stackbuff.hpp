/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__memory__stackbuff__hpp
#define	__libamtrs__memory__stackbuff__hpp
#include <cstdint>
#include <new>
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

/*
 * 型を持たないスタック
 */
class	stackbuffer
{
public:
	using	size_type	= size_t;
	static constexpr size_type	default_stacksize	= 4*1024;		// 4kBytes

	stackbuffer(stackbuffer&& r)
	{
		swap(r);
	}
	stackbuffer& operator = (stackbuffer&& r)
	{
		clear();
		swap(r);
		return	*this;
	}
	stackbuffer(size_type _size = default_stacksize)
	{
		if (_size > 0)
		{
			mBuffer	= new char[_size];
			if (mBuffer)
			{
				mSize		= _size;
				mCurrent	= mBuffer + mSize;
			}
		}
	}
	stackbuffer(stackbuffer const&) = delete;
	stackbuffer& operator = (stackbuffer const&) = delete;

	// set stack point
	void sp(void* _pos) noexcept { mCurrent = reinterpret_cast<char*>(_pos); }
	// get stack point
	void* sp() const noexcept { return mCurrent; }
	void* top() const noexcept { return mBuffer + mSize; }
	void* bottom() const noexcept { return mBuffer; }
	size_type size() const noexcept { return mSize; }
	bool operator == (std::nullptr_t) const noexcept { return mSize == 0; }
	bool operator != (std::nullptr_t) const noexcept { return mSize != 0; }

	// 指定するオフセットを加算したアドレスを返す
	void* address(intmax_t _offset) const noexcept { return reinterpret_cast<void*>(reinterpret_cast<intmax_t>(mCurrent) + _offset); }

	void* alloc(size_type _n) noexcept { return reinterpret_cast<void*>(mCurrent -= _n); }
	void* free(size_type _n) noexcept { void* r = mCurrent; mCurrent += _n; return r; }

	// 指定する型を確保するのに十分な容量を確保し確保したメモリ位置を返します。
	template<class T> T* push() noexcept { auto s = compute_size<T>(); return reinterpret_cast<T*>(alloc(s)); }
	template<class T> T* pop()  noexcept { auto s = compute_size<T>(); return reinterpret_cast<T*>(free (s)); }

	template<class T> T* address_top_of(size_t _n) noexcept { return reinterpret_cast<T*>(reinterpret_cast<intmax_t>(top()) - (compute_size<T>() * (_n + 1))); }
	template<class T> T* address_current_of(size_t _n) noexcept { return reinterpret_cast<T*>(reinterpret_cast<intmax_t>(top()) + (compute_size<T>() * _n)); }

	// pushした時のアドレスを計算する
	template<class T> static T* pushed(void* _current) noexcept { return reinterpret_cast<T*>((intmax_t)_current - compute_size<T>()); }
	// popした時のアドレスを計算する
	template<class T> static T* poped(void* _current) noexcept { return reinterpret_cast<T*>((intmax_t)_current + compute_size<T>()); }

	template<class T>
	static constexpr size_type compute_size() noexcept
	{
		return	(sizeof(T) + sizeof(void*) - 1) & ~(sizeof(void*)-1);
	}

	void swap(stackbuffer& r) noexcept
	{
		std::swap(mSize,    r.mSize);
		std::swap(mBuffer,  r.mBuffer);
		std::swap(mCurrent, r.mCurrent);
	}

	void clear()
	{
		if (mBuffer)
		{
			delete[] mBuffer;
			mCurrent	= nullptr;
			mBuffer		= nullptr;
			mSize		= 0;
		}
	}

protected:
	size_type		mSize		= 0;
	char*			mBuffer		= nullptr;
	char*			mCurrent	= nullptr;
};



template<class T>
class	stackbuf
{
public:
	using	containor_type	= stackbuffer;
	using	size_type		= typename containor_type::size_type;
	using	value_type		= T;
	static constexpr size_type	default_stacksize	= containor_type::default_stacksize;

	stackbuf(size_type _size = default_stacksize)
		: mBuffer(_size)
	{
	}

	stackbuf(stackbuf&& r)
		: mBuffer(std::move(r.mBuffer))
	{
	}
	stackbuf& operator = (stackbuf&& r)
	{
		mBuffer	= std::move(r.mBuffer);
		return	*this;
	}
	stackbuf(stackbuf const&) = delete;
	~stackbuf() { clear(); }
	stackbuf& operator = (stackbuf const&) = delete;

	size_type size() const noexcept { return mBuffer.size() / sizeof(value_type); }

	template<class... Args> void push(Args&&... _args) noexcept
	{
		new (mBuffer.template push<value_type>()) value_type(std::forward<Args>(_args)...);
	}

	void pop() noexcept
	{
		reinterpret_cast<value_type*>(mBuffer.template pop<value_type>())->~value_type();
	}

	value_type& top() noexcept
	{
		return	*reinterpret_cast<value_type*>(mBuffer.sp());
	}

	value_type const& top() const noexcept
	{
		return	*reinterpret_cast<value_type const*>(mBuffer.sp());
	}

	void swap(stackbuf& r) noexcept
	{
		std::swap(mBuffer, r.mBuffer);
	}

	void clear()
	{
		while (mBuffer.sp() != mBuffer.top())
		{
			pop();
		}
	}

protected:
	containor_type	mBuffer;
};

AMTRS_NAMESPACE_END
#endif
