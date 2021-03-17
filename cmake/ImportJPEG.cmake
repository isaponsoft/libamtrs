# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
amtrs_make_libname(__libname1	turbojpeg -static "")
amtrs_add_external_license(JPEG	"jpeg-turbo" https://raw.githubusercontent.com/libjpeg-turbo/libjpeg-turbo/master/README.ijg)

amtrs_find_libraries(turbojpeg.h ${__libname1})
if(_LIBRARIES)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()


amtrs_external_add(
	${NAME}
	LIBS			${__libname1}
	URL			https://github.com/libjpeg-turbo/libjpeg-turbo/archive/master.zip
)
