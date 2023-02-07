#include "program.hpp"
#include <util/types.hpp>
#include "tool/logger.hpp"
#include <iostream>
#include <vector>

namespace VTX::Renderer::GL
{
	Program::~Program()
	{
		if ( _id != GL_INVALID_INDEX )
		{
			// Detach but don't delete shaders, can be used by other programs.
			detachShaders();
			_gl->glDeleteProgram( _id );
		}
	}

	void Program::create( const std::string & p_name )
	{
		if ( _id == GL_INVALID_INDEX )
		{
			_name = p_name;
			_id	  = _gl->glCreateProgram();
		}
		else
		{
			VTX_WARNING( "Program already created" );
		}
	}

	void Program::attachShader( const GLuint p_shaderId )
	{
		if ( _id == GL_INVALID_INDEX )
		{
			VTX_ERROR( "Cannot attach shader: program is not created" );
			return;
		}

		_gl->glAttachShader( _id, p_shaderId );
	}

	void Program::link()
	{
		VTX_DEBUG( "Linking program " + _name );

		if ( _id == GL_INVALID_INDEX )
		{
			VTX_ERROR( "Can not link program: program is not created" );
			return;
		}

		GLint linked;
		_gl->glLinkProgram( _id );
		_gl->glGetProgramiv( _id, GL_LINK_STATUS, &linked );
		if ( linked == GL_FALSE )
		{
			std::string error = "Error linking program: ";
			error += _name;
			error += "\n";
			error += _getProgramErrors();
			_gl->glDeleteProgram( _id );
			VTX_ERROR( error );
			return;
		}
	}

	void Program::detachShaders()
	{
		GLint nbShaders = 0;
		_gl->glGetProgramiv( _id, GL_ATTACHED_SHADERS, &nbShaders );
		std::vector<GLuint> shaders( nbShaders );
		_gl->glGetAttachedShaders( _id, nbShaders, nullptr, shaders.data() );
		for ( GLuint shader : shaders )
		{
			_gl->glDetachShader( _id, shader );
		}
	}

	std::string Program::_getProgramErrors()
	{
		GLint length;
		_gl->glGetProgramiv( _id, GL_INFO_LOG_LENGTH, &length );
		if ( length == 0 )
		{
			return "";
		}
		std::vector<GLchar> log( length );
		_gl->glGetProgramInfoLog( _id, length, &length, &log[ 0 ] );
		return std::string( log.begin(), log.end() );
	}
} // namespace VTX::Renderer::GL
