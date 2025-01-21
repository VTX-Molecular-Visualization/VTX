#include "renderer/context/gl/program_manager.hpp"
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <vector>

namespace VTX::Renderer::Context::GL
{
	const std::map<std::string, ENUM_SHADER_TYPE> ProgramManager::_EXTENSIONS
		= { { ".vert", ENUM_SHADER_TYPE::VERTEX },		 { ".geom", ENUM_SHADER_TYPE::GEOMETRY },
			{ ".frag", ENUM_SHADER_TYPE::FRAGMENT },	 { ".comp", ENUM_SHADER_TYPE::COMPUTE },
			{ ".tesc", ENUM_SHADER_TYPE::TESS_CONTROL }, { ".tese", ENUM_SHADER_TYPE::TESS_EVALUATION } };

	ProgramManager::ProgramManager( const FilePath & p_shaderPath ) : _shaderPath( p_shaderPath ) {}

	ProgramManager::~ProgramManager() { dispose(); }

	ENUM_SHADER_TYPE ProgramManager::getShaderType( const FilePath & p_name )
	{
		std::string extension = p_name.extension().string();
		if ( ProgramManager::_EXTENSIONS.find( extension ) != ProgramManager::_EXTENSIONS.end() )
		{
			return ProgramManager::_EXTENSIONS.at( extension );
		}

		throw GraphicException( "Invalid extension: " + extension );
	}

	void ProgramManager::dispose()
	{
		for ( const auto & pair : _shaders )
		{
			glDeleteShader( pair.second );
		}

		_shaders.clear();
		_programs.clear();
	}

	Program * const ProgramManager::createProgram(
		const std::string &									  p_name,
		const std::variant<FilePath, std::vector<FilePath>> & p_shaders,
		const std::string &									  p_toInject,
		const std::string &									  p_suffix
	)
	{
		const std::string name = p_name + p_suffix;

#ifndef VTX_RENDERER_NO_OPENGL
		std::vector<FilePath> paths;
		if ( std::holds_alternative<FilePath>( p_shaders ) )
		{
			for ( const auto & file :
				  std::filesystem::directory_iterator { _shaderPath / std::get<FilePath>( p_shaders ) } )
			{
				// Filter only supported extensions.
				if ( _EXTENSIONS.find( file.path().extension().string() ) != _EXTENSIONS.end() )
				{
					ProgramManager::getShaderType( file );
					paths.emplace_back( file.path() );
				}
			}
		}
		else
		{
			paths = std::get<std::vector<FilePath>>( p_shaders );
		}

		if ( _programs.find( name ) == _programs.end() )
		{
			_programs[ name ] = std::make_unique<Program>( paths, p_toInject );
			Program & program = *_programs[ name ];
			program.create( name );

			for ( const FilePath & shader : paths )
			{
				uint32_t id = _createShader( shader, p_toInject, p_suffix );
				if ( id != GL_INVALID_INDEX )
				{
					program.attachShader( id );
				}
			}

			assert( glIsProgram( program.getId() ) );

			program.link();

			VTX_TRACE( "Program {} created: {}", _programs[ name ]->getId(), p_name );
		}
#else
		_programs[ name ] = std::make_unique<Program>();
#endif
		return _programs[ name ].get();
	}

	void ProgramManager::deleteProgram( const std::string & p_name )
	{
		if ( _programs.find( p_name ) == _programs.end() )
		{
			VTX_WARNING( "Program {} does not exists", p_name );
			return;
		}

		_programs.erase( p_name );
	}

	Program * const ProgramManager::getProgram( const std::string & p_name )
	{
		if ( _programs.find( p_name ) != _programs.end() )
		{
			return _programs.at( p_name ).get();
		}

		VTX_ERROR( "Program {} does not exists", p_name );
		return nullptr;
	}

