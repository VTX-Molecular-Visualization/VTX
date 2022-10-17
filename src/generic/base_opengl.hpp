#ifndef __VTX_BASE_OPENGL__
#define __VTX_BASE_OPENGL__

// To change openGL version: just change these lines.
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>
#include <QSurface>

using OpenGLFunctions				= QOpenGLFunctions_4_5_Core;
using OpenGLContext					= QOpenGLContext;
using OpenGLSurface					= QSurface;
using OpenGLWidget					= QOpenGLWidget;
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

			inline void enableDepthClamp() const { _gl->glEnable( GL_DEPTH_CLAMP ); }
			inline void disableDepthClamp() const { _gl->glDisable( GL_DEPTH_CLAMP ); }
			inline void enableDepthTest() const { _gl->glEnable( GL_DEPTH_TEST ); }
			inline void disableDepthTest() const { _gl->glDisable( GL_DEPTH_TEST ); }

		  protected:
			static inline OpenGLFunctions * _gl		= nullptr;
			static inline OpenGLWidget *	_widget = nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
