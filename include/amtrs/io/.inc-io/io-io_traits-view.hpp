/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__io_traits_view__hpp
#define	__libamtrs__io__io_traits_view__hpp
AMTRS_IO_NAMESPACE_BEGIN


template<class T>
struct	io_traits<view<T>>
{
	using	view_type	= view<T>;
	using	char_type	= typename view_type::char_type;

	size_t read(void* _buff, view_type& _io, size_t _size)
	{
		auto	rs	= std::min(_size, _io.size());
		std::copy_n(_io.data(), rs, reinterpret_cast<char_type*>(_buff));
		return	rs;
	}


	size_t write(view_type& _io, const void* _buff, size_t _size)
	{
		auto	ws	= std::min(_size, _io.size());
		std::copy_n(reinterpret_cast<const char_type*>(_buff), ws, _io.data());
		return	ws;
	}



	uintmax_t size(view_type& _io)
	{
		return	_io.size();
	}
};


AMTRS_IO_NAMESPACE_END
#endif
