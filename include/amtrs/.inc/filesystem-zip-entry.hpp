/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__zlp__zip_entry__hpp
#define __libamtrs__filesystem__zlp__zip_entry__hpp
AMTRS_FILESYSTEM_ZIP_NAMESPACE_BEGIN


template<class CharT>
struct	zip_traits
{
	using	char_typr		= CharT;

	using	filename_type	= shared_buffer<char_typr>;
};



template<class CharT, class Traits>
class	basic_zip_entry
		: public entry_data
{
public:
	using	traits_type		= Traits;
	using	filename_type	= typename traits_type::filename_type;
	using	filesize_type	= uintmax_t;

	//! ファイル名
	const filename_type& filename() const noexcept { return mFilename; }


	auto position() const noexcept { return mFilepos; }

	bool empty() const noexcept { return mFilepos == 0; }

private:
	filename_type		mFilename;
	filesize_type		mFilepos			= 0;

	template<class>
	friend	class	zip_iterator;

	template<class>
	friend	class	zip_archive;
};




AMTRS_FILESYSTEM_ZIP_NAMESPACE_END
#endif
