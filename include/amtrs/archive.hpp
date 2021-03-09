/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__archive__archive_enumrator__hpp
#define	__libamtrs__archive__archive_enumrator__hpp
#include "io.hpp"
#ifdef	AMTRS_ARCHIVE_ENABLE
#if		__has_include(<libarchive/archive.h>)
#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>
#elif	__has_include(<archive.h>)
#include <archive.h>
#include <archive_entry.h>
#endif
AMTRS_NAMESPACE_BEGIN

template<class Callback>
bool archive_enumrate(const char* _filename);



template<class T>
class	basic_archive_enumrator
{
public:
	class	iteratorable;

	class	entry
	{
	public:
		std::string_view name()
		{
			return	archive_entry_pathname(_entry);
		}

		size_t size()
		{
			return	archive_entry_size(_entry);
		}

		archive*		_arcive	= nullptr;
		archive_entry*	_entry	= nullptr;
		friend	iteratorable;
	};

	class	iteratorable
	{
	public:
		using	value_type	= entry;

		iteratorable() = default;
		iteratorable(iteratorable const&) = delete;
		iteratorable(iteratorable&& _r)
			: mScaner(_r.mScaner)
			, mEntry(_r.mEntry)
		{
			_r.mEntry._arcive	= mScaner->mArc;
			_r.mEntry._entry	= nullptr;
		}

		explicit iteratorable(basic_archive_enumrator* _scan)
			: mScaner(_scan)
		{
			mEntry._arcive	= mScaner ? mScaner->mArc : nullptr;
			mEntry._entry	= nullptr;
			init();
		}

		entry& operator * ()
		{
			return	mEntry;
		}

		entry* operator -> ()
		{
			return	&mEntry;
		}

		iteratorable& operator ++ ()
		{
			init();
			return	*this;
		}

		bool operator == (iteratorable const& _r) const noexcept { return mEntry._entry == _r.mEntry._entry; }
		bool operator != (iteratorable const& _r) const noexcept { return mEntry._entry != _r.mEntry._entry; }

		void init()
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

		basic_archive_enumrator*	mScaner	= nullptr;
		entry					mEntry;

		friend	basic_archive_enumrator;
	};

	using	iterator	= iteratorable;

	basic_archive_enumrator() = default;
	basic_archive_enumrator(basic_archive_enumrator const&) = delete;
	basic_archive_enumrator(basic_archive_enumrator&& _r)
		: mArc(_r.mArc)
	{
		_r.mArc	= nullptr;
	}

	~basic_archive_enumrator()
	{
		if (mArc)
		{
//			archive_read_close(mArc);
			archive_read_free(mArc);
		}
	}

	io::vstreamif*		mIn;
	shared_buffer<char>	mBuffer;

	static la_ssize_t _read_callback(struct archive* _arch, void* _client_data, const void **buffer)
	{
		auto	thiz	= reinterpret_cast<basic_archive_enumrator*>(_client_data);
		auto	sz		= io::read(thiz->mBuffer.data(), *thiz->mIn, thiz->mBuffer.size());
		*buffer			= thiz->mBuffer.data();
		return	sz;
	}

	basic_archive_enumrator(io::vstreamif* _in)
	{
		mIn		= _in;
		mArc	= archive_read_new();
		mBuffer	= shared_buffer<char>(8192);
		archive_read_support_format_all(mArc);
		archive_read_support_compression_all(mArc);
		if (archive_read_open(mArc, this, nullptr, &_read_callback, nullptr))
		{
			archive_read_free(mArc);
			mArc	= nullptr;
		}
	}

	iterator begin()
	{
		if (empty())
		{
			return	end();
		}
		return	iterator(this);
	}

	iterator end()
	{
		return	iterator(nullptr);
	}


	bool empty() const noexcept
	{
		return	mArc == nullptr;
	}

	struct archive*	mArc	= nullptr;
	friend iterator;
};


using	archive_enumrator	= basic_archive_enumrator<char>;

template<class Callback>
bool archive_enumrate(io::vstreamif* _in, Callback _callback)
{
	archive_enumrator	a(_in);
	if (a.empty())
	{
		return	false;
	}
	for (auto& e : a)
	{
		_callback(e);
	}
	return	true;
}


AMTRS_NAMESPACE_END
AMTRS_IO_NAMESPACE_BEGIN
template<class Elm>
struct	streamif_traits<typename basic_archive_enumrator<Elm>::entry, Elm>
{
	using	char_type	= Elm;
	using	value_type	= typename basic_archive_enumrator<Elm>::entry;
	using	size_type	= std::streamsize;
	using	fpos_type	= std::fpos<typename std::char_traits<char_type>::state_type>;


protected:
	template<class... A>
	streamif_traits(A...)
	{}

	const void*	mBlockBuffer	= nullptr;
	size_t		mBlockCursor	= 0;
	size_t		mBlockSize		= 0;
	la_int64_t	mBlockOffset	= 0;

	streamif_base::iostate read(value_type& _value, size_type& _readsize, char_type* _data, size_type _size)
	{
		_readsize	= 0;
		while (_readsize < _size)
		{
			// まずはバッファの中身を転送する
			if (mBlockCursor < mBlockSize)
			{
				size_t	cs	= std::min<size_t>(static_cast<size_t>(mBlockSize - mBlockCursor), _size);
				std::copy_n(reinterpret_cast<char const*>(mBlockBuffer) + mBlockCursor, cs, _data);
				mBlockCursor+= cs;
				_readsize	+= cs;
				_data		+= cs;
				_size		-= cs;
				// バッファが残っているなら終了
				if (mBlockCursor < mBlockSize && _size == 0)
				{
					return	streamif_base::goodbit;
				}
			}

			// 次のバッファを取得する
			auto	r = archive_read_data_block(_value._arcive, &mBlockBuffer, &mBlockSize, &mBlockOffset);
			if (r == ARCHIVE_OK)
			{
				mBlockCursor	= 0;
				continue;
			}
			if (r == ARCHIVE_EOF)
			{
				return	streamif_base::eofbit;
			}
			return	streamif_base::badbit;
		}
		return	streamif_base::goodbit;
	}

	streamif_base::iostate write(value_type& _value, size_type& _readsize, char_type const* _data, size_type _size)
	{
		return	streamif_base::failbit;
	}

	streamif_base::iostate tell(value_type& _value, fpos_type& _position)
	{
		_position	= _value._size;
		return	streamif_base::goodbit;
	}
};


inline auto make_streamif(typename basic_archive_enumrator<char>::entry& _value)
{
	return	make_basic_streamif<decltype(&_value), char, typename basic_archive_enumrator<char>::entry>(&_value);
}


AMTRS_IO_NAMESPACE_END
#endif
#endif
