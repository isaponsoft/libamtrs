# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
# ======================================================================
# g3d library:
#	$ENV{VULKAN_SDK}
#
# ----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10.0)
project(amtrs)

set(TEST_SRCS
	main.cpp
	chrono-time.cpp
	crypto-hash.cpp
	graphics-view2d.cpp
	io-reader-writer.cpp
	lang-lson.cpp
	lang-passion-prepro.cpp
	lang-passion-pfl.cpp
	lang-passion-psn.cpp
	lang-passion-psn-error.cpp
	lang-passion-vm.cpp
	memory-hash_map.cpp
	memory-simplearray.cpp
	memory-stackbuf.cpp
	memory-view.cpp
	string-shared_string.cpp
	string-split_iterator.cpp
	string-trnstext.cpp
	string-wildcard.cpp
	typeutil-eval_test.cpp
)

add_executable(selftest ${TEST_SRCS})
set_property(TARGET selftest PROPERTY CXX_STANDARD 17)
target_link_libraries(selftest amtrs_static)
