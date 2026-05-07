#include "renderer/context/backend/opengl.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/context/backend/gl/debug.hpp"
#include <numeric>
#include <util/exceptions.hpp>

namespace
{
	using namespace VTX;
	using namespace VTX::Renderer;

	struct GLPixelFormat
	{
		GLenum	internalFormat = 0; // glTextureStorage2D
		GLenum	uploadFormat   = 0; // glTextureSubImage2D / glClearTexImage
		GLenum	uploadType	   = 0; // glTextureSubImage2D / glClearTexImage
		uint8_t bytesPerPixel  = 0;
		bool	isInteger	   = false;
		bool	isDepth		   = false;
	};

	/**
	 * @brief All GL mapping.
	 */
	constexpr GLPixelFormat _toGL( const Desc::E_FORMAT p_format ) noexcept
	{
		using namespace Desc;

		switch ( p_format )
		{
		case E_FORMAT::RGBA8UI: return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4, false, false };
		case E_FORMAT::RGB16F: return { GL_RGB16F, GL_RGB, GL_HALF_FLOAT, 6, false, false };
		case E_FORMAT::RGBA16F: return { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 8, false, false };
		case E_FORMAT::RGBA32F: return { GL_RGBA32F, GL_RGBA, GL_FLOAT, 16, false, false };
		case E_FORMAT::R16F: return { GL_R16F, GL_RED, GL_HALF_FLOAT, 2, false, false };
		case E_FORMAT::R32F: return { GL_R32F, GL_RED, GL_FLOAT, 4, false, false };
		case E_FORMAT::R8UI: return { GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1, false, false };
		case E_FORMAT::RG32UI: return { GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, 8, true, false };
		case E_FORMAT::RGBA32UI: return { GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, 16, true, false };
		case E_FORMAT::DEPTH_COMPONENT32F:
			return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, 4, false, true };
		default: assert( false ); return {};
		}
	}

	constexpr GLenum _toGL( const Desc::E_WRAPPING p_wrapping ) noexcept
	{
		using namespace Desc;

		switch ( p_wrapping )
		{
		case E_WRAPPING::REPEAT: return GL_REPEAT;
		case E_WRAPPING::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
		case E_WRAPPING::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
		case E_WRAPPING::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
		case E_WRAPPING::MIRROR_CLAMP_TO_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
		default: assert( false ); return GL_INVALID_INDEX;
		}
	}

	constexpr GLenum _toGL( const Desc::E_FILTERING p_filtering ) noexcept
	{
		using namespace Desc;

		switch ( p_filtering )
		{
		case E_FILTERING::NEAREST: return GL_NEAREST;
		case E_FILTERING::LINEAR: return GL_LINEAR;
		case E_FILTERING::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
		case E_FILTERING::LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
		case E_FILTERING::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
		case E_FILTERING::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
		default: assert( false ); return GL_INVALID_INDEX;
		}
	}

	constexpr GLenum _toGLShaderTarget( const Desc::E_BUFFER_USAGE p_usage ) noexcept
	{
		using namespace Desc;

		if ( Util::Enum::hasBits( p_usage, E_BUFFER_USAGE::UNIFORM ) )
		{
			return GL_UNIFORM_BUFFER;
		}
		if ( Util::Enum::hasBits( p_usage, E_BUFFER_USAGE::STORAGE ) )
		{
			return GL_SHADER_STORAGE_BUFFER;
		}

		assert( false );
		return GL_INVALID_INDEX;
	}

	constexpr GLbitfield _toGLStorageFlags(
		const Desc::E_BUFFER_ACCESS	   p_access,
		const Desc::E_UPDATE_FREQUENCY p_freq
	)
	{
		using namespace Desc;

		GLbitfield flags = 0;

		switch ( p_access )
		{
		case E_BUFFER_ACCESS::NONE: break;
		case E_BUFFER_ACCESS::READ: flags |= GL_MAP_READ_BIT; break;
		case E_BUFFER_ACCESS::WRITE: flags |= GL_MAP_WRITE_BIT; break;
		case E_BUFFER_ACCESS::READ_WRITE:
		{
			flags |= GL_MAP_READ_BIT;
			flags |= GL_MAP_WRITE_BIT;
			break;
		}
		default: assert( false ); return 0;
		}

		switch ( p_freq )
		{
		case E_UPDATE_FREQUENCY::STATIC: break;
		case E_UPDATE_FREQUENCY::DYNAMIC:
		case E_UPDATE_FREQUENCY::STREAM: flags |= GL_DYNAMIC_STORAGE_BIT; break;
		default: assert( false ); return 0;
		}

		return flags;
	}

	constexpr GLenum _toGL( const Desc::E_UPDATE_FREQUENCY p_freq ) noexcept
	{
		using namespace Desc;

		switch ( p_freq )
		{
		case E_UPDATE_FREQUENCY::STATIC: return GL_STATIC_DRAW;
		case E_UPDATE_FREQUENCY::DYNAMIC: return GL_DYNAMIC_DRAW;
		case E_UPDATE_FREQUENCY::STREAM: return GL_STREAM_DRAW;
		default: assert( false ); return GL_INVALID_INDEX;
		}
	}

	/**
	 * @brief GL attribute description.
	 */
	struct GLAttrib
	{
		GLenum	 baseType	  = 0;
		uint8_t	 components	  = 0;
		bool	 normalized	  = false;
		uint32_t bytesPerComp = 0;
		uint8_t	 columns	  = 1;
	};

	constexpr GLAttrib toGLAttrib( Desc::E_TYPE p_type ) noexcept
	{
		using namespace Desc;

		switch ( p_type )
		{
		case E_TYPE::FLOAT: return { GL_FLOAT, 1, false, 4, 1 };
		case E_TYPE::INT: return { GL_INT, 1, false, 4, 1 };
		case E_TYPE::UINT: return { GL_UNSIGNED_INT, 1, false, 4, 1 };
		case E_TYPE::BYTE: return { GL_BYTE, 1, false, 1, 1 };
		case E_TYPE::UBYTE: return { GL_UNSIGNED_BYTE, 1, false, 1, 1 };
		case E_TYPE::SHORT: return { GL_SHORT, 1, false, 2, 1 };
		case E_TYPE::USHORT: return { GL_UNSIGNED_SHORT, 1, false, 2, 1 };
		case E_TYPE::BOOL: return { GL_UNSIGNED_BYTE, 1, false, 1, 1 };
		case E_TYPE::VEC2F: return { GL_FLOAT, 2, false, 4, 1 };
		case E_TYPE::VEC3F: return { GL_FLOAT, 3, false, 4, 1 };
		case E_TYPE::VEC4F: return { GL_FLOAT, 4, false, 4, 1 };
		case E_TYPE::VEC2I: return { GL_INT, 2, false, 4, 1 };
		case E_TYPE::VEC2U: return { GL_UNSIGNED_INT, 2, false, 4, 1 };
		case E_TYPE::VEC4U: return { GL_UNSIGNED_INT, 4, false, 4, 1 };
		case E_TYPE::MAT3F: return { GL_FLOAT, 3, false, 4, 3 };
		case E_TYPE::MAT4F: return { GL_FLOAT, 4, false, 4, 4 };
		default: assert( false ); return {};
		}
	}

	uint32_t _toSettingFlags( const std::vector<Desc::E_SETTING> & p_settings )
	{
		uint32_t mask = 0;
		for ( const Desc::E_SETTING setting : p_settings )
		{
			mask |= toUnderlying( setting );
		}
		return mask;
	}

} // namespace

