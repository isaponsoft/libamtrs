/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesysten.hpp>
#include <amtrs/tests.hpp>
using namespace amtrs;
using namespace amtrs::filesystem;

#define	_PRFX(_name)	filename_##_name
#if	0

AMTRS_TEST_F(_PRFX(md5))
{



array_shift(argv);
for (auto path = array_shift(argv); !path.empty(); path = array_shift(argv))
{
	std::cout << "----------------------------" << std::endl;
	std::cout << path << std::endl;
	std::cout << "---" << std::endl;
	std::cout << "normalize_path " << normalize_path(path) << std::endl;
	std::cout << "parent_path    " << parent_path(normalize_path(path)) << std::endl;
	std::cout << "filename       " << filename(normalize_path(path)) << std::endl;
	std::cout << "stem           " << stem(normalize_path(path)) << std::endl;
	std::cout << "ext            " << extension(normalize_path(path)) << std::endl;
	std::cout << "abs            " << absolute_path(normalize_path(path)) << std::endl;
	std::cout << "root_path      " << root_path(normalize_path(path)) << std::endl;
	std::cout << "is_absolute    " << is_absolute(normalize_path(path)) << std::endl;
	std::cout << "root_path      " << root_path(normalize_path(path)) << std::endl;

	std::cout << path << std::endl;
	std::cout << "---" << std::endl;
	std::cout << "normalize_path " << normalize_path(path, '\\') << std::endl;
	std::cout << "parent_path    " << parent_path(normalize_path(path, '\\')) << std::endl;
	std::cout << "filename       " << filename(normalize_path(path, '\\')) << std::endl;
	std::cout << "stem           " << stem(normalize_path(path, '\\')) << std::endl;
	std::cout << "ext            " << extension(normalize_path(path, '\\')) << std::endl;
	std::cout << "abs            " << absolute_path(normalize_path(path, '\\')) << std::endl;
	std::cout << "root_path      " << root_path(normalize_path(path, '\\')) << std::endl;
	std::cout << "is_absolute    " << is_absolute(normalize_path(path, '\\')) << std::endl;
	std::cout << "root_path      " << root_path(normalize_path(path, '\\')) << std::endl;

	std::cout << "---" << std::endl;
	std::cout << "to_slash_path  " << to_slash_path(path) << std::endl;
	std::cout << "parent_path    " << parent_path(to_slash_path(path)) << std::endl;
	std::cout << "filename       " << filename(to_slash_path(path)) << std::endl;
	std::cout << "stem           " << stem(to_slash_path(path)) << std::endl;
	std::cout << "ext            " << extension(to_slash_path(path)) << std::endl;
	std::cout << "abs            " << absolute_path(to_slash_path(path)) << std::endl;
	std::cout << "root_path      " << root_path(to_slash_path(path)) << std::endl;
	std::cout << "is_absolute    " << is_absolute(to_slash_path(path)) << std::endl;
	std::cout << "root_path      " << root_path(to_slash_path(path)) << std::endl;

	std::cout << "---" << std::endl;
	std::cout << "parent_path    " << parent_path(path) << std::endl;
	std::cout << "filename       " << filename(path) << std::endl;
	std::cout << "stem           " << stem(path) << std::endl;
	std::cout << "ext            " << extension(path) << std::endl;
	std::cout << "abs            " << absolute_path(path) << std::endl;
	std::cout << "root_path      " << root_path(path) << std::endl;
	std::cout << "is_absolute    " << is_absolute(path) << std::endl;
	std::cout << "root_path      " << root_path(path) << std::endl;

	std::cout << "---" << std::endl;
	std::cout << "parent_path    " << std::filesystem::path(path).parent_path() << std::endl;
	std::cout << "filename       " << std::filesystem::path(path).filename() << std::endl;
	std::cout << "stem           " << std::filesystem::path(path).stem() << std::endl;
	std::cout << "ext            " << std::filesystem::path(path).extension() << std::endl;
	std::cout << "root_name      " << std::filesystem::path(path).root_name() << std::endl;
	std::cout << "root_directory " << std::filesystem::path(path).root_directory() << std::endl;
	std::cout << "is_absolute    " << std::filesystem::path(path).is_absolute() << std::endl;
	std::cout << "root_path      " << std::filesystem::path(path).root_path() << std::endl;
}
#endif
