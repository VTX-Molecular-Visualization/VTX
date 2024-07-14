#include "qt/opengl_widget.hpp"

namespace VTX::UI::QT
{

	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		QSurfaceFormat format;
		format.setVersion( 4, 5 );
		format.setProfile( QSurfaceFormat::CoreProfile );
		format.setRenderableType( QSurfaceFormat::OpenGL );
		format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
		format.setSwapInterval( 0 );
		format.setDepthBufferSize( 24 );
		format.setStencilBufferSize( 8 );
		QSurfaceFormat::setDefaultFormat( format );

		_context = new QOpenGLContext();
		_context->setFormat( format );
		_context->create();

		_surface = new QWindow();
		_surface->setFormat( format );
		_surface->setSurfaceType( QSurface::OpenGLSurface );
		_surface->setFlags( Qt::FramelessWindowHint );
		_surface->create();

		// QScreen * screen = QGuiApplication::primaryScreen();

		// _context->setScreen( screen );
		// _surface->setScreen( screen );

		_context->setShareContext( nullptr );

		if ( not _context->isValid() )
		{
			throw std::runtime_error( "Failed to create OpenGL context" );
		}

		_container = createWindowContainer( _surface, this );
		//_container->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
		// setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	}

	OpenGLWidget::~OpenGLWidget() { _context->doneCurrent(); }

	void OpenGLWidget::init()
	{
		_context->makeCurrent( _surface );

		// TODO: move in App.
		VTX::Renderer::Facade & rendererFacade = App::RENDERER_SYSTEM().facade();
		rendererFacade.build( _context->defaultFramebufferObject() );
		App::Component::Render::ProxyColorLayout & colorLayout
			= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyColorLayout>();
		colorLayout.setup( rendererFacade );
		rendererFacade.setProxyColorLayout( colorLayout.getProxy().proxy() );
		static VTX::Renderer::Proxy::Representation			representation;
		std::vector<VTX::Renderer::Proxy::Representation *> representations { &representation };
		rendererFacade.addProxyRepresentations( representations );

		App::Component::Render::ProxyCamera & proxyCamera
			= App::MAIN_REGISTRY().getComponent<App::Component::Render::ProxyCamera>( App::SCENE().getCamera() );
		proxyCamera.setInRenderer( rendererFacade );

		APP().onRender += [ this ]( const float p_deltaTime )
		{
			render();
			// update();
		};
	}

	void OpenGLWidget::render()
	{
		if ( not _device )
		{
			_device = new QOpenGLPaintDevice();
		}

		_device->setSize( size() * _surface->devicePixelRatio() );
		_device->setDevicePixelRatio( _surface->devicePixelRatio() );

		// QPainter painter( _device );
		// render( &painter );

		// TODO: move in main loop.
		App::RENDERER_SYSTEM().facade().render( 0.15f );

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

		/*
		if ( _device )
		{
			_device->setSize( p_event->size() * _surface->devicePixelRatio() );
			_device->setDevicePixelRatio( _surface->devicePixelRatio() );
		}
		*/

		// TODO: move?
		App::SCENE().getCamera().setScreenSize( width(), height() );
		VTX::Renderer::Facade & rendererFacade = App::RENDERER_SYSTEM().facade();
		rendererFacade.resize( p_event->size().width(), p_event->size().height() );
		rendererFacade.setOutput( _context->defaultFramebufferObject() );
	}

} // namespace VTX::UI::QT
