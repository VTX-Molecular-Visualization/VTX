#include "gl.hpp"
#include "generic/factory.hpp"
#include "model/molecule.hpp"
#include "view/base_view_3d_molecule.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX
{
	namespace Renderer
	{
		GL::~GL()
		{
			delete _passGeometric;
			delete _passLinearizeDepth;
			delete _passSSAO;
			delete _passBlur;
			delete _passShading;
			delete _passOutline;
			delete _passFXAA;
		}

		const GLuint & GL::getRenderedTexture() const
		{
			return VTX_SETTING().activeAA
					   ? _passFXAA->getTexture()
					   : VTX_SETTING().activeOutline ? _passOutline->getTexture() : _passShading->getTexture();
		}
		const GLuint & GL::getRenderedFBO() const
		{
			return VTX_SETTING().activeAA
					   ? _passFXAA->getFbo()
					   : VTX_SETTING().activeOutline ? _passOutline->getFbo() : _passShading->getFbo();
		}

		void GL::init( const uint p_width, const uint p_height )
		{
			VTX_INFO( "Initializing renderer..." );

			// Set size.
			BaseRenderer::resize( p_width, p_height );

			// Init pass.
			_passGeometric->init( _programManager, p_width, p_height );
			_passLinearizeDepth->init( _programManager, _width, _height );
			_passSSAO->init( _programManager, p_width, p_height );
			_passBlur->init( _programManager, p_width, p_height );
			_passShading->init( _programManager, p_width, p_height );
			_passOutline->init( _programManager, p_width, p_height );
			_passFXAA->init( _programManager, p_width, p_height );

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
				_passFXAA->resize( _width, _height );
			}
		}

		void GL::_initQuadVAO()
		{
			// Init quad vao/vbo for deferred shading.

			// clang-format off
			GLfloat quadVertices[] =
			{
				-1.0f,  1.0f,
				-1.0f, -1.0f, 
				 1.0f,  1.0f,
				 1.0f, -1.0f,
			};
			// clang-format on

			// Setup plane VAO.
			glGenBuffers( 1, &_quadVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _quadVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), quadVertices, GL_STATIC_DRAW );
			glGenVertexArrays( 1, &_quadVAO );
			glBindVertexArray( _quadVAO );
			glBindBuffer( GL_ARRAY_BUFFER, _quadVBO );
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );
		}

		void GL::renderFrame( const Object3D::Scene & p_scene )
		{
			glViewport( 0, 0, _width, _height );

			// TODO: do not change each frame
			if ( VTX_SETTING().cameraNear == 0.f )
			{
				glEnable( GL_DEPTH_CLAMP );
			}
			else
			{
				glDisable( GL_DEPTH_CLAMP );
			}

			glEnable( GL_DEPTH_TEST );
			_passGeometric->render( p_scene, *this );
			glDisable( GL_DEPTH_TEST );

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

			if ( VTX_SETTING().activeAA )
			{
				_passFXAA->render( p_scene, *this );
			}
		};

		void GL::setShading() { _passShading->set(); }

		void GL::activeSSAO( const bool p_active )
		{
			if ( p_active == false )
			{
				_passBlur->clearTexture();
			}
		}
	} // namespace Renderer
} // namespace VTX
