#ifndef __VTX_APP_OLD_RENDER_GL_PROGRAM__
#define __VTX_APP_OLD_RENDER_GL_PROGRAM__

#include "app/old/render/generic/base_opengl.hpp"
#include <string>
#include <util/logger.hpp>
#include <util/math.hpp>
#include <util/types.hpp>

// TODO reimplement this without Qt (OpenGL embedded by Qt)
namespace VTX::App::Old::Render::Renderer::GL
{
	class Program : public Render::Generic::BaseOpenGL
	{
	  public:
		Program( const std::vector<FilePath> & p_shaderPaths, const std::string & p_toInject = "" ) :
			_shaderPaths( p_shaderPaths ), _toInject( p_toInject )
		{
		}
		~Program();

		inline const GLuint					 getId() const { return _id; }
		inline void							 setId( const GLuint p_id ) { _id = p_id; }
		inline const std::vector<FilePath> & getShaderPaths() const { return _shaderPaths; }
		inline const std::string &			 getToInject() const { return _toInject; }

		void create( const std::string & );
		void attachShader( const GLuint );
		void link();
		void detachShaders();

		inline int getUniformLocation( const std::string & p_name ) const
		{
			return 0;
			//			const int loc = _gl->glGetUniformLocation( _id, p_name.c_str() );
			// #ifdef _DEBUG
			//			/// TODO: handle it
			//			// if ( loc == -1 )
			//			//	VTX_WARNING( p_name + " is not a uniform variable of GLSL program " + std::to_string( _id )
			//); #endif 			return loc;
		}

		inline void use()
		{ /*_gl->glUseProgram( _id );*/
		}

		// ===================================== Uniform setters.
		// =============== Scalars.
		inline void setUInt( const std::string & p_name, const uint p_value ) const
		{
			//_gl->glUniform1ui( getUniformLocation( p_name ), GLuint( p_value ) );
		}
		inline void setInt( const std::string & p_name, const int p_value ) const
		{
			//_gl->glUniform1i( getUniformLocation( p_name ), GLint( p_value ) );
		}
		inline void setFloat( const std::string & p_name, const float p_value ) const
		{
			//_gl->glUniform1f( getUniformLocation( p_name ), GLfloat( p_value ) );
		}
		inline void setBool( const std::string & p_name, const bool p_value ) const
		{
			//_gl->glUniform1ui( getUniformLocation( p_name ), GLuint( p_value ) );
		}

		// =============== Vectors.
		// ===== Floats.
		inline void setVec2f( const std::string & p_name, const Vec2f & p_value ) const
		{
			//_gl->glUniform2fv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec3f( const std::string & p_name, const Vec3f & p_value ) const
		{
			//_gl->glUniform3fv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec4f( const std::string & p_name, const Vec4f & p_value ) const
		{
			//_gl->glUniform4fv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec2f( const std::string & p_name, const float p_x, const float p_y ) const
		{
			//_gl->glUniform2f( getUniformLocation( p_name ), GLfloat( p_x ), GLfloat( p_y ) );
		}
		inline void setVec3f( const std::string & p_name, const float p_x, const float p_y, const float p_z ) const
		{
			//_gl->glUniform3f( getUniformLocation( p_name ), GLfloat( p_x ), GLfloat( p_y ), GLfloat( p_z ) );
		}
		inline void setVec4f( const std::string & p_name,
							  const float		  p_x,
							  const float		  p_y,
							  const float		  p_z,
							  const float		  p_w ) const
		{
			//_gl->glUniform4f(
			//	getUniformLocation( p_name ), GLfloat( p_x ), GLfloat( p_y ), GLfloat( p_z ), GLfloat( p_w ) );
		}

		// ===== Ints.
		inline void setVec2i( const std::string & p_name, const Vec2i & p_value ) const
		{
			//_gl->glUniform2iv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec3i( const std::string & p_name, const Vec3i & p_value ) const
		{
			//_gl->glUniform3iv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec3u( const std::string & p_name, const Vec3u & p_value ) const
		{
			//_gl->glUniform3uiv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec4i( const std::string & p_name, const Vec4i & p_value ) const
		{
			//_gl->glUniform4iv( getUniformLocation( p_name ), 1, Util::Math::value_ptr( p_value ) );
		}
		inline void setVec2i( const std::string & p_name, const int p_x, const int p_y ) const
		{
			//_gl->glUniform2i( getUniformLocation( p_name ), GLint( p_x ), GLint( p_y ) );
		}
		inline void setVec3i( const std::string & p_name, const int p_x, const int p_y, const int p_z ) const
		{
			//_gl->glUniform3i( getUniformLocation( p_name ), GLint( p_x ), GLint( p_y ), GLint( p_z ) );
		}
		inline void setVec4i( const std::string & p_name,
							  const int			  p_x,
							  const int			  p_y,
							  const int			  p_z,
							  const int			  p_w ) const
		{
			//_gl->glUniform4i( getUniformLocation( p_name ), GLint( p_x ), GLint( p_y ), GLint( p_z ), GLint( p_w ) );
		}

		// =============== Matrices.
		inline void setMat3f( const std::string & p_name, const Mat3f & p_value ) const
		{
			//_gl->glUniformMatrix4fv( getUniformLocation( p_name ), 1, GL_FALSE, Util::Math::value_ptr( p_value ) );
		}
		inline void setMat4f( const std::string & p_name, const Mat4f & p_value ) const
		{
			//_gl->glUniformMatrix4fv( getUniformLocation( p_name ), 1, GL_FALSE, Util::Math::value_ptr( p_value ) );
		}

		// =============== Arrays.
		inline void setVec3fArray( const std::string & p_name, const uint p_count, const Vec3f * p_array ) const
		{
			//_gl->glUniform3fv( getUniformLocation( p_name ), GLsizei( p_count ), (const GLfloat *)p_array );
		}

		// =====================================

	  private:
		GLuint						_id			 = GL_INVALID_INDEX;
		std::string					_name		 = "";
		const std::vector<FilePath> _shaderPaths = std::vector<FilePath>();
		const std::string			_toInject	 = "";
		std::string					_getProgramErrors();

		friend class ProgramManager;
	};
} // namespace VTX::App::Old::Render::Renderer::GL

#endif
