cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(sandbox
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp	
)
target_include_directories(sandbox PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../libvermilion/include	
)
target_link_libraries(sandbox
	${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libvermilion_core.so
	${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libvermilion_window.so
)
