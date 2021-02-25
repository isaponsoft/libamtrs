/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__zip__fwd__hpp
#define __libamtrs__filesystem__zip__fwd__hpp
#include "def.hpp"
AMTRS_FILESYSTEM_ZIP_NAMESPACE_BEGIN

template<class CharT>
struct	zip_traits;


template<class IN>
class	zip_archive;


template<class CharT = char, class Traits = zip_traits<CharT>>
class	basic_zip_entry;

template<class IN>
class	zip_iterator;


using	zip_entry	= basic_zip_entry<char>;


AMTRS_FILESYSTEM_ZIP_NAMESPACE_END
#endif
