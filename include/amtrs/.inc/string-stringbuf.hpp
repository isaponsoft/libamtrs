/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__amtrs__stringbuf__hpp
#define	__libamtrs__amtrs__stringbuf__hpp
AMTRS_EXTERN_C_BEGIN



/*
 * string buffer utils.
 * c/c++ compatible.
 */
typedef	struct
{
	char*				data;
	amtrs_allocator*	allocator;
	size_t				size;
	size_t				allocated;
} amtrs_stringbuf;



amtrs_stringbuf amtrs_stringbuf_create(amtrs_allocator* allocator);

void amtrs_stringbuf_free(amtrs_stringbuf* buff);


int amtrs_stringbuf_allocate(amtrs_stringbuf* buff, size_t size);


int amtrs_stringbuf_resize(amtrs_stringbuf* buff, size_t size);


int amtrs_stringbuf_reserve(amtrs_stringbuf* buff, size_t size);


int amtrs_stringbuf_append(amtrs_stringbuf* buff, char const* str);


int amtrs_stringbuf_putc(amtrs_stringbuf* buff, char c);


int amtrs_stringbuf_puts(amtrs_stringbuf* buff, char const* str, size_t size);


int amtrs_stringbuf_clone(amtrs_stringbuf* dest, amtrs_stringbuf const* src);

AMTRS_EXTERN_C_END


// c++ extends.
#ifdef	__cplusplus
AMTRS_NAMESPACE_BEGIN

class	stringbuf
{
public:
	using	value_type		= char;
	using	pointer			= value_type*;
	using	const_pointer	= value_type const*;
	using	reference		= value_type&;
	using	const_reference	= value_type const&;
	using	size_type		= size_t;

	stringbuf(amtrs_allocator* _allocator = amtrs_allocator_get_default())
		: mBuff(amtrs_stringbuf_create(_allocator))
	{}

	stringbuf(stringbuf const& _r)
	{
		amtrs_stringbuf_clone(&mBuff, &_r.mBuff);
	}

	stringbuf(const_pointer _str, size_type _size)
	{
		amtrs_stringbuf_allocate(&mBuff, _size);
	}

	stringbuf(const_pointer _str)
		: stringbuf(_str, strlen(_str))
	{
	}

	stringbuf(stringbuf&& r)
	{
		mBuff	= r.mBuff;
		r.mBuff	= amtrs_stringbuf_create(r.mBuff.allocator);
	}

	explicit stringbuf(amtrs_stringbuf&& r)
	{
		mBuff	= r;
		r		= amtrs_stringbuf_create(r.allocator);
	}

	~stringbuf()
	{
		amtrs_stringbuf_free(&mBuff);
	}

	pointer data() noexcept
	{
		return	mBuff.data;
	}

	const_pointer data() const noexcept
	{
		return	mBuff.data;
	}

	size_type size() const noexcept
	{
		return	mBuff.size;
	}

	size_t empty() const noexcept
	{
		return	size() == 0;
	}

	void reserve(size_t size)
	{
		amtrs_stringbuf_reserve(&mBuff, size);
	}

	size_type capacity() const noexcept
	{
		return	mBuff.allocated - mBuff.size;
	}

	amtrs_stringbuf* get() noexcept
	{
		return	&mBuff;
	}

	amtrs_stringbuf const* get() const noexcept
	{
		return	&mBuff;
	}

	amtrs_stringbuf release() noexcept
	{
		amtrs_stringbuf	r	= mBuff;
		mBuff 	= amtrs_stringbuf_create(mBuff.allocator);
		return	r;
	}

	pointer begin() noexcept { return mBuff.data; }
	const_pointer begin() const noexcept { return mBuff.data; }
	const_pointer cbegin() const noexcept { return mBuff.data; }
	pointer end() noexcept { return mBuff.data + mBuff.size; }
	const_pointer end() const noexcept { return mBuff.data + mBuff.size; }
	const_pointer cend() const noexcept { return mBuff.data + mBuff.size; }


protected:
	amtrs_stringbuf	mBuff;
};


AMTRS_NAMESPACE_END
#endif
#endif
