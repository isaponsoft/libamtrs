/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <amtrs/string.hpp>
#include <string.h>


static inline void __amtrs_stringbuf_init(amtrs_stringbuf* buff, amtrs_allocator* allocator)
{
	buff->allocator	= allocator;
	buff->data		= NULL;
	buff->size		= 0;
	buff->allocated	= 0;
}


amtrs_stringbuf amtrs_stringbuf_create(amtrs_allocator* allocator)
{
	amtrs_stringbuf	r;
	__amtrs_stringbuf_init(&r, allocator);
	return	r;
}

void amtrs_stringbuf_free(amtrs_stringbuf* buff)
{
	if (buff->data)
	{
		buff->allocator->free(buff->data);
	}
	__amtrs_stringbuf_init(buff, buff->allocator);
}


int amtrs_stringbuf_allocate(amtrs_stringbuf* buff, size_t size)
{
	char*	tmp		= (char*)buff->allocator->allocate(size + 1);
	if (!tmp)
	{
		return	0;
	}
	amtrs_stringbuf_free(buff);
	buff->size		= size;
	buff->allocated	= size;
	buff->data[buff->size]	= '\0';
	return	1;
}

int amtrs_stringbuf_resize(amtrs_stringbuf* buff, size_t size)
{
	if (size > buff->allocated)
	{
		void*	nb;
		if (buff->data)
		{
			nb	= buff->allocator->reallocate(buff->data, size + 1);
		}
		else
		{
			nb	= buff->allocator->allocate(size + 1);
		}
		if (!nb)
		{
			return	0;
		}
		buff->data		= (char*)nb;
		buff->allocated	= size;
	}
	buff->size				= size;
	buff->data[buff->size]	= '\0';
	return	1;
}


int amtrs_stringbuf_reserve(amtrs_stringbuf* buff, size_t size)
{
	size_t	rs	= buff->size + size;
	if (rs > buff->allocated)
	{
		void*	nb;
		if (buff->data)
		{
			nb	= buff->allocator->reallocate(buff->data, rs + 1);
		}
		else
		{
			nb	= buff->allocator->allocate(rs + 1);
		}
		if (!nb)
		{
			return	0;
		}
		buff->data		= (char*)nb;
		buff->allocated	= rs;
	}
	return	1;
}


int amtrs_stringbuf_append(amtrs_stringbuf* buff, char const* str)
{
	return	amtrs_stringbuf_puts(buff, str, strlen(str));
}



int amtrs_stringbuf_putc(amtrs_stringbuf* buff, char c)
{
	if (!amtrs_stringbuf_reserve(buff, sizeof(c)))
	{
		return	0;
	}
	buff->data[buff->size++] = c;
	buff->data[buff->size]	= '\0';
	return	1;
}


int amtrs_stringbuf_puts(amtrs_stringbuf* buff, char const* str, size_t size)
{
	if (!amtrs_stringbuf_reserve(buff, size))
	{
		return	0;
	}
	memcpy(buff->data + buff->size, str, size);
	buff->size += size;
	buff->data[buff->size]	= '\0';
	return	1;
}


int amtrs_stringbuf_clone(amtrs_stringbuf* dest, amtrs_stringbuf const* src)
{
	__amtrs_stringbuf_init(dest, src->allocator);
	return	amtrs_stringbuf_puts(dest, src->data, src->size);
}
