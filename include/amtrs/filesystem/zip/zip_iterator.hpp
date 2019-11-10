/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. In NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER In CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING In ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
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
