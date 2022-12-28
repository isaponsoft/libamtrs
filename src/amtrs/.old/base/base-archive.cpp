/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <amtrs/archive.hpp>
#define	LIBARCHIVE_STATIC
#if		__has_include(<archive.h>)
#include <archive.h>
#include <archive_entry.h>
#elif	__has_include(<libarchive/archive.h>)
#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>
#else
#error "libarhive not found"
#endif
AMTRS_NAMESPACE_BEGIN
enum class libarchive_errcode : int;
AMTRS_NAMESPACE_END

namespace std {

template <>
struct	is_error_code_enum<amtrs::libarchive_errcode>
		: true_type
{};

}

AMTRS_NAMESPACE_BEGIN
class	libarchive_category
		: public std::error_category
{
public:
    char const* name() const noexcept override
	{
		return	"libarchive";
    }

#if	0
	std::string message(int _ec) const override
	{
		std::string	msg;
		auto		a	= archive_read_new();
		archive_read_support_filter_all(a);
		archive_read_support_format_all(a);
		archive_read_support_compression_all(a);
		archive_error_string(a, _ec);
		msg	= archive_error_string(a);
		archive_read_free(a);
		return	msg;
	}
#else
	std::string message(int _ec) const override
	{
		return	"";
	}
#endif

	static libarchive_category const& get_instance()
	{
		static	libarchive_category	thiz;
		return	thiz;
	}
};

std::string_view archive_enumrator::entry::name()
{
	return	archive_entry_pathname(_entry);
}


size_t archive_enumrator::entry::size()
{
	return	archive_entry_size(_entry);
}


void archive_enumrator::iteratorable::init()
{
	if (mScaner)
	{
		if (mEntry._entry)
		{
			archive_read_data_skip(mScaner->mArc);
			archive_entry_clear(mEntry._entry);
		}
		if (archive_read_next_header(mScaner->mArc, &mEntry._entry) == ARCHIVE_OK)
		{
		}
		else
		{
			mEntry._entry	= nullptr;
		}
	}
}


archive_enumrator::archive_enumrator()
	: mArc(nullptr)
{
}

archive_enumrator::archive_enumrator(archive_enumrator&& _r)
	: mArc(_r.mArc)
{
	_r.mArc	= nullptr;
}

archive_enumrator::~archive_enumrator()
{
	if (mArc)
	{
//		archive_read_close(mArc);
		archive_read_free(mArc);
	}
}

archive_enumrator::archive_enumrator(io::vstreamif* _in)
{
	auto	_read_callback	= [](struct archive* _arch, void* _client_data, const void ** _buffer) -> la_ssize_t
							{
								auto	thiz	= reinterpret_cast<archive_enumrator*>(_client_data);
								auto	sz		= io::read(thiz->mBuffer.data(), *thiz->mIn, thiz->mBuffer.size());
								*_buffer		= thiz->mBuffer.data();
								return	sz;
							};
	mIn		= _in;
	mArc	= archive_read_new();
	mBuffer	= shared_buffer<char>(8192);
	archive_read_support_filter_all(mArc);
	archive_read_support_format_all(mArc);
	archive_read_support_compression_all(mArc);
	if (archive_read_open(mArc, this, nullptr, _read_callback, nullptr) != ARCHIVE_OK)
	{
		std::string		em	= archive_error_string(mArc);
		std::error_code	ec	= std::error_code(archive_errno(mArc), libarchive_category::get_instance());
		archive_read_free(mArc);
		mArc	= nullptr;
		throw	std::system_error(std::move(ec), std::move(em));
	}
}

archive_enumrator::iterator archive_enumrator::begin()
{
	if (empty())
	{
		return	end();
	}
	return	iterator(this);
}

archive_enumrator::iterator archive_enumrator::end()
{
	return	iterator(nullptr);
}


io::streamif_base::iostate archive_enumrator::blocstatus::read(entry& _entry, size_type& _readsize, void* _data, size_type _size)
{
	char*	dest	= reinterpret_cast<char*>(_data);
	_readsize	= 0;
	while (_readsize < _size)
	{
		// まずはバッファの中身を転送する
		if (blockCursor < blockSize)
		{
			size_t	cs	= std::min<size_t>(static_cast<size_t>(blockSize - blockCursor), _size);
			std::copy_n(reinterpret_cast<char const*>(blockBuffer) + blockCursor, cs, dest);
			blockCursor+= cs;
			_readsize	+= cs;
			dest		+= cs;
			_size		-= cs;
			// バッファが残っているなら終了
			if (blockCursor < blockSize && _size == 0)
			{
				return	io::streamif_base::goodbit;
			}
		}
		// 次のバッファを取得する
		la_int64_t	laBlockOffset	= blockOffset;
		auto	r = archive_read_data_block(_entry._arcive, &blockBuffer, &blockSize, &laBlockOffset);
		blockOffset	= laBlockOffset;
		if (r == ARCHIVE_OK)
		{
			blockCursor	= 0;
			continue;
		}
		if (r == ARCHIVE_EOF)
		{
			return	io::streamif_base::eofbit;
		}
		return	io::streamif_base::badbit;
	}
	return	io::streamif_base::goodbit;
}

AMTRS_NAMESPACE_END
