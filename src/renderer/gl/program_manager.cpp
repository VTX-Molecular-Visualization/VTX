#include "program_manager.hpp"
#include "define.hpp"
#include "exception.hpp"
#include "generic/factory.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		namespace GLSL
		{
			const ProgramManager::MapStringToEnum ProgramManager::EXTENSIONS = MapStringToEnum( { { ".vert", SHADER_TYPE::VERTEX },
																								  { ".geom", SHADER_TYPE::GEOMETRY },
																								  { ".frag", SHADER_TYPE::FRAGMENT },
																								  { ".comp", SHADER_TYPE::COMPUTE },
																								  { ".tesc", SHADER_TYPE::TESS_CONTROL },
																								  { ".tese", SHADER_TYPE::TESS_EVALUATION } } );

			SHADER_TYPE ProgramManager::getShaderType( const Path & p_name )
			{
				std::string extension = p_name.extension().string();
				if ( ProgramManager::EXTENSIONS.find( extension ) != ProgramManager::EXTENSIONS.end() )
				{
					return ProgramManager::EXTENSIONS.at( extension );
				}

				VTX_WARNING( "Invalid extension: " + extension );
				return SHADER_TYPE::INVALID;
			}

			ProgramManager::~ProgramManager()
			{
				for ( const PairStringToGLuint & pair : _shaders )
				{
					OGL().glDeleteShader( pair.second );
				}

				Generic::clearMapAsValue( _programs );
				_shaders.clear();
				_programs.clear();
			}

			Program * const ProgramManager::createProgram( const std::string & p_name, const std::vector<std::string> & p_shaders )
			{
				VTX_DEBUG( "Creating program: " + p_name );

				if ( _programs.find( p_name ) == _programs.end() )
				{
					_programs[ p_name ] = new Program();
					Program & program	= *_programs[ p_name ];
					program.create( p_name );

					for ( const std::string & shader : p_shaders )
					{
						program.attachShader( _createShader( shader ) );
					}

					program.link();

					VTX_DEBUG( "Program " + std::to_string( _programs[ p_name ]->getId() ) + " created: " + p_name );
				}
				else
				{
					VTX_DEBUG( "Program " + p_name + " already exists" );
				}

				return _programs[ p_name ];
			}

			void ProgramManager::deleteProgram( const std::string & p_name )
			{
				if ( _programs.find( p_name ) == _programs.end() )
				{
					VTX_WARNING( "Program " + p_name + " does not exists" );
					return;
				}

				_programs.erase( p_name );
			}

			Program * const ProgramManager::getProgram( const std::string & p_name )
			{
				if ( _programs.find( p_name ) != _programs.end() )
				{
					return _programs.at( p_name );
				}

				VTX_ERROR( "Program " + p_name + " does not exists" );
				return nullptr;
			}

			GLuint ProgramManager::_createShader( const Path & p_path )
			{
				VTX_DEBUG( "Creating shader: " + p_path.filename().string() );

				const std::string name = p_path.filename().string();
				const SHADER_TYPE type = getShaderType( name );
				if ( type == SHADER_TYPE::INVALID )
				{
					VTX_ERROR( "Invalid shader extension: " + name );
					return GL_INVALID_INDEX;
				}

				GLuint shaderId = getShader( name );
				if ( shaderId == GL_INVALID_INDEX )
				{
					shaderId			   = OGL().glCreateShader( (int)type );
					Path			  path = Util::Filesystem::getShadersPath( p_path.string() );
					const std::string src  = Util::Filesystem::readPath( path );
					if ( src.empty() )
					{
						OGL().glDeleteShader( shaderId );
						return GL_INVALID_INDEX;
					}

					const GLchar * shaderCode = src.c_str();
					OGL().glShaderSource( shaderId, 1, &shaderCode, 0 );
					OGL().glCompileShader( shaderId );
					GLint compiled;
					OGL().glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled );
					if ( compiled == GL_FALSE )
					{
						std::string error = "Error compiling shader: ";
						error += name;
						error += "\n";
						error += _getShaderErrors( shaderId );
						OGL().glDeleteShader( shaderId );
						VTX_ERROR( error );
						return GL_INVALID_INDEX;
					}
					VTX_DEBUG( "Shader created: " + name );
				}
				else
				{
					VTX_DEBUG( "Shader already exists" );
				}

				return shaderId;
			}

			GLuint ProgramManager::getShader( const std::string & p_name ) const
			{
				if ( _shaders.find( p_name ) != _shaders.end() )
				{
					return _shaders.at( p_name );
				}

				return GL_INVALID_INDEX;
			}

			std::string ProgramManager::_getShaderErrors( const GLuint p_shader )
			{
				GLint length;
				OGL().glGetShaderiv( p_shader, GL_INFO_LOG_LENGTH, &length );
				if ( length == 0 )
				{
					return "";
				}
				std::vector<GLchar> log( length );
				OGL().glGetShaderInfoLog( p_shader, length, &length, &log[ 0 ] );
				return std::string( log.begin(), log.end() );
			}
		} // namespace GLSL
	}	  // namespace Renderer
} // namespace VTX
