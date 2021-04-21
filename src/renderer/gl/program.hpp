#ifndef __VTX_GL_PROGRAM__
#define __VTX_GL_PROGRAM__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_opengl.hpp"
#include <string>

namespace VTX::Renderer::GL
{
	class Program : public Generic::BaseOpenGL
	{
	  public:
		Program( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) {}
		~Program();

		inline GLuint getId() const { return _id; }

		void create( const std::string & );
		void attachShader( const GLuint );
		void link();
		void detachShaders();

		int getUniformLocation( const std::string & p_name ) const
		{
			return _gl->glGetUniformLocation( _id, p_name.c_str() );
		}

		void use() { _gl->glUseProgram( _id ); }

	  private:
		GLuint		_id	  = GL_INVALID_INDEX;
		std::string _name = "";

		std::string _getProgramErrors();

		friend class ProgramManager;
	};
} // namespace VTX::Renderer::GL

#endif
