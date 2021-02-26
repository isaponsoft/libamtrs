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
#include <list>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include "io.hpp"
#include "string.hpp"


#define	AMTRS_FILESYSTEM_NAMESPACE			AMTRS_NAMESPACE::filesystem
#define	AMTRS_FILESYSTEM_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace filesystem {
#define	AMTRS_FILESYSTEM_NAMESPACE_END		} AMTRS_NAMESPACE_END
#define	AMTRS_FILESYSTEM_ZIP_NAMESPACE_BEGIN	AMTRS_FILESYSTEM_NAMESPACE_BEGIN namespace zip {
#define	AMTRS_FILESYSTEM_ZIP_NAMESPACE_END		} AMTRS_FILESYSTEM_NAMESPACE_END

#include ".inc/filesystem-compati.hpp"
#include ".inc/filesystem-path.hpp"
#include ".inc/filesystem-types.hpp"
#include ".inc/filesystem-vfs.hpp"
#include ".inc/filesystem-assetvfs.hpp"
#include ".inc/filesystem-cascadevfs.hpp"
#include ".inc/filesystem-fileloader.hpp"
#include ".inc/filesystem-filename.hpp"
#include ".inc/filesystem-stdvfs.hpp"
#include ".inc/filesystem-functions.hpp"
#include ".inc/filesystem-util.hpp"


#if		AMTRS_ZLIB_ENABLE
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

#include ".inc/filesystem-zip-types.hpp"
#include ".inc/filesystem-zip-entry.hpp"
#include ".inc/filesystem-zip-archive.hpp"
#include ".inc/filesystem-zip-iterator.hpp"
#include ".inc/filesystem-zip-zipfs.hpp"
#endif

#endif