namespace VTX::Renderer::Context::Backend
{
	OpenGL::OpenGL(
		const size_t			   p_width,
		const size_t			   p_height,
		const Desc::NativeContextInfo & p_contextInfo,
		const FilePath & p_shaderDir

	) : _shaderPath( p_shaderDir )
	{
		VTX_INFO( "Initializing OpenGL {}.{} context", VTX_OPENGL_MAJOR_VERSION, VTX_OPENGL_MINOR_VERSION );

		assert( p_width > 0 );
		assert( p_height > 0 );

		assert( p_width <= static_cast<size_t>( TypeMax<GLsizei> ) );
		assert( p_height <= static_cast<size_t>( TypeMax<GLsizei> ) );

		_width	= static_cast<uint32_t>( p_width );
		_height = static_cast<uint32_t>( p_height );

		// Create EGL/WGL context.
		_glContext.init( p_contextInfo );

		// Load OpenGL 4.6 functions.
		// With external loader.
		if ( gladLoadGL( (GLADloadfunc)_glContext.getProcAddress() ) == 0 )
		{
			throw GraphicException( "Failed to load OpenGL functions" );
		}

		// Check version.
#if VTX_OPENGL_MINOR_VERSION == 6
		if ( not GLAD_GL_VERSION_4_6 )
		{
			throw GraphicException( "OpenGL 4.6 or higher is required" );
		}
#elif VTX_OPENGL_MINOR_VERSION == 5
		if ( not GLAD_GL_VERSION_4_5 )
		{
			throw GraphicException( "OpenGL 4.5 or higher is required" );
		}
#else
#error "Unsupported VTX_OPENGL_MINOR_VERSION"
#endif

		_getOpenglInfos();
		_openglInfos.print();

		glViewport( 0, 0, int32_t( p_width ), int32_t( p_height ) );
		glPatchParameteri( GL_PATCH_VERTICES, 4 );

		// TODO: set from graph.
		// glEnable( GL_CLIP_DISTANCE0 );

		// glEnable( GL_LINE_SMOOTH );
		// glLineWidth( 1.f );
		glDepthFunc( GL_LESS );

		glClearColor( 0.f, 0.f, 0.f, 1.0f );

#if _DEBUG
		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( GL::Debug::_debugMessageCallback, nullptr );
#endif
	}

