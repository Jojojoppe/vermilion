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
endif()

find_package(Vulkan)
if(Vulkan_FOUND)
	message("-- Vulkan found, add to source tree")
	add_compile_definitions(VMCORE_VULKAN)
	set(VERMILION_CORE_LIBS ${VERMILION_CORE_LIBS} ${Vulkan_LIBRARIES})
	set(VERMILION_CORE_INCLUDE_DIRS ${VERMILION_CORE_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
endif()

add_library(vermilion_core SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/instance.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/logger.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/core/src/platform/opengl/context.cpp
)
target_include_directories(vermilion_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include	
	${VERMILION_CORE_INCLUDE_DIRS}
)
target_link_libraries(vermilion_core ${VERMILION_CORE_LIBS})

# LIBVERMILION_WINDOW
# -------------------

if(WIN32)
	add_compile_definitions(VMWINDOW_WIN32)

	message("-- GLFW system being build, add to source tree")
	add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/glfw3 ${PROJECT_BINARY_DIR}/third_party/glfw3)
	set(VERMILION_WINDOW_INCLUDE_DIRS ${VERMILION_WINDOW_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/third_party/glfw3/include)
	add_compile_definitions(VMWINDOW_WIN32_GLFW)
	set(VERMILION_WINDOW_LIBS ${VERMILION_WINDOW_LIBS} glfw3)

endif()
if(UNIX AND NOT APPLE)
	add_compile_definitions(VMWINDOW_UNIX)

	find_package(X11)
	find_package(glfw3)

	if(X11_FOUND)
		message("-- X11 system found, add to source tree")
		add_compile_definitions(VMWINDOW_UNIX_X11)
	endif()

	if(glfw3_FOUND)
		message("-- GLFW system found, add to source tree")
		add_compile_definitions(VMWINDOW_UNIX_GLFW)
		set(VERMILION_WINDOW_LIBS ${VERMILION_WINDOW_LIBS} glfw)
	else()
		message("-- GLFW system being build, add to source tree")
		add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/glfw3 ${PROJECT_BINARY_DIR}/third_party/glfw3)
		add_compile_definitions(VMWINDOW_UNIX_GLFW3)
	endif()
	
endif()
if(APPLE)
	add_compile_definitions(VMWINDOW_APPLE)

	message("-- MacOS is not yet supported")

endif()

add_library(vermilion_window SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/window/src/window.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/window/src/platform/unix_glfw/window.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/window/src/platform/windows_glfw/window.cpp
)
target_include_directories(vermilion_window PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include	
	${VERMILION_WINDOW_INCLUDE_DIRS}
)
target_link_libraries(vermilion_window ${VERMILION_WINDOW_LIBS} vermilion_core)
