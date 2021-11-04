#ifndef __VTX_BASE_OPENGL__
#define __VTX_BASE_OPENGL__

// To change openGL version: just change these lines.
#include <QOpenGLFunctions_4_5_Core>
using OpenGLFunctions				= QOpenGLFunctions_4_5_Core;
constexpr uint OPENGL_MAJOR_VERSION = 4;
constexpr uint OPENGL_MINOR_VERSION = 5;

namespace VTX
{
	namespace Generic
	{
		class BaseOpenGL
		{
		  public:
			BaseOpenGL()		  = default;
			virtual ~BaseOpenGL() = default;

			static inline OpenGLFunctions * const getGL() { return _gl; }

			inline void enableDepthClamp() const { _gl->glEnable( GL_DEPTH_CLAMP ); }
			inline void disableDepthClamp() const { _gl->glDisable( GL_DEPTH_CLAMP ); }
			inline void enableDepthTest() const { _gl->glEnable( GL_DEPTH_TEST ); }
			inline void disableDepthTest() const { _gl->glDisable( GL_DEPTH_TEST ); }

		  protected:
			static inline OpenGLFunctions * _gl = nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