	// Create resources, configure, and push commands.
	// No OpenGL objects in this function, only Handles.
	void OpenGL::build(
		const Desc::RenderQueue & p_renderQueue,
		const Desc::Resources &	  p_resources,
		CommandBuffer &			  p_commands
	)
	{
		using namespace Desc;

		// Mark all resources as invalid.
		_resourceTables.invalidate();
		_framebuffers.invalidate();
		_textures.invalidate();
		_samplers.invalidate();
		_programs.invalidate();
		_buffers.invalidate();
		_vertexArrays.invalidate();

		// Create all resources.
		_getOrCreateQuad();

		for ( const auto & [ key, texture ] : p_resources.textures )
		{
			_getOrCreateTexture( key, texture );
		}
		for ( const auto & [ key, sampler ] : p_resources.samplers )
		{
			_getOrCreateSampler( key, sampler );
		}
		for ( const auto & [ key, vertexStream ] : p_resources.vertexStreams )
		{
			_getOrCreateVertexLayout( key, vertexStream );
		}
		for ( const auto & [ key, buffer ] : p_resources.buffers )
		{
			_getOrCreateBuffer( key, buffer );
		}

		// Bind geometries to VAOs.
		for ( const auto & [ key, geometry ] : p_resources.geometries )
		{
			_bindGeometryToVao( key, geometry, p_resources );
		}

		// Global resource table.
		_globalShaderBuffers = _buildGlobalShaderBuffers( p_resources );

		// Foreach pass.
		for ( const Pass * const passPtr : p_renderQueue )
		{
			// CONFIGURATION.
			const Pass & pass		= *passPtr;
			const bool	 isLastPass = ( passPtr == p_renderQueue.back() );

			// FBO.
			const Handle hFramebuffer = _getOrCreateFramebuffer( pass, p_resources, isLastPass );
			_attachTexturesToFramebuffer( pass, p_resources.textures );

			// Create programs.
			for ( const Program & program : pass.programs )
			{
				const Handle hProgram = _getOrCreateProgram( program );
			}

			// Resource table, clear each build.
			const Handle	hResourceTable = _getOrCreateResourceTable( pass, p_resources );
			ResourceTable & resourceTable  = _resourceTables.get( hResourceTable );
			resourceTable				   = _buildResourceTableForPass( pass, p_resources );

			// COMMANDS.
			// Push BEGIN_PASS/BIND_OUTPUT.
			uint32_t flags = _toSettingFlags( pass.settings );
			if ( not isLastPass )
			{
				PayloadBeginPass pBeginPass { hFramebuffer, flags };
				p_commands.push<E_COMMAND::BEGIN_PASS>( pBeginPass );
			}
			else
			{
				PayloadBindOutput pBindOutput { reinterpret_cast<uintptr_t>( &_target ) };
				p_commands.push<E_COMMAND::BIND_OUTPUT>( pBindOutput );
			}

			// Push BIND_RESOURCES.
			PayloadBindResources pBindResources { hResourceTable };
			p_commands.push<E_COMMAND::BIND_RESOURCES>( pBindResources );

			// Foreach program.
			for ( const Program & program : pass.programs )
			{
				const bool hasDrawCall = program.drawCall.has_value();
				Handle	   hVao;

				if ( hasDrawCall )
				{
					const Geometry g = p_resources.geometries.at( program.drawCall.value().geometry );
					hVao			 = _vertexArrays.handle( g.vertexLayout );
				}
				else
				{
					hVao = _vertexArrays.handle( _QUAD );
				}

				// Push DRAW.
				const Handle hProgram = _programs.handle( program.name );
				if ( hasDrawCall )
				{
					const DrawCall & drawCall = program.drawCall.value();
					const Geometry & geometry = p_resources.geometries.at( drawCall.geometry );
					const bool		 indexed  = geometry.indiceBuffer.has_value();
					const bool		 indirect = geometry.indirectBuffer.has_value();

					if ( not indirect )
					{
						const DrawCall::Range * rangePtr = std::get_if<DrawCall::Range>( &drawCall.ranges );
						assert( rangePtr );

						if ( indexed )
						{
							PayloadDrawIndexed pDraw { hProgram, hVao };
							pDraw.primitive	  = toUnderlying( drawCall.primitive );
							pDraw.indexBuffer = _buffers.handle( geometry.indiceBuffer.value() );
							pDraw.first		  = rangePtr->first;
							pDraw.count		  = rangePtr->count;
							p_commands.push<E_COMMAND::DRAW_INDEXED>( pDraw );
						}
						else
						{
							PayloadDraw pDraw { hProgram, hVao };
							pDraw.primitive = toUnderlying( drawCall.primitive );
							pDraw.first		= rangePtr->first;
							pDraw.count		= rangePtr->count;
							p_commands.push<E_COMMAND::DRAW>( pDraw );
						}
					}
					else
					{
						const uintptr_t * ptr = std::get_if<uintptr_t>( &drawCall.ranges );
						assert( ptr );

						if ( indexed )
						{
							PayloadDrawIndexedIndirect pDraw { hProgram, hVao };
							pDraw.primitive		 = toUnderlying( drawCall.primitive );
							pDraw.indirectBuffer = _buffers.handle( geometry.indirectBuffer.value() );
							pDraw.indiceBuffer	 = _buffers.handle( geometry.indiceBuffer.value() );
							pDraw.count			 = *ptr;
							p_commands.push<E_COMMAND::DRAW_INDEXED_INDIRECT>( pDraw );
						}
						else
						{
							PayloadDrawIndirect pDraw { hProgram, hVao };
							pDraw.primitive		 = toUnderlying( drawCall.primitive );
							pDraw.indirectBuffer = _buffers.handle( geometry.indirectBuffer.value() );
							pDraw.count			 = *ptr;
							p_commands.push<E_COMMAND::DRAW_INDIRECT>( pDraw );
						}
					}
				}
				else
				{
					// Fullscreen quad draw.
					PayloadDraw pDraw { hProgram, hVao };
					pDraw.primitive = static_cast<uint32_t>( toUnderlying( E_PRIMITIVE::TRIANGLES ) );
					pDraw.first		= 0;
					pDraw.count		= 4;
					p_commands.push<E_COMMAND::DRAW>( pDraw );
				}
			}

			// Push END_PASS.
			PayloadEndPass pEndPass { hFramebuffer, flags };
			p_commands.push<E_COMMAND::END_PASS>( pEndPass );
		}

		// Push PRESENT.
		p_commands.push<E_COMMAND::PRESENT>();

		setRenderTarget( Desc::E_RENDER_TARGET::SCREEN );

		// Purge invalid resources.
		_resourceTables.purge();
		_framebuffers.purge();
		_textures.purge();
		_samplers.purge();
		_programs.purge();
		_buffers.purge();
		_vertexArrays.purge();

		// Dump errors.
		GL::Debug::dumpGLError();

		// GLOBAL BINDINGS: done once at startup.
		for ( auto & bufferBinding : _globalShaderBuffers )
		{
			_buffers.get( bufferBinding.buffer ).bind( _toGLShaderTarget( bufferBinding.usage ), bufferBinding.binding );
		}
	}

