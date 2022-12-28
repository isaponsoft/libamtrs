/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__stream_in_fwd__hpp
#define	__libamtrs__io__stream_in_fwd__hpp
AMTRS_IO_NAMESPACE_BEGIN


// ****************************************************************************
//! 様々なオブジェクトに対して、std::istream に近いインターフェースを提供する。
// ----------------------------------------------------------------------------
//! あくまでもインターフェースのエミュレートのみを行い、内部的にはほぼ何もしない。
//! そのため、コンパイラの最適化によってオーバーヘッドはほぼ存在しないはずである。
//! 利用できるのは read(), gcount(), seekg(), tellg(), ステータスの取得系のみ。
// ----------------------------------------------------------------------------
template<class...>
class	stream_in;



template<class... T>
auto make_stream_in(stream_in<T...>& _in) -> stream_in<T...>&
{
	return	_in;
}


template<class... T>
auto make_stream_in(stream_in<T...>&& _in) -> stream_in<T...>
{
	return	_in;
}


AMTRS_IO_NAMESPACE_END
#endif
