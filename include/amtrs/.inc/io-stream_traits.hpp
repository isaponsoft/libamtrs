/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__io__stream_traits__hpp
#define	__libamtrs__io__stream_traits__hpp
#define	AMTRS_IO_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace io {
#define	AMTRS_IO_NAMESPACE_END		} AMTRS_NAMESPACE_END
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