	void OpenGL::resize(
		const uint32_t							 p_width,
		const uint32_t							 p_height,
		const Desc::PassList &					 p_passes,
		const Desc::ResourceMap<Desc::Texture> & p_textures
	)
	{
		using namespace Desc;

		assert( p_width > 0 );
		assert( p_height > 0 );

		assert( p_width <= static_cast<size_t>( TypeMax<GLsizei> ) );
		assert( p_height <= static_cast<size_t>( TypeMax<GLsizei> ) );

		_width	= p_width;
		_height = p_height;

		glViewport( 0, 0, static_cast<GLsizei>( _width ), static_cast<GLsizei>( _height ) );

		uint32_t texWidth  = p_width;
		uint32_t texHeight = p_height;

		for ( const auto & [ key, tex ] : p_textures )
		{
			const Size2D & size = tex.size;

			// Do not resize absolute sized textures.
			if ( std::get_if<Size2DAbsolute>( &size ) )
			{
				continue;
			}
			else if ( auto * sizePtr = std::get_if<Size2DRelative>( &tex.size ) )
			{
				texWidth  = static_cast<uint32_t>( static_cast<float>( _width ) * sizePtr->width );
				texHeight = static_cast<uint32_t>( static_cast<float>( _height ) * sizePtr->height );
				texWidth  = std::max( 1u, texWidth );
				texHeight = std::max( 1u, texHeight );
			}

			_textures.get( key ).resize( texWidth, texHeight );
		}

		for ( const auto & pass : p_passes )
		{
			_attachTexturesToFramebuffer( *pass, p_textures );
		}
	}

