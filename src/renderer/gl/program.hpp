#ifndef __VTX_GL_PROGRAM__
#define __VTX_GL_PROGRAM__

#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "tool/logger.hpp"
#include "util/math.hpp"
#include <string>

namespace VTX::Renderer::GL
{
	class Program : public Generic::BaseOpenGL
	{
	  public:
		Program( OpenGLFunctions * const p_gl, const std::vector<IO::FilePath> & p_shaderPaths ) :
			BaseOpenGL( p_gl ), _shaderPaths( p_shaderPaths )
		{
		}
		~Program();

		inline const GLuint						 getId() const { return _id; }
		inline void								 setId( const GLuint p_id ) { _id = p_id; }
		inline const std::vector<IO::FilePath> & getShaderPaths() const { return _shaderPaths; }

		void create( const std::string & );
		void attachShader( const GLuint );
		void link();
		void detachShaders();

		inline int getUniformLocation( const std::string & p_name ) const
		{
			const int loc = _gl->glGetUniformLocation( _id, p_name.c_str() );
#ifdef _DEBUG
			/// TODO: handle it
			// if ( loc == -1 )
			//	VTX_WARNING( p_name + " is not a uniform variable of GLSL program " + std::to_string( _id ) );
#endif
			return loc;
		}

		inline void use() { _gl->glUseProgram( _id ); }

		// ===================================== Uniform setters.
		// =============== Scalars.
		inline void setInt( const std::string & p_name, const GLint p_value ) const
		{
			_gl->glUniform1i( getUniformLocation( p_name ), p_value );
		}
		inline void setFloat( const std::string & p_name, const GLfloat p_value ) const
		{
			_gl->glUniform1f( getUniformLocation( p_name ), p_value );
		}
		inline void setBool( const std::string & p_name, const bool p_value ) const
		{
			_gl->glUniform1ui( getUniformLocation( p_name ), GLuint( p_value ) );
		}

		// =============== Vectors.
		// ===== Floats.
		inline void setVec2f( const std::string & p_name, const Vec2f & p_value ) const
		{
			_gl->glUniform2fv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec3f( const std::string & p_name, const Vec3f & p_value ) const
		{
			_gl->glUniform3fv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec4f( const std::string & p_name, const Vec4f & p_value ) const
		{
			_gl->glUniform4fv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec2f( const std::string & p_name, const GLfloat p_x, const GLfloat p_y ) const
		{
			_gl->glUniform2f( getUniformLocation( p_name ), p_x, p_y );
		}
		inline void setVec3f( const std::string & p_name,
							  const GLfloat		  p_x,
							  const GLfloat		  p_y,
							  const GLfloat		  p_z ) const
		{
			_gl->glUniform3f( getUniformLocation( p_name ), p_x, p_y, p_z );
		}
		inline void setVec4f( const std::string & p_name,
							  const GLfloat		  p_x,
							  const GLfloat		  p_y,
							  const GLfloat		  p_z,
							  const GLfloat		  p_w ) const
		{
			_gl->glUniform4f( getUniformLocation( p_name ), p_x, p_y, p_z, p_w );
		}

		// ===== Ints.
		inline void setVec2i( const std::string & p_name, const Vec2i & p_value ) const
		{
			_gl->glUniform2iv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec3i( const std::string & p_name, const Vec3i & p_value ) const
		{
			_gl->glUniform3iv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec4i( const std::string & p_name, const Vec4i & p_value ) const
		{
			_gl->glUniform4iv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec2i( const std::string & p_name, const GLint p_x, const GLint p_y ) const
		{
			_gl->glUniform2i( getUniformLocation( p_name ), p_x, p_y );
		}
		inline void setVec3i( const std::string & p_name, const GLint p_x, const GLint p_y, const GLint p_z ) const
		{
			_gl->glUniform3i( getUniformLocation( p_name ), p_x, p_y, p_z );
		}
		inline void setVec4i( const std::string & p_name,
							  const GLint		  p_x,
							  const GLint		  p_y,
							  const GLint		  p_z,
							  const GLint		  p_w ) const
		{
			_gl->glUniform4i( getUniformLocation( p_name ), p_x, p_y, p_z, p_w );
		}

		// =============== Matrices.
		inline void setMat3f( const std::string & p_name, const Mat3f & p_value ) const
		{
			_gl->glUniformMatrix4fv( getUniformLocation( p_name ), 1, GL_FALSE, Util::Math::value_ptr( p_value ) );
		}
		inline void setMat4f( const std::string & p_name, const Mat4f & p_value ) const
		{
			_gl->glUniformMatrix4fv( getUniformLocation( p_name ), 1, GL_FALSE, Util::Math::value_ptr( p_value ) );
		}

		// =============== Arrays.
		inline void setVec3fArray( const std::string & p_name, const GLsizei p_count, const Vec3f * p_array ) const
		{
			_gl->glUniform3fv( getUniformLocation( p_name ), p_count, (const GLfloat *)p_array );
		}

		// =====================================

	  private:
		GLuint							_id			 = GL_INVALID_INDEX;
		std::string						_name		 = "";
		const std::vector<IO::FilePath> _shaderPaths = std::vector<IO::FilePath>();
		std::string						_getProgramErrors();

		friend class ProgramManager;
	};
} // namespace VTX::Renderer::GL

#endif
