#include "gl.hpp"
#include <random>

namespace VTX::Renderer::GL
{
	GL::GL() : _quadVAO(), _quadVBO()
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

	void GL::init( const size_t p_width, const size_t p_height, const GLuint p_outputFramebufferId )
	{
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
	}

	void GL::resize( const size_t p_width, const size_t p_height, const GLuint p_outputFramebufferId )
	{
		_outputFramebuffer.assign( p_outputFramebufferId );
		_width	= p_width;
		_height = p_height;

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
			std::vector<glm::vec2> quadVertices =
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
		_quadVAO.setAttributeFormat( 0, 2, Wrapper::VertexArray::Type::FLOAT );
		_quadVAO.setAttributeBinding( 0, 0 );

		_quadVBO.set<Vec2f>( quadVertices, Wrapper::BufferData::Usage::STATIC_DRAW );
	}

	void GL::renderFrame()
	{
		// VTX_STAT().drawCalls = 0u;

		enableDepthTest();
		//_passGeometric->render( p_scene, *this );
		disableDepthTest();

		//_passLinearizeDepth->render( p_scene, *this );

		// if ( VTX_RENDER_EFFECT().isSSAOEnabled() )
		{
			//_passSSAO->render( p_scene, *this );
			//_passBlur->render( p_scene, *this );
		}

		//_passShading->render( p_scene, *this );

		// if ( VTX_RENDER_EFFECT().isOutlineEnabled() )
		{
			//_passOutline->render( p_scene, *this );
		}

		//_passSelection->render( p_scene, *this );

		// if ( VTX_RENDER_EFFECT().getAA() )
		{
			//_passFXAA->render( p_scene, *this );
		}

		// VTXApp::get().MASK = VTX_SETTING().getForceRenderer() ? VTX_MASK_NEED_UPDATE : VTX_MASK_NO_UPDATE;
	};

	void GL::updateRenderSetting( const RENDER_SETTING p_renderSetting )
	{
		switch ( p_renderSetting )
		{
		case RENDER_SETTING::SHADING: _passShading->set(); break;
		case RENDER_SETTING::SSAO:
			// if ( VTX_RENDER_EFFECT().isSSAOEnabled() == false )
			{
				_passBlur->clearTexture();
			}
			break;
		case RENDER_SETTING::OUTLINE: break;
		case RENDER_SETTING::FOG: break;
		case RENDER_SETTING::AA: _passSelection->updateOutputFBO( *this ); break;
		}
	}

	const Vec2i GL::getPickedIds( const size_t p_x, const size_t p_y ) const
	{
		_passGeometric->getFbo().bind( Wrapper::Framebuffer::Target::READ_FRAMEBUFFER );
		_passGeometric->getFbo().setReadBuffer( Wrapper::Framebuffer::Attachment::COLOR2 );
		Vec2i ids = Vec2i( -1, -1 );
		glReadPixels( p_x, p_y, 1, 1, GLenum( Wrapper::Texture2D::Format::RG_INTEGER ), GL_UNSIGNED_INT, &ids );
		_passGeometric->getFbo().unbind();

		return ids;
	}

} // namespace VTX::Renderer::GL
