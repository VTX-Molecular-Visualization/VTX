#include "app/render/renderer/gl/gl.hpp"
#include "app/application/render_effect/render_effect_preset.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/render/view/base_view_3d_molecule.hpp"
#include "app/vtx_app.hpp"
#include <random>

namespace VTX::App::Render::Renderer::GL
{
	GL::GL() : BaseRenderer(), _quadVAO(), _quadVBO()
	{
		_passGeometric		= new Pass::Geometric();
		_passLinearizeDepth = new Pass::LinearizeDepth();
		_passSSAO			= new Pass::SSAO();
		_passBlur			= new Pass::Blur();
		_passShading		= new Pass::Shading();
		_passOutline		= new Pass::Outline();
		_passSelection		= new Pass::Selection();
		_passFXAA			= new Pass::FXAA();
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

		// Set size.
		BaseRenderer::resize( p_width, p_height, p_outputFramebufferId );

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

		_quadVBO.set<Vec2f>( quadVertices );
	}

	void GL::renderFrame( const App::Application::Scene & p_scene )
	{
		VTX_STAT().drawCalls = 0u;

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

		if ( VTX_SETTING().getAA() )
		{
			_passFXAA->render( p_scene, *this );
		}

		App::VTXApp::get().MASK = VTX_SETTING().getForceRenderer() ? Render::VTX_MASK_NEED_UPDATE : Render::VTX_MASK_NO_UPDATE;
	};

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

	// TODO reimplement this without Qt (OpenGL embedded by Qt)
	const Vec2i GL::getPickedIds( const uint p_x, const uint p_y ) const
	{
		return { 0, 0 };
		//_passGeometric->getFbo().bind( Framebuffer::Target::READ_FRAMEBUFFER );
		//_passGeometric->getFbo().setReadBuffer( Framebuffer::Attachment::COLOR2 );
		// Vec2i ids = Vec2i( App::Core::Model::ID_UNKNOWN, App::Core::Model::ID_UNKNOWN );
		//_gl->glReadPixels( p_x, p_y, 1, 1, GLenum( Texture2D::Format::RG_INTEGER ), GL_UNSIGNED_INT, &ids );
		//_passGeometric->getFbo().unbind();

		// return ids;
	}

} // namespace VTX::App::Render::Renderer::GL
