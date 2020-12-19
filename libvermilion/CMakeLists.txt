cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

# LIBVERMILION_CORE
# -----------------

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
	set(VERMILION_CORE_LIBS ${VERMILION_CORE_LIBS} ${Vulkan_LIBRARIES})
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

add_library(vermilion_core SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/logger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/window.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/api.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/instance.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/opengl/api.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/api.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/vulkan/validationlayers.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/glfw/window.cpp

	${GLAD_SOURCES}
)
target_include_directories(vermilion_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include	
	${VERMILION_CORE_INCLUDE_DIRS}
)
target_link_libraries(vermilion_core ${VERMILION_CORE_LIBS})