	Desc::Handle OpenGL::_getOrCreateFramebuffer(
		const Desc::Pass &		p_pass,
		const Desc::Resources & p_res,
		const bool				p_isLast
	)
	{
		using namespace Desc;

		const Key & key = p_pass.name;
		Handle		h;

		if ( _framebuffers.validate( key ) )
		{
			h = _framebuffers.handle( key );
		}
		else
		{
			h = _framebuffers.emplace( key );
		}

		if ( p_isLast )
		{
			// Save as offscreen target.
			_offscreen = h;

			// Create a default fbo.
			if ( not _framebuffers.validate( _DEFAULT_FBO ) )
			{
				_default = _framebuffers.emplace( _DEFAULT_FBO, {}, 0 );
			}
		}

		return h;
	}

	Desc::Handle OpenGL::_getOrCreateResourceTable( const Desc::Pass & p_pass, const Desc::Resources & p_res )
	{
		using namespace Desc;

		const Key & key = p_pass.name;

		if ( _resourceTables.validate( key ) )
		{
			return _resourceTables.handle( key );
		}

		const Handle h = _resourceTables.emplace( key );

		return h;
	}

	Desc::Handle OpenGL::_getOrCreateTexture( const Desc::Key & p_key, const Desc::Texture & p_text )
	{
		using namespace Desc;

		if ( _textures.validate( p_key, p_text ) )
		{
			return _textures.handle( p_key );
		}

		uint32_t width	= _width;
		uint32_t height = _height;

		if ( auto * sizePtr = std::get_if<Size2DAbsolute>( &p_text.size ) )
		{
			width  = sizePtr->width;
			height = sizePtr->height;
		}
		else if ( auto * sizePtr = std::get_if<Size2DRelative>( &p_text.size ) )
		{
			width  = static_cast<uint32_t>( static_cast<float>( _width ) * sizePtr->width );
			height = static_cast<uint32_t>( static_cast<float>( _height ) * sizePtr->height );
			width  = std::max( 1u, width );
			height = std::max( 1u, height );
		}

		GLPixelFormat glFormat = _toGL( p_text.format );

		const Handle h
			= _textures.emplace( p_key, p_text, GLsizei( width ), GLsizei( height ), glFormat.internalFormat );

		if ( not p_text.data.empty() )
		{
			_textures.get( h ).fill( p_text.data.data(), glFormat.uploadFormat, glFormat.uploadType );
		}

		return h;
	}

	Desc::Handle OpenGL::_getOrCreateSampler( const Desc::Key & p_key, const Desc::Sampler & p_sampler )
	{
		using namespace Desc;

		if ( _samplers.validate( p_key, p_sampler ) )
		{
			return _samplers.handle( p_key );
		}

		const Handle h = _samplers.emplace(
			p_key,
			p_sampler,
			_toGL( p_sampler.wrapS ),
			_toGL( p_sampler.wrapT ),
			_toGL( p_sampler.minFilter ),
			_toGL( p_sampler.magFilter )

		);

		return h;
	}

	Desc::Handle OpenGL::_getOrCreateVertexLayout( const Desc::Key & p_key, const Desc::VertexLayout & p_vertexStream )
	{
		using namespace Desc;

		if ( _vertexArrays.validate( p_key, p_vertexStream ) )
		{
			return _vertexArrays.handle( p_key );
		}

		const Handle h = _vertexArrays.emplace( p_key, p_vertexStream );

		const auto & vao = _vertexArrays.get( h );
		vao.bind();
		GLuint location = 0;
		for ( const auto & a : p_vertexStream.attributes )
		{
			const GLAttrib ga = toGLAttrib( a.type );

			for ( uint8_t col = 0; col < ga.columns; ++col )
			{
				const GLuint attribIndex  = location;
				const GLuint bindingIndex = location; // planar : 1 binding per attribute
				const GLuint relOffset	  = GLuint( col * ga.components * ga.bytesPerComp );

				vao.enableAttribute( attribIndex );
				vao.setAttributeFormat(
					attribIndex, ga.components, ga.baseType, relOffset, ga.normalized ? GL_TRUE : GL_FALSE
				);
				vao.setAttributeBinding( attribIndex, bindingIndex );

				++location;
			}
		}
		vao.unbind();

		return h;
	}

