#ifndef __VTX_BASE_OPENGL__
#define __VTX_BASE_OPENGL__

#ifdef _MSC_VER
#pragma once
#endif

// To change openGL version: just change these 2 lines.
#include <QOpenGLFunctions_4_5_Core>
using OpenGLFunctions = QOpenGLFunctions_4_5_Core;

namespace VTX
{
	namespace Generic
	{
		class BaseOpenGL
		{
		  public:
			BaseOpenGL( OpenGLFunctions * const p_gl ) : _gl( p_gl ) {}
			virtual ~BaseOpenGL() = default;

			inline OpenGLFunctions * const gl() { return _gl; }

		  protected:
			OpenGLFunctions * _gl = nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
