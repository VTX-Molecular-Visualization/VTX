#ifndef __VTX_GLSL_PROGRAM_MANAGER__
#define __VTX_GLSL_PROGRAM_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "glsl_program.hpp"
#include <map>

namespace VTX
{
	namespace Shader
	{
		enum class SHADER_TYPE : int
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
			using MapStringToEnum	  = std::map<std::string, Shader::SHADER_TYPE>;
			using MapStringToProgram  = std::map<std::string, Shader::GLSLProgram>;
			using PairStringToProgram = std::pair<std::string, Shader::GLSLProgram>;
			using MapStringToGLuint	  = std::map<std::string, GLuint>;
			using PairStringToGLuint  = std::pair<std::string, GLuint>;

			GLSLProgramManager() = default;
			~GLSLProgramManager();
			GLSLProgram * const createProgram( const std::string & );
			void				deleteProgram( const std::string & );
			GLSLProgram * const getProgram( const std::string & );
			GLuint				createShader( const IO::Path & );
			GLuint				getShader( const std::string & ) const;

			static const MapStringToEnum EXTENSIONS;
			static SHADER_TYPE			 getShaderType( const std::string & );

		  private:
			MapStringToProgram _programs = MapStringToProgram();
			MapStringToGLuint  _shaders	 = MapStringToGLuint();

			std::string _getShaderErrors( const GLuint ) const;
		};
	}; // namespace Shader
} // namespace VTX

#endif
