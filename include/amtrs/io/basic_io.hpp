/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__basic_io__hpp
#define	__libamtrs__io__basic_io__hpp
#include "def.hpp"
#include "io_traits.hpp"
AMTRS_IO_NAMESPACE_BEGIN

// ****************************************************************************
//! 低レベルのIO入出力を提供します。
// ----------------------------------------------------------------------------
//! このクラスは、異なるI/Oのインターフェースを共通化します。
// ----------------------------------------------------------------------------
template<class T, class Traits = io_traits<T>>
class	basic_io
{
public:
	using	traits_type	= Traits;
	using	value_type	= typename traits_type::value_type;
	using	size_type	= typename traits_type::size_type;
	using	fpos_type	= typename traits_type::fpos_type;
	static constexpr value_type	nio	= traits_type::nio;

	basic_io() = default;
	basic_io(const basic_io&&) = delete;
	basic_io& operator = (const basic_io&&) = delete;


	basic_io(const char* _devicename)
		: mIO(traits_type{}.open(_devicename, 0))
	{}


	basic_io(value_type&& _io) noexcept
		: mIO(std::move(_io))
	{
		_io	= nio;
	}


	basic_io(basic_io&& _r) noexcept
		: mIO(std::move(_r.mIO))
		, mPos(std::move(_r.mPos))
	{
		_r.mIO	= nio;
		_r.mPos	= 0;
	}


	basic_io& operator = (basic_io&& _r) noexcept
	{
		close();
		mIO		= std::move(_r.mIO);
		mPos	= std::move(_r.mPos);
		_r.mIO	= nio;
		_r.mPos	= 0;
		return	*this;
	}


	~basic_io()
	{
		close();
	}


	static bool exists(const char* _devicename)
	{
		return	traits_type{}.exists(_devicename);
	}

	size_type read(void* _buffer, size_type _request)
	{
		auto	c	= traits_type{}.read(mIO, _buffer, _request);
		if (c > 0)
		{
			mPos += c;
		}
		return	c;
	}


	size_type write(void* _buffer, size_type _request)
	{
		auto	c	= traits_type{}.write(mIO, _buffer, _request);
		if (c > 0)
		{
			mPos += c;
		}
		return	c;
	}


	fpos_type seek(fpos_type _position, std::ios::seekdir _org)
	{
		auto	p	= traits_type{}.seek(mIO, _position, _org);
		if (p > 0)
		{
			mPos = p;
		}
		return	p;
	}


	size_type size() const
	{
		return	traits_type{}.size(mIO);
	}


	void close()
	{
		if (!empty())
		{
			traits_type{}.close(mIO);
			mIO	= nio;
		}
	}


	bool empty() const noexcept
	{
		return	mIO == nio;
	}


	const value_type& handle() const noexcept { return mIO; }

	value_type release() noexcept
	{
		value_type	retval(std::move(mIO));
		mIO		= nio;
		mPos	= 0;
		return	retval;
	}

	fpos_type position() const noexcept
	{
		return	mPos;
	}

private:
	value_type	mIO		= nio;
	fpos_type	mPos	= 0;
};


using	fio	= basic_io<int>;


AMTRS_IO_NAMESPACE_END
#endif
