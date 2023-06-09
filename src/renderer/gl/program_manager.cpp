#include "program_manager.hpp"
#include "define.hpp"
#include "exception.hpp"
#include "tool/logger.hpp"
#include "util/filesystem.hpp"
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

	SHADER_TYPE ProgramManager::getShaderType( const IO::FilePath & p_name )
	{
		std::string extension = p_name.extension();
		if ( ProgramManager::EXTENSIONS.find( extension ) != ProgramManager::EXTENSIONS.end() )
		{
			return ProgramManager::EXTENSIONS.at( extension );
		}

		VTX_WARNING( "Invalid extension: " + extension );
		return SHADER_TYPE::INVALID;
	}

	ProgramManager::~ProgramManager() { dispose(); }

	void ProgramManager::dispose()
	{
		for ( const PairStringToGLuint & pair : _shaders )
		{
			_gl->glDeleteShader( pair.second );
		}

		for ( const PairStringToProgram & pair : _programs )
		{
			delete pair.second;
		}

		_shaders.clear();
		_programs.clear();
	}

	Program * const ProgramManager::createProgram( const std::string &				 p_name,
												   const std::vector<IO::FilePath> & p_shaders,
												   const std::string &				 p_toInject,
												   const std::string &				 p_suffix )
	{
		const std::string name = p_name + p_suffix;
		if ( _programs.find( name ) == _programs.end() )
		{
			_programs[ name ] = new Program( p_shaders, p_toInject );
			Program & program = *_programs[ name ];
			program.create( name );

			for ( const IO::FilePath & shader : p_shaders )
			{
				GLuint id = _createShader( shader, p_toInject, p_suffix );
				if ( id != GL_INVALID_INDEX )
				{
					program.attachShader( id );
				}
			}

			program.link();

			VTX_DEBUG( "Program " + std::to_string( _programs[ name ]->getId() ) + " created: " + p_name );
		}

		return _programs[ name ];
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

	GLuint ProgramManager::_createShader( const IO::FilePath & p_path,
										  const std::string &  p_toInject,
										  const std::string &  p_suffix )
	{
		const std::string name = p_path.filename() + p_suffix;
		VTX_DEBUG( "Creating shader: " + name );

		const SHADER_TYPE type = getShaderType( p_path );
		if ( type == SHADER_TYPE::INVALID )
		{
			VTX_ERROR( "Invalid shader extension: " + name );
			return GL_INVALID_INDEX;
		}

		GLuint shaderId = getShader( name );
		if ( shaderId == GL_INVALID_INDEX )
		{
			shaderId		  = _gl->glCreateShader( (int)type );
			IO::FilePath path = Util::Filesystem::getShadersPath( p_path );
			std::string	 src  = Util::Filesystem::readPath( path );
			if ( src.empty() )
			{
				_gl->glDeleteShader( shaderId );
				return GL_INVALID_INDEX;
			}

			// Handle injection (after #version).
			if ( p_toInject != "" )
			{
				size_t startPosVersion = src.find( "#version" );
				assert( startPosVersion != std::string::npos );
				size_t endPosVersion = src.find( "\n", startPosVersion );
				src.insert( endPosVersion + 1, p_toInject );
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
				IO::FilePath includeAbsolutePath = path.dirpath();
				includeAbsolutePath /= includeRelativePath;
				const std::string srcInclude = Util::Filesystem::readPath( includeAbsolutePath );
				src.replace( startPosInclude, endPosInclude - startPosInclude, srcInclude );
			}

			const GLchar * shaderCode = src.c_str();
			_gl->glShaderSource( shaderId, 1, &shaderCode, 0 );
			_gl->glCompileShader( shaderId );
			GLint compiled;
			_gl->glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled );
			if ( compiled == GL_FALSE )
			{
				std::string error = "Error compiling shader: ";
				error += name;
				error += "\n";
				error += _getShaderErrors( shaderId );
				_gl->glDeleteShader( shaderId );
				VTX_ERROR( error );
				return GL_INVALID_INDEX;
			}
			_shaders.emplace( name, shaderId );
			VTX_DEBUG( "Shader created: " + name );
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
		_gl->glGetShaderiv( p_shader, GL_INFO_LOG_LENGTH, &length );
		if ( length == 0 )
		{
			return "";
		}
		std::vector<GLchar> log( length );
		_gl->glGetShaderInfoLog( p_shader, length, &length, &log[ 0 ] );
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
			_gl->glDeleteShader( pair.second );
		}

		_shaders.clear();

		// Then recreate them.
		for ( const PairStringToProgram & pair : _programs )
		{
			Program * const program = pair.second;
			// Don't need to delete program.
			//_gl->glDeleteProgram( program->getId() );
			// program->setId( _gl->glCreateProgram() );
			for ( const IO::FilePath & shader : program->getShaderPaths() )
			{
				GLuint id = _createShader( shader, program->getToInject() );
				if ( id != GL_INVALID_INDEX )
				{
					program->attachShader( id );
				}
			}

			program->link();
		}
	}
} // namespace VTX::Renderer::GL
