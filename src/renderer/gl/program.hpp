#ifndef __VTX_GLSL_PROGRAM__
#define __VTX_GLSL_PROGRAM__

#ifdef _MSC_VER
#pragma once
#endif


#include <GL/gl3w.h>
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
				inline void	  use() const { glUseProgram( _id ); }

				void create( const std::string & );
				void attachShader( const GLuint ) const;
				void link();

				void detachShaders();

			  private:
				GLuint		_id	  = GL_INVALID_INDEX;
				std::string _name = "";

				std::string _getProgramErrors() const;

				friend class ProgramManager;
			};
		} // namespace GLSL
	} // namespace Renderer
} // namespace VTX

#endif
