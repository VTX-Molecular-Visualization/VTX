#include <renderer/gl/program.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer::GL
{
	Program::~Program()
	{
		if ( _id != GL_INVALID_INDEX )
		{
			// Detach but don't delete shaders, can be used by other programs.
			detachShaders();
			glDeleteProgram( _id );
		}
	}

	void Program::create( const std::string & p_name )
	{
		if ( _id == GL_INVALID_INDEX )
		{
			_name = p_name;
			_id	  = glCreateProgram();
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

		glAttachShader( _id, p_shaderId );
	}

	void Program::link()
	{
		if ( _id == GL_INVALID_INDEX )
		{
			VTX_ERROR( "Can not link program: program is not created" );
			return;
		}

		GLint linked;
		glLinkProgram( _id );
		glGetProgramiv( _id, GL_LINK_STATUS, &linked );
		if ( linked == GL_FALSE )
		{
			std::string error = "Error linking program: ";
			error += _name;
			error += "\n";
			error += _getProgramErrors();
			glDeleteProgram( _id );
			VTX_ERROR( error );
			return;
		}
	}

	void Program::detachShaders()
	{
		GLint nbShaders = 0;
		glGetProgramiv( _id, GL_ATTACHED_SHADERS, &nbShaders );
		std::vector<GLuint> shaders( nbShaders );
		glGetAttachedShaders( _id, nbShaders, nullptr, shaders.data() );
		for ( GLuint shader : shaders )
		{
			glDetachShader( _id, shader );
		}
	}

	std::string Program::_getProgramErrors()
	{
		GLint length;
		glGetProgramiv( _id, GL_INFO_LOG_LENGTH, &length );
		if ( length == 0 )
		{
			return "";
		}
		std::vector<GLchar> log( length );
		glGetProgramInfoLog( _id, length, &length, &log[ 0 ] );
		return std::string( log.begin(), log.end() );
	}
} // namespace VTX::Renderer::GL
