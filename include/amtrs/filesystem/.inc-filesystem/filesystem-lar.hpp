/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__lar__hpp
#define	__libamtrs__filesystem__lar__hpp
#include "filesystem.hpp"
#define	AMTRS_LAR_NAMESPACE			AMTRS_FILESYSTEM_NAMESPACE
#define	AMTRS_LAR_NAMESPACE_BEGIN	namespace AMTRS_LAR_NAMESPACE {
#define	AMTRS_LAR_NAMESPACE_END		}

AMTRS_EXTERN_C_BEGIN

/*
 * LAR_SIZE_T		variable length size value.
 *
 *	uint8_t			first	D7 : 0=stop, 1=nextbyte
 *	uint8_t			next	D7 : 0=stop, 1=nextbyte
 *	uint8_t			next...
 *
 * Example
 *	83:AC:21		(((03 << 7)+ 2C) << 7) + 21
 */
typedef	uintmax_t		LAR_SIZE_T;



typedef	enum	_lar_compresstype
		{
			lar_raw		= 0,					// not compress
			lar_deflate	= 1,					// deflate (Default)
		} LAR_COMPRESSTYPE;


typedef	struct	_lar_filehader
		{
			uint8_t		signature[4];		// 0000 : 4 "LAR1"
			uint32_t	header_size;
		} LAR_FILEHEADER;


typedef	struct	_lar_entry
		{
			LAR_SIZE_T	file_position;
			LAR_SIZE_T	compress_size;
			LAR_SIZE_T	original_size;
			LAR_SIZE_T	name_size;
			uint8_t		compress;			// enum compresstype / LAR_COMPRESSTYPE
#ifdef	__cplusplus
			/* 未圧縮のデータのサイズを設定します */
			inline void set_raw_datasize(LAR_SIZE_T _size) noexcept
			{
				compress_size	= _size;
				original_size	= _size;
				compress		= lar_raw;
			}
#endif
		//	char		name[name_size]		// padding 4
		} LAR_ENTRY;



AMTRS_EXTERN_C_END
#ifdef	__cplusplus
AMTRS_LAR_NAMESPACE_BEGIN

using	fileheader		= LAR_FILEHEADER;
using	entry			= LAR_ENTRY;
using	compresstype	= LAR_COMPRESSTYPE;

template<class T>
class	basic_lar_iterator
{
public:
	using	value_type	= T;
	using	reference	= value_type&;
	using	pointer		= value_type*;

	basic_lar_iterator() = default;
	basic_lar_iterator(basic_lar_iterator const&) = default;
	basic_lar_iterator(basic_lar_iterator&&) = default;
	basic_lar_iterator& operator = (basic_lar_iterator const&) = default;
	basic_lar_iterator& operator = (basic_lar_iterator&&) = default;
	explicit basic_lar_iterator(pointer* _cur) : mCursor(_cur) {}
	reference operator * () const { return **mCursor; }
	pointer operator -> () const { return *mCursor; }
	basic_lar_iterator& operator ++ () noexcept { ++mCursor; return *this; }
	bool operator == (basic_lar_iterator const& _r) const noexcept { return mCursor == _r.mCursor; }
	bool operator != (basic_lar_iterator const& _r) const noexcept { return mCursor != _r.mCursor; }
protected:
	pointer*	mCursor;
};


class	lar
{
public:
	struct	entry
			: LAR_ENTRY
	{
		entry() = default;
		entry(entry const&) = delete;
		entry(entry&&) = delete;
		~entry()
		{
			if (mName)
			{
				delete[] mName;
			}
			if (mData)
			{
				delete[] mData;
			}
		}

		// 名前をセットします
		void name(std::string_view _name);

		// compress_sizeに従ってデータ保存用のバッファを確保します
		void data_alloc();

		char const* name() const noexcept { return mName; }
		void* data() const noexcept { return mData; }

		// 指定した形式へ変換する
		void compress_to(compresstype _type);


	private:
		char*	mName	= nullptr;
		char*	mData	= nullptr;
		friend	lar;
	};

	using	iterator		= basic_lar_iterator<entry>;
	using	const_iterator	= basic_lar_iterator<entry const>;


	lar();
	~lar();

	iterator begin() noexcept { return iterator(mEntries); }
	iterator end() noexcept { return iterator(mEntries + mEntryNum); }
	const_iterator begin() const noexcept { return const_iterator(const_cast<entry const**>(mEntries)); }
	const_iterator end() const noexcept { return const_iterator(const_cast<entry const**>(mEntries) + mEntryNum); }


	static size_t compute_padd(size_t _s, size_t _a)
	{
		auto	pad	= _s & (_a-1);
		return	pad ? _a - pad : 0;
	}

	size_t size() const noexcept
	{
		return	mEntryNum;	
	}

	size_t compute_entry_size() const noexcept;


	size_t compute_value_size() const noexcept;

	lar::iterator add_entry(std::string_view _name);
	iterator find(std::string_view _name) noexcept;
	const_iterator find(std::string_view _name) const noexcept;

protected:
	size_t	mEntryNum	= 0;
	entry**	mEntries	= nullptr;
};

AMTRS_LAR_NAMESPACE_END
AMTRS_IO_NAMESPACE_BEGIN

bool read(AMTRS_FILESYSTEM_NAMESPACE::lar& _s, amtrs_stream_read_seekable& _i, bool _headeronly = false);
bool write(amtrs_stream_write_seekable& _o, AMTRS_FILESYSTEM_NAMESPACE::lar& _s);

template<class In, std::enable_if_t<std::is_base_of_v<amtrs_stream_read_seekable, In> == 0, int> = 0>
size_t read(AMTRS_FILESYSTEM_NAMESPACE::lar& _s, In _i)
{
	auto	i	= make_read_seekable(std::move(_i));
	return	read(_s, static_cast<amtrs_stream_read_seekable&>(i));
}


template<class Out, std::enable_if_t<std::is_base_of_v<amtrs_stream_write_seekable, Out> == 0, int> = 0>
size_t write(Out _o, AMTRS_FILESYSTEM_NAMESPACE::lar& _s)
{
	auto	o	= make_write_seekable(std::move(_o));
	return	write(static_cast<amtrs_stream_write_seekable&>(o), _s);
}

AMTRS_IO_NAMESPACE_END
#endif
#endif