	uint32_t ProgramManager::_createShader(
		const FilePath &	p_path,
		const std::string & p_toInject,
		const std::string & p_suffix
	)
	{
		const std::string name = p_path.filename().string() + p_suffix;

		const ENUM_SHADER_TYPE type = getShaderType( p_path );

		uint32_t shaderId = getShader( name );
		if ( shaderId == GL_INVALID_INDEX )
		{
			VTX_TRACE( "Creating shader: {}", name );

			shaderId		 = glCreateShader( (int)type );
			FilePath	path = p_path.is_relative() ? _shaderPath / p_path : p_path;
			std::string src	 = Util::Filesystem::readPath( path );
			if ( src.empty() )
			{
				glDeleteShader( shaderId );
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
				FilePath includeAbsolutePath = path.parent_path();
				includeAbsolutePath /= includeRelativePath;
				const std::string srcInclude = Util::Filesystem::readPath( includeAbsolutePath );
				src.replace( startPosInclude, endPosInclude - startPosInclude, srcInclude );
			}

			const char * shaderCode = src.c_str();
			glShaderSource( shaderId, 1, &shaderCode, 0 );
			glCompileShader( shaderId );
			int32_t compiled;
			glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled );
			if ( compiled == GL_FALSE )
			{
				std::string error = "Error compiling shader: ";
				error += name;
				error += "\n";
				error += _getShaderErrors( shaderId );
				glDeleteShader( shaderId );
				throw GraphicException( error );
			}

			assert( glIsShader( shaderId ) );

			assert( _shaders.find( name ) == _shaders.end() );
			_shaders.emplace( name, shaderId );

			VTX_TRACE( "Shader {} created: {}", shaderId, name );
		}

		return shaderId;
	}

	uint32_t ProgramManager::getShader( const std::string & p_name ) const
	{
		if ( _shaders.find( p_name ) != _shaders.end() )
		{
			return _shaders.at( p_name );
		}

		return GL_INVALID_INDEX;
	}

	std::string ProgramManager::_getShaderErrors( const uint32_t p_shader )
	{
		int32_t length;
		glGetShaderiv( p_shader, GL_INFO_LOG_LENGTH, &length );
		if ( length == 0 )
		{
			return "";
		}
		std::vector<char> log( length );
		glGetShaderInfoLog( p_shader, length, &length, &log[ 0 ] );
		return std::string( log.begin(), log.end() );
	}

	void ProgramManager::compileShaders()
	{
		for ( const auto & pair : _programs )
		{
			pair.second->detachShaders();
		}

		// Delete shaders.
		for ( const auto & pair : _shaders )
		{
			glDeleteShader( pair.second );
		}

		_shaders.clear();

		// Then recreate them.
		for ( const auto & pair : _programs )
		{
			for ( const FilePath & shader : pair.second->getShaderPaths() )
			{
				uint32_t id = _createShader( shader, pair.second->getToInject() );
				if ( id != GL_INVALID_INDEX )
				{
					pair.second->attachShader( id );
				}
			}

			pair.second->link();
		}
	}

	// 	Buffer * const ProgramManager::createBuffer( const std::string & p_name )
	// 	{
	// 		if ( _buffers.find( p_name ) == _buffers.end() )
	// 		{
	// 			_buffers[ p_name ] = std::make_unique<Buffer>();
	// 		}
	//
	// 		return _buffers[ p_name ].get();
	// 	}
	//
	// 	void ProgramManager::deleteBuffer( const std::string & p_name )
	// 	{
	// 		if ( _buffers.find( p_name ) == _buffers.end() )
	// 		{
	// 			VTX_WARNING( "Buffer {} does not exists", p_name );
	// 			return;
	// 		}
	//
	// 		_buffers.erase( p_name );
	// 	}
	//
	// 	Buffer * const ProgramManager::getBuffer( const std::string & p_name )
	// 	{
	// 		if ( _buffers.find( p_name ) != _buffers.end() )
	// 		{
	// 			return _buffers.at( p_name ).get();
	// 		}
	//
	// 		VTX_ERROR( "Program {} does not exists", p_name );
	// 		return nullptr;
	// 	}
} // namespace VTX::Renderer::Context::GL
