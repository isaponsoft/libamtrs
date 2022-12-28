/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__hpp
#define __libamtrs__filesystem__hpp
#include "amtrs.hpp"
#include <cctype>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include "io.hpp"
#include "string.hpp"


#include ".inc-filesystem/filesystem-types.hpp"
#include ".inc-filesystem/filesystem-vfs.hpp"
#include ".inc-filesystem/filesystem-assetvfs.hpp"
#include ".inc-filesystem/filesystem-cascadevfs.hpp"
#include ".inc-filesystem/filesystem-fileloader.hpp"
#include ".inc-filesystem/filesystem-filename.hpp"
#include ".inc-filesystem/filesystem-findfiles.hpp"
#include ".inc-filesystem/filesystem-stdvfs.hpp"
#include ".inc-filesystem/filesystem-functions.hpp"
#include ".inc-filesystem/filesystem-util.hpp"


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

#include ".inc-filesystem/filesystem-zip-types.hpp"
#include ".inc-filesystem/filesystem-zip-entry.hpp"
#include ".inc-filesystem/filesystem-zip-archive.hpp"
#include ".inc-filesystem/filesystem-zip-iterator.hpp"
#include ".inc-filesystem/filesystem-zip-zipfs.hpp"

#endif
