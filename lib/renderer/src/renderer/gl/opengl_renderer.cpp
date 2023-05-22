#include "renderer/gl/opengl_renderer.hpp"
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer::GL
{
	OpenGLRenderer::OpenGLRenderer( void *			 p_proc,
									const size_t	 p_width,
									const size_t	 p_height,
									const FilePath & p_shaderPath ) :
		_width( p_width ),
		_height( p_height ), _programManager( p_shaderPath ), _passGeometric( p_width, p_height, _programManager ),
		_passLinearizeDepth( p_width, p_height, _programManager ), _passSSAO( p_width, p_height, _programManager ),
		_passBlur( p_width, p_height, _programManager ), _passShading( p_width, p_height, _programManager ),
		_passOutline( p_width, p_height, _programManager ), _passSelection( p_width, p_height, _programManager ),
		_passFXAA( p_width, p_height, _programManager )

	{
		VTX_INFO( "Creating renderer..." );

		// Load OpenGL.
		if ( gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
		{
			throw GLException( "Failed to initialize GLAD" );
		}

		const unsigned char * const glVendor   = glGetString( GL_VENDOR );
		const unsigned char * const glRenderer = glGetString( GL_RENDERER );

		// VTX_INFO( "Device: " + glVendor + " " + glRenderer );
		VTX_INFO( "OpenGL initialized: {}.{}", GLVersion.major, GLVersion.minor );

#if ( VTX_OPENGL_VERSION == 450 )
		// Debug infos.
		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( _debugMessageCallback, nullptr );
#endif

		// Add passes.
		_passes.emplace_back( &_passGeometric );
		_passes.emplace_back( &_passLinearizeDepth );
		_passes.emplace_back( &_passSSAO );
		_passes.emplace_back( &_passBlur );
		_passes.emplace_back( &_passShading );
		_passes.emplace_back( &_passOutline );
		_passes.emplace_back( &_passSelection );
		_passes.emplace_back( &_passFXAA );

		// Setup default routing.
		_passLinearizeDepth.in.textureDepth = &( _passGeometric.out.textureDepth );

		_passSSAO.in.textureViewPositionsNormals = &( _passGeometric.out.textureViewPositionsNormals );
		_passSSAO.in.textureDepth				 = &( _passLinearizeDepth.out.texture );

		_passBlur.in.texture			   = &( _passSSAO.out.texture );
		_passBlur.in.textureLinearizeDepth = &( _passLinearizeDepth.out.texture );

		_passShading.in.textureViewPositionsNormals = &( _passGeometric.out.textureViewPositionsNormals );
		_passShading.in.texture						= &( _passGeometric.out.textureColors );
		_passShading.in.textureBlur					= &( _passBlur.out.texture );

		_passOutline.in.texture		 = &( _passShading.out.texture );
		_passOutline.in.textureDepth = &( _passLinearizeDepth.out.texture );

		_passSelection.in.textureViewPositionsNormals = &( _passGeometric.out.textureViewPositionsNormals );
		_passSelection.in.texture					  = &( _passOutline.out.texture );
		_passSelection.in.textureDepth				  = &( _passLinearizeDepth.out.texture );

		_passFXAA.in.texture = &( _passSelection.out.texture );

		// Init quad vao/vbo for deferred shading.
		std::vector<Vec2f> quadVertices
			= { Vec2f( -1.f, 1.f ), Vec2f( -1.f, -1.f ), Vec2f( 1.f, 1.f ), Vec2f( 1.f, -1.f ) };

		_vaoQuad.enableAttribute( 0 );
		_vaoQuad.setVertexBuffer<float>( 0, _vboQuad, sizeof( Vec2f ) );
		_vaoQuad.setAttributeFormat<float>( 0, 2 );
		_vaoQuad.setAttributeBinding( 0, 0 );

		_vboQuad.set( quadVertices );

		// Global uniforms buffer.
		_ubo.set( _globalUniforms, GL_DYNAMIC_DRAW );

		VTX_INFO( "Renderer initialized" );
	}

	void OpenGLRenderer::resize( const size_t p_width, const size_t p_height )
	{
		for ( Pass::BasePass * const pass : _passes )
		{
			pass->resize( _width, _height );
		}
	}

	void OpenGLRenderer::renderFrame()
	{
		_vaoQuad.drawCalls = 0;
		_ubo.bind( GL_UNIFORM_BUFFER, 15 );

		for ( Pass::BasePass * const pass : _passes )
		{
			pass->render( _vaoQuad );
		}

		_ubo.unbind();
	};

	const Vec2i OpenGLRenderer::getPickedIds( const uint p_x, const uint p_y )
	{
		return _passGeometric.getPickedData( p_x, p_y );
	}

	void OpenGLRenderer::setCameraMatrix( const Mat4f & p_view, const Mat4f & p_proj )
	{
		_ubo.setSub( p_view, 0, sizeof( Mat4f ) );
		_ubo.setSub( p_proj, sizeof( Mat4f ), sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setBackgroundColor( Util::Color::Rgba & p_color )
	{
		_ubo.setSub( p_color, 10 * sizeof( Vec4f ), sizeof( Util::Color::Rgba ) );
	}

#if ( VTX_OPENGL_VERSION == 450 )
	void APIENTRY OpenGLRenderer::_debugMessageCallback( const GLenum	p_source,
														 const GLenum	p_type,
														 const GLuint	p_id,
														 const GLenum	p_severity,
														 const GLsizei	p_length,
														 const GLchar * p_msg,
														 const void *	p_data )
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
#endif

} // namespace VTX::Renderer::GL
