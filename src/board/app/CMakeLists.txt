set(libname app)
add_library(${libname}
STATIC
	Application.cpp
	Application.h
	Layer.h
)

target_include_directories(${libname} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
	${libname}
PUBLIC
	${GLEW_LIBRARIES}
	OpenGL::GL
	glfw
)

