#include "renderer/context/backend/opengl45.hpp"
#include <util/exceptions.hpp>

namespace
{
	using namespace VTX::Renderer;

	/**
	 * @brief All GL mapping.
	 */
	constexpr GLenum _toGL( const E_FORMAT p_format )
	{
		switch ( p_format )
		{
		case E_FORMAT::RGB16F: return GL_RGB16F;
		case E_FORMAT::RGBA16F: return GL_RGBA16F;
		case E_FORMAT::RGBA32UI: return GL_RGBA32UI;
		case E_FORMAT::RGBA32F: return GL_RGBA32F;
		case E_FORMAT::RG32UI: return GL_RG32UI;
		case E_FORMAT::R8: return GL_R8;
		case E_FORMAT::R16F: return GL_R16F;
		case E_FORMAT::R32F: return GL_R32F;
		case E_FORMAT::DEPTH_COMPONENT32F: return GL_DEPTH_COMPONENT32F;
		default: assert( false );
		}
	}

	constexpr GLenum _toGL( const E_WRAPPING p_wrapping )
	{
		switch ( p_wrapping )
		{
		case E_WRAPPING::REPEAT: return GL_REPEAT;
		case E_WRAPPING::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
		case E_WRAPPING::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
		case E_WRAPPING::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
		case E_WRAPPING::MIRROR_CLAMP_TO_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
		default: assert( false );
		}
	}

	constexpr GLenum _toGL( const E_FILTERING p_filtering )
	{
		switch ( p_filtering )
		{
		case E_FILTERING::NEAREST: return GL_NEAREST;
		case E_FILTERING::LINEAR: return GL_LINEAR;
		case E_FILTERING::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
		case E_FILTERING::LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
		case E_FILTERING::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
		case E_FILTERING::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
		default: assert( false );
		}
	}
} // namespace

namespace VTX::Renderer::Context::Backend
{

	OpenGL45::OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc )
	{
		assert( p_width > 0 );
		assert( p_height > 0 );

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

		// Default output.
		glGetIntegerv( GL_FRAMEBUFFER_BINDING, reinterpret_cast<int *>( &_output ) );
		VTX_TRACE( "Default framebuffer: {}", _output );

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
		glDebugMessageCallback( _debugMessageCallback, nullptr );

		glEnable( GL_CLIP_DISTANCE0 );

		glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	}

	void OpenGL45::build( const RenderQueue & p_renderQueue, const Resources & p_resources, CommandBuffer & p_commands )
	{
		// Clear all.
		p_commands.push<E_COMMAND::CLEAR>();

		// Foreach resource.
		for ( const auto & [ key, texture ] : p_resources.textures )
		{
			//
		}
		for ( const auto & [ key, vertexStream ] : p_resources.vertexStreams )
		{
			//
		}
		for ( const auto & [ key, buffer ] : p_resources.buffers )
		{
			//
		}

		// Foreach pass.
		for ( const Pass * const pass : p_renderQueue )
		{
			//
		}
	}

	void OpenGL45::resize( const size_t p_width, const size_t p_height )
	{
		assert( p_width > 0 );
		assert( p_height > 0 );
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

	void APIENTRY OpenGL45::_debugMessageCallback(
		const GLenum   p_source,
		const GLenum   p_type,
		const GLuint   p_id,
		const GLenum   p_severity,
		const GLsizei  p_length,
		const GLchar * p_msg,
		const void *   p_data
	) noexcept
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
		case GL_DEBUG_SEVERITY_HIGH: VTX_ERROR( "{}", message ); break;
		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW: VTX_WARNING( "{}", message ); break;
		default: break;
		}
	}

} // namespace VTX::Renderer::Context::Backend
