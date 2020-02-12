#ifndef __VTX_GLSL_PROGRAM_MANAGER__
#define __VTX_GLSL_PROGRAM_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "program.hpp"
#include <map>

namespace VTX
{
	namespace Renderer
	{
		namespace GLSL
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

			class ProgramManager
			{
			  public:
				using MapStringToEnum	  = std::map<std::string, SHADER_TYPE>;
				using MapStringToProgram  = std::map<std::string, Program>;
				using PairStringToProgram = std::pair<std::string, Program>;
				using MapStringToGLuint	  = std::map<std::string, GLuint>;
				using PairStringToGLuint  = std::pair<std::string, GLuint>;

				ProgramManager() = default;
				~ProgramManager();
				Program * const createProgram( const std::string & );
				void			deleteProgram( const std::string & );
				Program * const getProgram( const std::string & );
				GLuint			createShader( const IO::Path & );
				GLuint			getShader( const std::string & ) const;

				static const MapStringToEnum EXTENSIONS;
				static SHADER_TYPE			 getShaderType( const std::string & );

			  private:
				MapStringToProgram _programs = MapStringToProgram();
				MapStringToGLuint  _shaders	 = MapStringToGLuint();

				std::string _getShaderErrors( const GLuint ) const;
			};
		} // namespace GLSL
	} // namespace Renderer
} // namespace VTX

#endif
