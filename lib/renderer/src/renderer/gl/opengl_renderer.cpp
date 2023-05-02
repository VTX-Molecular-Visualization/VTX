#include "renderer/gl/opengl_renderer.hpp"
#include "renderer/gl/util.hpp"
#include <util/logger.hpp>

namespace VTX::Renderer::GL
{
	OpenGLRenderer::OpenGLRenderer( void * p_proc )
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
		glDebugMessageCallback( VTX::Renderer::GL::Util::debugMessageCallback, NULL );

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
			// pass->init( _width, _height, _programManager );
		}
		_passGeometric.init( _width, _height, _programManager );
		//_passLinearizeDepth.init( _width, _height, _programManager );
		//_passSSAO.init( _width, _height, _programManager );
		//_passBlur.init( _width, _height, _programManager );
		//_passShading.init( _width, _height, _programManager );
		//_passOutline.init( _width, _height, _programManager );
		//_passSelection.init( _width, _height, _programManager );
		//_passFXAA.init( _width, _height, _programManager );

		// Init quad vao/vbo for deferred shading.
		std::vector<Vec2f> quadVertices
			= { Vec2f( -1.f, 1.f ), Vec2f( -1.f, -1.f ), Vec2f( 1.f, 1.f ), Vec2f( 1.f, -1.f ) };

		_quadVBO.create();
		_quadVAO.create();

		_quadVAO.enableAttribute( 0 );
		_quadVAO.setVertexBuffer( 0, _quadVBO, sizeof( Vec2f ) );
		_quadVAO.setAttributeFormat( 0, 2, GL_FLOAT );
		_quadVAO.setAttributeBinding( 0, 0 );

		_quadVBO.set<Vec2f>( quadVertices );

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
		// VTX_STAT().drawCalls = 0u;
		for ( Pass::BasePass * const pass : _passes )
		{
			pass->render();
			_quadVAO.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		}

		/*
		_passGeometric->render( p_scene, *this );

		_passLinearizeDepth->render( p_scene, *this );

		if ( VTX_RENDER_EFFECT().isSSAOEnabled() )
		{
			_passSSAO->render( p_scene, *this );
			_passBlur->render( p_scene, *this );
		}

		_passShading->render( p_scene, *this );

		if ( VTX_RENDER_EFFECT().isOutlineEnabled() )
		{
			_passOutline->render( p_scene, *this );
		}

		_passSelection->render( p_scene, *this );

		if ( VTX_SETTING().getAA() )
		{
			_passFXAA->render( p_scene, *this );
		}

		VTXApp::get().MASK = VTX_SETTING().getForceRenderer() ? VTX_MASK_NEED_UPDATE : VTX_MASK_NO_UPDATE;
	*/
	};

	/*
	void GL::updateRenderSetting( const RENDER_SETTING p_renderSetting )
	{
		switch ( p_renderSetting )
		{
		case RENDER_SETTING::SHADING: _passShading->set(); break;
		case RENDER_SETTING::SSAO:
			if ( VTX_RENDER_EFFECT().isSSAOEnabled() == false )
			{
				_passBlur->clearTexture();
			}
			break;
		case RENDER_SETTING::OUTLINE: break;
		case RENDER_SETTING::FOG: break;
		case RENDER_SETTING::AA: _passSelection->updateOutputFBO( *this ); break;
		}
	}
	*/

	/*
	const Vec2i GL::getPickedIds( const uint p_x, const uint p_y ) const
	{
		_passGeometric->getFbo().bind( Framebuffer::Target::READ_FRAMEBUFFER );
		_passGeometric->getFbo().setReadBuffer( Framebuffer::Attachment::COLOR2 );
		Vec2i ids = Vec2i( Model::ID_UNKNOWN, Model::ID_UNKNOWN );
		_gl->glReadPixels( p_x, p_y, 1, 1, GLenum( Texture2D::Format::RG_INTEGER ), GL_UNSIGNED_INT, &ids );
		_passGeometric->getFbo().unbind();

		return ids;
	}
	*/

} // namespace VTX::Renderer::GL
