/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs__storage__cfile__hpp
#define	__inarikvs__storage__cfile__hpp
#include "../inc/inarikvs-def.hpp"
#include "../inc/inarikvs-types.hpp"
INARIKVS_NAMESPACE_BEGIN

// ******************************************************************
//! c language FILE*
// ------------------------------------------------------------------
//! 通常は lowio を使うことをお勧めします。
// ------------------------------------------------------------------
class	file
{
public:
	file() = default;
	file(const file&) = delete;
	file(file&& _r) = delete;

	~file()
	{
		close();
	}

	void sync()
	{
		fflush(mHandle);
	}
	
	bool open(const std::string& _filename, bool _create = true)
	{
		close();
		mFilename	= _filename;
		mHandle		= ::fopen(mFilename.c_str(), "rb+");
		if (!mHandle)
		{
			if (!_create)
			{
				return	false;
			}
			mHandle	= ::fopen(mFilename.c_str(), "wb+");
		}
		struct stat	fst;
		::fstat(fileno(mHandle), &fst);
		mFileSize	= fst.st_size;
		mFilePos	= 0;
		return	true;
	}

	void write(const void* _buffer, std::size_t _size)
	{
		::fwrite((const char*)_buffer, 1, _size, mHandle);
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
		std::size_t	rs	= ::fread((char*)_buffer, 1, _size, mHandle);
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
			::fseek(mHandle, _position, SEEK_SET);
			mFilePos	= _position;
		}
	}

	void close()
	{
		if (mHandle)
		{
			::fflush(mHandle);
			::fclose(mHandle);
			mHandle	= nullptr;
		}
	}

	void remove()
	{
		close();
		::remove(mFilename.c_str());
	}


private:
	std::string	mFilename;
	FILE*		mHandle		= nullptr;
	int64_t		mFilePos	= 0;
	int64_t		mFileSize	= 0;
};

INARIKVS_NAMESPACE_END
#endif
