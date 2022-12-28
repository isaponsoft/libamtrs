/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__archive__archive_enumrator__hpp
#define	__libamtrs__archive__archive_enumrator__hpp
#include "io.hpp"
#include ".inc/memory-shared_buffer.hpp"

// prototypes(libarchive)
struct archive;
struct archive_entry;

AMTRS_NAMESPACE_BEGIN

template<class Callback>
bool archive_enumrate(const char* _filename);



class	archive_enumrator
{
public:
	class	iteratorable;
	class	entry;

	struct	blocstatus
	{
		using		size_type	= std::streamsize;

		const void*	blockBuffer	= nullptr;
		size_t		blockCursor	= 0;
		size_t		blockSize	= 0;
		int64_t		blockOffset	= 0;
	
		io::streamif_base::iostate read(entry& _entry, size_type& _readsize, void* _data, size_type _size);
	};

	class	entry
	{
	public:
		std::string_view name();
		size_t size();

	private:
		archive*		_arcive	= nullptr;
		archive_entry*	_entry	= nullptr;
		friend	blocstatus;
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

		explicit iteratorable(archive_enumrator* _scan)
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

	private:
		void init();

		archive_enumrator*	mScaner	= nullptr;
		entry				mEntry;

		friend	archive_enumrator;
	};

	using	iterator	= iteratorable;

	archive_enumrator(archive_enumrator const&) = delete;

	archive_enumrator();
	archive_enumrator(archive_enumrator&& _r);
	~archive_enumrator();
	archive_enumrator(io::vstreamif* _in);
	iterator begin();
	iterator end();
	bool empty() const noexcept
	{
		return	mArc == nullptr;
	}

private:
	io::vstreamif*		mIn;
	shared_buffer<char>	mBuffer;

	struct archive*		mArc	= nullptr;
	friend iterator;
};

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
template<>
struct	streamif_traits<typename archive_enumrator::entry>
{
	using	char_type	= char;
	using	value_type	= typename archive_enumrator::entry;
	using	size_type	= std::streamsize;
	using	fpos_type	= std::fpos<typename std::char_traits<char_type>::state_type>;


protected:
	template<class... A>
	streamif_traits(A...)
	{}

	archive_enumrator::blocstatus	mStatus;

	streamif_base::iostate read(value_type& _value, size_type& _readsize, void* _data, size_type _size)
	{
		return	mStatus.read(_value, _readsize, _data, _size);
	}

	streamif_base::iostate write(value_type& _value, size_type& _readsize, char_type const* _data, size_type _size)
	{
		return	streamif_base::failbit;
	}

	streamif_base::iostate tell(value_type& _value, fpos_type& _position)
	{
		_position	= _value.size();
		return	streamif_base::goodbit;
	}
};


inline auto make_streamif(archive_enumrator::entry& _value)
{
	return	make_basic_streamif<decltype(&_value), archive_enumrator::entry>(&_value);
}


AMTRS_IO_NAMESPACE_END
#endif
