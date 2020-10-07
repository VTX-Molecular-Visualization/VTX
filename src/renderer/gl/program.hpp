#ifndef __VTX_GLSL_PROGRAM__
#define __VTX_GLSL_PROGRAM__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include <QOpenGLFunctions_4_5_Core>
#include <string>

namespace VTX
{
	namespace Renderer
	{
		namespace GLSL
		{
			class Program
			{
			  public:
				Program() = default;
				~Program();

				inline GLuint getId() const { return _id; }

				void create( const std::string & );
				void attachShader( const GLuint );
				void link();
				void detachShaders();
				void use();

			  private:
				GLuint		_id	  = GL_INVALID_INDEX;
				std::string _name = "";

				std::string _getProgramErrors();

				friend class ProgramManager;
			};
		} // namespace GLSL
	}	  // namespace Renderer
} // namespace VTX

#endif
