#ifndef __VTX_RENDERER_CONTEXT_GL_DEBUG__
#define __VTX_RENDERER_CONTEXT_GL_DEBUG__

#include <util/logger.hpp>

namespace VTX::Renderer::Context::Backend::GL::Debug
{
	inline void _debugMessageCallback(
		const GLenum   p_source,
		const GLenum   p_type,
		const GLuint   p_id,
		const GLenum   p_severity,
		const GLsizei  p_length,
		const GLchar * p_msg,
		const void *   p_data
	) noexcept
	{
		std::string source;
		std::string type;
		std::string severity;

		switch ( p_source )
		{
		case GL_DEBUG_SOURCE_API: source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source = "WINDOW SYSTEM"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: source = "SHADER COMPILER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: source = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: source = "APPLICATION"; break;
		case GL_DEBUG_SOURCE_OTHER: source = "UNKNOWN"; break;
		default: source = "UNKNOWN"; break;
		}

		switch ( p_type )
		{
		case GL_DEBUG_TYPE_ERROR: type = "ERROR"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = "DEPRECATED BEHAVIOR"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type = "UDEFINED BEHAVIOR"; break;
		case GL_DEBUG_TYPE_PORTABILITY: type = "PORTABILITY"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: type = "PERFORMANCE"; break;
		case GL_DEBUG_TYPE_OTHER: type = "OTHER"; break;
		case GL_DEBUG_TYPE_MARKER: type = "MARKER"; break;
		default: type = "UNKNOWN"; break;
		}

		switch ( p_severity )
		{
		case GL_DEBUG_SEVERITY_HIGH: severity = "HIGH"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: severity = "MEDIUM"; break;
		case GL_DEBUG_SEVERITY_LOW: severity = "LOW"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity = "NOTIFICATION"; break;
		default: severity = "UNKNOWN"; break;
		}

		std::string message( "[" + severity + "] [" + type + "] " + source + ": " + p_msg );

		switch ( p_severity )
		{
		case GL_DEBUG_SEVERITY_HIGH:
			VTX_ERROR( "{}", message );
			assert( false );
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW: VTX_WARNING( "{}", message ); break;
		default: break;
		}
	}

	inline void dumpGLError() noexcept
	{
		GLenum err;
		while ( ( err = glGetError() ) != GL_NO_ERROR )
		{
			fprintf( stderr, "[GL ERROR] 0x%X\n", err );
			assert( false );
		}
	};

	inline std::string getProgramErrors( const GLuint p_id )
	{
		GLint length;
		glGetProgramiv( p_id, GL_INFO_LOG_LENGTH, &length );
		if ( length == 0 )
		{
			return "";
		}
		std::vector<char> log( length );
		glGetProgramInfoLog( p_id, length, &length, &log[ 0 ] );

		return std::string( log.begin(), log.end() );
	}

	inline std::string getShaderErrors( const GLuint p_id )
	{
		GLint length;
		glGetShaderiv( p_id, GL_INFO_LOG_LENGTH, &length );
		if ( length == 0 )
		{
			return "";
		}
		std::vector<char> log( length );
		glGetShaderInfoLog( p_id, length, &length, &log[ 0 ] );

		return std::string( log.begin(), log.end() );
	}
} // namespace VTX::Renderer::Context::Backend::GL::Debug

#endif
