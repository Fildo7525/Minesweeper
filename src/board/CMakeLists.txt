add_subdirectory(app)
add_subdirectory(image)

set(libname board)
add_library(${libname}
STATIC
	Board.cpp
	Board.h
	Tile.cpp
	Tile.h
)

target_include_directories(${libname} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
	${libname}
PUBLIC
	app
	image
)

