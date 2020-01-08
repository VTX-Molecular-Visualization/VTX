#ifndef __VTX_GL_DEBUG__
#define __VTX_GL_DEBUG__

#ifdef _MSC_VER
#pragma once
#endif

#include <GL/gl3w.h>
#include <iostream>

namespace VTX
{
	namespace Util
	{
		namespace OpenGL
		{
			void APIENTRY debugMessageCallback( const GLenum   p_source,
												const GLenum   p_type,
												const GLuint   p_id,
												const GLenum   p_severity,
												const GLsizei  p_length,
												const GLchar * p_msg,
												const void *   p_data )
			{
				const char * _source;
				const char * _type;
				const char * _severity;

				switch ( p_source )
				{
				case GL_DEBUG_SOURCE_API: _source = "API"; break;
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM: _source = "WINDOW SYSTEM"; break;
				case GL_DEBUG_SOURCE_SHADER_COMPILER: _source = "SHADER COMPILER"; break;
				case GL_DEBUG_SOURCE_THIRD_PARTY: _source = "THIRD PARTY"; break;
				case GL_DEBUG_SOURCE_APPLICATION: _source = "APPLICATION"; break;
				case GL_DEBUG_SOURCE_OTHER: _source = "UNKNOWN"; break;
				default: _source = "UNKNOWN"; break;
				}

				switch ( p_type )
				{
				case GL_DEBUG_TYPE_ERROR: _type = "ERROR"; break;
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: _type = "DEPRECATED BEHAVIOR"; break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: _type = "UDEFINED BEHAVIOR"; break;
				case GL_DEBUG_TYPE_PORTABILITY: _type = "PORTABILITY"; break;
				case GL_DEBUG_TYPE_PERFORMANCE: _type = "PERFORMANCE"; break;
				case GL_DEBUG_TYPE_OTHER: _type = "OTHER"; break;
				case GL_DEBUG_TYPE_MARKER: _type = "MARKER"; break;
				default: _type = "UNKNOWN"; break;
				}

				switch ( p_severity )
				{
				case GL_DEBUG_SEVERITY_HIGH: _severity = "HIGH"; break;
				case GL_DEBUG_SEVERITY_MEDIUM: _severity = "MEDIUM"; break;
				case GL_DEBUG_SEVERITY_LOW: _severity = "LOW"; break;
				case GL_DEBUG_SEVERITY_NOTIFICATION: _severity = "NOTIFICATION"; break;
				default: _severity = "UNKNOWN"; break;
				}

				if ( p_severity != GL_DEBUG_SEVERITY_NOTIFICATION )
				{ std::printf( "[%s][%s] %s: %s\n", _severity, _type, _source, p_msg ); }
			}
		} // namespace OpenGL
	}	  // namespace Util
} // namespace VTX
#endif
