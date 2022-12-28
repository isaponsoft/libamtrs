/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__zlib__hpp
#define	__libamtrs__io__zlib__hpp
#include <zlib.h>
#include "io.hpp"
AMTRS_IO_NAMESPACE_BEGIN

struct	zlib_setting
{
//	static constexpr int	wndsize	= MAX_WBITS;
	static constexpr int	wndsize	= -MAX_WBITS;
};

struct	zlib_deflate
{
	static constexpr size_t	buffer_size	= 65536;

	void init()
	{
		std::memset(&mZ, 0, sizeof(mZ));
		::deflateInit2(&mZ, Z_DEFAULT_COMPRESSION, Z_DEFLATED, zlib_setting::wndsize, 8, Z_DEFAULT_STRATEGY);
	}

	void end()
	{
		::deflateEnd(&mZ);
	}

	int exec(int flag)
	{
		return	::deflate(&mZ, flag);
	}

	size_t _avail() const noexcept
	{
		return	buffer_size - mZ.avail_out;
	}


	z_stream		mZ;
	char			mOut[buffer_size]	= {1};
	bool			mStreamEnd			= false;

};


struct	zlib_inflate
{
	static constexpr size_t	buffer_size	= 65536;

	void init()
	{
		std::memset(&mZ, 0, sizeof(mZ));
		::inflateInit2(&mZ, zlib_setting::wndsize);
	}

	void end()
	{
		::inflateEnd(&mZ);
	}

	int exec(int flag)
	{
		return	::inflate(&mZ, flag);
	}

	size_t _avail() const noexcept
	{
		return	buffer_size - mZ.avail_out;
	}

	z_stream		mZ;
	char			mOut[buffer_size]	= {1};
	bool			mStreamEnd			= false;
};


template<class Function, class Stream>
struct	streamif_ozlib
		: Function
{
	using	value_type		= std::remove_pointer_t<Stream>;
	using	stream_type		= Stream;
	using	iostate			= amtrs::io::iostate;
	using	size_type		= std::streamsize;
	using	function_type	= Function;

	streamif_ozlib()
	{
		function_type::init();
	}

	~streamif_ozlib()
	{
		function_type::end();
	}

	streamif_base::iostate write(value_type& _stream, size_type& _writesize, void const* _data, size_type _size)
	{
		return	_write(_stream, _writesize, _data, _size, _size == 0 ? Z_FINISH : 0);
	}

	void close(value_type& _stream)
	{
		size_type	s;
		_write(_stream, s, nullptr, 0, Z_FINISH);
	}

protected:
	streamif_base::iostate _write(value_type& _stream, size_type& _writesize, void const* _buff, size_t _n, int _flag)
	{
		if (function_type::mStreamEnd)
		{
			return	0;
		}
		streamif_base::iostate	ret	= std::ios::goodbit;
		_writesize					= 0;
		function_type::mZ.next_in	= (Bytef *)_buff;
		function_type::mZ.avail_in 	= _n;
		do
		{
			function_type::mZ.next_out	= (Bytef*)function_type::mOut;
			function_type::mZ.avail_out	= function_type::buffer_size;
			if (_flag)
			{
				function_type::mStreamEnd	= true;
			}
			switch (function_type::exec(_flag))
			{
				case Z_OK :
					break;
				case Z_STREAM_END :
					ret	= std::ios::eofbit;
					break;
				case Z_NEED_DICT:
					return	std::ios::badbit;
				case Z_DATA_ERROR:
					return	std::ios::badbit;
				case Z_STREAM_ERROR:
					return	std::ios::failbit;
			}
			size_t	alive	= function_type::buffer_size - function_type::mZ.avail_out;
			auto	buf		= function_type::mOut;
			size_t	size	= 0;
			while (alive > 0)
			{
				auto	rs	= _stream.write(buf, alive).pcount();
				buf		+= rs;
				size	+= rs;
				alive	-= rs;
			}
		} while (function_type::mZ.avail_in > 0 || (function_type::buffer_size != function_type::mZ.avail_out));
		_writesize	= _n;
		return	ret;
	}
};



