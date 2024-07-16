#include "qt/widget/opengl_widget.hpp"

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
		//  format.setDepthBufferSize( 24 );
		//  format.setStencilBufferSize( 8 );

		// format.setRedBufferSize( 32 );
		// format.setGreenBufferSize( 32 );
		// format.setBlueBufferSize( 32 );
		// format.setAlphaBufferSize( 32 );

		// QSurfaceFormat::setDefaultFormat( format );

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
		_surface = new QWindow();
		_surface->setFormat( format );
		_surface->setSurfaceType( QSurface::OpenGLSurface );
		_surface->setFlags( Qt::FramelessWindowHint );
		_surface->create();

		// QScreen * screen = QGuiApplication::primaryScreen();
		// _context->setScreen( screen );
		// _surface->setScreen( screen );

		// Use a widget container to embed the window.
		_container = createWindowContainer( _surface, this );

		// Set context.
		_context->makeCurrent( _surface );

		//_container->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
		// setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

		// Connect signals.
		APP().onPostRender += [ this ]( const float )
		{
			render();
			// update();
			//_container->update();
			//_surface->requestUpdate();
		};

		// Connect signals.
		// connect( _surface, &QWindow::requestUpdate, this, &OpenGLWidget::render );
	}

	OpenGLWidget::~OpenGLWidget() { _context->doneCurrent(); }

	void OpenGLWidget::render()
	{
		// if ( not _device )
		//{
		//_device = new QOpenGLPaintDevice();
		//}

		//_device->setSize( size() * _surface->devicePixelRatio() );
		//_device->setDevicePixelRatio( _surface->devicePixelRatio() );

		// QPainter painter( _device );
		// render( &painter );

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

		App::VTX_ACTION().execute<App::Action::Application::Resize>(
			p_event->size().width(), p_event->size().height(), _context->defaultFramebufferObject()
		);

		/*
		if ( _device )
		{
			_device->setSize( p_event->size() * _surface->devicePixelRatio() );
			_device->setDevicePixelRatio( _surface->devicePixelRatio() );
		}
		*/
	}

} // namespace VTX::UI::QT::Widget
