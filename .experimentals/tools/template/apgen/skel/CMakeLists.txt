cmake_minimum_required(VERSION 3.0.2)
project(${cmake.PROJECTNAME})
set(CMAKE_C_STANDARD	11	CACHE INTERNAL	"" FORCE)
set(CMAKE_CXX_STANDARD	17	CACHE INTERNAL	"" FORCE)


set(PROJDIR		$\{CMAKE_SOURCE_DIR})

file(GLOB SRC  "src/*.cpp")



if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(CMAKE_CXX_FLAGS	"$\{CMAKE_CXX_FLAGS} /D_CONSOLE=1 /DDEBUG=1")
	add_executable(
		appmain
		$\{SRC}
	)
endif()



if(ANDROID)
	add_library(
		appmain SHARED
		$\{SRC}
	)
endif()



if(APPLE)
	set(CMAKE_OSX_SYSROOT						"iphoneos")
	set(CMAKE_CXX_FLAGS			 				"$(OTHER_CFLAGS) $\{CMAKE_CXX_FLAGS} -x objective-c++")
	set(CMAKE_OSX_DEPLOYMENT_TARGET				"11.0" CACHE STRING "" FORCE)
	set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE	"NO" CACHE INTERNAL "")
	set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM	"******")

	file(GLOB RESOURCE_FILES	"$\{PROJDIR}/asset/*")

	add_executable(
		appmain MACOSX_BUNDLE
		${SRC}
		${RESOURCE_FILES}
	)


	set_target_properties(
		appmain				PROPERTIES
		MACOSX_BUNDLE										TRUE
		OUTPUT_NAME											appmain
		MACOSX_BUNDLE_GUI_IDENTIFIER						"******"
		MACOSX_BUNDLE_LONG_VERSION_STRING					"1"
		MACOSX_BUNDLE_SHORT_VERSION_STRING					"1"
		MACOSX_BUNDLE_BUNDLE_VERSION						1
		XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME	"AppIcon"
		RESOURCE											"$\{RESOURCE_FILES}"
	)

endif()


#set(AMTRS_GFRAME_ENABLE		ON	CACHE BOOL "" FORCE)

find_path(LIBAMTRS_PATH libamtrs PATHS ../lib ../libs ../../lib ../../libs NO_CMAKE_FIND_ROOT_PATH)
add_subdirectory($\{LIBAMTRS_PATH}/libamtrs build-amtrs)



target_include_directories(
	appmain PUBLIC
	$\{PROJDIR}/inc
	$\{PROJDIR}/src
	inc
	src
)

target_link_libraries(appmain amtrs)
