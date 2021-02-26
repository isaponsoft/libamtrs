if(AMTRS_VULKAN_ENABLE OR AMTRS_GRAPHICS_ALL_ENABLE)

find_package(Vulkan REQUIRED)
target_include_directories(amtrs PUBLIC ${Vulkan_INCLUDE_DIR})
target_link_libraries(amtrs ${Vulkan_LIBRARIES})

endif()
