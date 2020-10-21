#ifndef __VTX_BASE_BUFFER_OPENGL__
#define __VTX_BASE_BUFFER_OPENGL__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_opengl.hpp"

namespace VTX
{
	namespace Buffer
	{
		class BaseBufferOpenGL : public Generic::BaseOpenGL
		{
		  public:
			BaseBufferOpenGL( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) {}
			virtual ~BaseBufferOpenGL() {};

			virtual void generate() = 0;
			virtual void free()		= 0;

			virtual void bind()	  = 0;
			virtual void unbind() = 0;
		};
	} // namespace Buffer
} // namespace VTX
#endif
