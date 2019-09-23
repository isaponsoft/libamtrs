/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__win32__windows__hpp
#define	__libamtrs__win32__windows__hpp
#define	NOMINMAX		// 
#include <winsock2.h>
#include <windows.h>
#include <algorithm>

// min, max マクロが無いと、GDIPlus が問題を起こすため
// std::min, std::max へのエイリアスを生成します。
namespace amtrs::os::win32::minmax {

template< class T > 
const T& min( const T& a, const T& b )
{
	return	std::min<T>(a, b);
}

template< class T, class Compare >
const T& min( const T& a, const T& b, Compare&& comp )
{
	return	std::min<T, Compare>(a, b, std::forward<Compare>(comp));
}
 
template< class T >
T min( std::initializer_list<T> ilist )
{
	return	std::min<T>(ilist);
}


template< class T, class Compare >
T min( std::initializer_list<T> ilist, Compare&& comp )
{
	return	std::min<T>(ilist, std::forward<Compare>(comp));
}


template< class T > 
const T& max( const T& a, const T& b )
{
	return	std::max<T>(a, b);
}

template< class T, class Compare >
const T& max( const T& a, const T& b, Compare&& comp )
{
	return	std::max<T, Compare>(a, b, std::forward<Compare>(comp));
}
 
template< class T >
T max( std::initializer_list<T> ilist )
{
	return	std::max<T>(ilist);
}


template< class T, class Compare >
T max( std::initializer_list<T> ilist, Compare&& comp )
{
	return	std::max<T>(ilist, std::forward<Compare>(comp));
}


} // amtrs::os::win32::minmax

using namespace amtrs::os::win32::minmax;

#endif
