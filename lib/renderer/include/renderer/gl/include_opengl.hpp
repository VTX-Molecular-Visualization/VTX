#ifndef __VTX_RENDERER_GL_INCLUDE_OPENGL__
#define __VTX_RENDERER_GL_INCLUDE_OPENGL__

// TODO: don't test for __APPLE__ here, but for the supported OpenGL version at startup.
// Keep that for debug on CI.
#if __APPLE__
#include <glad/glad41.h>
#define VTX_OPENGL_VERSION 410
#else
#include <glad/glad45.h>
#define VTX_OPENGL_VERSION 450
#endif

#endif
