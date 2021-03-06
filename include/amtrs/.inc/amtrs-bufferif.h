/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__amtrs__bufferif__hpp
#define	__libamtrs__amtrs__bufferif__hpp
AMTRS_EXTERN_C_BEGIN


/*
 * 関数内でメモリを確保し、データを格納する必要がある場合にバッファを操作するためのインターフェースを定義します。
 * この構造体を必要とする関数はメモリを１度だけ確保することを要求し、リサイズしないことを保証します。
 */
typedef	struct
{
	void*	object;

	void* (*allocate)(void* _thiz, size_t _size);

}	amtrs_bufferif_one_init;





AMTRS_EXTERN_C_END
#ifdef	__cplusplus
AMTRS_NAMESPACE_BEGIN


template<class...>
struct	bufferif_one_init_traits;


template<class T>
struct	bufferif_one_init_traits<T>
{
	using	value_type	= T;

	static void* allocate(value_type* _thiz, size_t _size)
	{
		_thiz->resize(_size);
		return	_thiz->data();
	}
};


AMTRS_NAMESPACE_END
#endif
#endif
