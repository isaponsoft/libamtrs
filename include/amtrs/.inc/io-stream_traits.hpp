/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__stream_traits__hpp
#define	__libamtrs__io__stream_traits__hpp
AMTRS_IO_NAMESPACE_BEGIN

template<class...>
struct	steame_traits;


// ****************************************************************************

template<class CharT, class Traits>
struct	steame_traits<std::basic_istream<CharT, Traits>>
{
	using	stream_type	= std::basic_istream<CharT, Traits>;

	size_t read(void* _buff, size_t _size, stream_type& _s)
	{
		_s.read(reinterpret_cast<CharT*>(_buff), _size / sizeof(CharT));
		return	_s.gcount();
	}


	bool good(stream_type& _s) const noexcept
	{
		return	_s.good();
	}
};

template<class CharT, class Traits>
auto choise_stream_traits(std::basic_istream<CharT, Traits>& _s)
	-> steame_traits<typename std::remove_reference<decltype(_s)>::type>;




// ****************************************************************************

template<class CharT, class Traits>
struct	steame_traits<std::basic_ostream<CharT, Traits>>
{
	using	stream_type	= std::basic_ostream<CharT, Traits>;

	bool good(stream_type& _s) const noexcept
	{
		return	_s.good();
	}

	void write(stream_type& _s, void const* _buff, size_t _size) const noexcept
	{
		_s.write(reinterpret_cast<CharT const*>(_buff), _size / sizeof(CharT));
	}
};

template<class CharT, class Traits>
auto choise_stream_traits(std::basic_ostream<CharT, Traits>& _s)
	-> steame_traits<typename std::remove_reference<decltype(_s)>::type>;




// ****************************************************************************

template<class CharT, class Traits>
struct	steame_traits<std::basic_iostream<CharT, Traits>>
{
	using	stream_type	= std::basic_iostream<CharT, Traits>;

	bool good(stream_type& _s) const noexcept
	{
		return	_s.good();
	}

	void write(stream_type& _s, void const* _buff, size_t _size) const noexcept
	{
		_s.write(reinterpret_cast<CharT const*>(_buff), _size / sizeof(CharT));
	}
};

template<class CharT, class Traits>
auto choise_stream_traits(std::basic_iostream<CharT, Traits>& _s)
	-> steame_traits<typename std::remove_reference<decltype(_s)>::type>;




AMTRS_IO_NAMESPACE_END
#endif
