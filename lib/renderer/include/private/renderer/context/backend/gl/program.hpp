#ifndef __VTX_RENDERER_CONTEXT_GL_PROGRAM__
#define __VTX_RENDERER_CONTEXT_GL_PROGRAM__

#include "renderer/context/backend/gl/debug.hpp"
#include <array>
#include <ranges>
#include <string_view>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/types.hpp>
#include <utility>
#include <variant>

namespace VTX::Renderer::Context::Backend::GL
{
	enum struct ENUM_SHADER_TYPE : uint16_t
	{
		VERTEX			= GL_VERTEX_SHADER,
		FRAGMENT		= GL_FRAGMENT_SHADER,
		GEOMETRY		= GL_GEOMETRY_SHADER,
		COMPUTE			= GL_COMPUTE_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		TESS_CONTROL	= GL_TESS_CONTROL_SHADER,
		INVALID			= GL_INVALID_ENUM
	};

	class Program
	{
	  public:
		Program(
			const std::string &									  p_name,
			const FilePath &									  p_rootPath,
			const std::variant<FilePath, std::vector<FilePath>> & p_shaderPaths,
			const std::string &									  p_toInject = "",
			const std::string &									  p_suffix	 = ""
		) : _toInject( p_toInject )
		{
			const std::string name = p_name + p_suffix;

			std::vector<FilePath> paths;
			if ( std::holds_alternative<FilePath>( p_shaderPaths ) )
			{
				for ( const auto & file :
					  std::filesystem::directory_iterator { p_rootPath / std::get<FilePath>( p_shaderPaths ) } )
				{
					// Filter only supported extensions.
					if ( getShaderType( file ) != ENUM_SHADER_TYPE::INVALID )
					{
						paths.emplace_back( file.path() );
					}
				}
			}
			else
			{
				paths = std::get<std::vector<FilePath>>( p_shaderPaths );
			}

			create( name );

			for ( const FilePath & shader : paths )
			{
				const GLuint id = _createShader( p_rootPath / shader, p_toInject, p_suffix );
				if ( id != GL_INVALID_INDEX )
				{
					attachShader( id );
				}
			}

			assert( glIsProgram( _id ) );

			link();

			VTX_TRACE( "Program {} created: {}", _id, p_name );
		}

		Program( const Program & )			   = delete;
		Program & operator=( const Program & ) = delete;

		Program( Program && p_other ) noexcept :
			_id( std::exchange( p_other._id, GL_INVALID_INDEX ) ), _name( std::move( p_other._name ) ),
			_toInject( std::move( p_other._toInject ) )
		{
		}

		Program & operator=( Program && p_other ) noexcept
		{
			if ( this != &p_other )
			{
				destroy();
				_id		  = std::exchange( p_other._id, GL_INVALID_INDEX );
				_name	  = std::move( p_other._name );
				_toInject = std::move( p_other._toInject );
			}

			return *this;
		}

		~Program() { destroy(); }

		void destroy()
		{
			if ( _id != GL_INVALID_INDEX )
			{
				deleteShaders();
				glDeleteProgram( _id );
				_id = GL_INVALID_INDEX;
			}
		}

		void use() const noexcept { glUseProgram( _id ); }

		void create( const std::string & p_name )
		{
			assert( _id == GL_INVALID_INDEX );

			_name = p_name;
			_id	  = glCreateProgram();

			assert( _id != GL_INVALID_INDEX );
		}

		void attachShader( const GLuint p_shaderId ) const
		{
			assert( _id != GL_INVALID_INDEX );

			glAttachShader( _id, p_shaderId );
		}

		void link()
		{
			assert( _id != GL_INVALID_INDEX );

			GLint linked {};
			glLinkProgram( _id );
			glGetProgramiv( _id, GL_LINK_STATUS, &linked );
			if ( linked == GL_FALSE )
			{
				std::string error = "Error linking program: ";
				error += _name;
				error += "\n";
				error += Debug::getProgramErrors( _id );
				glDeleteProgram( _id );
				throw GraphicException( error );
			}
		}

		void detachShaders() const
		{
			assert( _id != GL_INVALID_INDEX );

			GLint nbShaders = 0;
			glGetProgramiv( _id, GL_ATTACHED_SHADERS, &nbShaders );
			std::vector<GLuint> shaders( nbShaders );
			glGetAttachedShaders( _id, nbShaders, nullptr, shaders.data() );
			for ( const GLuint shader : shaders )
			{
				glDetachShader( _id, shader );
			}
		}

