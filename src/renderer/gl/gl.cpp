#include "gl.hpp"
#include "generic/factory.hpp"
#include "model/molecule.hpp"
#include "setting.hpp"
#include "view/base_view_3d_molecule.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX
{
	namespace Renderer
	{
		GL::~GL()
		{
			Generic::destroy( _passGeometric );
			Generic::destroy( _passSSAO );
			Generic::destroy( _passBlur );
			Generic::destroy( _passShading );
			Generic::destroy( _passFXAA );
		}

		void GL::init( const uint p_width, const uint p_height )
		{
			VTX_INFO( "Initializing renderer..." );

			// Set size.
			resize( p_width, p_height );

			// Init pass.
			_passGeometric->init( _programManager, p_width, p_height );
			_passSSAO->init( _programManager, p_width, p_height );
			_passBlur->init( _programManager, p_width, p_height );
			_passShading->init( _programManager, p_width, p_height );

			// Init VAO.
			_initQuadVAO();

			VTX_INFO( "Renderer initialized" );
		}

		void GL::resize( const uint p_width, const uint p_height )
		{
			if ( p_width != _width || p_height != _height )
			{
				_passGeometric->clean();
				_passSSAO->clean();
				_passBlur->clean();
				_passShading->clean();

				BaseRenderer::resize( p_width, p_height );

				_passGeometric->init( _programManager, _width, _height );
				_passSSAO->init( _programManager, _width, _height );
				_passBlur->init( _programManager, _width, _height );
				_passShading->init( _programManager, _width, _height );
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
			glEnable( GL_DEPTH_TEST );
			_passGeometric->render( p_scene, *this );
			glDisable( GL_DEPTH_TEST );
			_passSSAO->render( p_scene, *this );
			_passBlur->render( p_scene, *this );
			_passShading->render( p_scene, *this );
		};

		void GL::setShading() { _passShading->set(); }
	} // namespace Renderer
} // namespace VTX
