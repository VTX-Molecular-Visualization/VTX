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
	constexpr GLPixelFormat _toGL( const Desc::E_FORMAT p_format ) noexcept
	{
		using namespace Desc;

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

	constexpr GLenum _toGL( const Desc::E_SHADER_BUFFER_KIND p_bufferRole ) noexcept
	{
		using namespace Desc;

		switch ( p_bufferRole )
		{
		case E_SHADER_BUFFER_KIND::PARAMETERS: return GL_UNIFORM_BUFFER;
		case E_SHADER_BUFFER_KIND::STRUCTURED: return GL_SHADER_STORAGE_BUFFER;
		default: assert( false ); return GL_INVALID_INDEX;
		}
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

	constexpr GLbitfield toGLMapFlags( const Desc::E_BUFFER_ACCESS p_access )
	{
		using namespace Desc;

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

	constexpr GLenum _toGL( const Desc::E_PRIMITIVE p_primitive ) noexcept
	{
		using namespace Desc;

		switch ( p_primitive )
		{
		case E_PRIMITIVE::POINTS: return GL_POINTS;
		case E_PRIMITIVE::LINES: return GL_LINES;
		case E_PRIMITIVE::TRIANGLES: return GL_TRIANGLE_STRIP;
		case E_PRIMITIVE::PATCHES: return GL_PATCHES;
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
		case E_TYPE::MAT3F: return { GL_FLOAT, 3, false, 4, 3 };
		case E_TYPE::MAT4F: return { GL_FLOAT, 4, false, 4, 4 };
		default: assert( false ); return {};
		}
	}

	uint32_t _toSettingFlags( const std::vector<Desc::Setting> & p_settings )
	{
		uint32_t mask = 0;
		for ( const Desc::Setting setting : p_settings )
		{
			mask |= setting;
		}
		return mask;
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

		_getOpenglInfos();
		_openglInfos.print();

		// Program manager.
		_programManager = std::make_unique<GL::ProgramManager>( p_shaderPath );

		// Quad.
		_createQuad();

		glViewport( 0, 0, int32_t( p_width ), int32_t( p_height ) );
		glPatchParameteri( GL_PATCH_VERTICES, 4 );

		// TODO: set from graph.
		// glEnable( GL_CLIP_DISTANCE0 );
		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 2.f );
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
	void OpenGL45::build(
		const Desc::RenderQueue & p_renderQueue,
		const Desc::Resources &	  p_resources,
		CommandBuffer &			  p_commands
	)
	{
		using namespace Desc;

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
			_pipelineBufferProperties.emplace( key, _PipelineBufferCacheEntry { buffer.kind, buffer.frequency } );

			switch ( buffer.kind )
			{
			case E_PIPELINE_BUFFER_KIND::VERTEX: _getOrCreateVertexBuffer( key ); break;
			case E_PIPELINE_BUFFER_KIND::INDEX: _getOrCreateIndexBuffer( key ); break;
			default: assert( false ); break;
			}
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

			// Resource table, clear each build.
			const Handle	hResourceTable = _getOrCreateResourceTable( pass, p_resources );
			ResourceTable & resourceTable  = _resourceTables[ hResourceTable ];
			resourceTable				   = _buildResourceTableForPass( pass, p_resources );

			// FBO.
			const Handle hFramebuffer = _getOrCreateFramebuffer( pass, p_resources, isLastPass );
			if ( not isLastPass )
			{
				_attachTexturesToFramebuffer( pass, p_resources.textures );
			}

			// Create programs.
			for ( const Program & program : pass.programs )
			{
				const Handle hProgram = _getOrCreateProgram( program );
			}

			// COMMANDS.
			// Push BEGIN_PASS.
			uint32_t		 flags = _toSettingFlags( pass.settings );
			PayloadBeginPass pBeginPass { hFramebuffer, flags };
			p_commands.push<E_COMMAND::BEGIN_PASS>( pBeginPass );

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
					hVao			 = _cacheVertexLayouts.at( g.vertexLayout );
				}
				else
				{
					hVao = _cacheVertexLayouts.at( _QUAD );
				}

				// Push DRAW.
				const Handle hProgram = _cachePrograms.at( program.name );
				if ( hasDrawCall )
				{
					const DrawCall & drawCall = program.drawCall.value();
					const Geometry & geometry = p_resources.geometries.at( drawCall.geometry );

					if ( geometry.indexBuffer )
					{
						PayloadDrawElement pDraw { hProgram, hVao };
						pDraw.primitive	 = toUnderlying( drawCall.primitive );
						pDraw.indexCount = drawCall.indexCount;
						p_commands.push<E_COMMAND::DRAW_ELEMENT>( pDraw );
					}
					else if ( drawCall.indexCount )
					{
						PayloadDrawArray pDraw { hProgram, hVao };
						pDraw.primitive	  = toUnderlying( drawCall.primitive );
						pDraw.vertexCount = drawCall.vertexCount;
						p_commands.push<E_COMMAND::DRAW_ARRAY>( pDraw );
					}
					else if ( drawCall.vertexRanges )
					{
						PayloadDrawArrays pDraw { hProgram, hVao };
						pDraw.primitive	   = toUnderlying( drawCall.primitive );
						pDraw.vertexRanges = reinterpret_cast<uintptr_t>( drawCall.vertexRanges );
						p_commands.push<E_COMMAND::DRAW_ARRAYS>( pDraw );
					}
					else if ( drawCall.indexRanges )
					{
						PayloadDrawElements pDraw { hProgram, hVao };
						pDraw.primitive	  = toUnderlying( drawCall.primitive );
						pDraw.indexRanges = reinterpret_cast<uintptr_t>( drawCall.indexRanges );
						p_commands.push<E_COMMAND::DRAW_ELEMENTS>( pDraw );
					}
					else
					{
						assert( false && "DrawCall has no valid draw parameters." );
					}
				}
				else
				{
					// Fullscreen quad draw.
					PayloadDrawArray pDraw { hProgram, hVao };
					pDraw.primitive	  = static_cast<uint32_t>( _toGL( E_PRIMITIVE::TRIANGLES ) );
					pDraw.vertexCount = 4;
					p_commands.push<E_COMMAND::DRAW_ARRAY>( pDraw );
				}
			}

			// Push END_PASS.
			PayloadEndPass pEndPass { flags };
			p_commands.push<E_COMMAND::END_PASS>( pEndPass );
		}

		// GLOBAL BINDINGS: done once at startup.
		for ( auto & bufferBinding : _globalShaderBuffers )
		{
			_shaderBuffers.at( bufferBinding.buffer )->bind( _toGL( bufferBinding.kind ), bufferBinding.binding );
		}
	}

	void OpenGL45::resize(
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

			_textures.at( _cacheTextures.at( key ) )->resize( texWidth, texHeight );
		}

		for ( const auto & pass : p_passes )
		{
			if ( _cacheFramebuffers.contains( pass->name ) ) // Avoid the last pass.
			{
				_attachTexturesToFramebuffer( *pass, p_textures );
			}
		}
	}

	Desc::Handle OpenGL45::_getOrCreateFramebuffer(
		const Desc::Pass &		p_pass,
		const Desc::Resources & p_res,
		const bool				p_isLastpass
	)
	{
		using namespace Desc;

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

	Desc::Handle OpenGL45::_getOrCreateResourceTable( const Desc::Pass & p_pass, const Desc::Resources & p_res )
	{
		using namespace Desc;

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

	Desc::Handle OpenGL45::_getOrCreateTexture( const Desc::Key & p_key, const Desc::Texture & p_text )
	{
		using namespace Desc;

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

	Desc::Handle OpenGL45::_getOrCreateSampler( const Desc::Key & p_key, const Desc::Sampler & p_text )
	{
		using namespace Desc;

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

	Desc::Handle OpenGL45::_getOrCreateVertexLayout(
		const Desc::Key &		   p_key,
		const Desc::VertexLayout & p_vertexStream
	)
	{
		using namespace Desc;

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

	Desc::Handle OpenGL45::_getOrCreateShaderBuffer( const Desc::BufferShader & p_buffer )
	{
		using namespace Desc;

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
		_shaderBufferProperties.emplace(
			key, _ShaderBufferCacheEntry { p_buffer.role, p_buffer.mutability, p_buffer.access, p_buffer.frequency }
		);

		return h;
	}

	Desc::Handle OpenGL45::_getOrCreateVertexBuffer( const Desc::Key & p_key )
	{
		using namespace Desc;

		auto it = _cacheVertexBuffers.find( p_key );
		if ( it != _cacheVertexBuffers.end() )
		{
			return it->second;
		}

		auto glBuffer = std::make_unique<GL::Buffer>();

		const Handle h = static_cast<Handle>( _vertexBuffers.size() );
		_vertexBuffers.emplace_back( std::move( glBuffer ) );
		_cacheVertexBuffers.emplace( p_key, h );

		return h;
	}

	Desc::Handle OpenGL45::_getOrCreateIndexBuffer( const Desc::Key & p_key )
	{
		using namespace Desc;

		auto it = _cacheIndexBuffers.find( p_key );
		if ( it != _cacheIndexBuffers.end() )
		{
			return it->second;
		}

		auto glBuffer = std::make_unique<GL::Buffer>();

		const Handle h = static_cast<Handle>( _indexBuffers.size() );
		_indexBuffers.emplace_back( std::move( glBuffer ) );
		_cacheIndexBuffers.emplace( p_key, h );

		return h;
	}

	Desc::Handle OpenGL45::_getOrCreateProgram( const Desc::Program & p_program )
	{
		using namespace Desc;

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

	OpenGL45::GlobalShaderBuffers OpenGL45::_buildGlobalShaderBuffers( const Desc::Resources & p_resources )
	{
		using namespace Desc;

		OpenGL45::GlobalShaderBuffers gsb;

		for ( const auto & [ key, buffer ] : p_resources.shaderBuffers )
		{
			assert( buffer.binding );
			const Handle hBuf = _cacheShaderBuffers.at( key );
			gsb.emplace_back( hBuf, buffer.role, *buffer.binding );
		}

		return gsb;
	}

	OpenGL45::ResourceTable OpenGL45::_buildResourceTableForPass(
		const Desc::Pass &		p_pass,
		const Desc::Resources & p_resources
	)
	{
		using namespace Desc;

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
				const Handle		 hBuf	= _cacheShaderBuffers.at( input.primary );
				const BufferShader & buffer = p_resources.shaderBuffers.at( input.primary );
				rt.shaderBuffers.emplace_back( hBuf, buffer.role, b++ );
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
				rt.shaderBuffers.emplace_back( hBuf, E_SHADER_BUFFER_KIND::PARAMETERS, b++ );
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
				const Handle		 hBuf	= _cacheShaderBuffers.at( output.primary );
				const BufferShader & buffer = p_resources.shaderBuffers.at( output.primary );
				rt.shaderBuffers.emplace_back( hBuf, buffer.role, b++ );

				break;
			}
			default: break;
			}
		}

		return rt;
	}

	void OpenGL45::_attachTexturesToFramebuffer(
		const Desc::Pass &						 p_pass,
		const Desc::ResourceMap<Desc::Texture> & p_textures
	)
	{
		using namespace Desc;

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

	void OpenGL45::_bindGeometryToVao(
		const Desc::Key &		p_key,
		const Desc::Geometry &	p_geo,
		const Desc::Resources & p_resources
	)
	{
		using namespace Desc;

		const Key &			 kVao	= p_geo.vertexLayout;
		const VertexLayout & layout = p_resources.vertexStreams.at( kVao );
		const auto &		 vao	= *_vertexArrays.at( _cacheVertexLayouts.at( kVao ) );

		vao.bind();

		GLuint location = 0;

		for ( const auto & a : layout.attributes )
		{
			const GLAttrib	   ga		 = toGLAttrib( a.type );
			const Key		   bufferKey = p_geo.vertexLayout + "." + a.name;
			const Handle	   hVbo		 = _cacheVertexBuffers.at( bufferKey );
			const GL::Buffer & vbo		 = *_vertexBuffers[ hVbo ];

			const GLsizei stride = GLsizei( ga.columns * ga.components * ga.bytesPerComp );
			for ( uint8_t col = 0; col < ga.columns; ++col )
			{
				const GLuint bindingIndex = location;
				vao.setVertexBuffer( bindingIndex, vbo, stride, 0 );
				++location;
			}
		}

		if ( p_geo.indexBuffer )
		{
			const Handle	   hIbo = _cacheIndexBuffers.at( *p_geo.indexBuffer );
			const GL::Buffer & ibo	= *_indexBuffers[ hIbo ];
			vao.bindElementBuffer( ibo );
		}

		vao.unbind();
	}

	void OpenGL45::_createQuad()
	{
		using namespace Desc;

		const std::array<Vec2f, 4> quad
			= { Vec2f { -1.f, 1.f }, Vec2f { -1.f, -1.f }, Vec2f { 1.f, 1.f }, Vec2f { 1.f, -1.f } };

		const Key quadLayoutKey = _QUAD;

		VertexLayout quadLayout;
		quadLayout.attributes = { VertexAttribute { "Position", E_TYPE::VEC2F } };

		Geometry quadGeo;
		quadGeo.vertexLayout = quadLayoutKey;
		quadGeo.indexBuffer	 = std::nullopt;

		_getOrCreateVertexLayout( quadLayoutKey, quadLayout );

		const Key quadVboKey = quadLayoutKey + ".Position";
		_getOrCreateVertexBuffer( quadVboKey );

		Resources fakeRes;
		fakeRes.vertexStreams.emplace( quadLayoutKey, quadLayout );
		fakeRes.geometries.emplace( quadLayoutKey, quadGeo );

		_bindGeometryToVao( quadLayoutKey, quadGeo, fakeRes );

		const Handle hVbo = _cacheVertexBuffers.at( quadVboKey );
		_vertexBuffers[ hVbo ]->setData( quad.data(), GLsizei( sizeof( quad ) ), GL_STATIC_DRAW );
	}

	void OpenGL45::setShaderBufferData( const Desc::Key & p_key, SpanBytes p_bytes )
	{
		using namespace Desc;

		const auto & bufferDesc = _shaderBufferProperties.at( p_key );
		const Handle h			= _cacheShaderBuffers.at( p_key );
		_shaderBuffers[ h ]->setData( p_bytes.data(), GLsizei( p_bytes.size() ), _toGL( bufferDesc.frequency ) );
	}

	void OpenGL45::setPipelineBufferData( const Desc::Key & p_key, SpanBytes p_bytes )
	{
		using namespace Desc;

		const auto & bufferDesc = _pipelineBufferProperties.at( p_key );
		GLenum		 freq		= _toGL( bufferDesc.frequency );
		if ( bufferDesc.kind == E_PIPELINE_BUFFER_KIND::VERTEX )
		{
			const Handle h = _getOrCreateVertexBuffer( p_key );
			_vertexBuffers[ h ]->setData( p_bytes.data(), GLsizei( p_bytes.size() ), freq );
		}
		else
		{
			const Handle h = _getOrCreateIndexBuffer( p_key );
			_indexBuffers[ h ]->setData( p_bytes.data(), GLsizei( p_bytes.size() ), freq );
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
