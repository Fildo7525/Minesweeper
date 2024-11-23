set(libname status)
add_library(${libname}
STATIC
	Status.cpp
	Status.h
)

target_include_directories(${libname} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
	${libname}
PUBLIC
	board
	tbb
)

