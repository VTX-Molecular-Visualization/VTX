#include "opengl_widget.hpp"
#include "exception.hpp"
#include <QOffScreenSurface>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : QOpenGLWidget( p_parent ) {}

			OpenGLWidget::~OpenGLWidget()
			{
				// delete _context;
				// delete _format;
			}

			void OpenGLWidget::initializeGL()
			{
				std::cout << "initializeGL" << std::endl;

				// Set format.
				// format.setDepthBufferSize( 24 );
				// format.setStencilBufferSize( 8 );
				//_format->setVersion( 4, 5 );
				//_format->setProfile( QSurfaceFormat::CompatibilityProfile );
				//_format->setRenderableType( QSurfaceFormat::OpenGL );
				// format.setOption( QSurfaceFormat::DebugContext );

				// QSurfaceFormat::setDefaultFormat( *_format );
				format().setVersion( 4, 5 );
				format().setProfile( QSurfaceFormat::CompatibilityProfile );
				format().setRenderableType( QSurfaceFormat::OpenGL );
				// Create context.
				// context()->setFormat( *_format );
				if ( context()->create() == false )
				{
					throw Exception::GLException( "OpenGL context creation failed" );
				}

				// Init openGL.
				if ( initializeOpenGLFunctions() == false )
				{
					throw Exception::GLException( "OpenGL initialization failed" );
				}
			}

			void OpenGLWidget::paintGL() { std::cout << "paintGL" << std::endl; }

			void OpenGLWidget::resizeGL( int p_width, int p_height ) { std::cout << "resizeGL" << std::endl; }

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