	Desc::Handle OpenGL::_getOrCreateBuffer( const Desc::Key & p_key, const Desc::Buffer & p_buffer )
	{
		using namespace Desc;

		if ( _buffers.validate( p_key, p_buffer ) )
		{
			return _buffers.handle( p_key );
		}

		if ( p_buffer.values.empty() )
		{
			return _buffers.emplace( p_key, p_buffer );
		}

		using CpuBuffer = std::variant<BinaryBuffer<E_LAYOUT_TYPE::Std140>, BinaryBuffer<E_LAYOUT_TYPE::Std430>>;

		CpuBuffer cpuBuffer = Util::Enum::hasBits( p_buffer.usage, E_BUFFER_USAGE::UNIFORM )
								  ? CpuBuffer { BinaryBuffer<E_LAYOUT_TYPE::Std140> {} }
								  : CpuBuffer { BinaryBuffer<E_LAYOUT_TYPE::Std430> {} };

		return std::visit(
			[ & ]( auto & p_buf )
			{
				using BB = std::decay_t<decltype( p_buf )>;

				for ( const UniformValue & value : p_buffer.values )
				{
					const uint32_t	count = value.arrayCount ? *value.arrayCount : 1u;
					const size_t	elem  = BB::rawElementSizeBytes( value.type );
					const SpanBytes bytes { reinterpret_cast<const std::byte *>( value.data.data() ), elem };
					for ( uint32_t i = 0; i < count; ++i )
					{
						p_buf.write( value.type, bytes );
					}
				}

				p_buf.close();

				const uint32_t bufferSize = static_cast<uint32_t>( p_buf.size() );
				assert( bufferSize > 0 );

				const auto hBuffer = _buffers.emplace( p_key, p_buffer );
				auto &	   gl	   = _buffers.get( hBuffer );

				switch ( p_buffer.mutability )
				{
				case E_BUFFER_MUTABILITY::MUTABLE:
				{
					gl.setData( p_buf.data(), static_cast<GLsizei>( bufferSize ), 0, _toGL( p_buffer.frequency ) );
					break;
				}
				case E_BUFFER_MUTABILITY::IMMUTABLE:
				{
					gl.setStorage(
						p_buf.data(),
						static_cast<GLsizei>( bufferSize ),
						_toGLStorageFlags( p_buffer.access, p_buffer.frequency )
					);
					break;
				}
				default: break;
				}

				return hBuffer;
			},
			cpuBuffer
		);
	}

	Desc::Handle OpenGL::_getOrCreateProgram( const Desc::Program & p_program )
	{
		using namespace Desc;

		// Create shader buffer if uniforms.
		if ( not p_program.uniforms.empty() )
		{
			Buffer buffer;
			buffer.name		  = p_program.name;
			buffer.usage	  = E_BUFFER_USAGE::UNIFORM;
			buffer.mutability = E_BUFFER_MUTABILITY::IMMUTABLE;
			buffer.access	  = E_BUFFER_ACCESS::NONE;
			buffer.frequency  = E_UPDATE_FREQUENCY::DYNAMIC;
			buffer.values	  = p_program.uniforms;
			_getOrCreateBuffer( p_program.name, buffer );
		}

		const Key & key = p_program.name;

		if ( _programs.validate( key, p_program ) )
		{
			return _programs.handle( key );
		}
		const Handle h = _programs.emplace( key, p_program, p_program.name, _shaderPath, p_program.shaders );

		return h;
	}

	OpenGL::GlobalShaderBuffers OpenGL::_buildGlobalShaderBuffers( const Desc::Resources & p_resources )
	{
		using namespace Desc;

		OpenGL::GlobalShaderBuffers gsb;

		for ( const auto & [ key, buffer ] : p_resources.buffers )
		{
			if ( not buffer.binding )
			{
				continue;
			}
			assert( Util::Enum::hasBits( buffer.usage, E_BUFFER_USAGE::UNIFORM )
					|| Util::Enum::hasBits( buffer.usage, E_BUFFER_USAGE::STORAGE ) );
			assert( buffer.binding );
			const Handle hBuf = _buffers.handle( key );
			gsb.emplace_back( hBuf, buffer.usage, *buffer.binding );
		}

		return gsb;
	}

