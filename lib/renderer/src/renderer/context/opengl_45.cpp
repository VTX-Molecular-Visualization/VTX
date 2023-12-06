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

		glClearColor( 1.f, 0.f, 0.f, 1.f );
		glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );

		_getOpenglInfos();

		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( _debugMessageCallback, nullptr );

		VTX_INFO( "Device: {} {}", _openglInfos.glVendor, _openglInfos.glRenderer );
		VTX_INFO( "OpenGL initialized: {}.{}", GLVersion.major, GLVersion.minor );
	}

	void OpenGL45::build(
		const RenderQueue & p_renderQueue,
		const Links &		p_links,
		const Handle		p_output,
		const Uniforms &	p_uniforms,
		Instructions &		p_instructions
	)
	{
		assert( p_instructions.empty() );

		// Clear.
		p_instructions.emplace_back(
			[ & ]()
			{
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
				glDepthFunc( GL_LESS );
			}
		);

		// Create shared uniforms.
		if ( p_uniforms.empty() == false )
		{
			_ubo = std::make_unique<GL::Buffer>();
			_createUniforms( _ubo.get(), p_uniforms );
			p_instructions.emplace_back( [ this ]() { _ubo->bind( GL_UNIFORM_BUFFER, 15 ); } );
		}

		for ( const Pass * const descPassPtr : p_renderQueue )
		{
			/////////////////
			// Init resources.
			bool isLastPass		   = descPassPtr == p_renderQueue.back();
			bool hasDepthComponent = false;

			// Create input data.
			_createInputData( descPassPtr );

			// Create FBO.
			if ( isLastPass == false )
			{
				_fbos.emplace( descPassPtr, std::make_unique<GL::Framebuffer>() );

				// Create outputs.
				_createOuputResources( descPassPtr, hasDepthComponent );
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
				if ( descProgram.uniforms.empty() == false )
				{
					_ubos.emplace( &descProgram, std::make_unique<GL::Buffer>() );
					_createUniforms( _ubos[ &descProgram ].get(), descProgram.uniforms, &descProgram );
				}
			}

			////////////////////////
			// Enqueue instructions.

			// Enable options.
			if ( hasDepthComponent )
			{
				p_instructions.emplace_back( []() { glEnable( GL_DEPTH_TEST ); } );
			}

			// Bind fbo.
			if ( isLastPass == false )
			{
				p_instructions.emplace_back( [ this, descPassPtr ]()
											 { _fbos[ descPassPtr ]->bind( GL_DRAW_FRAMEBUFFER ); } );
			}
			else
			{
				GL::Framebuffer::bindDefault( p_output, GL_DRAW_FRAMEBUFFER );
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
			uint channelMax = 0;
			for ( const auto & [ channel, input ] : descPassPtr->inputs )
			{
				const Output * const src	= findInputSrcInLinks( channel );
				const IO &			 descIO = src->desc;

				if ( uint( channel ) > channelMax )
				{
					channelMax = uint( channel );
				}

				if ( src == nullptr )
				{
					VTX_WARNING( "Input channel {} from pass {} as no source", input.name, descPassPtr->name );
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
			for ( const Program & descProgram : descPassPtr->programs )
			{
				if ( descProgram.uniforms.empty() == false )
				{
					assert( _ubos.find( &descProgram ) != _ubos.end() );
					p_instructions.emplace_back(
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
						p_instructions.emplace_back(
							[ this, &program, &draw, &vao, &ebo ]()
							{
								vao->bind();
								vao->bindElementBuffer( *ebo );
								program->use();
								vao->drawArray( _mapPrimitives[ draw.primitive ], 0, GLsizei( *draw.count ) );
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
								vao->drawElement(
									_mapPrimitives[ draw.primitive ], GLsizei( *draw.count ), GL_UNSIGNED_INT
								);
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
			for ( const auto & [ channel, input ] : descPassPtr->inputs )
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
				p_instructions.emplace_back( [ this, descPassPtr ]() { _fbos[ descPassPtr ]->unbind(); } );
			}
			else
			{
				GL::Framebuffer::unbindDefault( GL_DRAW_FRAMEBUFFER );
			}

			// Disable options.
			if ( hasDepthComponent )
			{
				p_instructions.emplace_back( []() { glDisable( GL_DEPTH_TEST ); } );
			}
		}

		// Unbind main ubo.
		if ( p_uniforms.empty() == false )
		{
			p_instructions.emplace_back( [ this ]() { _ubo->unbind(); } );
		}
	}

	void OpenGL45::resize( const size_t p_width, const size_t p_height )
	{
		width  = p_width;
		height = p_height;
		glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );
		for ( auto & [ desc, texture ] : _textures )
		{
			texture->resize( width, height );
		}
	}

	void OpenGL45::_createInputData( const Pass * const p_descPassPtr )
	{
		for ( const auto & [ channel, input ] : p_descPassPtr->inputs )
		{
			const IO & descIO = input.desc;

			if ( std::holds_alternative<Data>( descIO ) )
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
					vaoData->setVertexBuffer( chan, *vbo, GLsizei( _mapTypeSizes[ entry.type ] ) );
					vaoData->setAttributeFormat( chan, GLint( entry.components ), GLint( _mapTypes[ entry.type ] ) );
					vaoData->setAttributeBinding( chan, chan );
					chan++;
				}
			}
		}
	}

	void OpenGL45::_createOuputResources( const Pass * const p_descPassPtr, bool p_hasDepthComponent )
	{
		std::vector<GLenum> drawBuffers;
		for ( const auto & [ channel, output ] : p_descPassPtr->outputs )
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
				_fbos[ p_descPassPtr ]->attachTexture( *_textures[ &attachment ], _mapAttachments[ channel ] );
				if ( channel == E_CHANNEL_OUTPUT::DEPTH )
				{
					p_hasDepthComponent = true;
				}
				else
				{
					drawBuffers.emplace_back( _mapAttachments[ channel ] );
				}
			}
			else
			{
				throw std::runtime_error( "unknown descriptor type" );
			}
		}

		// Set draw buffers.
		if ( drawBuffers.empty() == false )
		{
			_fbos[ p_descPassPtr ]->setDrawBuffers( drawBuffers );
		}
	}

	void OpenGL45::_createUniforms(
		GL::Buffer * const	  p_ubo,
		const Uniforms &	  p_uniforms,
		const Program * const p_descProgram
	)
	{
		size_t offset = 0;
		for ( const Uniform & descUniform : p_uniforms )
		{
			size_t		size = _mapTypeSizes[ descUniform.type ];
			std::string key	 = ( p_descProgram ? p_descProgram->name : "" ) + descUniform.name;

			assert( _uniforms.find( key ) == _uniforms.end() );

			_uniforms.emplace( key, std::make_unique<_StructUniformEntry>( p_ubo, offset, size ) );
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
		for ( GLint i = 0; i < numExtensions; ++i )
		{
			const char * extension = (const char *)glGetStringi( GL_EXTENSIONS, i );
			if ( strcmp( "GL_NVX_gpu_memory_info", extension ) == 0 )
			{
				_openglInfos.glExtensions[ GL::E_GL_EXTENSIONS::NVX_gpu_memory_info ] = true;
			}
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

		std::string message( "[OPENGL] [" + severity + "] [" + type + "] " + source + ": " + p_msg );

		switch ( p_severity )
		{
		case GL_DEBUG_SEVERITY_HIGH:
			std::cerr << message << std::endl;
			throw GLException( message );
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW: std::cout << message << std::endl; break;
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
																		   { E_PRIMITIVE::TRIANGLES, GL_TRIANGLES } };

	std::map<const E_FORMAT, const GLenum> OpenGL45::_mapFormats = {
		{ E_FORMAT::RGBA16F, GL_RGBA16F },
		{ E_FORMAT::RGBA32UI, GL_RGBA32UI },
		{ E_FORMAT::RGBA32F, GL_RGBA32F },
		{ E_FORMAT::RG32UI, GL_RG32UI },
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

	std::map<const E_TYPE, const GLenum> OpenGL45::_mapTypes
		= { { E_TYPE::BOOL, GL_BOOL },	 { E_TYPE::UINT, GL_UNSIGNED_INT }, { E_TYPE::INT, GL_INT },
			{ E_TYPE::FLOAT, GL_FLOAT }, { E_TYPE::VEC3F, GL_FLOAT },		{ E_TYPE::VEC4F, GL_FLOAT },
			{ E_TYPE::MAT3F, GL_FLOAT }, { E_TYPE::MAT4F, GL_FLOAT },		{ E_TYPE::COLOR4, GL_FLOAT } };

	std::map<const E_TYPE, const size_t> OpenGL45::_mapTypeSizes = {
		{ E_TYPE::BOOL, sizeof( bool ) },	{ E_TYPE::UINT, sizeof( uint ) },	{ E_TYPE::INT, sizeof( int ) },
		{ E_TYPE::FLOAT, sizeof( float ) }, { E_TYPE::VEC3F, sizeof( Vec3f ) }, { E_TYPE::VEC4F, sizeof( Vec4f ) },
		{ E_TYPE::MAT3F, sizeof( Mat3f ) }, { E_TYPE::MAT4F, sizeof( Mat4f ) }, { E_TYPE::COLOR4, sizeof( Vec4f ) }
	};
} // namespace VTX::Renderer::Context
