#include <renderer/context/gl/program.hpp>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer::Context::GL
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
		assert( _id == GL_INVALID_INDEX );

		_name = p_name;
		_id	  = glCreateProgram();

		assert( _id != GL_INVALID_INDEX );
	}

	void Program::attachShader( const uint32_t p_shaderId )
	{
		assert( _id != GL_INVALID_INDEX );

		glAttachShader( _id, p_shaderId );
	}

	void Program::link()
	{
		assert( _id != GL_INVALID_INDEX );

		int32_t linked;
		glLinkProgram( _id );
		glGetProgramiv( _id, GL_LINK_STATUS, &linked );
		if ( linked == GL_FALSE )
		{
			std::string error = "Error linking program: ";
			error += _name;
			error += "\n";
			error += _getProgramErrors();
			glDeleteProgram( _id );
			throw GLException( error );
		}
	}

	void Program::detachShaders()
	{
		assert( _id != GL_INVALID_INDEX );

		int32_t nbShaders = 0;
		glGetProgramiv( _id, GL_ATTACHED_SHADERS, &nbShaders );
		std::vector<uint32_t> shaders( nbShaders );
		glGetAttachedShaders( _id, nbShaders, nullptr, shaders.data() );
		for ( uint32_t shader : shaders )
		{
			glDetachShader( _id, shader );
		}
	}

	std::string Program::_getProgramErrors()
	{
		int32_t length;
		glGetProgramiv( _id, GL_INFO_LOG_LENGTH, &length );
		if ( length == 0 )
		{
			return "";
		}
		std::vector<char> log( length );
		glGetProgramInfoLog( _id, length, &length, &log[ 0 ] );
		return std::string( log.begin(), log.end() );
	}
} // namespace VTX::Renderer::Context::GL