		void deleteShaders() const
		{
			assert( _id != GL_INVALID_INDEX );
			GLint nbShaders = 0;
			glGetProgramiv( _id, GL_ATTACHED_SHADERS, &nbShaders );
			std::vector<GLuint> shaders( nbShaders );
			glGetAttachedShaders( _id, nbShaders, nullptr, shaders.data() );
			for ( const GLuint shader : shaders )
			{
				glDeleteShader( shader );
			}
		}

		static ENUM_SHADER_TYPE getShaderType( const FilePath & p_name )
		{
			const std::string extension = p_name.extension().string();
			for ( const auto & [ suffix, type ] : _EXTENSIONS )
			{
				if ( extension == suffix )
				{
					return type;
				}
			}

			return ENUM_SHADER_TYPE::INVALID;
		}

	  private:
		static constexpr std::array<std::pair<std::string_view, ENUM_SHADER_TYPE>, 6> _EXTENSIONS
			= { { { ".vert", ENUM_SHADER_TYPE::VERTEX },
				  { ".geom", ENUM_SHADER_TYPE::GEOMETRY },
				  { ".frag", ENUM_SHADER_TYPE::FRAGMENT },
				  { ".comp", ENUM_SHADER_TYPE::COMPUTE },
				  { ".tesc", ENUM_SHADER_TYPE::TESS_CONTROL },
				  { ".tese", ENUM_SHADER_TYPE::TESS_EVALUATION } } };

		GLuint		_id = GL_INVALID_INDEX;
		std::string _name;
		std::string _toInject;

		static GLuint _createShader(
			const FilePath &	p_path,
			const std::string & p_toInject,
			const std::string & p_suffix
		)
		{
			const std::string name = p_path.filename().string() + p_suffix;

			const ENUM_SHADER_TYPE type = getShaderType( p_path );

			VTX_TRACE( "Creating shader: {}", name );

			GLuint		shaderId = glCreateShader( (GLenum)type );
			std::string src		 = Util::Filesystem::readPath( p_path );
			if ( src.empty() )
			{
				glDeleteShader( shaderId );
				return GL_INVALID_INDEX;
			}

			// Handle injection (after #version).
			if ( not p_toInject.empty() )
			{
				const size_t startPosVersion = src.find( "#version" );
				assert( startPosVersion != std::string::npos );
				const size_t endPosVersion = src.find( '\n', startPosVersion );
				src.insert( endPosVersion + 1, p_toInject );
			}

			// Handle #include.
			_handleInclude( src, p_path.parent_path() );

			// VTX_DEBUG( "{}", src );

			const char * shaderCode = src.c_str();
			glShaderSource( shaderId, 1, &shaderCode, nullptr );
			glCompileShader( shaderId );
			GLint compiled = GL_FALSE;
			glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled );
			if ( compiled == GL_FALSE )
			{
				std::string error = "Error compiling shader: ";
				error += name;
				error += "\n";
				error += Debug::getShaderErrors( shaderId );
				glDeleteShader( shaderId );
				throw GraphicException( error );
			}

			assert( glIsShader( shaderId ) );

			VTX_TRACE( "Shader {} created: {}", shaderId, name );

			return shaderId;
		}

		static void _handleInclude( std::string & p_src, const FilePath & p_path )
		{
			std::vector<std::string> included;
			while ( true )
			{
				const size_t startPosInclude = p_src.find( "#include" );
				if ( startPosInclude == std::string::npos )
				{
					break;
				}

				const size_t endPosInclude		 = p_src.find( '\n', startPosInclude );
				std::string	 includeRelativePath = p_src.substr( startPosInclude, endPosInclude - startPosInclude );
				const size_t startPosPath		 = includeRelativePath.find( '"' );
				const size_t endPosPath			 = includeRelativePath.find( '"', startPosPath + 1 );
				includeRelativePath = includeRelativePath.substr( startPosPath + 1, endPosPath - startPosPath - 1 );

				// Check if already included.
				if ( std::ranges::find( included, includeRelativePath ) != included.end() )
				{
					p_src.erase( startPosInclude, endPosInclude - startPosInclude );
					continue;
				}

				const std::string srcInclude = Util::Filesystem::readPath( p_path / includeRelativePath );
				p_src.replace( startPosInclude, endPosInclude - startPosInclude, srcInclude );
				included.push_back( includeRelativePath );
			}
		}
	};
} // namespace VTX::Renderer::Context::Backend::GL

#endif
