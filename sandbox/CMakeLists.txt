cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(sandbox
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp	
	${CMAKE_CURRENT_SOURCE_DIR}/src/gui.cpp	

	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui_widgets.cpp
)
target_include_directories(sandbox PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../libvermilion/include	
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui
)
target_link_libraries(sandbox
	vermilion_core
)
