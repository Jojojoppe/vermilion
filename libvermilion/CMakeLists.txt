cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

# LIBVERMILION_CORE
# -----------------

add_compile_definitions(__VERMILION_INTERNAL__)

find_package(OpenGL)
if(OPENGL_FOUND)
	message("-- OpenGL found, add to source tree")
	add_compile_definitions(VMCORE_OPENGL)
	set(VERMILION_CORE_LIBS ${VERMILION_CORE_LIBS} OpenGL::GL)
	# Add glad
	aux_source_directory(${CMAKE_SOURCE_DIR}/third_party/glad/src GLAD_SOURCES)
	set(VERMILION_CORE_INCLUDE_DIRS ${VERMILION_CORE_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/third_party/glad/include)
	set(VERMILION_CORE_EXTRA_SRC ${VERMILION_CORE_EXTRA_SRC} ${GLAD_SOURCES})
endif()

find_package(Vulkan)
if(Vulkan_FOUND)
	message("-- Vulkan found, add to source tree")
	add_compile_definitions(VMCORE_VULKAN)
	set(VERMILION_CORE_LIBS ${VERMILION_CORE_LIBS} ${Vulkan_LIBRARIES} shaderc_shared)
	set(VERMILION_CORE_INCLUDE_DIRS ${VERMILION_CORE_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
endif()

find_package(glfw3)
if(glfw3_FOUND)
	message("-- GLFW system found, add to source tree")
	add_compile_definitions(VMCORE_GLFW)
	set(VERMILION_CORE_LIBS ${VERMILION_CORE_LIBS} glfw)
else()
	message("-- GLFW system being build, add to source tree")
	add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/glfw3 ${PROJECT_BINARY_DIR}/third_party/glfw3)
	add_compile_definitions(VMCORE_GLFW)
endif()

# Add STB libraries
set(VERMILION_CORE_INCLUDE_DIRS ${VERMILION_CORE_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/third_party/stb)

add_library(vermilion_core SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/logger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/window.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/api.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/instance.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/Buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/stb_image.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/vermilion.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/api.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/RenderTarget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/Shader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/Pipeline.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/Buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/Renderable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/Texture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vkInstance.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vkPhysicalDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vkDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vkSwapChain.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vkImageView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vkRenderPass.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vkFrameBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vkCommandPool.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/vk_mem_alloc.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/glfw/window.cpp

	${GLAD_SOURCES}
)
target_include_directories(vermilion_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include	
	${VERMILION_CORE_INCLUDE_DIRS}
)
target_link_libraries(vermilion_core ${VERMILION_CORE_LIBS})
