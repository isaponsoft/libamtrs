/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__zlp__zip_iterator__hpp
#define __libamtrs__filesystem__zlp__zip_iterator__hpp
#include "@fwd.hpp"
AMTRS_FILESYSTEM_ZIP_NAMESPACE_BEGIN





//! テンプレート引数INで指定するioを使用して zip アーカイブのファイルイテレータを作成します。
template<class In>
class	zip_iterator
{
public:
	using	filesize_type	= uintmax_t;
	using	size_type		= size_t;
	using	value_type		= zip_entry;
	static constexpr filesize_type	npos	= static_cast<filesize_type>(-1);

	zip_iterator() = default;
	zip_iterator(const zip_iterator&) = default;
	zip_iterator(zip_iterator&&) = default;
	zip_iterator& operator = (const zip_iterator&) = default;
	zip_iterator& operator = (zip_iterator&&) = default;


	bool operator == (const zip_iterator& _r) const noexcept { return (mPosition == _r.mPosition) && ((!mPosition) || (mArchive == _r.mArchive) ); }
	bool operator != (const zip_iterator& _r) const noexcept { return !(*this == _r); }

	const value_type& operator * () const noexcept { return mEntry; }

	const value_type* operator -> () const noexcept { return &mEntry; }

	zip_iterator& operator ++ ()
	{
		if (mPosition)
		{
			mPosition	+= filesystem::zip::centraldirentry_size + mCentral.fileInfo.filenameSize + mCentral.fileInfo.extraFieldSize + mCentral.commentSize;
			read();
		}
		return	*this;
	}

private:
	zip_iterator(zip_archive<In>* _arc, filesize_type _pos)
		: mArchive(_arc)
		, mPosition(_pos)
	{
		read();
	}

	void read()
	{
		if (!mArchive->read_entry(&mEntry, mCentral, mPosition))
		{
			mEntry				= {};
			mPosition			= 0;
		}
		else
		{
			mEntry.mFilepos				= mCentral.offsetCurFile;
		}
	}


	zip_archive<In>*					mArchive		= nullptr;
	filesystem::zip::centraldirentry	mCentral;
	filesize_type						mPosition		= 0;
	zip_entry							mEntry;

	template<class>
	friend	class	zip_archive;
};



AMTRS_FILESYSTEM_ZIP_NAMESPACE_END
#endif
