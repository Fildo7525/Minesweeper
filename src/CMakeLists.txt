add_library(lib
	Application.cpp
	Application.h
	Board.cpp
	Board.h
	Icon.cpp
	Icon.h
	Image.cpp
	Image.h
	Layer.h
	Status.cpp
	Status.h
	Tile.cpp
	Tile.h
)

target_include_directories(lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
	lib
PUBLIC
	${GLEW_LIBRARIES}
	OpenGL::GL
	glfw
)

