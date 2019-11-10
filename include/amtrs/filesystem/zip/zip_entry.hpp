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
#ifndef __libamtrs__filesystem__zlp__zip_entry__hpp
#define __libamtrs__filesystem__zlp__zip_entry__hpp
#include "../../memory/shared_buffer.hpp"
#include "@fwd.hpp"
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
