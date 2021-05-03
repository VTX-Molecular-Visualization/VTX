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

			/// TODO: this must not exist. GL refactoring should solve this.
			inline OpenGLFunctions * const getGL() const { return _gl; }

			inline void enableDepthClamp() const { _gl->glEnable( GL_DEPTH_CLAMP ); }
			inline void disableDepthClamp() const { _gl->glDisable( GL_DEPTH_CLAMP ); }
			inline void enableDepthTest() const { _gl->glEnable( GL_DEPTH_TEST ); }
			inline void disableDepthTest() const { _gl->glDisable( GL_DEPTH_TEST ); }
			inline void enableBlend() const
			{
				_gl->glEnable( GL_BLEND );
				_gl->glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			}
			inline void disableBlend() const { _gl->glDisable( GL_BLEND ); }

		  protected:
			OpenGLFunctions * _gl = nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
