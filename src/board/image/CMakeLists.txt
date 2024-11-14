set(libname image)

add_library(${libname}
STATIC
	Icon.cpp
	Icon.h
	IconPool.cpp
	IconPool.h
	Image.cpp
	Image.h
)

target_include_directories(${libname} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

