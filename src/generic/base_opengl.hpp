#ifndef __VTX_BASE_OPENGL__
#define __VTX_BASE_OPENGL__

// To change openGL version: just change these lines.
#include <QOpenGLFunctions_4_5_Core>
using OpenGLFunctions = QOpenGLFunctions_4_5_Core;
constexpr uint OPENGL_MAJOR_VERSION = 4;
constexpr uint OPENGL_MINOR_VERSION = 5;

namespace VTX
{
	namespace Generic
	{
		class BaseOpenGL
		{
		  public:
			BaseOpenGL( OpenGLFunctions * const p_gl ) : _gl( p_gl ) {}
			virtual ~BaseOpenGL() = default;

			/// TODO: this must not exist. GL refactoring should solve this.
			inline OpenGLFunctions * const getGL() const { return _gl; }

			inline void enableDepthClamp() const { _gl->glEnable( GL_DEPTH_CLAMP ); }
			inline void disableDepthClamp() const { _gl->glDisable( GL_DEPTH_CLAMP ); }
			inline void enableDepthTest() const { _gl->glEnable( GL_DEPTH_TEST ); }
			inline void disableDepthTest() const { _gl->glDisable( GL_DEPTH_TEST ); }

		  protected:
			OpenGLFunctions * _gl = nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
