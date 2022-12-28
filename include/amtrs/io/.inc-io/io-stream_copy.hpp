/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__stream_copy__hpp
#define	__libamtrs__io__stream_copy__hpp
AMTRS_IO_NAMESPACE_BEGIN



template<class Out, class In, size_t BUFFER_SIZE = 1024>
auto stream_copy(Out&& _out, In&& _in, size_t _size = static_cast<size_t>(-1))
{
	if (_size == static_cast<size_t>(-1))
	{
		while (_out && _in)
		{
			char	buff[BUFFER_SIZE];
			io::read(buff, _in);
			auto	c	= _in.gcount();
			_out.write(buff, c);
		}
	}
	else
	{
		while (_out && _in && (_size > 0))
		{
			char	buff[BUFFER_SIZE];
			auto	s	= std::min<size_t>(_size, BUFFER_SIZE);
			io::read(buff, _in, s);
			auto	c	= _in.gcount();
			_out.write(buff, c);
			_size -= s;
		}
	}
	if ((_in.good() || _in.eof()) && (_out.good() || _out.eof()))
	{
		return	true;
	}
	return	false;
}



AMTRS_IO_NAMESPACE_END
#endif
