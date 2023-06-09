add_library(lib
	Application.cpp
	Application.h
	Board.cpp
	Board.h
	Icon.cpp
	Icon.h
	Image.cpp
	Image.h
	Tile.cpp
	Tile.h
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

