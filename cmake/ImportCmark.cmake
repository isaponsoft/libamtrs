# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
amtrs_make_libname(__libname1	cmark-gfm				_static "")
amtrs_make_libname(__libname2	cmark-gfm-extensions	_static "")
amtrs_add_external_license(ARCHIVE	"libcmark-gfm" https://raw.githubusercontent.com/isaponsoft/libcmark-gfm/master/COPYING)


amtrs_find_libraries(archive.h ${__libname1} ${__libname2})
if(_INCLUDE_DIR)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()



amtrs_external_add(
	${NAME}
	LIBS		${__libname1} ${__libname2}
	URL			https://github.com/isaponsoft/libcmark-gfm/archive/master.zip
)