template<class Function, class Stream>
struct	streamif_izlib
		: Function
{
public:
	using	value_type		= Stream;
	using	iostate			= amtrs::io::iostate;
	using	size_type		= std::streamsize;
	using	function_type	= Function;

	streamif_izlib()
	{
		function_type::init();
 		_reset_zout();
	}

	~streamif_izlib()
	{
		function_type::end();
	}

	iostate read(value_type& _stream, size_type& _readsize, void* _buff, size_t _n)
	{
		return	_read(_stream, _readsize, _buff, _n);
	}

protected:
	iostate _read(value_type& _stream, size_type& _readsize, void* _buff, size_t _n)
	{
		char*	out		= reinterpret_cast<char*>(_buff);
		iostate	ret		= std::ios::goodbit;
		_readsize	= 0;
		while (_readsize < _n)
		{
			// out に蓄積されているデータを可能な限り転送する。
			if (auto avs = _avail(); avs > 0)
			{
				auto	sz	= std::min<size_t>(avs, _n - _readsize);
				std::copy_n(function_type::mOut + mCursor, sz, out);
				out				+= sz;
				mCursor			+= sz;
				_readsize		+= sz;
				// 転送先が満杯になったので終了
				if (_readsize == _n)
				{
					break;
				}
			}
			// 展開用のバッファが空になっているので埋めなおす
			_reset_zout();

			// 入力データが無いなら終了
			if (function_type::mStreamEnd)
			{
				break;
			}

			// 入力バッファが空なら入力バッファを埋める
			if (!function_type::mZ.avail_in)
			{
				function_type::mZ.next_in		=  (Bytef*)mIn;
				function_type::mZ.avail_in		=  (uInt)_stream.read(mIn, function_type::buffer_size).gcount();
				if (function_type::mZ.avail_in == 0 || !_stream.good())
				{
					mFlush		= Z_FINISH;
					function_type::mStreamEnd	= true;
				}
			}

			if (function_type::mZ.avail_in > 0)
			{
				switch (function_type::exec(mFlush))
				{
					case Z_OK:
						break;
					case Z_BUF_ERROR :
						return	std::ios::failbit;
					case Z_STREAM_END :
						function_type::mStreamEnd	= true;
						break;
					case Z_NEED_DICT:    // 辞書が必要
						return	std::ios::badbit;
					case Z_DATA_ERROR:   // 不正なデータ
						return	std::ios::badbit;
					case Z_STREAM_ERROR:
						return	std::ios::failbit;
				}
			}
		}
		if (!_avail() && function_type::mStreamEnd)
		{
			ret	= std::ios::eofbit;
		}
		return	ret;
	}

protected:
	size_t _avail() const noexcept
	{
		auto	bufferingSize = function_type::buffer_size - function_type::mZ.avail_out;
		return	bufferingSize - mCursor;
	}

	void _reset_zout()
	{
		function_type::mZ.next_out	= (Bytef*)function_type::mOut;
		function_type::mZ.avail_out	= function_type::buffer_size;
		mCursor			= 0;
	}

	char			mIn[function_type::buffer_size];
	int				mCursor		= 0;
	int				mFlush		= Z_SYNC_FLUSH;
};




template<class S>
auto make_ideflate_streamif(S&& _stream)
{
	auto	in	= make_streamif(std::forward<S>(_stream));
	return	basic_streamif<decltype(in), streamif_izlib<zlib_deflate, decltype(in)>>(std::move(in));
}


template<class S>
auto make_iinflate_streamif(S&& _stream)
{
	auto	in	= make_streamif(std::forward<S>(_stream));
	return	basic_streamif<decltype(in), streamif_izlib<zlib_inflate, decltype(in)>>(std::move(in));
}



template<class S>
auto make_odeflate_streamif(S&& _stream)
{
	auto	in	= make_ostreamif(std::forward<S>(_stream));
	return	basic_streamif<decltype(in), streamif_ozlib<zlib_deflate, decltype(in)>>(std::move(in));
}


template<class S>
auto make_oinflate_streamif(S&& _stream)
{
	auto	in	= make_ostreamif(std::forward<S>(_stream));
	return	basic_streamif<decltype(in), streamif_ozlib<zlib_inflate, decltype(in)>>(std::move(in));
}

AMTRS_IO_NAMESPACE_END
#endif
