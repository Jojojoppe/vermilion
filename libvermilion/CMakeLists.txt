cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# LIBVERMILION_CORE
# -----------------
add_library(vermilion_core SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/instance.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/src/logger.cpp
)
target_include_directories(vermilion_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include	
)

# LIBVERMILION_WINDOW
# -------------------

if(WIN32)
	add_compile_definitions(VMWINDOW_WIN32)

	find_package(glfw3)

	if(glfw3_FOUND)
		message("-- GLFW system found, add to source tree")
		add_compile_definitions(VMWINDOW_UNIX_GLFW)
		set(VERMILION_WINDOW_LIBS ${VERMILION_WINDOW_LIBS} glfw)
	endif()

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
	endif()
	
endif()
if(APPLE)
	add_compile_definitions(VMWINDOW_APPLE)

	message("-- MacOS is not yet supported")

endif()

add_library(vermilion_window SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/window/src/window.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/window/src/platform/unix_glfw/window.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/window/src/platform/unix_X11/window.cpp
)
target_include_directories(vermilion_window PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include	
)
target_link_libraries(vermilion_window ${VERMILION_WINDOW_LIBS} vermilion_core)
