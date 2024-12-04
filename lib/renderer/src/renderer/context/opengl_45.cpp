#include "renderer/context/opengl_45.hpp"
#include <util/enum.hpp>

namespace VTX::Renderer::Context
{

	OpenGL45::OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc ) :
		BaseContext { p_width, p_height, p_shaderPath }
	{
#ifdef VTX_RENDERER_NO_OPENGL
		VTX_WARNING( "Creating fake OpenGL context" );
#endif

		assert( p_width > 0 );
		assert( p_height > 0 );

		// Load opengl 4.5.
		// With external loader.
		if ( p_proc && gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
		{
			throw GLException( "Failed to load OpenGL" );
		}
		// With glad integrated loader.
		else if ( gladLoadGL() == 0 )
		{
			throw GLException( "Failed to load OpenGL" );
		}

		// Check version.
		if ( not GLAD_GL_VERSION_4_5 )
		{
			throw GLException( "OpenGL 4.5 or higher is required" );
		}
		else
		{
			_getOpenglInfos();
		}

		VTX_INFO( "Device: {} {}", _openglInfos.glVendor, _openglInfos.glRenderer );
		VTX_INFO( "OpenGL initialized: {}.{}", GLVersion.major, GLVersion.minor );

		// Program manager.
		_programManager = std::make_unique<GL::ProgramManager>( p_shaderPath );

		// Init quad vao/vbo for deferred shading.
		std::vector<Vec2f> quad = { { -1.f, 1.f }, { -1.f, -1.f }, { 1.f, 1.f }, { 1.f, -1.f } };

		_vertexArrays.emplace( _KEY_QUAD, std::make_unique<GL::VertexArray>() );
		_buffers.emplace( _KEY_QUAD, std::make_unique<GL::Buffer>() );
		auto & vao = _vertexArrays[ _KEY_QUAD ];
		auto & vbo = _buffers[ _KEY_QUAD ];

		vao->bind();
		vao->enableAttribute( 0 );
		vao->setVertexBuffer( 0, *vbo, GLsizei( _mapTypeSizes[ E_TYPE::FLOAT ] * 2 ) );
		vao->setAttributeFormat( 0, 2, _mapTypes[ E_TYPE::FLOAT ] );
		vao->setAttributeBinding( 0, 0 );
		vbo->set( quad, 0, GL_STATIC_DRAW );
		vao->unbind();

		glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );

		glPatchParameteri( GL_PATCH_VERTICES, 4 );
		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 1.f );

		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( _debugMessageCallback, nullptr );
	}

	void OpenGL45::build(
		const RenderQueue &			 p_renderQueue,
		const Links &				 p_links,
		const Handle				 p_output,
		const SharedUniforms &		 p_uniforms,
		Instructions &				 p_outInstructions,
		InstructionsDurationRanges & p_outInstructionsDurationRanges
	)
	{
		// Store created items to free no more used later.
		std::vector<Key> vertexArrays;
		std::vector<Key> buffers;
		std::vector<Key> framebuffers;
		std::vector<Key> textures;
		std::vector<Key> programs;
		std::vector<Key> uniforms;
		vertexArrays.push_back( _KEY_QUAD );
		buffers.push_back( _KEY_QUAD );

		// Clear instructions.
		p_outInstructions.clear();
		p_outInstructionsDurationRanges.clear();

		// Set passes.
		_descPasses.clear();
		for ( const Pass * const descPassPtr : p_renderQueue )
		{
			const Key keyPass = _getKey( *descPassPtr );
			_descPasses.emplace( keyPass, descPassPtr );
		}

		// Output.
		_output = p_output;

		// Create shared buffers.
		if ( not p_uniforms.empty() )
		{
			p_outInstructionsDurationRanges.emplace_back( InstructionsDurationRange { "Start",
																					  p_outInstructions.size() } );

			for ( const SharedUniform & uniform : p_uniforms )
			{
				const Key keyBuffer = _getKey( uniform );
				buffers.push_back( keyBuffer );

				if ( not _buffers.contains( keyBuffer ) )
				{
					_buffers.emplace( keyBuffer, std::make_unique<GL::Buffer>() );
				}
				_createUniforms( _buffers[ keyBuffer ].get(), uniform.uniforms, uniforms );

				assert( _buffers.contains( keyBuffer ) );
				GL::Buffer * const buffer	  = _buffers[ keyBuffer ].get();
				GLenum			   bufferType = uniform.isDynamic ? GL_SHADER_STORAGE_BUFFER : GL_UNIFORM_BUFFER;
				uint			   binding	  = uniform.binding;
				p_outInstructions.emplace_back( [ buffer, bufferType, binding ]()
												{ buffer->bind( bufferType, binding ); } );
			}

			p_outInstructionsDurationRanges.back().last = p_outInstructions.size() - 1;
		}

		for ( const Pass * const descPassPtr : p_renderQueue )
		{
			/////////////////
			// Init resources.
			p_outInstructionsDurationRanges.emplace_back( InstructionsDurationRange { descPassPtr->name,
																					  p_outInstructions.size() } );
			const Key  keyPass	  = _getKey( *descPassPtr );
			const bool isLastPass = descPassPtr == p_renderQueue.back();

			std::set<GLenum> drawBuffers;

			// Create input data.
			_createInputs( p_links, descPassPtr, vertexArrays, buffers, textures );

			// Create FBO.
			if ( not isLastPass )
			{
				Key keyFramebuffer = _getKey( *descPassPtr );
				framebuffers.push_back( keyFramebuffer );

				if ( not _framebuffers.contains( keyFramebuffer ) )
				{
					_framebuffers.emplace( keyFramebuffer, std::make_unique<GL::Framebuffer>() );
				}

				// Create outputs.
				_createOuputs( descPassPtr, drawBuffers, textures );

				// Set draw buffers.
				if ( not drawBuffers.empty() )
				{
					_framebuffers[ keyFramebuffer ]->setDrawBuffers(
						std::vector<GLenum>( drawBuffers.begin(), drawBuffers.end() )
					);
				}
			}

			// Create programs.
			for ( const Program & descProgram : descPassPtr->programs )
			{
				const GL::Program * const program = _programManager->createProgram(
					descProgram.name, descProgram.shaders, descProgram.toInject, descProgram.suffix
				);

				const Key keyProgram = _getKey( descPassPtr, descProgram );
				programs.push_back( keyProgram );

				if ( not _programs.contains( keyProgram ) )
				{
					_programs.emplace( keyProgram, program );
				}

				// Uniforms.
				if ( not descProgram.uniforms.empty() )
				{
					const Key keyBuffer = _getKey( descPassPtr, descProgram );
					buffers.push_back( keyBuffer );

					if ( not _buffers.contains( keyBuffer ) )
					{
						_buffers.emplace( keyBuffer, std::make_unique<GL::Buffer>() );
					}
					_createUniforms(
						_buffers[ keyBuffer ].get(), descProgram.uniforms, uniforms, &descProgram, descPassPtr
					);
				}
			}

			////////////////////////
			// Enqueue instructions.

			// Enable options.
			bool hasDepthComponent = _hasDepthComponent( descPassPtr );

			if ( hasDepthComponent )
			{
				p_outInstructions.emplace_back(
					[]()
					{
						glEnable( GL_DEPTH_TEST );
						glDepthFunc( GL_LESS );
					}
				);
			}

			// Bind fbo.
			if ( not isLastPass )
			{
				GL::Framebuffer * const fbo = _framebuffers[ keyPass ].get();
				p_outInstructions.emplace_back( [ fbo ]() { fbo->bind( GL_DRAW_FRAMEBUFFER ); } );
			}
			else
			{
				const Handle * const output = &_output;
				p_outInstructions.emplace_back( [ output ]()
												{ GL::Framebuffer::bindDefault( *output, GL_DRAW_FRAMEBUFFER ); } );
			}

			// Settings.
			for ( const E_SETTING setting : descPassPtr->settings )
			{
				switch ( setting )
				{
				case E_SETTING::CLEAR:
					p_outInstructions.emplace_back( []() { glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); } );
					break;
				default: break;
				}
			}

			// Bind inputs.
			uint						   channelMax = 0;
			std::map<E_CHAN_IN, const Key> mapBoundAttachments;
			for ( const auto & [ channel, input ] : descPassPtr->inputs )
			{
				const IO & descIO = input.desc;

				if ( std::holds_alternative<Attachment>( descIO ) )
				{
					const auto src = _getInputTextureKey( p_links, descPassPtr, channel );

					if ( uint( channel ) > channelMax )
					{
						channelMax = uint( channel );
					}

					// Bind linked texture.
					if ( src.has_value() )
					{
						const IO & descIOSrc = src.value().first->desc;

						if ( std::holds_alternative<Attachment>( descIOSrc ) )
						{
							const Key keyTexture = src.value().second;
							assert( _textures.contains( keyTexture ) );

							GL::Texture2D * const texture = _textures[ keyTexture ].get();

							p_outInstructions.emplace_back( [ texture, channel = channel ]()
															{ texture->bindToUnit( GLuint( channel ) ); } );
							mapBoundAttachments.emplace( channel, keyTexture );
						}
						else
						{
							throw std::runtime_error( "unknown descriptor type" );
						}
					}
					// Bind prefilled texture.
					else
					{
						const Attachment & attachment = std::get<Attachment>( descIO );

						if ( attachment.data != nullptr )
						{
							const Key keyTexture = _getKey( *descPassPtr, true, uint( channel ) );
							assert( _textures.contains( keyTexture ) );

							GL::Texture2D * const texture = _textures[ keyTexture ].get();

							p_outInstructions.emplace_back( [ texture, channel = channel ]()
															{ texture->bindToUnit( GLuint( channel ) ); } );
							mapBoundAttachments.emplace( channel, keyTexture );
						}
						else
						{
							VTX_WARNING( "Input channel {} from pass {} has no source", input.name, descPassPtr->name );
						}
					}
				}
			}

			// Programs.
			for ( const Program & descProgram : descPassPtr->programs )
			{
				const Key keyProgram = _getKey( descPassPtr, descProgram );

				if ( not descProgram.uniforms.empty() )
				{
					assert( _buffers.contains( keyProgram ) );

					GL::Buffer * const buffer = _buffers[ keyProgram ].get();
					assert( buffer != nullptr );

					p_outInstructions.emplace_back(
						[ buffer, channelMax ]()
						{
							// Bind local ubo after last input.
							buffer->bind( GL_UNIFORM_BUFFER, channelMax + 1 );
						}
					);
				}

				assert( _programs.contains( keyProgram ) );
				const GL::Program * const program = _programs[ keyProgram ];

				// Draw custom.
				if ( descProgram.draw.has_value() )
				{
					const Draw &			draw = descProgram.draw.value();
					GL::VertexArray * const vao	 = _vertexArrays[ _getKey( draw ) ].get();

					assert( draw.ranges != nullptr );

					GLenum				   primitive	 = _mapPrimitives[ draw.primitive ];
					Draw::Range * const	   ranges		 = draw.ranges;
					const NeedRenderFunc & needRenderFun = draw.needRenderFunc;

					// Element.
					if ( draw.useIndices )
					{
						const Key keyEbo = _getKey( draw, true );
						assert( _buffers.contains( keyEbo ) );

						GL::Buffer * const ebo = _buffers[ keyEbo ].get();

						p_outInstructions.emplace_back(
							[ program, vao, ebo, primitive, ranges, needRenderFun ]()
							{
								if ( needRenderFun == nullptr || needRenderFun() )
								{
									vao->bind();
									vao->bindElementBuffer( *ebo );
									program->use();
									vao->multiDrawElement(
										primitive,
										(GLsizei *)( ranges->counts.data() ),
										GL_UNSIGNED_INT,
										(GLvoid **)( ranges->offsets.data() ),
										GLsizei( ranges->counts.size() )
									);
									vao->unbindElementBuffer();
									vao->unbind();
								}
							}
						);
					}
					// Array.
					else
					{
						p_outInstructions.emplace_back(
							[ program, vao, primitive, ranges, needRenderFun ]()
							{
								if ( needRenderFun == nullptr || needRenderFun() )
								{
									vao->bind();
									program->use();
									vao->multiDrawArray(
										primitive,
										(GLint *)( ( ranges->offsets.data() ) ),
										(GLsizei *)( ranges->counts.data() ),
										GLsizei( ranges->counts.size() )
									);
									vao->unbind();
								}
							}
						);
					}
				}
				// Or quad.
				else
				{
					assert( _vertexArrays.contains( _KEY_QUAD ) );
					GL::VertexArray * const vao = _vertexArrays[ _KEY_QUAD ].get();
					p_outInstructions.emplace_back(
						[ program, vao ]()
						{
							vao->bind();
							program->use();
							vao->drawArray( GL_TRIANGLE_STRIP, 0, 4 );
							vao->unbind();
						}
					);
				}

				if ( not descProgram.uniforms.empty() )
				{
					assert( _buffers.contains( keyProgram ) );
					GL::Buffer * ubo = _buffers[ keyProgram ].get();
					p_outInstructions.emplace_back( [ ubo ]() { ubo->unbind(); } );
				}
			}

			// Unbind inputs.
			for ( const auto & [ channel, keyTexture ] : mapBoundAttachments )
			{
				assert( _textures.contains( keyTexture ) );
				GL::Texture2D * const texture = _textures[ keyTexture ].get();
				p_outInstructions.emplace_back( [ texture, channel = channel ]()
												{ texture->unbindFromUnit( GLuint( channel ) ); } );
			}

			// Unbind fbo.
			if ( not isLastPass )
			{
				const Key k = _getKey( *descPassPtr );
				assert( _framebuffers.contains( k ) );
				GL::Framebuffer * const fbo = _framebuffers[ k ].get();
				p_outInstructions.emplace_back( [ fbo ]() { fbo->unbind(); } );
			}
			else
			{
				p_outInstructions.emplace_back( []() { GL::Framebuffer::unbindDefault( GL_DRAW_FRAMEBUFFER ); } );
			}

			// Disable options.
			if ( hasDepthComponent )
			{
				p_outInstructions.emplace_back( []() { glDisable( GL_DEPTH_TEST ); } );
			}

			p_outInstructionsDurationRanges.back().last = p_outInstructions.size() - 1;
		}

		p_outInstructionsDurationRanges.emplace_back( InstructionsDurationRange { "End", p_outInstructions.size() } );

		// Unbind shared buffers.
		for ( const SharedUniform & uniform : p_uniforms )
		{
			const Key k = _getKey( uniform );
			assert( _buffers.contains( k ) );
			GL::Buffer * const buffer = _buffers[ k ].get();
			p_outInstructions.emplace_back( [ buffer ]() { buffer->unbind(); } );
		}

		p_outInstructionsDurationRanges.back().last = p_outInstructions.size() - 1;

		// Purge unused resources.
		_purgeResources( vertexArrays, buffers, framebuffers, textures, programs, uniforms );
	}

	void OpenGL45::resize( const RenderQueue & p_renderQueue, const size_t p_width, const size_t p_height )
	{
		width  = p_width;
		height = p_height;

		glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );

		for ( const Pass * const descPassPtr : p_renderQueue )
		{
			// Resize only output textures (inputs are fixed for now).
			for ( const auto & [ channel, output ] : descPassPtr->outputs )
			{
				const IO & descIO = output.desc;
				if ( std::holds_alternative<Attachment>( descIO ) )
				{
					Attachment attachment = std::get<Attachment>( descIO );

					const Key keyTexture = _getKey( *descPassPtr, false, uint( channel ) );
					const Key keyFbo	 = _getKey( *descPassPtr );

					if ( _textures.contains( keyTexture ) )
					{
						auto & texture = _textures[ keyTexture ];

						assert( _framebuffers.contains( keyFbo ) );
						auto & fbo = _framebuffers[ keyFbo ];

						const Vec2i size = _getTextureSize( attachment );
						texture->resize( size.x, size.y );
						fbo->attachTexture( *texture, _mapAttachments[ channel ] );
						VTX_TRACE(
							"Texture resized: {} ({}) = {}x{}",
							output.name,
							Util::Enum::enumName( channel ),
							size.x,
							size.y
						);
					}
				}
				else
				{
					throw std::runtime_error( "unknown descriptor type" );
				}
			}
		}
	}

	void OpenGL45::snapshot(
		std::vector<uchar> & p_image,
		const RenderQueue &	 p_renderQueue,
		const Instructions & p_instructions,
		const size_t		 p_width,
		const size_t		 p_height
	)
	{
		const size_t widthOld  = width;
		const size_t heightOld = height;
		const Handle outputOld = _output;

		p_image.resize( p_width * p_height * 4 );

		// Creates a framebuffer with a texture to render "offscreen".
		// It permits to render in a given resolution without resizing the actual renderer.
		GL::Framebuffer fbo;
		GL::Texture2D	texture(
			  p_width, p_height, GL_RGBA32F, GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR
		  );

		fbo.attachTexture( texture, GL_COLOR_ATTACHMENT0 );

		resize( p_renderQueue, p_width, p_height );
		setOutput( fbo.getId() );

		// Render
		for ( const Instruction & instruction : p_instructions )
		{
			instruction();
		}

		// Copy framebuffer data to CPU array.
		fbo.bind( GL_READ_FRAMEBUFFER );
		glReadnPixels(
			0,
			0,
			GLsizei( p_width ),
			GLsizei( p_height ),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			GLsizei( p_image.size() ),
			p_image.data()
		);
		fbo.unbind();

		// Reset all texture sizes and output.
		resize( p_renderQueue, widthOld, heightOld );
		setOutput( outputOld );

		for ( const Instruction & instruction : p_instructions )
		{
			instruction();
		}
	}

	void OpenGL45::compute( const ComputePass & p_pass )
	{
		return;
		// TODO: Create program and uniforms (refacto build).
		const Program & descProgram = p_pass.program;

		std::string definesToInject = "#define LOCAL_SIZE_X " + std::to_string( LOCAL_SIZE_X ) + "\n"
									  + "#define LOCAL_SIZE_Y " + std::to_string( LOCAL_SIZE_Y ) + "\n"
									  + "#define LOCAL_SIZE_Z " + std::to_string( LOCAL_SIZE_Z ) + "\n";

		const GL::Program * const program
			= _programManager->createProgram( descProgram.name, descProgram.shaders, definesToInject );

		// Create and bind buffers.
		for ( ComputePass::Data * const data : p_pass.data )
		{
			if ( not _computeBuffers.contains( data ) )
			{
				_computeBuffers.emplace( data, std::make_unique<GL::Buffer>( GLsizei( data->size ), data->data ) );
			}

			_computeBuffers[ data ]->bind( GL_SHADER_STORAGE_BUFFER, data->binding );
		}

		program->use();

		// Compute size and dispath.
		uint x, y, z;
		if ( std::holds_alternative<Vec3i>( p_pass.size ) )
		{
			Vec3i v = std::get<Vec3i>( p_pass.size );
			x		= v.x;
			y		= v.y;
			z		= v.z;
		}
		else if ( std::holds_alternative<size_t>( p_pass.size ) )
		{
			// TODO: disatch on other dimensions.
			size_t	  size			  = std::get<size_t>( p_pass.size );
			const int workGroupNeeded = int( ( float( size ) / LOCAL_SIZE_X ) + 1.f );
			assert( workGroupNeeded <= _openglInfos.glMaxComputeWorkGroupCount[ 0 ] );
			x = std::min( workGroupNeeded, _openglInfos.glMaxComputeWorkGroupCount[ 0 ] );
			y = 1;
			z = 1;
		}

		assert( x && y && z );

		glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
		glDispatchCompute( x, y, z );
		glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

		// Unbind buffers.
		for ( ComputePass::Data * const data : p_pass.data )
		{
			_computeBuffers[ data ]->unbind();
		}
	}

	void OpenGL45::clearComputeBuffers( std::optional<std::vector<ComputePass::Data *>> p_buffers )
	{
		if ( p_buffers.has_value() )
		{
			for ( ComputePass::Data * const data : p_buffers.value() )
			{
				_computeBuffers.erase( data );
			}
		}
		else
		{
			_computeBuffers.clear();
		}
	}

	void OpenGL45::_createInputs(
		const Links &	   p_links,
		const Pass * const p_descPassPtr,
		std::vector<Key> & p_vertexArrays,
		std::vector<Key> & p_buffers,
		std::vector<Key> & p_textures
	)
	{
		for ( const auto & [ channel, input ] : p_descPassPtr->inputs )
		{
			VTX_TRACE( "Creating input: {}", input.name );

			const IO & descIO = input.desc;

			// Create texture if data provided and not linked.
			if ( std::holds_alternative<Attachment>( descIO ) )
			{
				const Attachment & attachment = std::get<Attachment>( descIO );
				const auto		   src		  = _getInputTextureKey( p_links, p_descPassPtr, channel );

				if ( not src.has_value() && attachment.data != nullptr )
				{
					_createTexture( descIO, _getKey( *p_descPassPtr, true, uint( channel ) ), p_textures );
				}
			}
			// Create vao if data provided.
			else if ( std::holds_alternative<Data>( descIO ) )
			{
				const Data & data = std::get<Data>( descIO );

				// Create vao.
				const Key keyVao = _getKey( input );
				const Key keyEbo = _getKey( input, true );

				p_vertexArrays.push_back( keyVao );
				p_buffers.push_back( keyEbo );

				if ( not _vertexArrays.contains( keyVao ) )
				{
					_vertexArrays.emplace( keyVao, std::make_unique<GL::VertexArray>() );
				}

				if ( not _buffers.contains( keyEbo ) )
				{
					_buffers.emplace( keyEbo, std::make_unique<GL::Buffer>() );
				}

				auto & vaoData = _vertexArrays[ keyVao ];
				auto & eboData = _buffers[ keyEbo ];

				vaoData->bindElementBuffer( *eboData );

				GLuint chan = 0;
				for ( const Data::Entry & entry : data.entries )
				{
					const Key keyData = _getKey( input, entry );
					p_buffers.push_back( keyData );

					if ( not _buffers.contains( keyData ) )
					{
						_buffers.emplace( keyData, std::make_unique<GL::Buffer>() );
					}

					auto & vbo = _buffers[ keyData ];
					vaoData->enableAttribute( chan );
					vaoData->setVertexBuffer(
						chan, *vbo, GLint( entry.components ) * GLsizei( _mapTypeSizes[ entry.nativeType ] )
					);
					vaoData->setAttributeFormat(
						chan, GLint( entry.components ), GLint( _mapTypes[ entry.nativeType ] )
					);
					vaoData->setAttributeBinding( chan, chan );
					chan++;
				}
			}
		}
	}

	void OpenGL45::_createOuputs(
		const Pass * const p_pass,
		std::set<GLenum> & p_drawBuffers,
		std::vector<Key> & p_textures
	)
	{
		for ( const auto & [ channel, output ] : p_pass->outputs )
		{
			VTX_TRACE( "Creating output: {} ({})", output.name, Util::Enum::enumName( channel ) );
			const IO & descIO = output.desc;
			if ( std::holds_alternative<Attachment>( descIO ) )
			{
				const Key keyTexture = _getKey( *p_pass, false, uint( channel ) );
				_createTexture( descIO, keyTexture, p_textures );

				// Attach.
				const Key keyFbo = _getKey( *p_pass );
				assert( _framebuffers.contains( keyFbo ) );
				assert( _textures.contains( keyTexture ) );

				auto & fbo = _framebuffers[ keyFbo ];
				fbo->attachTexture( *_textures[ keyTexture ], _mapAttachments[ channel ] );
				if ( channel == E_CHAN_OUT::DEPTH ) {}
				else
				{
					p_drawBuffers.insert( _mapAttachments[ channel ] );
				}
			}
			else
			{
				throw std::runtime_error( "unknown descriptor type" );
			}
		}
	}

	std::optional<std::pair<const Output * const, const OpenGL45::Key>> OpenGL45::_getInputTextureKey(
		const Links &	   p_links,
		const Pass * const p_pass,
		const E_CHAN_IN	   p_channel
	)
	{
		const auto it = std::find_if(
			p_links.begin(),
			p_links.end(),
			[ p_pass, p_channel ]( const std::unique_ptr<Link> & p_e )
			{ return p_e->dest == p_pass && p_e->channelDest == p_channel; }
		);

		if ( it == p_links.end() )
		{
			return std::nullopt;
		}

		const Output & srcOutput = ( it->get()->src->outputs[ it->get()->channelSrc ] );

		const Pass * srcPass = nullptr;
		for ( const auto & [ key, pass ] : _descPasses )
		{
			for ( const auto & [ channel, output ] : pass->outputs )
			{
				if ( &output == &srcOutput )
				{
					srcPass = pass;
				}
			}
		}

		assert( srcPass != nullptr );
		return std::make_pair( &srcOutput, _getKey( *srcPass, false, uint( it->get()->channelSrc ) ) );
	}

	bool OpenGL45::_hasDepthComponent( const Pass * const p_descPassPtr ) const
	{
		for ( const auto & [ channel, output ] : p_descPassPtr->outputs )
		{
			const IO & descIO = output.desc;
			if ( std::holds_alternative<Attachment>( descIO ) )
			{
				if ( channel == E_CHAN_OUT::DEPTH )
				{
					return true;
				}
			}
		}
		return false;
	}

	void OpenGL45::_createTexture( const IO & p_descIO, const Key p_key, std::vector<Key> & p_textures )
	{
		const Attachment & attachment = std::get<Attachment>( p_descIO );

		p_textures.push_back( p_key );

		if ( not _textures.contains( p_key ) )
		{
			VTX_TRACE(
				"Creating texture: {} ({} - {})",
				p_key,
				Util::Enum::enumName( attachment.format ),
				_mapFormats[ attachment.format ]
			);

			Vec2i size = _getTextureSize( attachment );

			_textures.emplace(
				p_key,
				std::make_unique<GL::Texture2D>(
					size.x,
					size.y,
					_mapFormats[ attachment.format ],
					_mapWrappings[ attachment.wrappingS ],
					_mapWrappings[ attachment.wrappingT ],
					_mapFilterings[ attachment.filteringMin ],
					_mapFilterings[ attachment.filteringMag ]
				)
			);

			auto & texture = _textures[ p_key ];
			assert( texture != nullptr );

			VTX_TRACE( "Texture created ({}x{})", texture->getWidth(), texture->getHeight() );
			if ( attachment.data != nullptr )
			{
				texture->fill( attachment.data );
			}
		}
	}

	Vec2i OpenGL45::_getTextureSize( const Attachment & p_Attachment ) const
	{
		// Texture size can be:
		// - fixed size.
		// - relative size to the renderer size.
		// - same as the renderer size.
		const size_t textureWidth = p_Attachment.width.has_value()
										? std::holds_alternative<float>( p_Attachment.width.value() )
											  ? size_t( std::get<float>( p_Attachment.width.value() ) * width )
											  : std::get<size_t>( p_Attachment.width.value() )
										: width;

		const size_t textureHeight = p_Attachment.height.has_value()
										 ? std::holds_alternative<float>( p_Attachment.height.value() )
											   ? size_t( std::get<float>( p_Attachment.height.value() ) * height )
											   : std::get<size_t>( p_Attachment.height.value() )
										 : height;

		return Vec2i( textureWidth, textureHeight );
	}

	void OpenGL45::_createUniforms(
		GL::Buffer * const	  p_ubo,
		const Uniforms &	  p_uniforms,
		std::vector<Key> &	  p_uniformKeys,
		const Program * const p_descProgram,
		const Pass * const	  p_descPass

	)
	{
		// Create uniform entries.
		size_t offset = 0;
		for ( const Uniform & descUniform : p_uniforms )
		{
			size_t		size = _mapTypeSizes[ descUniform.type ];
			std::string key	 = _getKey( p_descPass, p_descProgram, descUniform );
			p_uniformKeys.emplace_back( key );

			if ( _uniforms.contains( key ) )
			{
				continue;
			}

			// Auto padding to 4, 8 or 16 bytes.
			size_t padding = 0;
			if ( size % 4 != 0 )
			{
				padding = 4 - ( size % 4 );
			}
			else if ( size > 4 && size % 8 != 0 )
			{
				padding = 8 - ( size % 8 );
			}
			else if ( size > 8 && size % 16 != 0 )
			{
				padding = 16 - ( size % 16 );
			}

			assert( size > 0 );

			_uniforms.emplace( key, std::make_unique<_StructUniformEntry>( p_ubo, offset, size, padding ) );
			VTX_TRACE( "Register uniform: {} (s{})(o{})(p{})", key, size, offset, padding );

			offset += size;
			offset += padding;
		}

		// Set totalSize.
		size_t totalSize = offset;

		if ( totalSize == 0 )
		{
			return;
		}

		for ( const Uniform & descUniform : p_uniforms )
		{
			std::string key				= _getKey( p_descPass, p_descProgram, descUniform );
			_uniforms[ key ]->totalSize = totalSize;
		}

		// Init ubo.
		p_ubo->set( GLsizei( totalSize ), 0, GL_STATIC_DRAW );

		// Fill default values.
		for ( const Uniform & descUniform : p_uniforms )
		{
			switch ( descUniform.type )
			{
			case E_TYPE::BOOL: _setUniformDefaultValue<bool>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::BYTE: _setUniformDefaultValue<char>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::UBYTE: _setUniformDefaultValue<uchar>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::SHORT: _setUniformDefaultValue<short>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::USHORT: _setUniformDefaultValue<ushort>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::INT: _setUniformDefaultValue<int>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::UINT: _setUniformDefaultValue<uint>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::FLOAT: _setUniformDefaultValue<float>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::VEC2I: _setUniformDefaultValue<Vec2i>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::VEC2F: _setUniformDefaultValue<Vec2f>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::VEC3F: _setUniformDefaultValue<Vec3f>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::VEC4F: _setUniformDefaultValue<Vec4f>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::MAT3F: _setUniformDefaultValue<Mat3f>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::MAT4F: _setUniformDefaultValue<Mat4f>( descUniform, p_descProgram, p_descPass ); break;
			case E_TYPE::COLOR4:
				_setUniformDefaultValue<Util::Color::Rgba>( descUniform, p_descProgram, p_descPass );
				break;
			default: throw std::runtime_error( "unknown type: " + std::to_string( int( descUniform.type ) ) );
			}
		}
	}

	void OpenGL45::_purgeResources(
		const std::vector<Key> & vertexArrays,
		const std::vector<Key> & buffers,
		const std::vector<Key> & framebuffers,
		const std::vector<Key> & textures,
		const std::vector<Key> & programs,
		const std::vector<Key> & uniforms
	)
	{
		std::erase_if(
			_vertexArrays,
			[ &vertexArrays ]( const auto & p )
			{ return std::find( vertexArrays.begin(), vertexArrays.end(), p.first ) == vertexArrays.end(); }
		);

		std::erase_if(
			_buffers,
			[ &buffers ]( const auto & p )
			{ return std::find( buffers.begin(), buffers.end(), p.first ) == buffers.end(); }
		);

		std::erase_if(
			_framebuffers,
			[ &framebuffers ]( const auto & p )
			{ return std::find( framebuffers.begin(), framebuffers.end(), p.first ) == framebuffers.end(); }
		);

		std::erase_if(
			_textures,
			[ &textures ]( const auto & p )
			{ return std::find( textures.begin(), textures.end(), p.first ) == textures.end(); }
		);

		std::erase_if(
			_programs,
			[ &programs ]( const auto & p )
			{ return std::find( programs.begin(), programs.end(), p.first ) == programs.end(); }
		);

		std::erase_if(
			_uniforms,
			[ &uniforms ]( const auto & p )
			{ return std::find( uniforms.begin(), uniforms.end(), p.first ) == uniforms.end(); }
		);
	}

	void OpenGL45::_getOpenglInfos()
	{
		_openglInfos.glVendor	 = std::string( (const char *)glGetString( GL_VENDOR ) );
		_openglInfos.glRenderer	 = std::string( (const char *)glGetString( GL_RENDERER ) );
		_openglInfos.glVersion	 = std::string( (const char *)glGetString( GL_VERSION ) );
		_openglInfos.glslVersion = std::string( (const char *)glGetString( GL_SHADING_LANGUAGE_VERSION ) );

		glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &_openglInfos.glMaxUniformBlockSize );
		glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &_openglInfos.glMaxUniformBufferBindings );
		glGetIntegerv( GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &_openglInfos.glMaxShaderStorageBlockSize );
		glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &_openglInfos.glMaxShaderStorageBufferBindings );
		glGetIntegerv( GL_MAX_TEXTURE_SIZE, &_openglInfos.glMaxTextureSize );
		glGetIntegerv( GL_MAX_PATCH_VERTICES, &_openglInfos.glMaxPatchVertices );
		glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &_openglInfos.glMaxTessGenLevel );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &_openglInfos.glMaxComputeWorkGroupCount[ 0 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &_openglInfos.glMaxComputeWorkGroupCount[ 1 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &_openglInfos.glMaxComputeWorkGroupCount[ 2 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &_openglInfos.glMaxComputeWorkGroupSize[ 0 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &_openglInfos.glMaxComputeWorkGroupSize[ 1 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &_openglInfos.glMaxComputeWorkGroupSize[ 2 ] );
		glGetIntegerv( GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &_openglInfos.glMaxComputeWorkGroupInvocations );

		// Extensions.
		GLint numExtensions = 0;
		glGetIntegerv( GL_NUM_EXTENSIONS, &numExtensions );
		VTX_TRACE( "{} GL extensions", numExtensions );
		for ( GLint i = 0; i < numExtensions; ++i )
		{
			const char * extension = (const char *)glGetStringi( GL_EXTENSIONS, i );
			if ( strcmp( "GL_NVX_gpu_memory_info", extension ) == 0 )
			{
				_openglInfos.glExtensions[ GL::E_GL_EXTENSIONS::NVX_gpu_memory_info ] = true;
			}
			if ( strcmp( "GL_ATI_meminfo", extension ) == 0 )
			{
				_openglInfos.glExtensions[ GL::E_GL_EXTENSIONS::ATI_meminfo ] = true;
			}

			// VTX_DEBUG( "GL extension loaded: {}", extension );
		}
	}

	void OpenGL45::fillInfos( StructInfos & p_infos ) const
	{
		p_infos.renderer = _openglInfos.glRenderer;

// NVX_gpu_memory_info
#if ( GL_NVX_gpu_memory_info == 1 )
		if ( _openglInfos.glExtensions[ GL::E_GL_EXTENSIONS::NVX_gpu_memory_info ] )
		{
			int gpuMemoryInfoDedicated, gpuMemoryInfoTotalAvailable, gpuMemoryInfoCurrentAvailable;

			glGetIntegerv( GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &gpuMemoryInfoDedicated );
			glGetIntegerv( GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &gpuMemoryInfoTotalAvailable );
			glGetIntegerv( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &gpuMemoryInfoCurrentAvailable );

			p_infos.gpuMemoryInfoDedicated		  = gpuMemoryInfoDedicated * 1000ll;
			p_infos.gpuMemoryInfoTotalAvailable	  = gpuMemoryInfoTotalAvailable * 1000ll;
			p_infos.gpuMemoryInfoCurrentAvailable = gpuMemoryInfoCurrentAvailable * 1000ll;
		}
#endif
#if ( GL_ATI_meminfo == 1 )
		if ( _openglInfos.glExtensions[ GL::E_GL_EXTENSIONS::ATI_meminfo ] )
		{
			// TODO?
			// VBO_FREE_MEMORY_ATI 0x87FB
			// TEXTURE_FREE_MEMORY_ATI 0x87FC
			// RENDERBUFFER_FREE_MEMORY_ATI 0x87FD
		}
#endif

		// Count total size used by buffers.
		size_t totalSizeBuffers = 0;
		for ( const auto & [ key, buffer ] : _buffers )
		{
			totalSizeBuffers += buffer->size();
		}

		// Count total size used by textures.
		size_t totalSizeTextures = 0;
		for ( const auto & [ key, texture ] : _textures )
		{
			size_t textureSize = texture->getWidth() * texture->getHeight() * _mapFormatSizes[ texture->getFormat() ];
			totalSizeTextures += textureSize;
		}

		p_infos.currentSizeBuffers	= totalSizeBuffers;
		p_infos.currentSizeTextures = totalSizeTextures;

		p_infos.currentCountBuffers	 = _buffers.size();
		p_infos.currentCountTextures = _textures.size();
	}
	void APIENTRY OpenGL45::_debugMessageCallback(
		const GLenum   p_source,
		const GLenum   p_type,
		const GLuint   p_id,
		const GLenum   p_severity,
		const GLsizei  p_length,
		const GLchar * p_msg,
		const void *   p_data
	)
	{
		std::string source;
		std::string type;
		std::string severity;

		switch ( p_source )
		{
		case GL_DEBUG_SOURCE_API: source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source = "WINDOW SYSTEM"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: source = "SHADER COMPILER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: source = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: source = "APPLICATION"; break;
		case GL_DEBUG_SOURCE_OTHER: source = "UNKNOWN"; break;
		default: source = "UNKNOWN"; break;
		}

		switch ( p_type )
		{
		case GL_DEBUG_TYPE_ERROR: type = "ERROR"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = "DEPRECATED BEHAVIOR"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type = "UDEFINED BEHAVIOR"; break;
		case GL_DEBUG_TYPE_PORTABILITY: type = "PORTABILITY"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: type = "PERFORMANCE"; break;
		case GL_DEBUG_TYPE_OTHER: type = "OTHER"; break;
		case GL_DEBUG_TYPE_MARKER: type = "MARKER"; break;
		default: type = "UNKNOWN"; break;
		}

		switch ( p_severity )
		{
		case GL_DEBUG_SEVERITY_HIGH: severity = "HIGH"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: severity = "MEDIUM"; break;
		case GL_DEBUG_SEVERITY_LOW: severity = "LOW"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity = "NOTIFICATION"; break;
		default: severity = "UNKNOWN"; break;
		}

		std::string message( "[" + severity + "] [" + type + "] " + source + ": " + p_msg );

		switch ( p_severity )
		{
		case GL_DEBUG_SEVERITY_HIGH:
			// VTX_ERROR( "{}", message );
			throw GLException( message );
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW: VTX_WARNING( "{}", message ); break;
		default: break;
		}
	}

	std::map<const E_CHAN_OUT, const GLenum> OpenGL45::_mapAttachments = {
		{ E_CHAN_OUT::COLOR_0, GL_COLOR_ATTACHMENT0 },
		{ E_CHAN_OUT::COLOR_1, GL_COLOR_ATTACHMENT1 },
		{ E_CHAN_OUT::COLOR_2, GL_COLOR_ATTACHMENT2 },
		{ E_CHAN_OUT::DEPTH, GL_DEPTH_ATTACHMENT },
	};

	std::map<const E_PRIMITIVE, const GLenum> OpenGL45::_mapPrimitives = { { E_PRIMITIVE::POINTS, GL_POINTS },
																		   { E_PRIMITIVE::LINES, GL_LINES },
																		   { E_PRIMITIVE::TRIANGLES, GL_TRIANGLES },
																		   { E_PRIMITIVE::PATCHES, GL_PATCHES } };

	std::map<const E_FORMAT, const GLenum> OpenGL45::_mapFormats = {
		{ E_FORMAT::RGB16F, GL_RGB16F },
		{ E_FORMAT::RGBA16F, GL_RGBA16F },
		{ E_FORMAT::RGBA32UI, GL_RGBA32UI },
		{ E_FORMAT::RGBA32F, GL_RGBA32F },
		{ E_FORMAT::RG32UI, GL_RG32UI },
		{ E_FORMAT::R8, GL_R8 },
		{ E_FORMAT::R16F, GL_R16F },
		{ E_FORMAT::R32F, GL_R32F },
		{ E_FORMAT::DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F },
	};

	std::map<const GLenum, const size_t> OpenGL45::_mapFormatSizes = {
		{ GL_RGB16F, 6 }, { GL_RGBA16F, 8 }, { GL_RGBA32UI, 16 }, { GL_RGBA32F, 16 },			{ GL_RG32UI, 8 },
		{ GL_R8, 1 },	  { GL_R16F, 2 },	 { GL_R32F, 4 },	  { GL_DEPTH_COMPONENT32F, 4 },
	};

	std::map<const E_WRAPPING, const GLint> OpenGL45::_mapWrappings = {
		{ E_WRAPPING::REPEAT, GL_REPEAT },
		{ E_WRAPPING::MIRRORED_REPEAT, GL_MIRRORED_REPEAT },
		{ E_WRAPPING::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
		{ E_WRAPPING::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER },
		{ E_WRAPPING::MIRROR_CLAMP_TO_EDGE, GL_MIRROR_CLAMP_TO_EDGE },
	};

	std::map<const E_FILTERING, const GLint> OpenGL45::_mapFilterings = {
		{ E_FILTERING::NEAREST, GL_NEAREST },
		{ E_FILTERING::LINEAR, GL_LINEAR },
		{ E_FILTERING::NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST },
		{ E_FILTERING::LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST },
		{ E_FILTERING::NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR },
		{ E_FILTERING::LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR },
	};

	std::map<const E_TYPE, const GLenum> OpenGL45::_mapTypes = { { E_TYPE::BOOL, GL_BOOL },
																 { E_TYPE::BYTE, GL_BYTE },
																 { E_TYPE::UBYTE, GL_UNSIGNED_BYTE },
																 { E_TYPE::SHORT, GL_SHORT },
																 { E_TYPE::USHORT, GL_UNSIGNED_SHORT },
																 { E_TYPE::INT, GL_INT },
																 { E_TYPE::UINT, GL_UNSIGNED_INT },
																 { E_TYPE::FLOAT, GL_FLOAT },
																 { E_TYPE::VEC2I, GL_INT },
																 { E_TYPE::VEC2F, GL_FLOAT },
																 { E_TYPE::VEC3F, GL_FLOAT },
																 { E_TYPE::VEC4F, GL_FLOAT },
																 { E_TYPE::MAT3F, GL_FLOAT },
																 { E_TYPE::MAT4F, GL_FLOAT },
																 { E_TYPE::COLOR4, GL_FLOAT } };

	std::map<const E_TYPE, const size_t> OpenGL45::_mapTypeSizes = {
		{ E_TYPE::BOOL, sizeof( bool ) },	{ E_TYPE::BYTE, sizeof( char ) },	  { E_TYPE::UBYTE, sizeof( uchar ) },
		{ E_TYPE::SHORT, sizeof( short ) }, { E_TYPE::USHORT, sizeof( ushort ) }, { E_TYPE::INT, sizeof( int ) },
		{ E_TYPE::UINT, sizeof( uint ) },	{ E_TYPE::FLOAT, sizeof( float ) },	  { E_TYPE::VEC2I, sizeof( Vec2i ) },
		{ E_TYPE::VEC2F, sizeof( Vec2f ) }, { E_TYPE::VEC3F, sizeof( Vec3f ) },	  { E_TYPE::VEC4F, sizeof( Vec4f ) },
		{ E_TYPE::MAT3F, sizeof( Mat3f ) }, { E_TYPE::MAT4F, sizeof( Mat4f ) },	  { E_TYPE::COLOR4, sizeof( Vec4f ) }
	};

	std::map<const E_FORMAT, const E_TYPE> OpenGL45::_mapFormatTypes = { { E_FORMAT::RG32UI, E_TYPE::UINT } };

	std::map<const E_FORMAT, const GLenum> OpenGL45::_mapFormatInternalTypes = { { E_FORMAT::RG32UI, GL_RG_INTEGER } };
} // namespace VTX::Renderer::Context