	OpenGL::ResourceTable OpenGL::_buildResourceTableForPass(
		const Desc::Pass &		p_pass,
		const Desc::Resources & p_resources
	)
	{
		using namespace Desc;

		OpenGL::ResourceTable rt;

		// It's better to not use the same binding/unit several times in different contexts.
		Binding b = 0;

		// For each input.
		for ( auto & input : p_pass.inputs )
		{
			switch ( input.type )
			{
			case E_RESOURCE_TYPE::TEXTURE:
			{
				assert( input.secondary );
				const Handle hTex  = _textures.handle( input.primary );
				const Handle hSamp = _samplers.handle( *input.secondary );
				rt.textures.emplace_back( hTex, hSamp, b++ );
				break;
			}
			case E_RESOURCE_TYPE::BUFFER:
			{
				const Handle   hBuf	  = _buffers.handle( input.primary );
				const Buffer & buffer = p_resources.buffers.at( input.primary );
				rt.buffers.emplace_back( hBuf, buffer.usage, b++ );
				break;
			}
			default: break;
			}
		}

		// For each program check if shader buffer exists.
		for ( auto & program : p_pass.programs )
		{
			const Key & key = program.name;
			if ( _buffers.contains( key ) )
			{
				const Handle hBuf = _buffers.handle( key );
				rt.buffers.emplace_back( hBuf, E_BUFFER_USAGE::UNIFORM, b++ );
			}
		}

		b = 0;
		// For each output.
		for ( auto & output : p_pass.outputs )
		{
			switch ( output.type )
			{
			case E_RESOURCE_TYPE::BUFFER:
			{
				const Handle   hBuf	  = _buffers.handle( output.primary );
				const Buffer & buffer = p_resources.buffers.at( output.primary );
				rt.buffers.emplace_back( hBuf, buffer.usage, b++ );

				break;
			}
			default: break;
			}
		}

		return rt;
	}

	void OpenGL::_attachTexturesToFramebuffer(
		const Desc::Pass &						 p_pass,
		const Desc::ResourceMap<Desc::Texture> & p_textures
	)
	{
		using namespace Desc;

		const Key &				key = p_pass.name;
		const GL::Framebuffer & fbo = _framebuffers.get( key );

		// Attach.
		uint colorAttach = 0;
		for ( auto & output : p_pass.outputs )
		{
			if ( output.type != E_RESOURCE_TYPE::TEXTURE )
			{
				continue;
			}

			const GL::Texture2D & texture = _textures.get( output.primary );

			const E_FORMAT format  = p_textures.at( output.primary ).format;
			const bool	   isDepth = _toGL( format ).isDepth;

			if ( isDepth )
			{
				fbo.attachTexture( texture, GL_DEPTH_ATTACHMENT );
			}
			else
			{
				fbo.attachTexture( texture, GL_COLOR_ATTACHMENT0 + colorAttach++ );
			}
		}

		// Set draw buffers.
		fbo.setReadBuffer( GL_NONE );
		if ( colorAttach )
		{
			std::vector<uint32_t> drawBuffers( colorAttach );
			std::iota( drawBuffers.begin(), drawBuffers.end(), GL_COLOR_ATTACHMENT0 );
			fbo.setDrawBuffers( drawBuffers );
		}
		else
		{
			fbo.setDrawBuffers( { GL_NONE } );
		}

		// Check FBO complete.
		assert( fbo.checkStatus() );
	}

	void OpenGL::_bindGeometryToVao(
		const Desc::Key &		p_key,
		const Desc::Geometry &	p_geo,
		const Desc::Resources & p_resources
	)
	{
		using namespace Desc;

		const Key &			 kVao	= p_geo.vertexLayout;
		const VertexLayout & layout = p_resources.vertexStreams.at( kVao );
		const auto &		 vao	= _vertexArrays.get( kVao );

		vao.bind();

		GLuint location = 0;

		for ( const auto & a : layout.attributes )
		{
			const GLAttrib	   ga		 = toGLAttrib( a.type );
			const Key		   bufferKey = a.name;
			const GL::Buffer & vbo		 = _buffers.get( bufferKey );

			const GLsizei stride = GLsizei( ga.columns * ga.components * ga.bytesPerComp );
			for ( uint8_t col = 0; col < ga.columns; ++col )
			{
				const GLuint bindingIndex = location;
				vao.setVertexBuffer( bindingIndex, vbo, 0, stride );
				++location;
			}
		}

		vao.unbind();
	}

