#ifndef __VTX_GLSL_PROGRAM_MANAGER__
#define __VTX_GLSL_PROGRAM_MANAGER__

#include "glsl_program.hpp"
#include <map>

namespace VTX
{
	namespace Shader
	{
		enum SHADER_TYPE
		{
			VERTEX			= GL_VERTEX_SHADER,
			FRAGMENT		= GL_FRAGMENT_SHADER,
			GEOMETRY		= GL_GEOMETRY_SHADER,
			COMPUTE			= GL_COMPUTE_SHADER,
			TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
			TESS_CONTROL	= GL_TESS_CONTROL_SHADER,
			INVALID			= GL_INVALID_VALUE
		};

		class GLSLProgramManager
		{
		  public:
			GLSLProgramManager() = default;
			~GLSLProgramManager();
			GLSLProgram * const createProgram( const std::string & );
			void				deleteProgram( const std::string & );
			GLSLProgram * const getProgram( const std::string & );
			GLuint				createShader( const IO::Path & );
			GLuint				getShader( const std::string & ) const;

			static const std::map<std::string, Shader::SHADER_TYPE> EXTENSIONS;
			static SHADER_TYPE										getShaderType( const std::string & );

		  private:
			GLuint									   _currentProgram = GL_INVALID_INDEX;
			std::map<std::string, Shader::GLSLProgram> _programs	   = std::map<std::string, Shader::GLSLProgram>();
			std::map<std::string, GLuint>			   _shaders		   = std::map<std::string, GLuint>();

			std::string _getShaderErrors( const GLuint ) const;
		};
	}; // namespace Shader
} // namespace VTX

#endif
