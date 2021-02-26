/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs__storage__lowio__hpp
#define	__inarikvs__storage__lowio__hpp
#include <io.h>
#include <fcntl.h>
#include <amtrs/io/iohandle.hpp>
#include "../inc/inarikvs-def.hpp"
#include "../inc/inarikvs-types.hpp"



INARIKVS_NAMESPACE_BEGIN

// ******************************************************************
//! c language. Low level I/O.
// ------------------------------------------------------------------
class	lowio
{
public:
	lowio() = default;
	lowio(const lowio&) = delete;
	lowio(lowio&& _r)
		: mFilename(std::move(_r.mFilename))
		, mHandle(std::move(_r.mHandle))
		, mFilePos(std::move(_r.mFilePos))
		, mFileSize(std::move(_r.mFileSize))
	{}

	~lowio()
	{
		close();
	}

	void sync()
	{
	}
	
	bool open(const std::string& _filename, bool _create = true)
	{
		close();
		mFilename	= _filename;
		mHandle.open(mFilename.c_str(), (_create ? amtrs::io::openmode::open_always : amtrs::io::openmode::open) | amtrs::io::openmode::read | amtrs::io::openmode::write);
		if (mHandle.empty())
		{
			return	false;
		}
		mFileSize	= mHandle.size();
		mFilePos	= 0;
		return	true;
	}

	void write(const void* _buffer, std::size_t _size)
	{
		mHandle.write((const char*)_buffer, static_cast<unsigned int>(_size));
		auto	sz	= mFilePos + _size;
		if (mFileSize < sz)
		{
			mFileSize	= sz;
		}
		mFilePos += _size;
	}

	void write(std::string_view _data)
	{
		write(_data.data(), _data.size());
	}

	void write(int64_t _position, const void* _buffer, std::size_t _size)
	{
		set_position(_position);
		write(_buffer, _size);
	}

	void write(int64_t _position, std::string_view _data)
	{
		write(_position, _data.data(), _data.size());
	}

	std::size_t read(void* _buffer, std::size_t _size)
	{
		std::size_t	rs	= mHandle.read((char*)_buffer, static_cast<unsigned int>(_size));
		auto		sz	= mFilePos + rs;
		if (mFileSize < sz)
		{
			mFileSize	= sz;
		}
		mFilePos += rs;
		return	rs;
	}

	void read(int64_t _position, void* _buffer, std::size_t _size)
	{
		set_position(_position);
		read(_buffer, _size);
	}

	int64_t position() const noexcept
	{
		return	mFilePos;
	}

	int64_t size() const noexcept
	{
		return	mFileSize;
	}

	void set_position(int64_t _position)
	{
		if (mFilePos != _position)
		{
			mHandle.seek(_position - mFilePos, std::ios::cur);
			mFilePos	= _position;
		}
	}

	void close()
	{
		mHandle.close();
	}

	void remove()
	{
		close();
		::remove(mFilename.c_str());
	}


private:
	std::string					mFilename;
	amtrs::io::iohandle<int>	mHandle;
	uint64_t					mFilePos	= 0;
	uint64_t					mFileSize	= 0;
};


INARIKVS_NAMESPACE_END
#endif
