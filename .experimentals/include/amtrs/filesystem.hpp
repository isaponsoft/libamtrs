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
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include "string.hpp"
#include "vstream.hpp"


#define	AMTRS_FILESYSTEM_NAMESPACE			AMTRS_NAMESPACE::filesystem
#define	AMTRS_FILESYSTEM_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace filesystem {
#define	AMTRS_FILESYSTEM_NAMESPACE_END		} AMTRS_NAMESPACE_END

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
#endif
