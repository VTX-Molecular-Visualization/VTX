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

		// const unsigned char * const glVendor   = glGetString( GL_VENDOR );
		// const unsigned char * const glRenderer = glGetString( GL_RENDERER );

		// VTX_INFO( "Device: " + glVendor + " " + glRenderer );
		VTX_INFO( "OpenGL initialized: {}.{}", GLVersion.major, GLVersion.minor );

#if ( VTX_OPENGL_VERSION == 450 )
		// Debug infos.
		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( _debugMessageCallback, nullptr );
#endif

		// Program manager.
		_programManager = std::make_unique<ProgramManager>( p_shaderPath );

		// Setup default routing.
		_passLinearizeDepth.in.textureDepth = &( _passGeometric.out.textureDepth );

		_passSSAO.in.textureDataPacked = &( _passGeometric.out.textureDataPacked );
		_passSSAO.in.textureDepth	   = &( _passLinearizeDepth.out.texture );

		_passBlur.in.textureColor = &( _passSSAO.out.texture );
		_passBlur.in.textureDepth = &( _passLinearizeDepth.out.texture );

		_passShading.in.textureDataPacked = &( _passGeometric.out.textureDataPacked );
		_passShading.in.texture			  = &( _passGeometric.out.textureColors );
		_passShading.in.textureBlur		  = &( _passBlur.out.texture );

		_passOutline.in.texture		 = &( _passShading.out.texture );
		_passOutline.in.textureDepth = &( _passLinearizeDepth.out.texture );

		_passSelection.in.textureDataPacked = &( _passGeometric.out.textureDataPacked );
		_passSelection.in.texture			= &( _passOutline.out.texture );
		_passSelection.in.textureDepth		= &( _passLinearizeDepth.out.texture );

		_passFXAA.in.texture = &( _passSelection.out.texture );
	}

	void OpenGLRenderer::init( const size_t p_width, const size_t p_height )
	{
		VTX_INFO( "Initializing renderer..." );

		// Set size.
		_width	= p_width;
		_height = p_height;

		// Init passes.
		_passGeometric.init( p_width, p_height, *_programManager );
		_passLinearizeDepth.init( p_width, p_height, *_programManager );
		_passSSAO.init( p_width, p_height, *_programManager );
		_passBlur.init( p_width, p_height, *_programManager );
		_passShading.init( p_width, p_height, *_programManager );
		_passOutline.init( p_width, p_height, *_programManager );
		_passSelection.init( p_width, p_height, *_programManager );
		_passFXAA.init( p_width, p_height, *_programManager );

		// Init quad vao/vbo for deferred shading.
		std::vector<Vec2f> quad = { Vec2f( -1.f, 1.f ), Vec2f( -1.f, -1.f ), Vec2f( 1.f, 1.f ), Vec2f( 1.f, -1.f ) };

		_vbo.create();
		_vao.create();
		_ubo.create();

		_vao.enableAttribute( 0 );
		_vao.setVertexBuffer<float>( 0, _vbo, sizeof( Vec2f ) );
		_vao.setAttributeFormat<float>( 0, 2 );
		_vao.setAttributeBinding( 0, 0 );

		_vbo.set( quad );

		// Global uniforms buffer.
		_ubo.set( _globalUniforms, GL_DYNAMIC_DRAW );

		glViewport( 0, 0, GLsizei( _width ), GLsizei( _height ) );

		VTX_INFO( "Renderer initialized" );
	}

	void OpenGLRenderer::resize( const size_t p_width, const size_t p_height )
	{
		_width	= p_width;
		_height = p_height;

		_passGeometric.resize( _width, _height );
		_passLinearizeDepth.resize( _width, _height );
		_passSSAO.resize( _width, _height );
		_passBlur.resize( _width, _height );
		_passShading.resize( _width, _height );
		_passOutline.resize( _width, _height );
		_passSelection.resize( _width, _height );
		_passFXAA.resize( _width, _height );

		glViewport( 0, 0, GLsizei( _width ), GLsizei( _height ) );
	}

	void OpenGLRenderer::renderFrame()
	{
		if ( _needUpdate )
		{
			//_vao.drawCalls = 0;

			_ubo.bind( GL_UNIFORM_BUFFER, 15 );

			_passGeometric.render( _vao );
			_passLinearizeDepth.render( _vao );
			_passSSAO.render( _vao );
			_passBlur.render( _vao );
			_passShading.render( _vao );
			_passOutline.render( _vao );
			_passSelection.render( _vao );
			_passFXAA.render( _vao );

			_ubo.unbind();

			//_needUpdate = false;
		}
	}

	const Vec2i OpenGLRenderer::getPickedIds( const uint p_x, const uint p_y )
	{
		return _passGeometric.getPickedData( p_x, p_y );
	}

	void OpenGLRenderer::setMatrixModelTmp( const Mat4f & p_model )
	{
		_ubo.setSub( p_model, 0, sizeof( Mat4f ) );
		_ubo.setSub( Util::Math::transpose( Util::Math::inverse( p_model ) ), 1 * sizeof( Mat4f ), sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setMatrixView( const Mat4f & p_view )
	{
		_ubo.setSub( p_view, 2 * sizeof( Mat4f ), sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setMatrixProjection( const Mat4f & p_proj )
	{
		_ubo.setSub( p_proj, 3 * sizeof( Mat4f ), sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setBackgroundColor( Util::Color::Rgba & p_color )
	{
		_ubo.setSub( p_color, 18 * sizeof( Vec4f ), sizeof( Util::Color::Rgba ) );
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
