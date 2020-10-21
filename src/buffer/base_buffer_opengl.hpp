#ifndef __VTX_BASE_BUFFER_OPENGL__
#define __VTX_BASE_BUFFER_OPENGL__

#ifdef _MSC_VER
#pragma once
#endif

#include <QOpenGLFunctions_4_5_Core>
#include <iostream>
#include <map>

namespace VTX
{
	namespace Buffer
	{
		class BaseBufferOpenGL
		{
		  public:
			BaseBufferOpenGL( QOpenGLFunctions_4_5_Core * const p_gl ) : _gl( p_gl ) {}
			virtual ~BaseBufferOpenGL() {};

			virtual void generate() = 0;
			virtual void free()		= 0;

			virtual void bind()	  = 0;
			virtual void unbind() = 0;

			inline QOpenGLFunctions_4_5_Core * const gl() { return _gl; }

		  protected:
			QOpenGLFunctions_4_5_Core * _gl;
		};
	} // namespace Buffer
} // namespace VTX
#endif
