#ifndef __VTX_GLSL_PROGRAM__
#define __VTX_GLSL_PROGRAM__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include <QOpenGLFunctions_4_5_Compatibility>
#include <string>

namespace VTX
{
	namespace Renderer
	{
		namespace GLSL
		{
			class Program : public QOpenGLFunctions_4_5_Compatibility
			{
			  public:
				Program() = default;
				~Program();

				inline GLuint getId() const { return _id; }
				inline void	  use() { glUseProgram( _id ); }

				void create( const std::string & );
				void attachShader( const GLuint );
				void link();

				void detachShaders();

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
