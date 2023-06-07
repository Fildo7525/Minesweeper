#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <GL/gl.h>

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
