cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(example_triangle
	${CMAKE_CURRENT_SOURCE_DIR}/src/example_triangle.cpp	
)
target_include_directories(example_triangle PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../libvermilion/include	
)
target_link_libraries(example_triangle
	vermilion_core
)

add_executable(example_uniform
	${CMAKE_CURRENT_SOURCE_DIR}/src/example_uniform.cpp	
)
target_include_directories(example_uniform PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../libvermilion/include	
)
target_link_libraries(example_uniform
	vermilion_core
)

add_executable(example_uniform_buffer
	${CMAKE_CURRENT_SOURCE_DIR}/src/example_uniform_buffer.cpp	
)
target_include_directories(example_uniform_buffer PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../libvermilion/include	
)
target_link_libraries(example_uniform_buffer
	vermilion_core
)