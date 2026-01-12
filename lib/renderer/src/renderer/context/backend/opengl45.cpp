#include "renderer/context/backend/opengl45.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/context/gl/debug.hpp"
#include <numeric>
#include <util/exceptions.hpp>

namespace
{
	using namespace VTX;
	using namespace VTX::Renderer;

	struct GLPixelFormat
	{
		GLenum internalFormat = 0; // glTextureStorage2D
		GLenum uploadFormat	  = 0; // glTextureSubImage2D / glClearTexImage
		GLenum uploadType	  = 0; // glTextureSubImage2D / glClearTexImage
		bool   isInteger	  = false;
		bool   isDepth		  = false;
	};

	/**
	 * @brief All GL mapping.
	 */
	constexpr GLPixelFormat _toGL( const E_FORMAT p_format ) noexcept
	{
		switch ( p_format )
		{
		case E_FORMAT::RGB16F: return { GL_RGB16F, GL_RGB, GL_HALF_FLOAT, false, false };
		case E_FORMAT::RGBA16F: return { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, false, false };
		case E_FORMAT::RGBA32F: return { GL_RGBA32F, GL_RGBA, GL_FLOAT, false, false };
		case E_FORMAT::R16F: return { GL_R16F, GL_RED, GL_HALF_FLOAT, false, false };
		case E_FORMAT::R32F: return { GL_R32F, GL_RED, GL_FLOAT, false, false };
		case E_FORMAT::R8: return { GL_R8, GL_RED, GL_UNSIGNED_BYTE, false, false };
		case E_FORMAT::RG32UI: return { GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, true, false };
		case E_FORMAT::RGBA32UI: return { GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, true, false };
		case E_FORMAT::DEPTH_COMPONENT32F: return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false, true };
		default: assert( false ); return {};
		}
	}

	constexpr GLenum _toGL( const E_WRAPPING p_wrapping ) noexcept
	{
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

	constexpr GLenum _toGL( const E_FILTERING p_filtering ) noexcept
	{
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

	constexpr GLenum _toGL( const E_SHADER_BUFFER_KIND p_bufferRole ) noexcept
	{
		switch ( p_bufferRole )
		{
		case E_SHADER_BUFFER_KIND::PARAMETERS: return GL_UNIFORM_BUFFER;
		case E_SHADER_BUFFER_KIND::STRUCTURED: return GL_SHADER_STORAGE_BUFFER;
		default: assert( false ); return GL_INVALID_INDEX;
		}
	}

	constexpr GLbitfield _toGLStorageFlags( const E_BUFFER_ACCESS p_access, const E_UPDATE_FREQUENCY p_freq )
	{
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

	constexpr GLbitfield toGLMapFlags( const E_BUFFER_ACCESS p_access )
	{
		GLbitfield flags = 0;

		switch ( p_access )
		{
		case E_BUFFER_ACCESS::READ: flags |= GL_MAP_READ_BIT; break;
		case E_BUFFER_ACCESS::WRITE: flags |= GL_MAP_WRITE_BIT; break;
		case E_BUFFER_ACCESS::READ_WRITE:
			flags |= GL_MAP_READ_BIT;
			flags |= GL_MAP_WRITE_BIT;
			break;

		default: assert( false ); return 0;
		}

		return flags;
	}

	constexpr GLenum _toGL( const E_UPDATE_FREQUENCY p_freq ) noexcept
	{
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

	constexpr GLAttrib toGLAttrib( E_TYPE p_type ) noexcept
	{
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
		case E_TYPE::MAT3F: return { GL_FLOAT, 3, false, 4, 3 };
		case E_TYPE::MAT4F: return { GL_FLOAT, 4, false, 4, 4 };
		default: assert( false ); return {};
		}
	}

	GLbitfield _toGLClearMask( const std::vector<E_SETTINGS> & p_settings )
	{
		GLenum mask = 0;
		for ( const E_SETTINGS setting : p_settings )
		{
			switch ( setting )
			{
			case E_SETTINGS::CLEAR_COLOR: mask |= GL_COLOR_BUFFER_BIT; break;
			case E_SETTINGS::CLEAR_DEPTH: mask |= GL_DEPTH_BUFFER_BIT; break;
			default: break;
			}
		}
		return mask;
	}

	GLbitfield _toGLEnableMask( const std::vector<E_SETTINGS> & p_settings )
	{
		GLenum enable = 0;
		for ( const E_SETTINGS setting : p_settings )
		{
			switch ( setting )
			{
			case E_SETTINGS::ENABLE_DEPTH: enable |= GL_DEPTH_TEST; break;
			default: break;
			}
		}
		return enable;
	}

} // namespace

namespace VTX::Renderer::Context::Backend
{

	OpenGL45::OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc )
	{
		assert( p_width > 0 );
		assert( p_height > 0 );

		assert( p_width <= static_cast<size_t>( TypeMax<GLsizei> ) );
		assert( p_height <= static_cast<size_t>( TypeMax<GLsizei> ) );

		_width	= static_cast<uint32_t>( p_width );
		_height = static_cast<uint32_t>( p_height );

		// Load opengl 4.5.
		// With external loader.
		if ( p_proc && gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
		{
			throw GraphicException( "Failed to load OpenGL" );
		}
		// With glad integrated loader.
		else if ( gladLoadGL() == 0 )
		{
			throw GraphicException( "Failed to load OpenGL" );
		}

		// Check version.
		if ( not GLAD_GL_VERSION_4_5 )
		{
			throw GraphicException( "OpenGL 4.5 or higher is required" );
		}
		else
		{
			_getOpenglInfos();
			_openglInfos.print();
		}

		// Program manager.
		_programManager = std::make_unique<GL::ProgramManager>( p_shaderPath );

		// Init quad vao/vbo for deferred shading.
		std::vector<Vec2f> quad = { { -1.f, 1.f }, { -1.f, -1.f }, { 1.f, 1.f }, { 1.f, -1.f } };

		// const Hash hashQuadVao	  = Util::hash( _KEY_QUAD_VAO );
		// const Hash hashQuadBuffer = Util::hash( _KEY_QUAD_BUFFER );

		//_vertexArrays.emplace( hashQuadVao, std::make_unique<GL::VertexArray>() );
		//_buffers.emplace( hashQuadBuffer, std::make_unique<GL::Buffer>() );
		// auto & vao = _vertexArrays[ hashQuadVao ];
		// auto & vbo = _buffers[ hashQuadBuffer ];

		// vao->bind();
		// vao->enableAttribute( 0 );
		// vao->setVertexBuffer( 0, *vbo, int32_t( _mapTypeSizes[ E_TYPE::FLOAT ] * 2 ) );
		// vao->setAttributeFormat( 0, 2, _mapTypes[ E_TYPE::FLOAT ] );
		// vao->setAttributeBinding( 0, 0 );
		// vbo->set( quad.data(), int32_t( quad.size() * sizeof( Vec2f ) ), 0, GL_STATIC_DRAW );
		// vao->unbind();

		glViewport( 0, 0, int32_t( p_width ), int32_t( p_height ) );

		glPatchParameteri( GL_PATCH_VERTICES, 4 );
		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 1.f );

		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( GL::Debug::_debugMessageCallback, nullptr );

		// TODO: set from graph.
		glEnable( GL_CLIP_DISTANCE0 );
		glDepthFunc( GL_LESS );

		glClearColor( 0.f, 0.f, 0.f, 1.0f );
	}

	// Create resources, configure, and push commands.
	// No OpenGL objects in this function, only Handles.
	void OpenGL45::build( const RenderQueue & p_renderQueue, const Resources & p_resources, CommandBuffer & p_commands )
	{
		// Create all resources.
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
		for ( const auto & [ key, buffer ] : p_resources.shaderBuffers )
		{
			_getOrCreateShaderBuffer( buffer );
		}
		for ( const auto & [ key, buffer ] : p_resources.pipelineBuffers )
		{
			switch ( buffer.kind )
			{
			case E_PIPELINE_BUFFER_KIND::VERTEX: _getOrCreateVertexBuffer( key );
			case E_PIPELINE_BUFFER_KIND::INDEX: _getOrCreateIndexBuffer( key );
			default: break;
			}
		}

		// Global resource table.
		_globalShaderBuffers = _buildGlobalShaderBuffers( p_resources );

		// Push BEGIN_FRAME.
		// TODO: read from graph.
		p_commands.push<E_COMMAND::BEGIN_FRAME>();

		// Foreach pass.
		for ( const Pass * const passPtr : p_renderQueue )
		{
			const Pass & pass		= *passPtr;
			const bool	 isLastPass = ( passPtr == p_renderQueue.back() );

			// Resource table, clear each build.
			const Handle	hResourceTable = _getOrCreateResourceTable( pass, p_resources );
			ResourceTable & resourceTable  = _resourceTables[ hResourceTable ];
			resourceTable				   = _buildResourceTableForPass( pass, p_resources );

			// FBO.
			const Handle hFramebuffer = _getOrCreateFramebuffer( pass, p_resources, isLastPass );
			if ( not isLastPass )
			{
				_attachTexturesToFramebuffer( pass, p_resources );
			}

			// Create programs.
			for ( const Program & program : pass.programs )
			{
				const Handle hProgram = _getOrCreateProgram( program );
			}

			// Push BIND_FRAMEBUFFER.
			PayloadBindFramebuffer bindFBO { hFramebuffer };
			p_commands.push<E_COMMAND::BIND_FRAMEBUFFER>( bindFBO );

			// Push BEGIN_PASS.
			uint32_t		 clearFlags	 = static_cast<uint32_t>( _toGLClearMask( pass.settings ) );
			uint32_t		 enableFlags = static_cast<uint32_t>( _toGLEnableMask( pass.settings ) );
			PayloadBeginPass beginPass { clearFlags, enableFlags };
			p_commands.push<E_COMMAND::BEGIN_PASS>( beginPass );

			// Push END_PASS.
			PayloadEndPass endPass { enableFlags };
			p_commands.push<E_COMMAND::END_PASS>( endPass );
		}

		// Push END_FRAME.
		p_commands.push<E_COMMAND::END_FRAME>();
	}

	void OpenGL45::resize( const size_t p_width, const size_t p_height )
	{
		assert( p_width > 0 );
		assert( p_height > 0 );

		assert( p_width <= static_cast<size_t>( TypeMax<GLsizei> ) );
		assert( p_height <= static_cast<size_t>( TypeMax<GLsizei> ) );

		glViewport( 0, 0, static_cast<GLsizei>( p_width ), static_cast<GLsizei>( p_height ) );

		// TODO: resize textures.
	}

	Handle OpenGL45::_getOrCreateFramebuffer( const Pass & p_pass, const Resources & p_res, const bool p_isLastpass )
	{
		const Key & key = p_pass.name;

		auto it = _cacheFramebuffers.find( key );
		if ( it != _cacheFramebuffers.end() )
		{
			return it->second;
		}

		if ( p_isLastpass )
		{
			return NO_HANDLE;
		}

		const Handle h = static_cast<Handle>( _framebuffers.size() );
		_framebuffers.emplace_back( std::make_unique<GL::Framebuffer>() );
		_cacheFramebuffers.emplace( key, h );

		return h;
	}

	Handle OpenGL45::_getOrCreateResourceTable( const Pass & p_pass, const Resources & p_res )
	{
		const Key & key = p_pass.name;

		auto it = _cacheResourceTables.find( key );
		if ( it != _cacheResourceTables.end() )
		{
			return it->second;
		}

		const Handle h = static_cast<Handle>( _resourceTables.size() );
		_resourceTables.emplace_back();
		_cacheResourceTables.emplace( key, h );

		return h;
	}

	Handle OpenGL45::_getOrCreateTexture( const Key & p_key, const Texture & p_text )
	{
		auto it = _cacheTextures.find( p_key );
		if ( it != _cacheTextures.end() )
		{
			return it->second;
		}

		const Handle h = static_cast<Handle>( _textures.size() );

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

		_textures.emplace_back(
			std::make_unique<GL::Texture2D>( GLsizei( width ), GLsizei( height ), glFormat.internalFormat )
		);
		_cacheTextures.emplace( p_key, h );

		if ( not p_text.data.empty() )
		{
			_textures[ h ]->fill( p_text.data.data(), glFormat.uploadFormat, glFormat.uploadType );
		}

		return h;
	}

	Handle OpenGL45::_getOrCreateSampler( const Key & p_key, const Sampler & p_text )
	{
		auto it = _cacheSamplers.find( p_key );
		if ( it != _cacheSamplers.end() )
		{
			return it->second;
		}

		const Handle h = static_cast<Handle>( _samplers.size() );
		_samplers.emplace_back(
			std::make_unique<GL::Sampler>(
				_toGL( p_text.wrapS ), _toGL( p_text.wrapT ), _toGL( p_text.minFilter ), _toGL( p_text.magFilter )
			)
		);
		_cacheSamplers.emplace( p_key, h );

		return h;
	}

	Handle OpenGL45::_getOrCreateVertexLayout( const Key & p_key, const VertexLayout & p_vertexStream )
	{
		auto it = _cacheVertexLayouts.find( p_key );
		if ( it != _cacheVertexLayouts.end() )
		{
			return it->second;
		}
		const Handle h = static_cast<Handle>( _vertexArrays.size() );
		_vertexArrays.emplace_back( std::make_unique<GL::VertexArray>() );
		_cacheVertexLayouts.emplace( p_key, h );

		const auto & vao = *_vertexArrays[ h ];
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

	Handle OpenGL45::_getOrCreateShaderBuffer( const BufferShader & p_buffer )
	{
		const Key key = p_buffer.name;

		auto it = _cacheShaderBuffers.find( key );
		if ( it != _cacheShaderBuffers.end() )
		{
			return it->second;
		}

		BinaryBuffer<E_LAYOUT_TYPE::Std140> cpuBuffer;

		for ( const UniformValue & value : p_buffer.values )
		{
			const uint32_t count = value.arrayCount ? *value.arrayCount : 1u;
			const size_t   elem	 = BinaryBuffer<E_LAYOUT_TYPE::Std140>::rawElementSizeBytes( value.type );

			const SpanBytes bytes { reinterpret_cast<const std::byte *>( value.data.data() ), elem * count };

			cpuBuffer.write( value.type, bytes );
		}

		cpuBuffer.close();

		const uint32_t bufferSize = static_cast<uint32_t>( cpuBuffer.size() );
		assert( bufferSize > 0 );

		auto glBuffer = std::make_unique<GL::Buffer>();

		switch ( p_buffer.mutability )
		{
		case E_BUFFER_MUTABILITY::MUTABLE:
		{
			glBuffer->setData( cpuBuffer.data(), static_cast<GLsizei>( bufferSize ), _toGL( p_buffer.frequency ) );
			break;
		}
		case E_BUFFER_MUTABILITY::IMMUTABLE:
		{
			glBuffer->setStorage(
				cpuBuffer.data(),
				static_cast<GLsizei>( bufferSize ),
				_toGLStorageFlags( p_buffer.access, p_buffer.frequency )
			);
			break;
		}
		default: break;
		}

		const Handle h = static_cast<Handle>( _shaderBuffers.size() );
		_shaderBuffers.emplace_back( std::move( glBuffer ) );
		_cacheShaderBuffers.emplace( key, h );

		return h;
	}

	Handle OpenGL45::_getOrCreateVertexBuffer( const Key & p_key )
	{
		auto it = _cacheVertexBuffers.find( p_key );
		if ( it != _cacheVertexBuffers.end() )
		{
			return it->second;
		}

		auto glBuffer = std::make_unique<GL::Buffer>(); // pas de storage ici

		const Handle h = static_cast<Handle>( _vertexBuffers.size() );
		_vertexBuffers.emplace_back( std::move( glBuffer ) );
		_cacheVertexBuffers.emplace( p_key, h );

		return h;
	}

	Handle OpenGL45::_getOrCreateIndexBuffer( const Key & p_key )
	{
		auto it = _cacheIndexBuffers.find( p_key );
		if ( it != _cacheIndexBuffers.end() )
		{
			return it->second;
		}

		auto glBuffer = std::make_unique<GL::Buffer>(); // pas de storage ici

		const Handle h = static_cast<Handle>( _indexBuffers.size() );
		_indexBuffers.emplace_back( std::move( glBuffer ) );
		_cacheIndexBuffers.emplace( p_key, h );

		return h;
	}

	Handle OpenGL45::_getOrCreateProgram( const Program & p_program )
	{
		const Key & key = p_program.name;
		auto		it	= _cachePrograms.find( key );
		if ( it != _cachePrograms.end() )
		{
			return it->second;
		}
		const Handle h = static_cast<Handle>( _programs.size() );

		GL::Program * const program = _programManager->createProgram( p_program.name, p_program.shaders );
		_programs.emplace_back( program );
		_cachePrograms.emplace( key, h );

		// Create shader buffer if uniforms.
		if ( not p_program.uniforms.empty() )
		{
			BufferShader buffer;
			buffer.name		  = p_program.name;
			buffer.role		  = E_SHADER_BUFFER_KIND::PARAMETERS;
			buffer.mutability = E_BUFFER_MUTABILITY::IMMUTABLE;
			buffer.access	  = E_BUFFER_ACCESS::NONE;
			buffer.frequency  = E_UPDATE_FREQUENCY::DYNAMIC;
			buffer.values	  = p_program.uniforms;
			_getOrCreateShaderBuffer( buffer );
		}

		return h;
	}

	OpenGL45::GlobalShaderBuffers OpenGL45::_buildGlobalShaderBuffers( const Resources & p_resources )
	{
		OpenGL45::GlobalShaderBuffers gsb;

		for ( const auto & [ key, buffer ] : p_resources.shaderBuffers )
		{
			assert( buffer.binding );
			const Handle hBuf = _cacheShaderBuffers.at( key );
			gsb.emplace_back( hBuf, *buffer.binding );
		}

		return gsb;
	}

	OpenGL45::ResourceTable OpenGL45::_buildResourceTableForPass( const Pass & p_pass, const Resources & p_resources )
	{
		OpenGL45::ResourceTable rt;

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
				const Handle hTex  = _cacheTextures.at( input.primary );
				const Handle hSamp = _cacheSamplers.at( *input.secondary );
				rt.textures.emplace_back( hTex, hSamp, b++ );
				break;
			}
			case E_RESOURCE_TYPE::BUFFER:
			{
				const Handle hBuf = _cacheShaderBuffers.at( input.primary );
				rt.shaderBuffers.emplace_back( hBuf, b++ );
				break;
			}
			default: break;
			}
		}

		// For each program check if shader buffer exists.
		for ( auto & program : p_pass.programs )
		{
			const Key & key = program.name;
			if ( _cacheShaderBuffers.contains( key ) )
			{
				const Handle hBuf = _cacheShaderBuffers.at( key );
				rt.shaderBuffers.emplace_back( hBuf, b++ );
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
				const Handle hBuf = _cacheShaderBuffers.at( output.primary );
				rt.shaderBuffers.emplace_back( hBuf, b++ );

				break;
			}
			default: break;
			}
		}

		return rt;
	}

	void OpenGL45::_attachTexturesToFramebuffer( const Pass & p_pass, const Resources & p_resources )
	{
		const Key &				key			 = p_pass.name;
		const Handle			hFramebuffer = _cacheFramebuffers.at( key );
		const GL::Framebuffer & fbo			 = *_framebuffers[ hFramebuffer ];

		// Attach.
		uint colorAttach = 0;
		for ( auto & output : p_pass.outputs )
		{
			if ( output.type != E_RESOURCE_TYPE::TEXTURE )
			{
				continue;
			}

			const Handle		  hTex	  = _cacheTextures.at( output.primary );
			const GL::Texture2D & texture = *_textures[ hTex ];

			const E_FORMAT format  = p_resources.textures.at( output.primary ).format;
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

	void OpenGL45::_bindGeometryToVao(
		const Handle		 p_hVao,
		const VertexLayout & p_layout,
		const Geometry &	 p_geom,
		const bool			 p_useIndices
	)
	{
		auto & vao = *_vertexArrays[ p_hVao ];
		vao.bind();

		GLuint location = 0;

		for ( const auto & a : p_layout.attributes )
		{
			const GLAttrib ga = toGLAttrib( a.type );

			// auto itBufKey = p_geom.attributeBuffers.find( a.name );
			// assert( itBufKey != p_geom.attributeBuffers.end() );

			// const Key & bufferKey = itBufKey->second;

			const Handle hVbo = 0;
			//_getOrCreateVertexBuffer( bufferKey );
			auto & vbo = *_vertexBuffers[ hVbo ];

			const GLsizei strideOneColumn = GLsizei( ga.components * ga.bytesPerComp );

			for ( uint8_t col = 0; col < ga.columns; ++col )
			{
				const GLuint   bindingIndex = location;
				const GLintptr offset		= GLintptr( col * strideOneColumn );

				vao.setVertexBuffer( bindingIndex, vbo, strideOneColumn, offset );

				++location;
			}
		}

		if ( p_useIndices )
		{
			assert( p_geom.indexBuffer.has_value() );

			const Handle hEbo = 0;
			//_getOrCreateIndexBuffer( *geom.indexBuffer );
			vao.bindElementBuffer( *_indexBuffers[ hEbo ] );
		}

		vao.unbind();
	}

	void OpenGL45::setShaderBufferData( const BufferShader & p_desc, SpanBytes p_bytes )
	{
		const Handle h	 = _getOrCreateShaderBuffer( p_desc );
		GL::Buffer & buf = *_shaderBuffers[ h ];

		if ( p_desc.mutability == E_BUFFER_MUTABILITY::MUTABLE )
		{
			buf.setData( p_bytes.data(), GLsizei( p_bytes.size() ), _toGL( p_desc.frequency ) );
		}
		else
		{
			assert( p_bytes.size() <= buf.size() );
			buf.setSub( p_bytes.data(), static_cast<GLsizeiptr>( p_bytes.size() ), 0 );
		}
	}

	void OpenGL45::setPipelineBufferData( const BufferPipeline & p_desc, SpanBytes p_bytes )
	{
		if ( p_desc.kind == E_PIPELINE_BUFFER_KIND::VERTEX )
		{
			const Handle h = _getOrCreateVertexBuffer( p_desc.name );
			_vertexBuffers[ h ]->setData( p_bytes.data(), GLsizei( p_bytes.size() ), _toGL( p_desc.frequency ) );
		}
		else
		{
			const Handle h = _getOrCreateIndexBuffer( p_desc.name );
			_indexBuffers[ h ]->setData( p_bytes.data(), GLsizei( p_bytes.size() ), _toGL( p_desc.frequency ) );
		}
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
