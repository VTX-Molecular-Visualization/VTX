#include "ui/qt/widget/opengl_widget.hpp"
#include <app/action/application.hpp>

namespace VTX::UI::QT::Widget
{

	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		// Create surface.
		QSurfaceFormat format;
		format.setVersion( 4, 5 );
		format.setProfile( QSurfaceFormat::CoreProfile );
		format.setRenderableType( QSurfaceFormat::OpenGL );
		format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
		format.setSwapInterval( 1 );

		// Create context.
		_context = new QOpenGLContext();
		_context->setFormat( format );
		_context->setShareContext( nullptr );
		_context->create();

		if ( not _context->isValid() )
		{
			throw std::runtime_error( "Failed to create OpenGL context" );
		}

		// Create window.
		_surface = new OpenGLWindow();
		_surface->setFormat( format );
		_surface->setSurfaceType( QSurface::OpenGLSurface );
		_surface->setFlags( Qt::FramelessWindowHint );
		_surface->create();

		// Use a widget container to embed the window.
		_container = createWindowContainer( _surface, this );

		// Set context.
		_context->makeCurrent( _surface );

		// Connect signals.
		APP::onPostRender += [ this ]( const float ) { render(); };
	}

	OpenGLWidget::~OpenGLWidget() { _context->doneCurrent(); }

	void OpenGLWidget::render()
	{
		_context->swapBuffers( _surface );
		_context->makeCurrent( _surface );
	}

	void OpenGLWidget::resizeEvent( QResizeEvent * p_event )
	{
		assert( _surface );
		assert( _container );

		QWidget::resizeEvent( p_event );

		_surface->resize( p_event->size() );
		_container->resize( p_event->size() );

		App::ACTION_SYSTEM().execute<App::Action::Application::Resize>(
			p_event->size().width(), p_event->size().height(), _context->defaultFramebufferObject()
		);
	}

} // namespace VTX::UI::QT::Widget
