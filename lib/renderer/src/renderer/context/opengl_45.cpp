#include "renderer/context/opengl_45.hpp"

namespace VTX::Renderer::Context
{

	OpenGL45::OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc ) :
		BaseContext { p_width, p_height, p_shaderPath }
	{
		assert( p_width > 0 );
		assert( p_height > 0 );

		if ( gladLoaded )
		{
			VTX_WARNING( "GLAD loaded" );
		}
		else
		{
			VTX_WARNING( "GLAD not loaded" );
		}

		// Load opengl 4.5.
		// With external loader.
		if ( p_proc && gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
		{
			VTX_ERROR( "Failed to load OpenGL" );
			// throw GLException( "Failed to load OpenGL" );
		}
		// With glad integrated loader.
		else if ( gladLoadGL() == 0 )
		{
			VTX_ERROR( "Failed to load OpenGL" );
			// throw GLException( "Failed to load OpenGL" );
		}

		// Check version.
		if ( GLAD_GL_VERSION_4_5 == false )
		{
			VTX_ERROR( "OpenGL 4.5 or higher is required" );
			// throw GLException( "OpenGL 4.5 or higher is required" );
		}
		else
		{
			_getOpenglInfos();
			loaded = true;
		}

		/*
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
		vbo->setData( quad, GL_STATIC_DRAW );
		vao->unbind();

		glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );

		glPatchParameteri( GL_PATCH_VERTICES, 4 );
		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 4.f );

		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( _debugMessageCallback, nullptr );

		VTX_INFO( "Device: {} {}", _openglInfos.glVendor, _openglInfos.glRenderer );
		VTX_INFO( "OpenGL initialized: {}.{}", GLVersion.major, GLVersion.minor );
		*/
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
		// TODO: Compare render queue with previous one and delete resources.
		// for ( const Pass * const descPassPtr : _renderQueue )
		{
			/*
			if ( std::find( p_renderQueue.begin(), p_renderQueue.end(), descPassPtr ) == p_renderQueue.end() )
			{
				// Delete fbo.
				if ( _fbos.find( descPassPtr ) != _fbos.end() )
				{
					_fbos.erase( descPassPtr );
				}

				// Delete inputs.
				for ( const auto & [ channel, input ] : descPassPtr->inputs )
				{
					const IO & descIO = input.desc;
					if ( std::holds_alternative<Attachment>( descIO ) )
					{
						if ( _textures.find( &descIO ) != _textures.end() )
						{
							_textures.erase( &descIO );
						}
					}
					else if ( std::holds_alternative<Data>( descIO ) )
					{
						const Data & data = std::get<Data>( descIO );

						// if ( _vaos.find( input.name ) != _vaos.end() )
						{
							_vaos.erase( input.name );
						}
						for ( const Data::Entry & entry : data.entries )
						{
							_bos.erase( input.name + entry.name );
						}
						if ( _bos.find( input.name + "Ebo" ) != _bos.end() )
						{
							_bos.erase( input.name + "Ebo" );
						}
					}
				}

				// Delete outputs.
				for ( const auto & [ channel, output ] : descPassPtr->outputs )
				{
					const IO & descIO = output.desc;
					if ( std::holds_alternative<Attachment>( descIO ) )
					{
						if ( _textures.find( &descIO ) != _textures.end() )
						{
							_textures.erase( &descIO );
						}
					}
				}
			}
			*/
		}

		// Clear instructions.
		p_outInstructions.clear();
		p_outInstructionsDurationRanges.clear();

		_output = p_output;

		// Create shared buffers.
		if ( p_uniforms.empty() == false )
		{
			p_outInstructionsDurationRanges.emplace_back( InstructionsDurationRange { "Start",
																					  p_outInstructions.size() } );

			for ( const SharedUniform & uniform : p_uniforms )
			{
				const Key k = _getKey( uniform );

				if ( _buffers.contains( k ) == false )
				{
					_buffers.emplace( k, std::make_unique<GL::Buffer>() );
					_createUniforms( _buffers[ k ].get(), uniform.uniforms );
				}

				GL::Buffer * const buffer	  = _buffers[ k ].get();
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
			const Key keyPass = _getKey( *descPassPtr );
			_descPasses.emplace( keyPass, descPassPtr );
			const bool isLastPass = descPassPtr == p_renderQueue.back();

			// Check if already created.
			// assert( _fbos.find( descPassPtr ) == _fbos.end() );
			{
				std::vector<GLenum> drawBuffers;

				// Create input data.
				_createInputs( p_links, descPassPtr );

				// Create FBO.
				if ( isLastPass == false )
				{
					_framebuffers.emplace( keyPass, std::make_unique<GL::Framebuffer>() );

					// Create outputs.
					_createOuputs( descPassPtr, drawBuffers );

					// Set draw buffers.
					if ( drawBuffers.empty() == false )
					{
						_framebuffers[ keyPass ]->setDrawBuffers( drawBuffers );
					}
				}

				// Create programs.
				std::vector<uint> offsets;
				for ( const Program & descProgram : descPassPtr->programs )
				{
					const GL::Program * const program = _programManager->createProgram(
						descProgram.name, descProgram.shaders, descProgram.toInject, descProgram.suffix
					);

					const Key k = _getKey( descProgram );
					_programs.emplace( k, program );

					// Uniforms.
					if ( descProgram.uniforms.empty() == false )
					{
						_buffers.emplace( k, std::make_unique<GL::Buffer>() );
						_createUniforms( _buffers[ k ].get(), descProgram.uniforms, &descProgram, descPassPtr );
					}
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
			if ( isLastPass == false )
			{
				GL::Framebuffer * const fbo = _framebuffers[ keyPass ].get();
				p_outInstructions.emplace_back( [ this, fbo ]() { fbo->bind( GL_DRAW_FRAMEBUFFER ); } );
			}
			else
			{
				p_outInstructions.emplace_back( [ this ]()
												{ GL::Framebuffer::bindDefault( _output, GL_DRAW_FRAMEBUFFER ); } );
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
			uint										channelMax = 0;
			std::map<E_CHANNEL_INPUT, const IO * const> mapBoundAttachments;
			for ( const auto & [ channel, input ] : descPassPtr->inputs )
			{
				const IO & descIO = input.desc;

				if ( std::holds_alternative<Attachment>( descIO ) )
				{
					const Output * const src	   = _getInputSource( p_links, descPassPtr, channel );
					const IO &			 descIOSrc = src->desc;

					if ( uint( channel ) > channelMax )
					{
						channelMax = uint( channel );
					}

					// Bind linked texture.
					if ( src != nullptr )
					{
						if ( std::holds_alternative<Attachment>( descIOSrc ) )
						{
							const Key			  k		  = _getKey( descIOSrc, *descPassPtr, uint( channel ) );
							GL::Texture2D * const texture = _textures[ k ].get();

							p_outInstructions.emplace_back( [ texture, channel = channel ]()
															{ texture->bindToUnit( GLuint( channel ) ); } );
							mapBoundAttachments.emplace( channel, &descIOSrc );
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
							const Key			  k		  = _getKey( descIOSrc, *descPassPtr, uint( channel ) );
							GL::Texture2D * const texture = _textures[ k ].get();

							p_outInstructions.emplace_back( [ texture, channel = channel ]()
															{ texture->bindToUnit( GLuint( channel ) ); } );
							mapBoundAttachments.emplace( channel, &descIO );
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
				const Key k = _getKey( descProgram );

				if ( descProgram.uniforms.empty() == false )
				{
					assert( _buffers.contains( k ) );

					GL::Buffer * const buffer = _buffers[ k ].get();
					p_outInstructions.emplace_back(
						[ this, buffer, channelMax ]()
						{
							// Bind local ubo after last input.
							buffer->bind( GL_UNIFORM_BUFFER, channelMax + 1 );
						}
					);
				}

				assert( _programs.contains( k ) );
				const GL::Program * const program = _programs[ k ];

				// Draw custom.
				if ( descProgram.draw.has_value() )
				{
					const Draw &			draw = descProgram.draw.value();
					GL::VertexArray * const vao	 = _vertexArrays[ _getKey( draw ) ].get();
					// Element.
					if ( draw.useIndices )
					{
						GL::Buffer * const ebo = _buffers[ _getKey( draw, true ) ].get();
						p_outInstructions.emplace_back(
							[ this, program, &draw, vao, ebo ]()
							{
								size_t count = draw.countFunction();
								if ( count > 0 )
								{
									vao->bind();
									vao->bindElementBuffer( *ebo );
									program->use();
									vao->drawElement(
										_mapPrimitives[ draw.primitive ], GLsizei( count ), GL_UNSIGNED_INT
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
							[ this, program, &draw, vao ]()
							{
								size_t count = draw.countFunction();
								if ( count > 0 )
								{
									vao->bind();
									program->use();
									vao->drawArray( _mapPrimitives[ draw.primitive ], 0, GLsizei( count ) );
									vao->unbind();
								}
							}
						);
					}
				}
				// Or quad.
				else
				{
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

				if ( descProgram.uniforms.empty() == false )
				{
					GL::Buffer * ubo = _buffers[ k ].get();
					p_outInstructions.emplace_back( [ ubo ]() { ubo->unbind(); } );
				}
			}

			// Unbind inputs.
			for ( const auto & [ channel, descIOPtr ] : mapBoundAttachments )
			{
				const Key			  k		  = _getKey( *descIOPtr, *descPassPtr, uint( channel ) );
				GL::Texture2D * const texture = _textures[ k ].get();
				p_outInstructions.emplace_back( [ texture, channel = channel ]()
												{ texture->unbindFromUnit( GLuint( channel ) ); } );
			}

			// Unbind fbo.
			if ( isLastPass == false )
			{
				const Key				k	= _getKey( *descPassPtr );
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
			const Key		   k	  = _getKey( uniform );
			GL::Buffer * const buffer = _buffers[ k ].get();
			p_outInstructions.emplace_back( [ buffer ]() { buffer->unbind(); } );
		}

		// glFinish();

		p_outInstructionsDurationRanges.back().last = p_outInstructions.size() - 1;

		// Backup render queue for next build.
		//_renderQueue = p_renderQueue;
	}

	void OpenGL45::resize( const RenderQueue & p_renderQueue, const size_t p_width, const size_t p_height )
	{
		width  = p_width;
		height = p_height;

		VTX_DEBUG( "Resizing to {}x{}", width, height );

		glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );

		for ( const Pass * const descPassPtr : p_renderQueue )
		{
			// Resize only output textures (inputs are fixed for now).
			for ( const auto & [ channel, output ] : descPassPtr->outputs )
			{
				const IO & descIO = output.desc;
				if ( std::holds_alternative<Attachment>( descIO ) )
				{
					// TODO: check if still needed.

					const Key k = _getKey( descIO, *descPassPtr, uint( channel ) );
					if ( _textures.contains( k ) )
					{
						auto & texture = _textures[ k ];
						VTX_DEBUG(
							"Texture resized ({}x{} => {}x{})", texture->getWidth(), texture->getHeight(), width, height
						);
						_textures[ k ]->resize( width, height );
						_framebuffers[ _getKey( *descPassPtr ) ]->attachTexture(
							*_textures[ k ], _mapAttachments[ channel ]
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
		// TODO: transparency.

		const size_t widthOld  = width;
		const size_t heightOld = height;
		const Handle outputOld = _output;

		p_image.resize( p_width * p_height * 4 );

		GL::Framebuffer fbo;
		GL::Texture2D	texture(
			  p_width, p_height, GL_RGBA32F, GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR
		  );

		fbo.attachTexture( texture, GL_COLOR_ATTACHMENT0 );

		resize( p_renderQueue, p_width, p_height );
		setOutput( fbo.getId() );

		for ( const Instruction & instruction : p_instructions )
		{
			instruction();
		}

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

		resize( p_renderQueue, widthOld, heightOld );
		setOutput( outputOld );

		for ( const Instruction & instruction : p_instructions )
		{
			instruction();
		}
	}

	void OpenGL45::getTextureData(
		std::any &			   p_textureData,
		const size_t		   p_x,
		const size_t		   p_y,
		const Key &			   p_key,
		const E_CHANNEL_OUTPUT p_channel
	)
	{
		assert( _framebuffers.contains( p_key ) );

		GL::Framebuffer * const fbo	   = _framebuffers[ p_key ].get();
		const Pass * const		pass   = _descPasses[ p_key ];
		const IO &				descIO = pass->outputs.at( p_channel ).desc;

		assert( std::holds_alternative<Attachment>( descIO ) );

		const Attachment & attachment = std::get<Attachment>( descIO );
		const E_FORMAT	   format	  = attachment.format;

		fbo->bind( GL_READ_FRAMEBUFFER );
		fbo->setReadBuffer( _mapAttachments[ p_channel ] );
		glReadPixels(
			GLint( p_x ),
			GLint( p_y ),
			1,
			1,
			_mapFormatInternalTypes[ format ],
			_mapTypes[ _mapFormatTypes[ format ] ],
			&p_textureData
		);
		fbo->unbind();
	}

	void OpenGL45::_createInputs( const Links & p_links, const Pass * const p_descPassPtr )
	{
		for ( const auto & [ channel, input ] : p_descPassPtr->inputs )
		{
			const IO & descIO = input.desc;

			// Create texture if data provided and not linked.
			if ( std::holds_alternative<Attachment>( descIO ) )
			{
				const Attachment &	 attachment = std::get<Attachment>( descIO );
				const Output * const source		= _getInputSource( p_links, p_descPassPtr, channel );

				if ( source == nullptr && attachment.data != nullptr )
				{
					_createAttachment( descIO, _getKey( descIO, *p_descPassPtr, uint( channel ) ) );
				}
			}
			// Create vao if data provided.
			else if ( std::holds_alternative<Data>( descIO ) )
			{
				const Data & data = std::get<Data>( descIO );

				// Create vao.
				const Key keyFbo = _getKey( *p_descPassPtr );
				_vertexArrays.emplace( input.name, std::make_unique<GL::VertexArray>() );
				_buffers.emplace( input.name + "Ebo", std::make_unique<GL::Buffer>() );
				auto & vaoData = _vertexArrays[ input.name ];
				auto & eboData = _buffers[ input.name + "Ebo" ];
				vaoData->bindElementBuffer( *eboData );

				GLuint chan = 0;
				for ( const Data::Entry & entry : data.entries )
				{
					_buffers.emplace( input.name + entry.name, std::make_unique<GL::Buffer>() );
					auto & vbo = _buffers[ input.name + entry.name ];
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

	void OpenGL45::_createOuputs( const Pass * const p_pass, std::vector<GLenum> & p_drawBuffers )
	{
		for ( const auto & [ channel, output ] : p_pass->outputs )
		{
			const IO & descIO = output.desc;
			if ( std::holds_alternative<Attachment>( descIO ) )
			{
				const Key k = _getKey( descIO, *p_pass, uint( channel ) );
				_createAttachment( descIO, k );

				// Attach.
				_framebuffers[ _getKey( *p_pass ) ]->attachTexture( *_textures[ k ], _mapAttachments[ channel ] );
				if ( channel == E_CHANNEL_OUTPUT::DEPTH ) {}
				else
				{
					p_drawBuffers.emplace_back( _mapAttachments[ channel ] );
				}
			}
			else
			{
				throw std::runtime_error( "unknown descriptor type" );
			}
		}
	}

	const Output * const OpenGL45::_getInputSource(
		const Links &		  p_links,
		const Pass * const	  p_pass,
		const E_CHANNEL_INPUT p_channel
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
			return nullptr;
		}

		return &( it->get()->src->outputs[ it->get()->channelSrc ] );
	}

	bool OpenGL45::_hasDepthComponent( const Pass * const p_descPassPtr ) const
	{
		for ( const auto & [ channel, output ] : p_descPassPtr->outputs )
		{
			const IO & descIO = output.desc;
			if ( std::holds_alternative<Attachment>( descIO ) )
			{
				if ( channel == E_CHANNEL_OUTPUT::DEPTH )
				{
					return true;
				}
			}
		}
		return false;
	}

	void OpenGL45::_createAttachment( const IO & p_descIO, const Key p_key )
	{
		const Attachment & attachment = std::get<Attachment>( p_descIO );

		if ( _textures.contains( p_key ) )
		{
			return;
		}

		_textures.emplace(
			p_key,
			std::make_unique<GL::Texture2D>(
				attachment.width.has_value() ? attachment.width.value() : width,
				attachment.height.has_value() ? attachment.height.value() : height,
				_mapFormats[ attachment.format ],
				_mapWrappings[ attachment.wrappingS ],
				_mapWrappings[ attachment.wrappingT ],
				_mapFilterings[ attachment.filteringMin ],
				_mapFilterings[ attachment.filteringMag ]
			)
		);

		GL::Texture2D * texture = _textures[ p_key ].get();
		VTX_DEBUG( "Texture created ({}x{})", texture->getWidth(), texture->getHeight() );
		if ( attachment.data != nullptr )
		{
			texture->fill( attachment.data );
		}
	}

	void OpenGL45::_createUniforms(
		GL::Buffer * const	  p_ubo,
		const Uniforms &	  p_uniforms,
		const Program * const p_descProgram,
		const Pass * const	  p_descPass
	)
	{
		// Create uniform entries.
		size_t offset = 0;
		for ( const Uniform & descUniform : p_uniforms )
		{
			size_t		size = _mapTypeSizes[ descUniform.type ];
			std::string key	 = ( p_descPass ? p_descPass->name : "" ) + ( p_descProgram ? p_descProgram->name : "" )
							  + descUniform.name;

			if ( _uniforms.find( key ) != _uniforms.end() )
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
			VTX_DEBUG( "Register uniform: {} (s{})(o{})(p{})", key, size, offset, padding );

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
			std::string key = ( p_descPass ? p_descPass->name : "" ) + ( p_descProgram ? p_descProgram->name : "" )
							  + descUniform.name;
			_uniforms[ key ]->totalSize = totalSize;
		}

		// Init ubo.
		p_ubo->setData( GLsizei( totalSize ), GL_STATIC_DRAW );

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

	void OpenGL45::_getOpenglInfos()
	{
		_openglInfos.glVendor	 = std::string( (const char *)glGetString( GL_VENDOR ) );
		_openglInfos.glRenderer	 = std::string( (const char *)glGetString( GL_RENDERER ) );
		_openglInfos.glVersion	 = std::string( (const char *)glGetString( GL_VERSION ) );
		_openglInfos.glslVersion = std::string( (const char *)glGetString( GL_SHADING_LANGUAGE_VERSION ) );

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
		glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &_openglInfos.glMaxUniformBlockSize );
		glGetIntegerv( GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &_openglInfos.glMaxShaderStorageBlockSize );
		glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &_openglInfos.glMaxShaderStorageBufferBindings );

		// Extensions.
		GLint numExtensions = 0;
		glGetIntegerv( GL_NUM_EXTENSIONS, &numExtensions );
		VTX_DEBUG( "{} GL extensions", numExtensions );
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
			totalSizeBuffers += buffer->getSize();
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

	std::map<const E_CHANNEL_OUTPUT, const GLenum> OpenGL45::_mapAttachments = {
		{ E_CHANNEL_OUTPUT::COLOR_0, GL_COLOR_ATTACHMENT0 },
		{ E_CHANNEL_OUTPUT::COLOR_1, GL_COLOR_ATTACHMENT1 },
		{ E_CHANNEL_OUTPUT::COLOR_2, GL_COLOR_ATTACHMENT2 },
		{ E_CHANNEL_OUTPUT::DEPTH, GL_DEPTH_ATTACHMENT },
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
