#ifndef __VTX_RENDERER_CONTEXT_OPENGL_45__
#define __VTX_RENDERER_CONTEXT_OPENGL_45__

#include "concept_context.hpp"
#include "gl/buffer.hpp"
#include "gl/framebuffer.hpp"
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

			_vbo = std::make_unique<GL::Buffer>();
			_vao = std::make_unique<GL::VertexArray>();

			_vao->bind();
			_vao->enableAttribute( 0 );
			_vao->setVertexBuffer<float>( 0, *_vbo, sizeof( Vec2f ) );
			_vao->setAttributeFormat<float>( 0, 2 );
			_vao->setAttributeBinding( 0, 0 );
			_vbo->set( quad );
			_vao->unbind();

			glClearColor( 1.f, 0.f, 0.f, 1.f );
			glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );
		}

		~OpenGL45() { VTX_DEBUG( "{}", "Delete context opengl 4.5" ); }

		void build( const RenderQueue & p_renderQueue,
					const Links &		p_links,
					const Handle		p_output,
					Instructions &		p_instructions )
		{
			assert( p_instructions.empty() );

			p_instructions.emplace_back( [ & ]() { glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); } );

			for ( const Pass * const pass : p_renderQueue )
			{
				// Create FBO.
				_fbos.emplace( pass, std::make_unique<GL::Framebuffer>() );

				// Create outputs.
				for ( const auto & [ channel, output ] : pass->outputs )
				{
					const IO & desc = output.desc;
					if ( std::holds_alternative<Attachment>( desc ) )
					{
						const Attachment & attachment = std::get<Attachment>( desc );
						_textures.emplace(
							&attachment,
							std::make_unique<GL::Texture2D>( width,
															 height,
															 _mapFormat[ attachment.format ],
															 _mapWrapping[ attachment.wrappingS ],
															 _mapWrapping[ attachment.wrappingT ],
															 _mapFiltering[ attachment.filteringMin ],
															 _mapFiltering[ attachment.filteringMag ] ) );

						// Attach.
						_fbos[ pass ]->attachTexture( *_textures[ &attachment ], _mapAttachment[ channel ] );
					}
					else
					{
						throw std::runtime_error( "unknown descriptor type" );
					}
				}

				// Create programs.
				for ( const Program & desc : pass->programs )
				{
					const GL::Program * const program
						= _programManager->createProgram( desc.name, desc.shaders, desc.toInject, desc.suffix );
					_programs.emplace( &desc, program );
				}
			}
		}

		void resize( const size_t p_width, const size_t p_height )
		{
			width  = p_width;
			height = p_height;
			glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );
			for ( auto & [ desc, texture ] : _textures )
			{
				texture->resize( width, height );
			}
		}

	  private:
		// TODO: find a better solution (magic enum explodes compile time).

		std::map<const E_CHANNEL, const GLenum> _mapAttachment = {
			{ E_CHANNEL::COLOR_0, GL_COLOR_ATTACHMENT0 },
			{ E_CHANNEL::COLOR_1, GL_COLOR_ATTACHMENT1 },
			{ E_CHANNEL::COLOR_2, GL_COLOR_ATTACHMENT2 },
			{ E_CHANNEL::DEPTH, GL_DEPTH_ATTACHMENT },
		};

		std::map<const E_PRIMTIVE, const GLenum> _mapPrimitive = { { E_PRIMTIVE::POINTS, GL_POINTS },
																   { E_PRIMTIVE::LINES, GL_LINES },
																   { E_PRIMTIVE::TRIANGLES, GL_TRIANGLES } };

		std::map<const E_FORMAT, const GLenum> _mapFormat = {
			{ E_FORMAT::RGBA16F, GL_RGBA16F },
			{ E_FORMAT::RGBA32UI, GL_RGBA32UI },
			{ E_FORMAT::RGBA32F, GL_RGBA32F },
			{ E_FORMAT::RG32UI, GL_RG32UI },
			{ E_FORMAT::R16F, GL_R16F },
			{ E_FORMAT::R32F, GL_R32F },
			{ E_FORMAT::DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F },
		};

		std::map<const E_WRAPPING, const GLint> _mapWrapping = {
			{ E_WRAPPING::REPEAT, GL_REPEAT },
			{ E_WRAPPING::MIRRORED_REPEAT, GL_MIRRORED_REPEAT },
			{ E_WRAPPING::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
			{ E_WRAPPING::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER },
			{ E_WRAPPING::MIRROR_CLAMP_TO_EDGE, GL_MIRROR_CLAMP_TO_EDGE },
		};

		std::map<const E_FILTERING, const GLint> _mapFiltering = {
			{ E_FILTERING::NEAREST, GL_NEAREST },
			{ E_FILTERING::LINEAR, GL_LINEAR },
			{ E_FILTERING::NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST },
			{ E_FILTERING::LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST },
			{ E_FILTERING::NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR },
			{ E_FILTERING::LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR },
		};

		std::unique_ptr<GL::ProgramManager> _programManager;
		std::unique_ptr<GL::VertexArray>	_vao;
		std::unique_ptr<GL::Buffer>			_vbo;
		std::unique_ptr<GL::Buffer>			_ubo;

		std::unordered_map<const Attachment *, std::unique_ptr<GL::Texture2D>> _textures;
		std::unordered_map<const Program *, const GL::Program * const>		   _programs;
		std::unordered_map<const Pass *, std::unique_ptr<GL::Framebuffer>>	   _fbos;
		std::unordered_map<const Pass *, std::unique_ptr<GL::Buffer>>		   _ubos;
	};
} // namespace VTX::Renderer::Context

#endif
