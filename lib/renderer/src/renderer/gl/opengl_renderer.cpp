#include "renderer/gl/opengl_renderer.hpp"
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer::GL
{
	OpenGLRenderer::OpenGLRenderer( void * p_proc, const FilePath & p_shaderPath )
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

		// Debug infos.
		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( _debugMessageCallback, NULL );

		// Program manager.
		_programManager = std::make_unique<ProgramManager>( p_shaderPath );

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
		_passSSAO.in.textureLinearizeDepth		 = &( _passGeometric.out.textureDepth );

		_passBlur.in.texture			   = &( _passSSAO.out.texture );
		_passBlur.in.textureLinearizeDepth = &( _passLinearizeDepth.out.texture );

		_passShading.in.textureViewPositionsNormals = &( _passGeometric.out.textureViewPositionsNormals );
		_passShading.in.texture						= &( _passGeometric.out.textureColors );
		_passShading.in.textureBlur					= &( _passBlur.out.texture );

		_passOutline.in.texture				  = &( _passShading.out.texture );
		_passOutline.in.textureLinearizeDepth = &( _passLinearizeDepth.out.texture );

		_passSelection.in.textureViewPositionsNormals = &( _passGeometric.out.textureViewPositionsNormals );
		_passSelection.in.texture					  = &( _passShading.out.texture );
		_passSelection.in.textureLinearizeDepth		  = &( _passLinearizeDepth.out.texture );

		_passFXAA.in.texture = &( _passSelection.out.texture );
	}

	void OpenGLRenderer::init( const size_t p_width, const size_t p_height )
	{
		VTX_INFO( "Initializing renderer..." );

		// Set size.
		_width	= p_width;
		_height = p_height;

		// Init passes.
		for ( Pass::BasePass * const pass : _passes )
		{
			pass->init( _width, _height, *_programManager );
		}

		// Init quad vao/vbo for deferred shading.
		std::vector<Vec2f> quadVertices
			= { Vec2f( -1.f, 1.f ), Vec2f( -1.f, -1.f ), Vec2f( 1.f, 1.f ), Vec2f( 1.f, -1.f ) };

		_vboQuad.create();
		_vaoQuad.create();
		_ubo.create();
		_ssbo.create();

		_vaoQuad.enableAttribute( 0 );
		_vaoQuad.setVertexBuffer( 0, _vboQuad, sizeof( Vec2f ) );
		_vaoQuad.setAttributeFormat( 0, 2, GL_FLOAT );
		_vaoQuad.setAttributeBinding( 0, 0 );

		_vboQuad.set( quadVertices );

		// Global uniforms buffer.
		_ubo.set( _globalUniforms, GL_DYNAMIC_DRAW );

		// Debug SSBO.
		auto debug = std::vector<Vec4f>( 100, Vec4f( 0.f, 1.f, 2.f, 30.f ) );
		_ssbo.set( debug, GL_DYNAMIC_STORAGE_BIT );

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
		_ubo.bind( GL_UNIFORM_BUFFER, 0 );
		_ssbo.bind( GL_SHADER_STORAGE_BUFFER, 10 );

		for ( Pass::BasePass * const pass : _passes )
		{
			pass->render( _vaoQuad );
		}

		_ssbo.unbind();
		_ubo.unbind();
	};

	const Vec2i OpenGLRenderer::getPickedIds( const uint p_x, const uint p_y )
	{
		return _passGeometric.getPickedData( p_x, p_y );
	}

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

} // namespace VTX::Renderer::GL
