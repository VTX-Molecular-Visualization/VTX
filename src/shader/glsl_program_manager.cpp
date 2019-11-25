#include "glsl_program_manager.hpp"
#include "../defines.hpp"
#include "../exceptions.hpp"
#include <vector>

namespace VTX
{
	namespace Shader
	{
		const std::map<std::string, Shader::SHADER_TYPE> GLSLProgramManager::EXTENSIONS
			= std::map<std::string, Shader::SHADER_TYPE>( { { ".vert", SHADER_TYPE::VERTEX },
															{ ".geom", SHADER_TYPE::GEOMETRY },
															{ ".frag", SHADER_TYPE::FRAGMENT },
															{ ".comp", SHADER_TYPE::COMPUTE },
															{ ".tesc", SHADER_TYPE::TESS_CONTROL },
															{ ".tese", SHADER_TYPE::TESS_EVALUATION } } );

		SHADER_TYPE GLSLProgramManager::getShaderType( const std::string & p_name )
		{
			IO::Path path( p_name );

			try
			{
				return GLSLProgramManager::EXTENSIONS.at( path.getExtension() );
			}
			catch ( const std::exception )
			{
				return SHADER_TYPE::INVALID;
			}
		}

		GLSLProgramManager::~GLSLProgramManager()
		{
			for ( const auto & [ nname, shader ] : _shaders )
			{
				glDeleteShader( shader );
			}
		}

		GLSLProgram * const GLSLProgramManager::createProgram( const std::string & p_name )
		{
			VTX_INFO( "Create program " + p_name );

			if ( _programs.find( p_name ) == _programs.end() )
			{
				_programs[ p_name ] = GLSLProgram();
				_programs[ p_name ].create( p_name );
			}
			else
			{
				VTX_WARNING( "Program " + p_name + " already exists" );
			}

			return &( _programs[ p_name ] );
		}

		void GLSLProgramManager::deleteProgram( const std::string & p_name )
		{
			try
			{
				_programs.erase( p_name );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Program " + p_name + " does not exists" );
			}
		}

		GLSLProgram * const GLSLProgramManager::getProgram( const std::string & p_name )
		{
			try
			{
				return &( _programs.at( p_name ) );
			}
			catch ( const std::exception )
			{
				VTX_ERROR( "Program " + p_name + " does not exists" );
				return nullptr;
			}
		}

		GLuint GLSLProgramManager::createShader( const IO::Path & p_path )
		{
			VTX_INFO( "Creating shader: " + p_path.getFileName() );

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
				shaderId					 = glCreateShader( type );
				const std::string str		 = IO::Path::read( SHADER_DIR + p_path.str() );
				const GLchar *	  shaderCode = str.c_str();
				glShaderSource( shaderId, 1, &shaderCode, 0 );
				glCompileShader( shaderId );
				GLint compiled;
				glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled );
				if ( !compiled )
				{
					std::string error = "Error compiling shader: ";
					error += name;
					error += "\n";
					error += _getShaderErrors( shaderId );
					glDeleteShader( shaderId );
					VTX_ERROR( error );
					return GL_INVALID_INDEX;
				}
				_shaders[ name ] = shaderId;
				VTX_INFO( "Shader compiled" );
			}
			else
			{
				VTX_WARNING( "Shader already exists" );
			}
			return shaderId;
		}

		GLuint GLSLProgramManager::getShader( const std::string & p_name ) const
		{
			try
			{
				return _shaders.at( p_name );
			}
			catch ( const std::exception )
			{
				return GL_INVALID_INDEX;
			}
		}

		std::string GLSLProgramManager::_getShaderErrors( const GLuint p_shader ) const
		{
			GLint length;
			glGetShaderiv( p_shader, GL_INFO_LOG_LENGTH, &length );
			std::vector<GLchar> log( length );
			glGetShaderInfoLog( p_shader, length, &length, &( log[ 0 ] ) );
			return std::string( log.begin(), log.end() );
		}

	} // namespace Shader
} // namespace VTX
