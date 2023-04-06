#include "program_manager.hpp"
#include "define.hpp"
#include <vector>

namespace VTX::Renderer::GL
{
	const ProgramManager::MapStringToEnum ProgramManager::EXTENSIONS
		= MapStringToEnum( { { "vert", SHADER_TYPE::VERTEX },
							 { "geom", SHADER_TYPE::GEOMETRY },
							 { "frag", SHADER_TYPE::FRAGMENT },
							 { "comp", SHADER_TYPE::COMPUTE },
							 { "tesc", SHADER_TYPE::TESS_CONTROL },
							 { "tese", SHADER_TYPE::TESS_EVALUATION } } );

	SHADER_TYPE ProgramManager::getShaderType( const Util::FilePath & p_name )
	{
		std::string extension = p_name.extension();
		if ( ProgramManager::EXTENSIONS.find( extension ) != ProgramManager::EXTENSIONS.end() )
		{
			return ProgramManager::EXTENSIONS.at( extension );
		}

		// VTX_WARNING( "Invalid extension: " + extension );
		return SHADER_TYPE::INVALID;
	}

	ProgramManager::~ProgramManager() { dispose(); }

	void ProgramManager::dispose()
	{
		for ( const PairStringToGLuint & pair : _shaders )
		{
			glDeleteShader( pair.second );
		}

		for ( const PairStringToProgram & pair : _programs )
		{
			delete pair.second;
		}

		_shaders.clear();
		_programs.clear();
	}

	Program * const ProgramManager::createProgram( const std::string &				   p_name,
												   const std::vector<Util::FilePath> & p_shaders )
	{
		// VTX_DEBUG( "Creating program: " + p_name );

		if ( _programs.find( p_name ) == _programs.end() )
		{
			_programs[ p_name ] = new Program( p_shaders );
			Program & program	= *_programs[ p_name ];
			program.create( p_name );

			for ( const Util::FilePath & shader : p_shaders )
			{
				GLuint id = _createShader( shader );
				if ( id != GL_INVALID_INDEX )
				{
					program.attachShader( id );
				}
			}

			program.link();

			// VTX_DEBUG( "Program " + std::to_string( _programs[ p_name ]->getId() ) + " created: " + p_name );
		}
		else
		{
			// VTX_DEBUG( "Program " + p_name + " already exists" );
		}

		return _programs[ p_name ];
	}

	void ProgramManager::deleteProgram( const std::string & p_name )
	{
		if ( _programs.find( p_name ) == _programs.end() )
		{
			// VTX_WARNING( "Program " + p_name + " does not exists" );
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

		// VTX_ERROR( "Program " + p_name + " does not exists" );
		return nullptr;
	}

	GLuint ProgramManager::_createShader( const Util::FilePath & p_path )
	{
		const std::string name = p_path.filename();
		// VTX_DEBUG( "Creating shader: " + name );

		const SHADER_TYPE type = getShaderType( name );
		if ( type == SHADER_TYPE::INVALID )
		{
			// VTX_ERROR( "Invalid shader extension: " + name );
			return GL_INVALID_INDEX;
		}

		GLuint shaderId = getShader( name );
		if ( shaderId == GL_INVALID_INDEX )
		{
			shaderId = glCreateShader( (int)type );
			// IO::FilePath path = Util::Filesystem::getShadersPath( p_path );
			std::string src = ""; // Util::Filesystem::readPath(p_path);
			if ( src.empty() )
			{
				glDeleteShader( shaderId );
				return GL_INVALID_INDEX;
			}

			// Handle #include.
			while ( true )
			{
				size_t startPosInclude = src.find( "#include" );
				if ( startPosInclude == std::string::npos )
				{
					break;
				}

				size_t		endPosInclude		= src.find( "\n", startPosInclude );
				std::string includeRelativePath = src.substr( startPosInclude, endPosInclude - startPosInclude );
				size_t		startPosPath		= includeRelativePath.find( '"' );
				size_t		endPosPath			= includeRelativePath.find( '"', startPosPath + 1 );
				includeRelativePath = includeRelativePath.substr( startPosPath + 1, endPosPath - startPosPath - 1 );
				Util::FilePath includeAbsolutePath = p_path.dirpath();
				includeAbsolutePath /= includeRelativePath;
				const std::string srcInclude = ""; // Util::Filesystem::readPath(includeAbsolutePath);
				src.replace( startPosInclude, endPosInclude - startPosInclude, srcInclude );
			}

			const GLchar * shaderCode = src.c_str();
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
				// VTX_ERROR( error );
				return GL_INVALID_INDEX;
			}
			_shaders.emplace( name, shaderId );
			// VTX_DEBUG( "Shader created: " + name );
		}
		else
		{
			// VTX_DEBUG( "Shader already exists" );
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
		glGetShaderiv( p_shader, GL_INFO_LOG_LENGTH, &length );
		if ( length == 0 )
		{
			return "";
		}
		std::vector<GLchar> log( length );
		glGetShaderInfoLog( p_shader, length, &length, &log[ 0 ] );
		return std::string( log.begin(), log.end() );
	}

	void ProgramManager::refreshShaders()
	{
		for ( const PairStringToProgram & pair : _programs )
		{
			Program * const program = pair.second;
			program->detachShaders();
		}

		// Delete shaders.
		for ( const PairStringToGLuint & pair : _shaders )
		{
			glDeleteShader( pair.second );
		}

		_shaders.clear();

		// Then recreate them.
		for ( const PairStringToProgram & pair : _programs )
		{
			Program * const program = pair.second;
			// Don't need to delete program.
			// glDeleteProgram( program->getId() );
			// program->setId( glCreateProgram() );
			for ( const Util::FilePath & shader : program->getShaderPaths() )
			{
				GLuint id = _createShader( shader );
				if ( id != GL_INVALID_INDEX )
				{
					program->attachShader( id );
				}
			}

			program->link();
		}
	}
} // namespace VTX::Renderer::GL
