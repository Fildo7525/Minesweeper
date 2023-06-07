add_library(lib
	Minesweeper.cpp
	Minesweeper.h
)

target_include_directories(lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

