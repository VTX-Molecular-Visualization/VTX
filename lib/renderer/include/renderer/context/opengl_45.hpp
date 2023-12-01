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
		OpenGL45(
			const size_t	 p_width,
			const size_t	 p_height,
			const FilePath & p_shaderPath,
			void *			 p_proc = nullptr
		) :
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

			_vaos.emplace( "quad", std::make_unique<GL::VertexArray>() );
			_bos.emplace( "quad", std::make_unique<GL::Buffer>() );
			auto & vao = _vaos[ "quad" ];
			auto & vbo = _bos[ "quad" ];

			vao->bind();
			vao->enableAttribute( 0 );
			vao->setVertexBuffer( 0, *vbo, GLsizei( _mapTypeSizes[ E_TYPE::FLOAT ] * 2 ) );
			vao->setAttributeFormat( 0, 2, _mapTypes[ E_TYPE::FLOAT ] );
			vao->setAttributeBinding( 0, 0 );
			vbo->setData( quad, GL_STATIC_DRAW );
			vao->unbind();

			glClearColor( 1.f, 0.f, 0.f, 1.f );
			glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );
		}

		~OpenGL45() { VTX_DEBUG( "{}", "Delete context opengl 4.5" ); }

		void build(
			const RenderQueue & p_renderQueue,
			const Links &		p_links,
			const Handle		p_output,
			const Uniforms &	p_uniforms,
			Instructions &		p_instructions
		)
		{
			assert( p_instructions.empty() );

			// Clear.
			p_instructions.emplace_back( [ & ]() { glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); } );

			// Create shared uniforms.
			if ( p_uniforms.empty() == false )
			{
				_ubo = std::make_unique<GL::Buffer>();
				_createUniforms( _ubo.get(), p_uniforms );
				p_instructions.emplace_back( [ this ]() { _ubo->bind( GL_UNIFORM_BUFFER, 15 ); } );
			}

			for ( const Pass * const descPass : p_renderQueue )
			{
				bool isLastPass = descPass == p_renderQueue.back();

				// Create data for each pass.
				for ( const auto & [ channel, input ] : descPass->inputs )
				{
					const IO & descIO = input.desc;

					if ( std::holds_alternative<Data>( descIO ) )
					{
						const Data & data = std::get<Data>( descIO );

						// Create vao.
						_vaos.emplace( input.name, std::make_unique<GL::VertexArray>() );
						_bos.emplace( input.name + "ebo", std::make_unique<GL::Buffer>() );
						auto & vaoData = _vaos[ input.name ];
						auto & eboData = _bos[ input.name + "ebo" ];
						vaoData->bindElementBuffer( *eboData );

						GLuint chan = 0;
						for ( const Data::Entry & entry : data.entries )
						{
							_bos.emplace( input.name + entry.name, std::make_unique<GL::Buffer>() );
							auto & vbo = _bos[ input.name + entry.name ];
							vaoData->enableAttribute( chan );
							vaoData->setVertexBuffer( chan, *vbo, GLsizei( _mapTypeSizes[ entry.type ] ) );
							vaoData->setAttributeFormat(
								chan, GLint( entry.components ), GLint( _mapTypes[ entry.type ] )
							);
							vaoData->setAttributeBinding( chan, chan );
							chan++;
						}
					}
				}

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
								std::make_unique<GL::Texture2D>(
									width,
									height,
									_mapFormats[ attachment.format ],
									_mapWrappings[ attachment.wrappingS ],
									_mapWrappings[ attachment.wrappingT ],
									_mapFilterings[ attachment.filteringMin ],
									_mapFilterings[ attachment.filteringMag ]
								)
							);

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
						descProgram.name, descProgram.shaders, descProgram.toInject, descProgram.suffix
					);

					_programs.emplace( &descProgram, program );

					// Uniforms.
					if ( descProgram.uniforms.empty() == false )
					{
						_ubos.emplace( &descProgram, std::make_unique<GL::Buffer>() );
						_createUniforms( _ubos[ &descProgram ].get(), descProgram.uniforms, &descProgram );
					}
				}

				// Enqueue instructions
				// TODO: optimize pointer access?
				// GL::Framebuffer * const fbo = _fbos[ descPass ].get();
				// Yes.
				//
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
					const auto it = std::find_if(
						p_links.begin(),
						p_links.end(),
						[ descPass, p_channel ]( const std::unique_ptr<Link> & p_e )
						{ return p_e->dest == descPass && p_e->channelDest == p_channel; }
					);

					if ( it == p_links.end() )
					{
						return nullptr;
					}

					return &( it->get()->src->outputs[ it->get()->channelSrc ] );
				};

				// Bind inputs.
				for ( const auto & [ channel, input ] : descPass->inputs )
				{
					const Output * const src	= findInputSrcInLinks( channel );
					const IO &			 descIO = src->desc;

					if ( src == nullptr )
					{
						VTX_WARNING( "Input channel {} from pass {} as no source", input.name, descPass->name );
						// TODO: bind dummy texture?
						continue;
					}

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
						assert( _ubos.find( &descProgram ) != _ubos.end() );

						p_instructions.emplace_back(
							[ this, &descProgram ]()
							{
								// Bind local ubo after last input.
								_ubos[ &descProgram ]->bind( GL_UNIFORM_BUFFER, 10 );
							}
						);
					}

					auto & program = _programs[ &descProgram ];
					// Draw custom.
					if ( descProgram.draw.has_value() )
					{
						const Draw & draw = descProgram.draw.value();
						auto &		 vao  = _vaos[ draw.name ];
						// Element.
						if ( draw.useIndices )
						{
							auto & ebo = _bos[ draw.name + "ebo" ];
							p_instructions.emplace_back(
								[ this, &program, &draw, &vao, &ebo ]()
								{
									vao->bind();
									vao->bindElementBuffer( *ebo );
									program->use();
									vao->drawArray( _mapPrimitives[ draw.primitive ], 0, GLsizei( 0 ) );
									vao->unbindElementBuffer();
									vao->unbind();
								}
							);
						}
						// Array.
						else
						{
							p_instructions.emplace_back(
								[ this, &program, &draw, &vao ]()
								{
									vao->bind();
									program->use();
									vao->drawElement( _mapPrimitives[ draw.primitive ], GLsizei( 0 ), GL_UNSIGNED_INT );
									vao->unbind();
								}
							);
						}
					}
					// Or quad.
					else
					{
						auto & vao = _vaos[ "quad" ];
						p_instructions.emplace_back(
							[ &program, &vao ]()
							{
								vao->bind();
								program->use();
								vao->drawArray( GL_TRIANGLE_STRIP, 0, 4 );
								vao->unbind();
							}
						);
					}

					if ( descProgram.uniforms.empty() == false )
					{
						auto & ubo = _ubos[ &descProgram ];
						p_instructions.emplace_back( [ &ubo ]() { ubo->unbind(); } );
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

						p_instructions.emplace_back( [ this, channel = channel, attachment ]()
													 { _textures[ attachment ]->unbindFromUnit( GLuint( channel ) ); }
						);
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
			}
			// Unbind main ubo.
			if ( p_uniforms.empty() == false )
			{
				p_instructions.emplace_back( [ this ]() { _ubo->unbind(); } );
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

		template<typename T>
		inline void setUniform( T & p_value, const std::string & p_key )
		{
			assert( _uniforms.find( p_key ) != _uniforms.end() );

			std::unique_ptr<StructUniformEntry> & entry = _uniforms[ p_key ];
			auto * const						  src	= entry->value;
			auto * const						  dest	= &p_value;

			assert( src != nullptr && dest != nullptr && entry->size );

			memcpy( src, dest, entry->size );
			entry->buffer->setSubData( p_value, entry->offset, GLsizei( entry->size ) );
		}

		template<typename T>
		inline void getUniform( T & p_value, const std::string & p_key )
		{
			assert( _uniforms.find( p_key ) != _uniforms.end() );

			std::unique_ptr<StructUniformEntry> & entry = _uniforms[ p_key ];
			auto * const						  src	= &p_value;
			auto * const						  dest	= entry->value;

			assert( src != nullptr && dest != nullptr && entry->size );

			memcpy( src, dest, entry->size );
		}

	  private:
		// TODO: find a better solution (magic enum explodes compile time).

		std::map<const E_CHANNEL_OUTPUT, const GLenum> _mapAttachments = {
			{ E_CHANNEL_OUTPUT::COLOR_0, GL_COLOR_ATTACHMENT0 },
			{ E_CHANNEL_OUTPUT::COLOR_1, GL_COLOR_ATTACHMENT1 },
			{ E_CHANNEL_OUTPUT::COLOR_2, GL_COLOR_ATTACHMENT2 },
			{ E_CHANNEL_OUTPUT::DEPTH, GL_DEPTH_ATTACHMENT },
		};

		std::map<const E_PRIMITIVE, const GLenum> _mapPrimitives = { { E_PRIMITIVE::POINTS, GL_POINTS },
																	 { E_PRIMITIVE::LINES, GL_LINES },
																	 { E_PRIMITIVE::TRIANGLES, GL_TRIANGLES } };

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
			= { { E_TYPE::BOOL, GL_BOOL },	 { E_TYPE::UINT, GL_UNSIGNED_INT }, { E_TYPE::INT, GL_INT },
				{ E_TYPE::FLOAT, GL_FLOAT }, { E_TYPE::VEC3F, GL_FLOAT },		{ E_TYPE::VEC4F, GL_FLOAT },
				{ E_TYPE::MAT3F, GL_FLOAT }, { E_TYPE::MAT4F, GL_FLOAT },		{ E_TYPE::COLOR4, GL_FLOAT } };

		std::map<const E_TYPE, const size_t> _mapTypeSizes = {
			{ E_TYPE::BOOL, sizeof( bool ) },	{ E_TYPE::UINT, sizeof( uint ) },	{ E_TYPE::INT, sizeof( int ) },
			{ E_TYPE::FLOAT, sizeof( float ) }, { E_TYPE::VEC3F, sizeof( Vec3f ) }, { E_TYPE::VEC4F, sizeof( Vec4f ) },
			{ E_TYPE::MAT3F, sizeof( Mat3f ) }, { E_TYPE::MAT4F, sizeof( Mat4f ) }, { E_TYPE::COLOR4, sizeof( Vec4f ) }
		};

		std::unique_ptr<GL::ProgramManager>								  _programManager;
		std::unordered_map<std::string, std::unique_ptr<GL::VertexArray>> _vaos;
		std::unordered_map<std::string, std::unique_ptr<GL::Buffer>>	  _bos;
		std::unique_ptr<GL::Buffer>										  _ubo;

		// TODO: check if mapping is useful.
		std::unordered_map<const Attachment *, std::unique_ptr<GL::Texture2D>> _textures;
		std::unordered_map<const Program *, const GL::Program * const>		   _programs;
		std::unordered_map<const Pass *, std::unique_ptr<GL::Framebuffer>>	   _fbos;
		std::unordered_map<const Program *, std::unique_ptr<GL::Buffer>>	   _ubos;

		struct StructUniformEntry
		{
			GL::Buffer * buffer;
			size_t		 offset;
			size_t		 size;
			void *		 value;
			StructUniformEntry( GL::Buffer * p_buffer, const size_t p_offset, const size_t p_size ) :
				buffer( p_buffer ), offset( p_offset ), size( p_size ), value( malloc( p_size ) )
			{
			}
			~StructUniformEntry() { free( value ); }
		};
		std::unordered_map<std::string, std::unique_ptr<StructUniformEntry>> _uniforms;

		void _createUniforms(
			GL::Buffer * const	  p_ubo,
			const Uniforms &	  p_uniforms,
			const Program * const p_descProgram = nullptr
		)
		{
			size_t offset = 0;
			for ( const Uniform & descUniform : p_uniforms )
			{
				size_t		size = _mapTypeSizes[ descUniform.type ];
				std::string key	 = ( p_descProgram ? p_descProgram->name : "" ) + descUniform.name;

				assert( _uniforms.find( key ) == _uniforms.end() );

				_uniforms.emplace( key, std::make_unique<StructUniformEntry>( p_ubo, offset, size ) );
				offset += size;
			}

			assert( offset > 0 );

			p_ubo->setData( GLsizei( offset ), GL_STATIC_DRAW );

			for ( const Uniform & descUniform : p_uniforms )
			{
				switch ( descUniform.type )
				{
				case E_TYPE::UINT: _setUniformDefaultValue<uint>( descUniform, p_descProgram ); break;
				case E_TYPE::INT: _setUniformDefaultValue<int>( descUniform, p_descProgram ); break;
				case E_TYPE::FLOAT: _setUniformDefaultValue<float>( descUniform, p_descProgram ); break;
				case E_TYPE::VEC3F: _setUniformDefaultValue<Vec3f>( descUniform, p_descProgram ); break;
				case E_TYPE::VEC4F: _setUniformDefaultValue<Vec4f>( descUniform, p_descProgram ); break;
				case E_TYPE::MAT3F: _setUniformDefaultValue<Mat3f>( descUniform, p_descProgram ); break;
				case E_TYPE::MAT4F: _setUniformDefaultValue<Mat4f>( descUniform, p_descProgram ); break;
				case E_TYPE::COLOR4: _setUniformDefaultValue<Util::Color::Rgba>( descUniform, p_descProgram ); break;
				default: throw std::runtime_error( "unknown type" );
				}
			}
		}

		template<typename T>
		void _setUniformDefaultValue( const Uniform & p_descUniform, const Program * const p_descProgram = nullptr )
		{
			assert( std::holds_alternative<StructUniformValue<T>>( p_descUniform.value ) );

			std::string key = ( p_descProgram ? p_descProgram->name : "" ) + p_descUniform.name;
			setUniform( std::get<StructUniformValue<T>>( p_descUniform.value ).value, key );
		}
	};

} // namespace VTX::Renderer::Context

#endif
