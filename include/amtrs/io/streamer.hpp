/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__streamer__hpp
#define	__libamtrs__io__streamer__hpp
#include "../string/text.hpp"
AMTRS_IO_NAMESPACE_BEGIN
template<class T>
class	streamer



using	bytereader	= streamer<const std::byte>;
using	bytewriter	= streamer<std::byte>;


template<class T>
class	streamer
{
public:
	using	value_type		= T;
	using	size_type		= size_t;
	using	pointer			= T*;
	using	const_pointer	= typename std::add_const<T>::type*;
	using	writable		= typename std::conditional<std::is_const<pointer>::value, std::true_type, std::false_type>::type;
	static constexpr size_type	npos		= static_cast<size_type>(-1);

	streamer(pointer _data, size_type _size = npos)
		: m_data(_data)
		, m_cursor(0)
		, m_size(_size)
	{}

	std::string_view read(size_type _size)
	{
		std::string_view	retval(reinterpret_cast<const char*>(m_data + m_cursor), _size);
		m_cursor += _size;
		return	retval;
	}

	template<class T, std::endian From>
	size_type find_first_of(const T& _v)
	{
		auto		data	= reinterpret_cast<const T*>(m_data + m_cursor);
		for (size_type i = 0; i < m_size; ++i)
		{
			if (endiancvt<From>{}.from(data[i]))
			{
				return	i;
			}
		}
		return	npos;
	}

	template<class T, std::endian From>
	size_type read(T* _buffer, size_type _n)
	{
		auto	data	= reinterpret_cast<const T*>(m_data + m_cursor);
		for (size_type i = 0; i < _n; ++i)
		{
			_buffer[i] = endiancvt<From>{}.from(data[i]);
			m_cursor += sizeof(T);
		}
		return	_n;
	}

	template<class T>
	size_type read_be(T* _buffer, size_type _n)
	{
		return	read<T, std::endian::big>(_buffer, _n);
	}

	template<class T>
	size_type read_le(T* _buffer, size_type _n)
	{
		return	read<T, std::endian::little>(_buffer, _n);
	}

	struct	is_array
	{
		template<class T>
		void operator () (T value)
		{
			static_cast<typename T::value_type>(value[0]);
		}
	};

	template<std::endian From, class T, typename std::enable_if<is_concept<T, is_array>::value>::type* = 0>
	size_type read(T& _buffer, size_type _n)
	{
		using	value_type	= typename T::value_type;
		auto	data		= reinterpret_cast<const value_type*>(m_data + m_cursor);
		for (size_type i = 0; i < _n; ++i)
		{
			_buffer[i] = endiancvt<From>{}.from(data[i]);
			m_cursor += sizeof(value_type);
		}
		return	_n;
	}

	template<class T>
	size_type read_be(T& _buffer, size_type _n)
	{
		return	read<std::endian::little>(std::forward<T>(_buffer), _n);
	}

	template<class T>
	size_type read_le(T& _buffer, size_type _n)
	{
		return	read<std::endian::little>(std::forward<T>(_buffer), _n);
	}

	template<class T, std::endian From>
	T read()
	{
		T	retval(endiancvt<From>{}.from(*reinterpret_cast<const T*>(m_data + m_cursor)));
		m_cursor += sizeof(T);
		return	retval;
	}

	template<class T>
	T read_be()
	{
		return	read<T, std::endian::big>();
	}

	template<class T>
	T read_le()
	{
		return	read<T, std::endian::little>();
	}

	pointer data() const noexcept { return m_data; }
	size_type size() const noexcept { return m_size; }
	size_type capacity() const noexcept { return m_size - m_cursor; }
	size_type offset() const noexcept { return m_cursor; }
	pointer current() const noexcept { return m_data + m_cursor; }


protected:
	pointer		m_data;
	size_type	m_cursor;
	size_type	m_size;
};





AMTRS_IO_NAMESPACE_END
#endif
