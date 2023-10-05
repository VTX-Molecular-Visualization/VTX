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
			std::vector<Vec2f> quad = { { -1.f, 1.f }, { -1.f, -1.f }, { 1.f, 1.f }, { 1.f, -1.f } };

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

			// TODO: not in geometric pass.
			p_instructions.emplace_back( [ & ]() { _vao->bind(); } );

			// TODO: bind main ubo.

			for ( const Pass * const descPass : p_renderQueue )
			{
				bool isLastPass = descPass == p_renderQueue.back();

				// Create FBO.
				if ( isLastPass == false )
				{
					_fbos.emplace( descPass, std::make_unique<GL::Framebuffer>() );

					// Create outputs.
					for ( const auto & [ channel, output ] : descPass->outputs )
					{
						const IO & descIO = output.desc;
						if ( std::holds_alternative<Attachment>( descIO ) )
						{
							const Attachment & attachment = std::get<Attachment>( descIO );
							_textures.emplace(
								&attachment,
								std::make_unique<GL::Texture2D>( width,
																 height,
																 _mapFormats[ attachment.format ],
																 _mapWrappings[ attachment.wrappingS ],
																 _mapWrappings[ attachment.wrappingT ],
																 _mapFilterings[ attachment.filteringMin ],
																 _mapFilterings[ attachment.filteringMag ] ) );

							// Attach.
							_fbos[ descPass ]->attachTexture( *_textures[ &attachment ], _mapAttachments[ channel ] );
						}
						else
						{
							throw std::runtime_error( "unknown descriptor type" );
						}
					}
				}

				// Create programs.
				std::vector<uint> offsets;
				for ( const Program & descProgram : descPass->programs )
				{
					const GL::Program * const program = _programManager->createProgram(
						descProgram.name, descProgram.shaders, descProgram.toInject, descProgram.suffix );

					_programs.emplace( &descProgram, program );

					// Uniforms.
					size_t offset = 0;
					for ( const Uniform & descUniform : descProgram.uniforms )
					{
						_uniformOffsets.emplace( &descUniform, offset );
						offset += _mapTypeSizes[ descUniform.type ];
					}

					if ( offset > 0 )
					{
						_ubos.emplace( &descProgram, std::make_unique<GL::Buffer>( offset, GL_STATIC_DRAW ) );
					}
				}

				// Enqueue instructions
				// TODO: optimize pointer access?
				// GL::Framebuffer * const fbo = _fbos[ descPass ].get();

				// Bind fbo.
				if ( isLastPass == false )
				{
					p_instructions.emplace_back( [ this, descPass ]()
												 { _fbos[ descPass ]->bind( GL_DRAW_FRAMEBUFFER ); } );
				}
				else
				{
					GL::Framebuffer::bindDefault( p_output, GL_DRAW_FRAMEBUFFER );
				}

				// Find source for input.
				auto findInputSrcInLinks
					= [ &p_links, descPass ]( const E_CHANNEL_INPUT p_channel ) -> const Output * const
				{
					const auto it = std::find_if( p_links.begin(),
												  p_links.end(),
												  [ descPass, p_channel ]( const std::unique_ptr<Link> & p_e )
												  { return p_e->dest == descPass && p_e->channelDest == p_channel; } );

					if ( it == p_links.end() )
					{
						return nullptr;
					}

					return &( it->get()->src->outputs[ it->get()->channelSrc ] );
				};

				// Bind inputs.
				int channelMax = 0;
				for ( const auto & [ channel, input ] : descPass->inputs )
				{
					const Output * const src = findInputSrcInLinks( channel );

					channelMax = std::max( channelMax, int( channel ) );
					if ( src == nullptr )
					{
						VTX_WARNING( "Input channel {} from pass {} as no source", input.name, descPass->name );
						// TODO: bind dummy texture?
						continue;
					}

					const IO & descIO = src->desc;
					if ( std::holds_alternative<Attachment>( descIO ) )
					{
						const Attachment * const attachment = &std::get<Attachment>( descIO );

						p_instructions.emplace_back( [ this, channel = channel, attachment ]()
													 { _textures[ attachment ]->bindToUnit( GLuint( channel ) ); } );
					}
					else
					{
						throw std::runtime_error( "unknown descriptor type" );
					}
				}

				// Programs.
				for ( const Program & descProgram : descPass->programs )
				{
					if ( descProgram.uniforms.empty() == false )
					{
						p_instructions.emplace_back(
							[ this, &descProgram, &channelMax ]()
							{ _ubos[ &descProgram ]->bind( GL_UNIFORM_BUFFER, channelMax + 1 ); } );
					}

					p_instructions.emplace_back(
						[ this, &descProgram ]()
						{
							_programs[ &descProgram ]->use();
							_vao->drawArray( GL_TRIANGLE_STRIP, 0, 4 );
						} );

					if ( descProgram.uniforms.empty() == false )
					{
						p_instructions.emplace_back( [ this, &descProgram ]() { _ubos[ &descProgram ]->unbind(); } );
					}
				}

				// Unbind inputs.
				for ( const auto & [ channel, input ] : descPass->inputs )
				{
					const Output * const src = findInputSrcInLinks( channel );

					if ( src == nullptr )
					{
						continue;
					}

					const IO & descIO = src->desc;
					if ( std::holds_alternative<Attachment>( descIO ) )
					{
						const Attachment * const attachment = &std::get<Attachment>( descIO );

						p_instructions.emplace_back(
							[ this, channel = channel, attachment ]()
							{ _textures[ attachment ]->unbindFromUnit( GLuint( channel ) ); } );
					}
					else
					{
						throw std::runtime_error( "unknown descriptor type" );
					}
				}

				// Unbind fbo.
				if ( isLastPass == false )
				{
					p_instructions.emplace_back( [ this, descPass ]() { _fbos[ descPass ]->unbind(); } );
				}
				else
				{
					GL::Framebuffer::unbindDefault( GL_DRAW_FRAMEBUFFER );
				}

				// TODO: not in geometric pass.
				p_instructions.emplace_back( [ this ]() { _vao->unbind(); } );
			}

			// TODO: unbind main ubo.
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

		// TODO
		void setValue( const Uniform & p_uniform, const void * p_value ) {}

	  private:
		// TODO: find a better solution (magic enum explodes compile time).

		std::map<const E_CHANNEL_OUTPUT, const GLenum> _mapAttachments = {
			{ E_CHANNEL_OUTPUT::COLOR_0, GL_COLOR_ATTACHMENT0 },
			{ E_CHANNEL_OUTPUT::COLOR_1, GL_COLOR_ATTACHMENT1 },
			{ E_CHANNEL_OUTPUT::COLOR_2, GL_COLOR_ATTACHMENT2 },
			{ E_CHANNEL_OUTPUT::DEPTH, GL_DEPTH_ATTACHMENT },
		};

		std::map<const E_PRIMTIVE, const GLenum> _mapPrimitives = { { E_PRIMTIVE::POINTS, GL_POINTS },
																	{ E_PRIMTIVE::LINES, GL_LINES },
																	{ E_PRIMTIVE::TRIANGLES, GL_TRIANGLES } };

		std::map<const E_FORMAT, const GLenum> _mapFormats = {
			{ E_FORMAT::RGBA16F, GL_RGBA16F },
			{ E_FORMAT::RGBA32UI, GL_RGBA32UI },
			{ E_FORMAT::RGBA32F, GL_RGBA32F },
			{ E_FORMAT::RG32UI, GL_RG32UI },
			{ E_FORMAT::R16F, GL_R16F },
			{ E_FORMAT::R32F, GL_R32F },
			{ E_FORMAT::DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F },
		};

		std::map<const E_WRAPPING, const GLint> _mapWrappings = {
			{ E_WRAPPING::REPEAT, GL_REPEAT },
			{ E_WRAPPING::MIRRORED_REPEAT, GL_MIRRORED_REPEAT },
			{ E_WRAPPING::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
			{ E_WRAPPING::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER },
			{ E_WRAPPING::MIRROR_CLAMP_TO_EDGE, GL_MIRROR_CLAMP_TO_EDGE },
		};

		std::map<const E_FILTERING, const GLint> _mapFilterings = {
			{ E_FILTERING::NEAREST, GL_NEAREST },
			{ E_FILTERING::LINEAR, GL_LINEAR },
			{ E_FILTERING::NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST },
			{ E_FILTERING::LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST },
			{ E_FILTERING::NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR },
			{ E_FILTERING::LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR },
		};

		std::map<const E_TYPE, const GLenum> _mapTypes
			= { { E_TYPE::UINT, GL_UNSIGNED_INT }, { E_TYPE::INT, GL_INT },		{ E_TYPE::FLOAT, GL_FLOAT },
				{ E_TYPE::VEC3F, GL_FLOAT },	   { E_TYPE::VEC4F, GL_FLOAT }, { E_TYPE::MAT3F, GL_FLOAT },
				{ E_TYPE::MAT4F, GL_FLOAT } };

		std::map<const E_TYPE, const size_t> _mapTypeSizes
			= { { E_TYPE::UINT, sizeof( uint ) },	{ E_TYPE::INT, sizeof( int ) },
				{ E_TYPE::FLOAT, sizeof( float ) }, { E_TYPE::VEC3F, sizeof( Vec3f ) },
				{ E_TYPE::VEC4F, sizeof( Vec4f ) }, { E_TYPE::MAT3F, sizeof( Mat3f ) },
				{ E_TYPE::MAT4F, sizeof( Mat4f ) } };

		std::unique_ptr<GL::ProgramManager> _programManager;
		std::unique_ptr<GL::VertexArray>	_vao;
		std::unique_ptr<GL::Buffer>			_vbo;
		std::unique_ptr<GL::Buffer>			_ubo;

		// TODO: check if mapping is useful.
		std::unordered_map<const Attachment *, std::unique_ptr<GL::Texture2D>> _textures;
		std::unordered_map<const Program *, const GL::Program * const>		   _programs;
		std::unordered_map<const Pass *, std::unique_ptr<GL::Framebuffer>>	   _fbos;
		std::unordered_map<const Program *, std::unique_ptr<GL::Buffer>>	   _ubos;
		std::unordered_map<const Uniform *, size_t>							   _uniformOffsets;
	};
} // namespace VTX::Renderer::Context

#endif
