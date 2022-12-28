/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__stream__hpp
#define	__libamtrs__io__stream__hpp
AMTRS_IO_NAMESPACE_BEGIN
template<class Interface, class Stream>
class	read_seekable
		: public amtrs_stream_read_seekable
{
public:
	using	super_type		= amtrs_stream_read_seekable;
	using	interface_type	= Interface;
	using	stream_type		= Stream;

	read_seekable(stream_type _stream)
		: mStream(std::move(_stream))
	{
		super_type::obj			= &mStream;
		super_type::readproc	= [](void* s, void* data, size_t size) -> size_t
		{
			return	io::read(data, get_interface(s), size);

		};
		super_type::seekproc	= [](void* s, size_t _offset, amtrs_seekdir _dir) -> int
		{
			io::seek(get_interface(s), _offset, io::to_seekdir(_dir));
			return	0;
		};
		super_type::tellproc	= [](void* s) -> size_t
		{
			return	io::tell(get_interface(s));
		};
	}


	read_seekable(read_seekable&& r)
		: mStream(std::move(r.mStream))
	{
		super_type::obj		= &mStream;
	}

protected:
	static interface_type& get_interface(void* _obj)
	{
		if constexpr (std::is_base_of_v<Interface, Stream>)
		{
			return	*(interface_type*)_obj;
		}
		else
		{
			return	**(interface_type**)_obj;
		}
	}

	stream_type	mStream;
};


inline amtrs_stream_read_seekable& make_read_seekable(amtrs_stream_read_seekable& _in)
{
	return	_in;
}


template<class In>
auto make_read_seekable(In _in)
{
	return	read_seekable<In, In>(std::move(_in));
}


template<class In>
auto make_read_seekable(In* _in)
{
	return	read_seekable<In, In*>(_in);
}




template<class Interface, class Stream>
class	write_seekable
		: public amtrs_stream_write_seekable
{
public:
	using	super_type		= amtrs_stream_write_seekable;
	using	interface_type	= Interface;
	using	stream_type		= Stream;

	write_seekable(stream_type _stream)
		: mStream(std::move(_stream))
	{
		super_type::obj			= &mStream;
		super_type::writeproc	= [](void* s, void const* data, size_t size) -> size_t
		{
			return	io::write(get_interface(s), data, size);

		};
		super_type::seekproc	= [](void* s, size_t _offset, amtrs_seekdir _dir) -> int
		{
			io::seek(get_interface(s), _offset, io::to_seekdir(_dir));
			return	0;
		};
		super_type::tellproc	= [](void* s) -> size_t
		{
			return	io::tell(get_interface(s));
		};
	}


	write_seekable(write_seekable&& r)
		: mStream(std::move(r.mStream))
	{
		super_type::obj		= &mStream;
	}

protected:
	static interface_type& get_interface(void* _obj)
	{
		if constexpr (std::is_base_of_v<Interface, Stream>)
		{
			return	*(interface_type*)_obj;
		}
		else
		{
			return	**(interface_type**)_obj;
		}
	}

	stream_type	mStream;
};


inline amtrs_stream_write_seekable& make_write_seekable(amtrs_stream_write_seekable& _in)
{
	return	_in;
}


template<class In>
auto make_write_seekable(In&& _in)
{
	return	write_seekable<In, In>(std::move(_in));
}


template<class In>
auto make_write_seekable(In* _in)
{
	return	write_seekable<In, In*>(_in);
}
AMTRS_IO_NAMESPACE_END
#endif
