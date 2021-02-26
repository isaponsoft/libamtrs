/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__win32__windows__hpp
#define	__libamtrs__win32__windows__hpp
#define	AMTRS_OS_WIN32_NAMESPACE_BEGIN	AMTRS_OS_NAMESPACE_BEGIN namespace win32 {
#define	AMTRS_OS_WIN32_NAMESPACE_END	} AMTRS_OS_NAMESPACE_END

#pragma comment(lib, "windowsapp")


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

namespace amtrs::os::win32 {

inline std::error_code make_last_error_code()
{
	auto	ec = GetLastError();
	return	std::error_code(ec, std::generic_category());
}


template<class... Args>
inline std::system_error make_system_error(Args&&... _args)
{
	return	std::system_error(make_last_error_code(), std::forward<Args>(_args)...);
}


template<class... Args>
inline void throw_system_error(Args&&... _args)
{
	throw make_system_error(std::forward<Args>(_args)...);
}

} // namespace amtrs::os::win32

using namespace amtrs::os::win32::minmax;

#endif
