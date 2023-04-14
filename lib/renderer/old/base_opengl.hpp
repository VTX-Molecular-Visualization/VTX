#ifndef __VTX_BASE_OPENGL__
#define __VTX_BASE_OPENGL__

// To change openGL version: just change these lines.
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>
#include <QSurface>
#include <iostream>

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
			inline void memoryBarrier( const GLbitfield p_barrier ) const { _gl->glMemoryBarrier( p_barrier ); }
			inline void flush() const { _gl->glFlush(); }
			inline void finish() const { _gl->glFinish(); }
			inline void makeContextCurrent() { _context->makeCurrent( _surface ); }
			inline void doneContextCurrent() { _context->doneCurrent(); }

		  protected:
			static inline OpenGLFunctions * _gl		 = nullptr;
			static inline OpenGLContext *	_context = nullptr;
			static inline OpenGLSurface *	_surface = nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
