#ifndef __VTX_RENDERER_CONTEXT_OPENGL_45__
#define __VTX_RENDERER_CONTEXT_OPENGL_45__

#include "concept_context.hpp"
#include "gl/buffer.hpp"
#include "gl/program_manager.hpp"
#include "gl/texture_2d.hpp"
#include "gl/vertex_array.hpp"
#include <glad/glad.h>
#include <util/enum.hpp>
#include <util/exceptions.hpp>

namespace VTX::Renderer::Context
{

	class OpenGL45 : public BaseContext
	{
	  public:
		OpenGL45() = delete;
		OpenGL45( const size_t	   p_width,
				  const size_t	   p_height,
				  const FilePath & p_shaderPath,
				  void *		   p_proc = nullptr ) :
			BaseContext { p_width, p_height, p_shaderPath }
		{
			VTX_DEBUG( "{}", "Create context opengl 4.5" );

			// Load opengl 4.5.
			if ( p_proc && gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
			{
				throw GLException( "Failed to load OpenGL" );
			}
			else if ( gladLoadGL() == 0 )
			{
				throw GLException( "Failed to load OpenGL" );
			}
			if ( GLAD_GL_VERSION_4_5 == false )
			{
				throw GLException( "OpenGL 4.5 or higher is required" );
			}

			// Program manager.
			_programManager = std::make_unique<GL::ProgramManager>( p_shaderPath );

			// Init quad vao/vbo for deferred shading.
			std::vector<Vec2f> quad
				= { Vec2f( -1.f, 1.f ), Vec2f( -1.f, -1.f ), Vec2f( 1.f, 1.f ), Vec2f( 1.f, -1.f ) };

			GL::Buffer::create( &_vbo );
			GL::VertexArray::create( &_vao );
			GL::VertexArray::bind( _vao );
			GL::VertexArray::enableAttribute( _vao, 0 );
			GL::VertexArray::setVertexBuffer<float>( _vao, 0, _vbo, sizeof( Vec2f ) );
			GL::VertexArray::setAttributeFormat<float>( _vao, 0, 2 );
			GL::VertexArray::setAttributeBinding( _vao, 0, 0 );
			GL::Buffer::setData( _vbo, quad, GL_STATIC_DRAW );
			GL::VertexArray::unbind();

			glClearColor( 1.f, 0.f, 0.f, 1.f );
			glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );
		}

		~OpenGL45()
		{
			VTX_DEBUG( "{}", "Delete context opengl 4.5" );

			GL::Buffer::destroy( &_vbo );
			GL::VertexArray::destroy( &_vao );
		}

		inline void clear() { glClear( GL_COLOR_BUFFER_BIT ); }

		void resize( const size_t p_width, const size_t p_height )
		{
			width  = p_width;
			height = p_height;
		}

		// I/O.
		inline void create( Handle & p_handle, const DescAttachment & p_desc )
		{
			GL::Texture2D::create( &p_handle,
								   GLsizei( width ),
								   GLsizei( height ),
								   _mapFormat[ p_desc.format ],
								   _mapWrapping[ p_desc.wrappingS ],
								   _mapWrapping[ p_desc.wrappingT ],
								   _mapFiltering[ p_desc.filteringMin ],
								   _mapFiltering[ p_desc.filteringMag ] );
		}

		inline void resize( Handle & p_handle, const DescAttachment & p_desc )
		{
			destroy( p_handle, p_desc );
			create( p_handle, p_desc );
		}

		inline void destroy( Handle & p_handle, const DescAttachment & p_desc ) { GL::Texture2D::destroy( &p_handle ); }

		inline void create( Handle & p_handle, const DescStorage & p_desc ) {}

		// Program.
		inline void create( Handle & p_handle, const DescProgram & p_desc )
		{
			_programManager->createProgram( p_desc.name, p_desc.shaders, p_desc.toInject, p_desc.suffix );
		}

		inline void destroy( Handle & p_handle, const DescProgram & p_desc )
		{
			_programManager->deleteProgram( p_desc.name );
		}

	  private:
		// TODO: find a better solution (magic enum explodes compile time)
		std::map<E_FORMAT, GLenum> _mapFormat = {
			{ E_FORMAT::RGBA16F, GL_RGBA16F },
			{ E_FORMAT::RGBA32UI, GL_RGBA32UI },
			{ E_FORMAT::RGBA32F, GL_RGBA32F },
			{ E_FORMAT::RG32UI, GL_RG32UI },
			{ E_FORMAT::R16F, GL_R16F },
			{ E_FORMAT::R32F, GL_R32F },
			{ E_FORMAT::DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F },
		};

		std::map<E_WRAPPING, GLint> _mapWrapping = {
			{ E_WRAPPING::REPEAT, GL_REPEAT },
			{ E_WRAPPING::MIRRORED_REPEAT, GL_MIRRORED_REPEAT },
			{ E_WRAPPING::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
			{ E_WRAPPING::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER },
			{ E_WRAPPING::MIRROR_CLAMP_TO_EDGE, GL_MIRROR_CLAMP_TO_EDGE },
		};

		std::map<E_FILTERING, GLint> _mapFiltering = {
			{ E_FILTERING::NEAREST, GL_NEAREST },
			{ E_FILTERING::LINEAR, GL_LINEAR },
			{ E_FILTERING::NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST },
			{ E_FILTERING::LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST },
			{ E_FILTERING::NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR },
			{ E_FILTERING::LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR },
		};

		// Program manager.
		std::unique_ptr<GL::ProgramManager> _programManager;

		// Quad VAO.
		GLuint _vao;
		GLuint _vbo;
	};
} // namespace VTX::Renderer::Context

#endif
