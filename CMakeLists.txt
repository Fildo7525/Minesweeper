cmake_minimum_required(VERSION 3.22.1)
project(HalloWorld VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# Dear ImGui
set(IMGUI_DIR ../imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCES
	main.cpp
)

set(IM_GUI_FILES
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
)

add_subdirectory(src)

add_executable(${PROJECT_NAME}
	${SOURCES}
	${IM_GUI_FILES}
)

target_link_libraries(
	${PROJECT_NAME}
PUBLIC
	lib
)