	Desc::Handle OpenGL::_getOrCreateQuad()
	{
		using namespace Desc;

		const Key	 quadLayoutKey = _QUAD;
		VertexLayout quadLayout;
		quadLayout.attributes = { VertexAttribute { _QUAD_VBO, E_TYPE::VEC2F } };

		const Key quadVboKey = _QUAD_VBO;
		Buffer	  quadVboDesc;
		quadVboDesc.name	  = quadVboKey;
		quadVboDesc.usage	  = E_BUFFER_USAGE::VERTEX;
		quadVboDesc.frequency = E_UPDATE_FREQUENCY::STATIC;

		if ( _vertexArrays.validate( quadLayoutKey, quadLayout ) && _buffers.validate( quadVboKey, quadVboDesc ) )
		{
			return _vertexArrays.handle( quadLayoutKey );
		}

		const std::array<Vec2f, 4> quad
			= { Vec2f { -1.f, 1.f }, Vec2f { -1.f, -1.f }, Vec2f { 1.f, 1.f }, Vec2f { 1.f, -1.f } };

		Geometry quadGeo;
		quadGeo.vertexLayout = quadLayoutKey;
		quadGeo.indiceBuffer = std::nullopt;

		const Handle h = _getOrCreateVertexLayout( quadLayoutKey, quadLayout );
		_getOrCreateBuffer( quadVboKey, quadVboDesc );

		Resources fakeRes;
		fakeRes.vertexStreams.emplace( quadLayoutKey, quadLayout );
		fakeRes.geometries.emplace( quadLayoutKey, quadGeo );

		_bindGeometryToVao( quadLayoutKey, quadGeo, fakeRes );

		_buffers.get( quadVboKey ).setStorage( quad.data(), GLsizei( sizeof( quad ) ) );

		return h;
	}

	void OpenGL::setBufferData( const Desc::Key & p_key, SpanBytes p_bytes, const size_t p_offset )
	{
		using namespace Desc;

		const Handle h	  = _buffers.handle( p_key );
		const auto & desc = _buffers.descriptor( p_key );

		if ( ( Util::Enum::hasBits( desc.usage, E_BUFFER_USAGE::UNIFORM )
			   || Util::Enum::hasBits( desc.usage, E_BUFFER_USAGE::STORAGE ) )
			 && desc.mutability == E_BUFFER_MUTABILITY::IMMUTABLE )
		{
			_buffers.get( h ).setSub( p_bytes.data(), GLsizei( p_bytes.size() ), p_offset );
			return;
		}

		_buffers.get( h ).setData( p_bytes.data(), GLsizei( p_bytes.size() ), p_offset, _toGL( desc.frequency ) );
	}

	std::vector<std::byte> OpenGL::getTextureData(
		const Desc::Key &			  p_key,
		std::optional<Desc::E_FORMAT> p_format,
		std::optional<size_t>		  p_x,
		std::optional<size_t>		  p_y
	) const
	{
		using namespace Desc;

		const Handle		handle		= _textures.handle( p_key );
		const auto &		textureProp = _textures.descriptor( p_key );
		const GLPixelFormat glFormat	= _toGL( p_format ? *p_format : textureProp.format );
		const auto &		texture		= _textures.get( handle );

		const bool	 single = p_x && p_y;
		const size_t w		= single ? 1 : _width;
		const size_t h		= single ? 1 : _height;

		std::vector<std::byte> data( w * h * glFormat.bytesPerPixel );

		texture.getSubImage(
			0,
			single ? GLint( *p_x ) : 0,
			single ? GLint( *p_y ) : 0,
			GLsizei( w ),
			GLsizei( h ),
			glFormat.uploadFormat,
			glFormat.uploadType,
			GLsizei( data.size() ),
			data.data()
		);

		return data;
	}

	void OpenGL::setTextureData( const Desc::Key & p_key, SpanBytes p_bytes )
	{
		using namespace Desc;

		const Handle		h			= _textures.handle( p_key );
		const auto &		textureProp = _textures.descriptor( p_key );
		const GLPixelFormat glFormat	= _toGL( textureProp.format );
		const auto &		texture		= _textures.get( h );

		texture.fill( p_bytes.data(), glFormat.uploadFormat, glFormat.uploadType );
	}

	void OpenGL::setRenderTarget( const Desc::E_RENDER_TARGET p_target )
	{
		_target = p_target == Desc::E_RENDER_TARGET::SCREEN ? _default : _offscreen;
	}

	// Old way: reading framebuffer. New way: read from texture directly.
	/*
	std::vector<std::byte> OpenGL::snapshot() const
	{
		std::vector<std::byte> data( _width * _height * 4 );
		glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
		glReadBuffer( GL_BACK );
		glReadPixels( 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, data.data() );

		return data;
	}
	*/

	void OpenGL::setOption( const Desc::E_OPTION p_option, const bool p_value )
	{
		switch ( p_option )
		{
		case Desc::E_OPTION::VSYNC: _glContext.setSwapInterval( p_value ? 1 : 0 ); break;
		default: break;
		}
	}

	void OpenGL::fillInfos( StructInfos & p_infos ) const
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
	}

	void OpenGL::_getOpenglInfos()
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
		int32_t numExtensions = 0;
		glGetIntegerv( GL_NUM_EXTENSIONS, &numExtensions );
		for ( int32_t i = 0; i < numExtensions; ++i )
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
		}
	}

} // namespace VTX::Renderer::Context::Backend
