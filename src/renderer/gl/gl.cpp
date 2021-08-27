#include "gl.hpp"
#include "model/molecule.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "view/base_view_3d_molecule.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX::Renderer::GL
{
	GL::GL( OpenGLFunctions * const p_gl ) : BaseRenderer( p_gl ), _quadVAO( p_gl ), _quadVBO( p_gl )
	{
		_passGeometric		= new Pass::Geometric( p_gl );
		_passLinearizeDepth = new Pass::LinearizeDepth( p_gl );
		_passSSAO			= new Pass::SSAO( p_gl );
		_passBlur			= new Pass::Blur( p_gl );
		_passShading		= new Pass::Shading( p_gl );
		_passOutline		= new Pass::Outline( p_gl );
		_passSelection		= new Pass::Selection( p_gl );
		_passFXAA			= new Pass::FXAA( p_gl );
	}

	GL::~GL()
	{
		delete _passGeometric;
		delete _passLinearizeDepth;
		delete _passSSAO;
		delete _passBlur;
		delete _passShading;
		delete _passOutline;
		delete _passSelection;
		delete _passFXAA;
	}

	void GL::init( const uint p_width, const uint p_height, const GLuint p_outputFramebufferId )
	{
		VTX_INFO( "Initializing renderer..." );

		/// TODO: here we cannot set the output framebuffer cause it isn't create
		/// So "assert( _gl->glIsFramebuffer( p_id ) );" fails in Framebuffer::asign
		// Set size.
		// BaseRenderer::resize( p_width, p_height, p_outputFramebufferId );
		_width	= p_width;
		_height = p_height;

		// Init pass.
		_passGeometric->init( _width, _height, *this );
		_passLinearizeDepth->init( _width, _height, *this );
		_passSSAO->init( _width, _height, *this );
		_passBlur->init( _width, _height, *this );
		_passShading->init( _width, _height, *this );
		_passOutline->init( _width, _height, *this );
		_passSelection->init( _width, _height, *this );
		_passFXAA->init( _width, _height, *this );

		// Init VAO.
		_initQuadVAO();

		VTX_INFO( "Renderer initialized" );
	}

	void GL::resize( const uint p_width, const uint p_height, const GLuint p_outputFramebufferId )
	{
		if ( p_width != _width || p_height != _height )
		{
			BaseRenderer::resize( p_width, p_height, p_outputFramebufferId );

			_passGeometric->resize( _width, _height, *this );
			_passLinearizeDepth->resize( _width, _height, *this );
			_passSSAO->resize( _width, _height, *this );
			_passBlur->resize( _width, _height, *this );
			_passShading->resize( _width, _height, *this );
			_passOutline->resize( _width, _height, *this );
			_passSelection->resize( _width, _height, *this );
			_passFXAA->resize( _width, _height, *this );
		}
	}

	void GL::_initQuadVAO()
	{
		// Init quad vao/vbo for deferred shading.

		// clang-format off
			std::vector<Vec2f> quadVertices =
			{
				Vec2f(-1.f,  1.f),
				Vec2f(-1.f,  -1.f),
				Vec2f(1.f,  1.f),
				Vec2f(1.f,  -1.f)
			};
		// clang-format on

		_quadVBO.create();

		_quadVAO.create();

		_quadVAO.enableAttribute( 0 );
		_quadVAO.setVertexBuffer( 0, _quadVBO, sizeof( Vec2f ) );
		_quadVAO.setAttributeFormat( 0, 2, VertexArray::Type::FLOAT );
		_quadVAO.setAttributeBinding( 0, 0 );

		_quadVBO.set<Vec2f>( quadVertices, Buffer::Usage::STATIC_DRAW );
	}

	void GL::renderFrame( const Object3D::Scene & p_scene )
	{
		VTX_STAT().drawCalls = 0u;

		getGL()->glViewport( 0, 0, _width, _height );

		// TODO: do not change each frame
		/// TODO2: why this?
		if ( VTX_RENDER_EFFECT().getCameraNearClip() == 0.f )
		{
			enableDepthClamp();
		}
		else
		{
			disableDepthClamp();
		}

		enableDepthTest();
		_passGeometric->render( p_scene, *this );
		disableDepthTest();

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

		if ( VTX_RENDER_EFFECT().getAA() )
		{
			_passFXAA->render( p_scene, *this );
		}

		VTXApp::get().MASK = VTX_SETTING().getForceRenderer() ? VTX_MASK_NEED_UPDATE : VTX_MASK_NO_UPDATE;
	};

	void GL::setShading() { _passShading->set(); }

	void GL::activeSSAO( const bool p_active )
	{
		if ( p_active == false )
		{
			_passBlur->clearTexture();
		}
	}

	void GL::activeAA( const bool p_active ) { _passSelection->updateOutputFBO( *this ); }

} // namespace VTX::Renderer::GL
