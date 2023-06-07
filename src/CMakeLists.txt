add_library(lib
	Application.cpp
	Application.h
	Image.cpp
	Image.h
	Layer.h
)

target_include_directories(lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
	lib
PUBLIC
	${GLEW_LIBRARIES}
	OpenGL::GL
	glfw
)

