if(AMTRS_OPENGL_ENABLE OR AMTRS_GRAPHICS_ALL_ENABLE)

find_package(OpenGL QUIET)
if(NOT OpenGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
	target_link_libraries(amtrs ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
endif()

endif()
