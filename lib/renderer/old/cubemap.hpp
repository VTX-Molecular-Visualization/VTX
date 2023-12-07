#ifndef __VTX_RENDERER_GL_CUBEMAP__
#define __VTX_RENDERER_GL_CUBEMAP__

#include "program_manager.hpp"
#include "renderer/gl/include_opengl.hpp"
#include "vertex_array.hpp"
#include <array>
#include <cassert>

namespace VTX::Renderer::GL
{
	class Cubemap
	{
	  public:
		Cubemap( const size_t							p_width,
				 const size_t							p_height,
				 const std::array<unsigned char *, 6> & p_textures,
				 ProgramManager &						p_pm )
		{
			// Textures.
			glCreateTextures( GL_TEXTURE_CUBE_MAP, 1, &_id );
			glTextureParameteri( _id, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTextureParameteri( _id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTextureParameteri( _id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTextureParameteri( _id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTextureParameteri( _id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

			glTextureStorage2D( _id, 1, GL_RGB8, GLsizei( p_width ), GLsizei( p_height ) );

			for ( size_t i = 0; i < p_textures.size(); ++i )
			{
				glTextureSubImage3D( _id,
									 0,
									 0,
									 0,
									 GLint( i ),
									 GLsizei( p_width ),
									 GLsizei( p_height ),
									 1,
									 GL_RGB,
									 GL_UNSIGNED_BYTE,
									 p_textures[ i ] );
			}

			// Vertices.
			const std::vector<float> vertices = { -1.0f, 1.0f,	-1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
												  1.0f,	 -1.0f, -1.0f, 1.0f,  1.0f,	 -1.0f, -1.0f, 1.0f,  -1.0f,

												  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
												  -1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,	 1.0f,	-1.0f, -1.0f, 1.0f,

												  1.0f,	 -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,	1.0f,  1.0f,  1.0f,
												  1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,	 -1.0f, 1.0f,  -1.0f, -1.0f,

												  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,
												  1.0f,	 1.0f,	1.0f,  1.0f,  -1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,

												  -1.0f, 1.0f,	-1.0f, 1.0f,  1.0f,	 -1.0f, 1.0f,  1.0f,  1.0f,
												  1.0f,	 1.0f,	1.0f,  -1.0f, 1.0f,	 1.0f,	-1.0f, 1.0f,  -1.0f,

												  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,	1.0f,  -1.0f, -1.0f,
												  1.0f,	 -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,	1.0f,  -1.0f, 1.0f };

			_vao.enableAttribute( 0 );
			_vao.setVertexBuffer<float>( 0, _vbo, 3 * sizeof( float ) );
			_vao.setAttributeFormat<float>( 0, 3 );
			_vao.setAttributeBinding( 0, 0 );

			_vbo.set( vertices );

			// Program.
			_program = p_pm.createProgram( "Cubemap", FilePath( "cubemap" ) );
			assert( _program != nullptr );
		}

		~Cubemap() { glDeleteTextures( 1, &_id ); }

		inline int getId() const { return _id; }

		void render()
		{
			glDepthMask( GL_FALSE );

			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
			glBindTextureUnit( 0, _id );
			_program->use();
			_vao.drawArray( GL_TRIANGLES, 0, 36 );

			glDepthMask( GL_TRUE );
		}

	  private:
		GLuint		_id = GL_INVALID_INDEX;
		VertexArray _vao;
		Buffer		_vbo;
		Program *	_program = nullptr;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_TEXTURE_2D__
