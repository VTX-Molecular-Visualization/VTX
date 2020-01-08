#include "glsl_program_manager.hpp"
#include "../defines.hpp"
#include "../exceptions.hpp"
#include <vector>

namespace VTX
{
	namespace Shader
	{
		const GLSLProgramManager::MapStringToEnum GLSLProgramManager::EXTENSIONS
			= MapStringToEnum( { { "vert", SHADER_TYPE::VERTEX },
								 { "geom", SHADER_TYPE::GEOMETRY },
								 { "frag", SHADER_TYPE::FRAGMENT },
								 { "comp", SHADER_TYPE::COMPUTE },
								 { "tesc", SHADER_TYPE::TESS_CONTROL },
								 { "tese", SHADER_TYPE::TESS_EVALUATION } } );

		SHADER_TYPE GLSLProgramManager::getShaderType( const std::string & p_name )
		{
			IO::Path path( p_name );

			if ( GLSLProgramManager::EXTENSIONS.find( path.getExtension() ) != GLSLProgramManager::EXTENSIONS.end() )
			{ return GLSLProgramManager::EXTENSIONS.at( path.getExtension() ); }

			VTX_WARNING( "Invalid extension: " + p_name );
			return SHADER_TYPE::INVALID;
		}

		GLSLProgramManager::~GLSLProgramManager()
		{
			for ( const PairStringToGLuint pair : _shaders )
			{
				glDeleteShader( pair.second );
			}
		}

		GLSLProgram * const GLSLProgramManager::createProgram( const std::string & p_name )
		{
			VTX_DEBUG( "Creating program: " + p_name );

			if ( _programs.find( p_name ) == _programs.end() )
			{
				_programs[ p_name ] = GLSLProgram();
				_programs[ p_name ].create( p_name );
			}
			else
			{
				VTX_WARNING( "Program " + p_name + " already exists" );
			}

			VTX_DEBUG( "Program created: " + p_name );
			return &( _programs[ p_name ] );
		}

		void GLSLProgramManager::deleteProgram( const std::string & p_name )
		{
			_programs.erase( p_name );

			if ( _programs.find( p_name ) == _programs.end() )
			{
				VTX_WARNING( "Program " + p_name + " does not exists" );
				return;
			}
		}

		GLSLProgram * const GLSLProgramManager::getProgram( const std::string & p_name )
		{
			if ( _programs.find( p_name ) != _programs.end() ) { return &( _programs.at( p_name ) ); }

			VTX_ERROR( "Program " + p_name + " does not exists" );
			return nullptr;
		}

		GLuint GLSLProgramManager::createShader( const IO::Path & p_path )
		{
			VTX_DEBUG( "Creating shader: " + p_path.getFileName() );

			const std::string name = p_path.getFileName();
			const SHADER_TYPE type = getShaderType( name );
			if ( type == SHADER_TYPE::INVALID )
			{
				VTX_ERROR( "Invalid shader extension: " + name );
				return GL_INVALID_INDEX;
			}

			GLuint shaderId = getShader( name );
			if ( shaderId == GL_INVALID_INDEX )
			{
				shaderId			  = glCreateShader( (int)type );
				const std::string str = IO::Path::read( SHADER_DIR + p_path.str() );

				if ( str.empty() )
				{
					glDeleteShader( shaderId );
					return GL_INVALID_INDEX;
				}

				const GLchar * shaderCode = str.c_str();
				glShaderSource( shaderId, 1, &shaderCode, 0 );
				glCompileShader( shaderId );
				GLint compiled;
				glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled );
				if ( compiled == GL_FALSE )
				{
					std::string error = "Error compiling shader: ";
					error += name;
					error += "\n";
					error += _getShaderErrors( shaderId );
					glDeleteShader( shaderId );
					VTX_ERROR( error );
					return GL_INVALID_INDEX;
				}
			}
			else
			{
				VTX_WARNING( "Shader already exists" );
			}

			VTX_DEBUG( "Shader created: " + name );

			return shaderId;
		}

		GLuint GLSLProgramManager::getShader( const std::string & p_name ) const
		{
			if ( _shaders.find( p_name ) != _shaders.end() ) { return _shaders.at( p_name ); }

			return GL_INVALID_INDEX;
		}

		std::string GLSLProgramManager::_getShaderErrors( const GLuint p_shader ) const
		{
			GLint length;
			glGetShaderiv( p_shader, GL_INFO_LOG_LENGTH, &length );
			if ( length == 0 ) { return ""; }
			std::vector<GLchar> log( length );
			glGetShaderInfoLog( p_shader, length, &length, &log[ 0 ] );
			return std::string( log.begin(), log.end() );
		}

	} // namespace Shader
} // namespace VTX
