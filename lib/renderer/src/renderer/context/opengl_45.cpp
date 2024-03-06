#include "renderer/context/opengl_45.hpp"

namespace VTX::Renderer::Context
{

	OpenGL45::OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc ) :
		BaseContext { p_width, p_height, p_shaderPath }
	{
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

		glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );

		glPatchParameteri( GL_PATCH_VERTICES, 4 );
		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 4.f );

		_getOpenglInfos();

		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( _debugMessageCallback, nullptr );

		VTX_INFO( "Device: {} {}", _openglInfos.glVendor, _openglInfos.glRenderer );
		VTX_INFO( "OpenGL initialized: {}.{}", GLVersion.major, GLVersion.minor );
	}

	void OpenGL45::build(
		const RenderQueue &			  p_renderQueue,
		const Links &				  p_links,
		const Handle				  p_output,
		const std::vector<Uniforms> & p_uniforms,
		Instructions &				  p_outInstructions,
		InstructionsDurationRanges &  p_outInstructionsDurationRanges
	)
	{
		assert( p_outInstructions.empty() );

		_output = p_output;

		// Create shared uniforms.
		if ( p_uniforms.empty() == false )
		{
			p_outInstructionsDurationRanges.emplace_back( InstructionsDurationRange { "Start",
																					  p_outInstructions.size() } );
			uint binding = 15;
			for ( const Uniforms & uniforms : p_uniforms )
			{
				_ubosShared.emplace_back( std::make_unique<GL::Buffer>() );
				const auto ubo = _ubosShared.back().get();
				_createUniforms( ubo, uniforms );
				p_outInstructions.emplace_back( [ ubo, binding ]() { ubo->bind( GL_UNIFORM_BUFFER, binding ); } );
				binding--;
			}

			p_outInstructionsDurationRanges.back().last = p_outInstructions.size() - 1;
		}

		for ( const Pass * const descPassPtr : p_renderQueue )
		{
			/////////////////
			// Init resources.
			p_outInstructionsDurationRanges.emplace_back( InstructionsDurationRange { descPassPtr->name,
																					  p_outInstructions.size() } );
			bool				isLastPass = descPassPtr == p_renderQueue.back();
			std::vector<GLenum> drawBuffers;
			bool				hasDepthComponent = false;

			// Create input data.
			_createInputs( descPassPtr );

			// Create FBO.
			if ( isLastPass == false )
			{
				_fbos.emplace( descPassPtr, std::make_unique<GL::Framebuffer>() );

				// Create outputs.
				_createOuputs( descPassPtr, drawBuffers, hasDepthComponent );

				// Set draw buffers.
				if ( drawBuffers.empty() == false )
				{
					_fbos[ descPassPtr ]->setDrawBuffers( drawBuffers );
				}
			}

			// Create programs.
			std::vector<uint> offsets;
			for ( const Program & descProgram : descPassPtr->programs )
			{
				const GL::Program * const program = _programManager->createProgram(
					descProgram.name, descProgram.shaders, descProgram.toInject, descProgram.suffix
				);

				_programs.emplace( &descProgram, program );

				// Uniforms.
				if ( descProgram.uniforms.entries.empty() == false )
				{
					_ubos.emplace( &descProgram, std::make_unique<GL::Buffer>() );
					_createUniforms( _ubos[ &descProgram ].get(), descProgram.uniforms, &descProgram, descPassPtr );
				}
			}

			////////////////////////
			// Enqueue instructions.

			// Enable options.
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
				p_outInstructions.emplace_back( [ this, descPassPtr ]()
												{ _fbos[ descPassPtr ]->bind( GL_DRAW_FRAMEBUFFER ); } );
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

			// Find source for input.
			auto findInputSrcInLinks
				= [ &p_links, descPassPtr ]( const E_CHANNEL_INPUT p_channel ) -> const Output * const
			{
				const auto it = std::find_if(
					p_links.begin(),
					p_links.end(),
					[ descPassPtr, p_channel ]( const std::unique_ptr<Link> & p_e )
					{ return p_e->dest == descPassPtr && p_e->channelDest == p_channel; }
				);

				if ( it == p_links.end() )
				{
					return nullptr;
				}

				return &( it->get()->src->outputs[ it->get()->channelSrc ] );
			};

			// Bind inputs.
			uint										channelMax = 0;
			std::map<E_CHANNEL_INPUT, const IO * const> mapBoundAttachments;
			for ( const auto & [ channel, input ] : descPassPtr->inputs )
			{
				const IO & descIO = input.desc;

				if ( std::holds_alternative<Attachment>( descIO ) )
				{
					const Output * const src	   = findInputSrcInLinks( channel );
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
							p_outInstructions.emplace_back(
								[ this, channel = channel, &descIOSrc ]()
								{ _textures[ &descIOSrc ]->bindToUnit( GLuint( channel ) ); }
							);
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
							p_outInstructions.emplace_back( [ this, channel = channel, &descIO ]()
															{ _textures[ &descIO ]->bindToUnit( GLuint( channel ) ); }
							);
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
				if ( descProgram.uniforms.entries.empty() == false )
				{
					assert( _ubos.find( &descProgram ) != _ubos.end() );
					p_outInstructions.emplace_back(
						[ this, &descProgram, channelMax ]()
						{
							// Bind local ubo after last input.
							_ubos[ &descProgram ]->bind( GL_UNIFORM_BUFFER, channelMax + 1 );
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
						auto & ebo = _bos[ draw.name + "Ebo" ];
						p_outInstructions.emplace_back(
							[ this, &program, &draw, &vao, &ebo ]()
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
							[ this, &program, &draw, &vao ]()
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
					auto & vao = _vaos[ "quad" ];
					p_outInstructions.emplace_back(
						[ &program, &vao ]()
						{
							vao->bind();
							program->use();
							vao->drawArray( GL_TRIANGLE_STRIP, 0, 4 );
							vao->unbind();
						}
					);
				}

				if ( descProgram.uniforms.entries.empty() == false )
				{
					auto & ubo = _ubos[ &descProgram ];
					p_outInstructions.emplace_back( [ &ubo ]() { ubo->unbind(); } );
				}
			}

			// Unbind inputs.
			for ( const auto & [ channel, descIOPtr ] : mapBoundAttachments )
			{
				p_outInstructions.emplace_back( [ this, channel = channel, descIOPtr = descIOPtr ]()
												{ _textures[ descIOPtr ]->unbindFromUnit( GLuint( channel ) ); } );
			}

			// Unbind fbo.
			if ( isLastPass == false )
			{
				p_outInstructions.emplace_back( [ this, descPassPtr ]() { _fbos[ descPassPtr ]->unbind(); } );
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

		// Unbind main ubo.
		for ( const auto & ubo : _ubosShared )
		{
			p_outInstructions.emplace_back( [ &ubo ]() { ubo->unbind(); } );
		}

		// glFinish();

		p_outInstructionsDurationRanges.back().last = p_outInstructions.size() - 1;
	}

	void OpenGL45::resize( const RenderQueue & p_renderQueue, const size_t p_width, const size_t p_height )
	{
		width  = p_width;
		height = p_height;

		glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );

		for ( const Pass * const descPassPtr : p_renderQueue )
		{
			for ( const auto & [ channel, output ] : descPassPtr->outputs )
			{
				const IO & descIO = output.desc;
				if ( std::holds_alternative<Attachment>( descIO ) )
				{
					if ( _textures.find( &descIO ) != _textures.end() )
					{
						_textures[ &descIO ]->resize( width, height );
						_fbos[ descPassPtr ]->attachTexture( *_textures[ &descIO ], _mapAttachments[ channel ] );
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
		const std::string &	   p_pass,
		const E_CHANNEL_OUTPUT p_channel
	) const
	{
		for ( auto & [ passPtr, fbo ] : _fbos )
		{
			if ( passPtr->name == p_pass )
			{
				const IO & descIO = passPtr->outputs.at( p_channel ).desc;

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
				return;
			}
		}
		assert( false );
	}

	void OpenGL45::_createInputs( const Pass * const p_descPassPtr )
	{
		for ( const auto & [ channel, input ] : p_descPassPtr->inputs )
		{
			const IO & descIO = input.desc;

			// Create texture if data provided.
			if ( std::holds_alternative<Attachment>( descIO ) )
			{
				const Attachment & attachment = std::get<Attachment>( descIO );

				if ( attachment.data != nullptr )
				{
					_createAttachment( descIO );
				}
			}
			// Create vao if data provided.
			else if ( std::holds_alternative<Data>( descIO ) )
			{
				const Data & data = std::get<Data>( descIO );

				// Create vao.
				_vaos.emplace( input.name, std::make_unique<GL::VertexArray>() );
				_bos.emplace( input.name + "Ebo", std::make_unique<GL::Buffer>() );
				auto & vaoData = _vaos[ input.name ];
				auto & eboData = _bos[ input.name + "Ebo" ];
				vaoData->bindElementBuffer( *eboData );

				GLuint chan = 0;
				for ( const Data::Entry & entry : data.entries )
				{
					_bos.emplace( input.name + entry.name, std::make_unique<GL::Buffer>() );
					auto & vbo = _bos[ input.name + entry.name ];
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
		const Pass * const	  p_descPassPtr,
		std::vector<GLenum> & p_drawBuffers,
		bool &				  p_hasDepthComponent
	)
	{
		for ( const auto & [ channel, output ] : p_descPassPtr->outputs )
		{
			const IO & descIO = output.desc;
			if ( std::holds_alternative<Attachment>( descIO ) )
			{
				_createAttachment( descIO );

				// Attach.
				_fbos[ p_descPassPtr ]->attachTexture( *_textures[ &descIO ], _mapAttachments[ channel ] );
				if ( channel == E_CHANNEL_OUTPUT::DEPTH )
				{
					p_hasDepthComponent = true;
				}
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

	void OpenGL45::_createAttachment( const IO & p_descIO )
	{
		const Attachment & attachment = std::get<Attachment>( p_descIO );
		_textures.emplace(
			&p_descIO,
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

		if ( attachment.data != nullptr )
		{
			_textures[ &p_descIO ]->fill( attachment.data );
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
		for ( const Uniform & descUniform : p_uniforms.entries )
		{
			size_t		size = _mapTypeSizes[ descUniform.type ];
			std::string key	 = ( p_descPass ? p_descPass->name : "" ) + ( p_descProgram ? p_descProgram->name : "" )
							  + descUniform.name;

			assert( _uniforms.find( key ) == _uniforms.end() );

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

			_uniforms.emplace(
				key, std::make_unique<_StructUniformEntry>( p_ubo, offset, size, padding, p_uniforms.arraySize )
			);
			VTX_DEBUG( "Register uniform: {} (s{})(o{})(p{})", key, size, offset, padding );

			offset += size;
			offset += padding;
		}

		// Set totalSize.
		size_t totalSize = offset;

		assert( totalSize > 0 );

		for ( const Uniform & descUniform : p_uniforms.entries )
		{
			std::string key = ( p_descPass ? p_descPass->name : "" ) + ( p_descProgram ? p_descProgram->name : "" )
							  + descUniform.name;
			_uniforms[ key ]->totalSize = totalSize;
		}

		// Init ubo.
		p_ubo->setData( GLsizei( totalSize ), GL_STATIC_DRAW );

		// Fill default values.
		for ( const Uniform & descUniform : p_uniforms.entries )
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
			else if ( strcmp( "GL_ATI_meminfo", extension ) == 0 )
			{
				_openglInfos.glExtensions[ GL::E_GL_EXTENSIONS::GL_ATI_meminfo ] = true;
			}

			VTX_DEBUG( "GL extension loaded: {}", extension );
		}

// NVX_gpu_memory_info
#if ( GL_NVX_gpu_memory_info == 1 )
		if ( _openglInfos.glExtensions[ GL::E_GL_EXTENSIONS::NVX_gpu_memory_info ] )
		{
			glGetIntegerv( GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &_openglInfos.gpuMemoryInfoDedicatedVidmemNVX );
			glGetIntegerv(
				GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &_openglInfos.gpuMemoryInfoTotalAvailableMemoryNVX
			);
			glGetIntegerv(
				GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &_openglInfos.gpuMemoryInfoCurrentAvailableVidMemNVX
			);
		}
#endif
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
