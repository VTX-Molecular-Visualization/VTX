#include "gl.hpp"
#include "model/molecule.hpp"
#include "view/base_view_3d_molecule.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX::Renderer::GL
{
	GL::GL( OpenGLFunctions * const p_gl ) : BaseRenderer( p_gl )
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
		if ( _quadVAO != GL_INVALID_VALUE )
		{
			gl()->glDisableVertexArrayAttrib( _quadVAO, 0 );
			gl()->glDeleteVertexArrays( 1, &_quadVAO );
		}
		if ( _quadVBO != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_quadVBO );
		}

		delete _passGeometric;
		delete _passLinearizeDepth;
		delete _passSSAO;
		delete _passBlur;
		delete _passShading;
		delete _passOutline;
		delete _passSelection;
		delete _passFXAA;
	}

	const GLuint & GL::getRenderedTexture() const
	{
		return VTX_SETTING().activeAA ? _passFXAA->getTexture() : _passSelection->getTexture();
	}
	const GLuint & GL::getRenderedFBO() const
	{
		return VTX_SETTING().activeAA ? _passFXAA->getFbo() : _passSelection->getFbo();
	}

	void GL::init( const uint p_width, const uint p_height )
	{
		VTX_INFO( "Initializing renderer..." );

		// Set size.
		BaseRenderer::resize( p_width, p_height );

		// Init pass.
		_passGeometric->init( p_width, p_height );
		_passLinearizeDepth->init( _width, _height );
		_passSSAO->init( p_width, p_height );
		_passBlur->init( p_width, p_height );
		_passShading->init( p_width, p_height );
		_passOutline->init( p_width, p_height );
		_passSelection->init( p_width, p_height );
		_passFXAA->init( p_width, p_height );

		// Init VAO.
		_initQuadVAO();

		VTX_INFO( "Renderer initialized" );
	}

	void GL::resize( const uint p_width, const uint p_height )
	{
		if ( p_width != _width || p_height != _height )
		{
			BaseRenderer::resize( p_width, p_height );

			_passGeometric->resize( _width, _height );
			_passLinearizeDepth->resize( _width, _height );
			_passSSAO->resize( _width, _height );
			_passBlur->resize( _width, _height );
			_passShading->resize( _width, _height );
			_passOutline->resize( _width, _height );
			_passSelection->resize( _width, _height );
			_passFXAA->resize( _width, _height );
		}
	}

	void GL::_initQuadVAO()
	{
		// Init quad vao/vbo for deferred shading.

		// clang-format off
			Vec2f quadVertices[] =
			{
				Vec2f(-1.f,  1.f),
				Vec2f(-1.f,  -1.f),
				Vec2f(1.f,  1.f),
				Vec2f(1.f,  -1.f)
			};
		// clang-format on

		gl()->glCreateBuffers( 1, &_quadVBO );

		gl()->glCreateVertexArrays( 1, &_quadVAO );

		gl()->glEnableVertexArrayAttrib( _quadVAO, 0 );
		gl()->glVertexArrayVertexBuffer( _quadVAO, 0, _quadVBO, 0, sizeof( Vec2f ) );
		gl()->glVertexArrayAttribFormat( _quadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding( _quadVAO, 0, 0 );

		gl()->glNamedBufferData( _quadVBO, sizeof( quadVertices ), quadVertices, GL_STATIC_DRAW );
	}

	void GL::renderFrame( const Object3D::Scene & p_scene )
	{
		if ( !VTXApp::get().MASK )
		{
			return;
		}

		gl()->glViewport( 0, 0, _width, _height );

		// TODO: do not change each frame
		if ( VTX_SETTING().cameraNear == 0.f )
		{
			gl()->glEnable( GL_DEPTH_CLAMP );
		}
		else
		{
			gl()->glDisable( GL_DEPTH_CLAMP );
		}

		gl()->glEnable( GL_DEPTH_TEST );
		_passGeometric->render( p_scene, *this );
		gl()->glDisable( GL_DEPTH_TEST );

		_passLinearizeDepth->render( p_scene, *this );

		if ( VTX_SETTING().activeAO )
		{
			_passSSAO->render( p_scene, *this );
			_passBlur->render( p_scene, *this );
		}

		_passShading->render( p_scene, *this );

		if ( VTX_SETTING().activeOutline )
		{
			_passOutline->render( p_scene, *this );
		}

		_passSelection->render( p_scene, *this );

		if ( VTX_SETTING().activeAA )
		{
			_passFXAA->render( p_scene, *this );
		}

		VTXApp::get().MASK = VTX_SETTING().forceRenderer ? VTX_MASK_NEED_UPDATE : VTX_MASK_NO_UPDATE;
	};

	void GL::setShading() { _passShading->set(); }

	void GL::activeSSAO( const bool p_active )
	{
		if ( p_active == false )
		{
			_passBlur->clearTexture();
		}
	}
} // namespace VTX::Renderer::GL
