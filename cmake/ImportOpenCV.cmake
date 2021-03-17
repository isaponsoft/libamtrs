# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
amtrs_add_external_license(OPENCV	OpenCV			https://raw.githubusercontent.com/opencv/opencv/master/LICENSE)
amtrs_add_external_license(QUIRC	QUIRC			https://raw.githubusercontent.com/dlbeer/quirc/master/LICENSE)
amtrs_add_external_license(PROTBUF	protocolbuffers	https://raw.githubusercontent.com/protocolbuffers/protobuf/master/LICENSE)
amtrs_add_external_license(OPENEXR	OpenEXR			https://raw.githubusercontent.com/AcademySoftwareFoundation/openexr/master/LICENSE.md)
amtrs_add_external_license(TIFF		libtiff			https://gitlab.com/libtiff/libtiff/-/raw/master/COPYRIGHT)
amtrs_add_external_license(PNG		libpng			http://www.libpng.org/pub/png/src/libpng-LICENSE.txt)
amtrs_add_external_license(JPEG		jpeg-turbo		https://raw.githubusercontent.com/libjpeg-turbo/libjpeg-turbo/master/README.ijg)
amtrs_add_external_license(VORBIS	zlib			https://raw.githubusercontent.com/madler/zlib/master/README)


set(OpenCV_VERSION				451)
if(MSVC)
amtrs_make_libname(__libname1	opencv_world${OpenCV_VERSION} "" "d")
else()
amtrs_make_libname(__libname1	opencv_world "" "")
endif()



amtrs_find_libraries(opencv2/opencv.hpp ${__libname1})
if(_LIBRARIES)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()


set(OPENCV_OPTIONS)
if(${AMTRS_NO_COPYLEFT})
set(OPENCV_OPTIONS	-DWITH_FFMPEG=OFF)
endif()


amtrs_external_add(
	${NAME}
	LIBS			${__libname1}
	URL 			"https://github.com/opencv/opencv/archive/4.5.1.zip"
	CMAKE_ARGS		-DOPENCV_BIN_INSTALL_PATH=bin
					-DOPENCV_LIB_INSTALL_PATH=lib
					-DOPENCV_INCLUDE_INSTALL_PATH=include
					-DOPENCV_3P_LIB_INSTALL_PATH=lib
					-DOPENCV_CONFIG_INSTALL_PATH=etc
					-DOPENCV_SAMPLES_BIN_INSTALL_PATH=${CMAKE_BINARY_DIR}
					-DOPENCV_SAMPLES_SRC_INSTALL_PATH=${CMAKE_BINARY_DIR}
					-DOPENCV_LICENSES_INSTALL_PATH=${CMAKE_BINARY_DIR}
					-DOPENCV_TEST_DATA_INSTALL_PATH=${CMAKE_BINARY_DIR}
					-DBUILD_SHARED_LIBS=OFF
					-DBUILD_opencv_world=ON
					-DBUILD_JAVA=OFF
					-DBUILD_OBJC=OFF
					-DBUILD_TEST=OFF
					${OPENCV_OPTIONS}
)
