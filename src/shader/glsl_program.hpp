#ifndef __VTX_GLSL_PROGRAM__
#define __VTX_GLSL_PROGRAM__

#include "../io/path.hpp"
#include <GL/gl3w.h>
#include <string>

namespace VTX
{
	namespace Shader
	{
		class GLSLProgram
		{
		  public:
			GLSLProgram() = default;
			/*
			~GLSLProgram();

			inline GLuint getId() const;
			inline void	  use() const;

			void create( const std::string & name );
			void attachShader( const GLuint shaderId ) const;
			void attachShader( const IO::Path & shaderFilePath ) const;
			void link();

			void		detachShaders();
			std::string getInfoLog() const;

		  private:
			std::string m_name = UNDEFINED_STRING;

			GLuint m_id = GL_INVALID_INDEX;
			friend class GLSLProgramManager;
			*/
		};
	}; // namespace Shader
} // namespace VTX

#endif
