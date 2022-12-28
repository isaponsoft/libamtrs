/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__iohandle__hpp
#define	__libamtrs__io__iohandle__hpp
AMTRS_IO_NAMESPACE_BEGIN




template<class IO, class Traits = io_traits<IO>>
class	iohandle
{
public:
	using	traits_type		= Traits;
	using	value_type		= int;
	using	size_type		= size_t;
	using	fpos_type		= int64_t;

	iohandle() = default;
	iohandle(iohandle const&) = delete;		//!< no copy
	iohandle(iohandle&& _r)
		: mHandle(_r.mHandle)
	{
		_r.mHandle	= traits_type::nio;
	}

	~iohandle()
	{
		close();
	}

	iohandle& operator = (iohandle const&) = delete;

	iohandle& operator = (iohandle&& _r)
	{
		close();
		mHandle		= _r.mHandle;
		_r.mHandle	= traits_type::nio;
		return	*this;
	}


	bool empty() const noexcept
	{
		return	mHandle == traits_type::nio;
	}


	//! _mode is openmode flags
	template<class T>
	iohandle& open(T _device, unsigned int _mode)
	{
		close();
		mHandle	= traits_type{}.open(std::forward<T>(_device), _mode);
		return	*this;
	}

	size_type read(void* _buffer, size_type _request)
	{
		return	empty() ? 0 : traits_type{}.read(mHandle, _buffer, _request);
	}

	size_type write(const void* _buffer, size_type _request)
	{
		return	empty() ? 0 : traits_type{}.write(mHandle, _buffer, _request);
	}

	fpos_type seek(fpos_type _position, std::ios::seekdir _org)
	{
		return	empty() ? 0 : traits_type{}.seek(mHandle, _position, _org);
	}

	size_type size()
	{
		return	empty() ? 0 : traits_type{}.size(mHandle);
	}

	void close()
	{
		if (!empty())
		{
			traits_type{}.close(mHandle);
			mHandle	= traits_type::nio;
		}
	}

protected:
	value_type	mHandle	= traits_type::nio;
};


AMTRS_IO_NAMESPACE_END
#endif
