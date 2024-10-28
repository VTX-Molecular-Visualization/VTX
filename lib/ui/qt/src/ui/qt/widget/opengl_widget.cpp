#include "ui/qt/widget/opengl_widget.hpp"
#include "ui/qt/application.hpp"
#include <app/action/application.hpp>

namespace VTX::UI::QT::Widget
{

	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setAcceptDrops( true );

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
		_window = new Window::EventCatchWindow();
		_window->setFormat( format );
		_window->setSurfaceType( QSurface::OpenGLSurface );
		_window->setFlags( Qt::FramelessWindowHint );
		_window->create();

		// Use a widget container to embed the window.
		_container = createWindowContainer( _window, this );
		//_container->setAcceptDrops( true );
		_container->installEventFilter( this );

		// Set context.
		_context->makeCurrent( _window );

		// Connect signals.
		APP::onPostRender += [ this ]( const float ) { render(); };
	}

	OpenGLWidget::~OpenGLWidget() { _context->doneCurrent(); }

	void OpenGLWidget::render()
	{
		_context->swapBuffers( _window );
		_context->makeCurrent( _window );
	}

	void OpenGLWidget::resizeEvent( QResizeEvent * p_event )
	{
		assert( _window );
		assert( _container );

		QWidget::resizeEvent( p_event );

		_window->resize( p_event->size() );
		_container->resize( p_event->size() );

		App::ACTION_SYSTEM().execute<App::Action::Application::Resize>(
			p_event->size().width(), p_event->size().height(), _context->defaultFramebufferObject()
		);
	}

	void OpenGLWidget::setVSync( const bool p_vsync )
	{
		assert( _context != nullptr );

		// Windows only.
		auto wglSwapIntervalEXT
			= reinterpret_cast<void ( * )( int )>( _context->getProcAddress( "wglSwapIntervalEXT" ) );

		if ( wglSwapIntervalEXT )
		{
			wglSwapIntervalEXT( p_vsync ? 1 : 0 );
		}
	}

	bool OpenGLWidget::eventFilter( QObject * p_watched, QEvent * p_event )
	{
		if ( p_watched == _container )
		{
			auto * e = p_event->clone();
			if ( p_event->type() == QEvent::DragEnter )
			{
				QCoreApplication::sendEvent( APP_QT::getMainWindow(), e );
				return true;
			}
			else if ( p_event->type() == QEvent::Drop )
			{
				QCoreApplication::sendEvent( APP_QT::getMainWindow(), e );
				return true;
			}
		}
		return QWidget::eventFilter( p_watched, p_event );
	}

} // namespace VTX::UI::QT::Widget
