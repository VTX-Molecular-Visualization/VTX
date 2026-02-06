#include "ui/qt/widget/opengl_widget.hpp"
#include "app/services.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/action/selection.hpp>
#include <app/events.hpp>
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>

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
		_window = new Window::Renderer();
		_window->setFormat( format );
		_window->setSurfaceType( QSurface::OpenGLSurface );
		_window->setFlags( Qt::FramelessWindowHint );
		_window->create();

		// Use a widget container to embed the window.
		_container = createWindowContainer( _window, this );
		_container->installEventFilter( this );

		// Set context.
		_context->makeCurrent( _window );

		// Focus policy.
		_container->setFocusPolicy( Qt::StrongFocus );
		this->setFocusPolicy( Qt::NoFocus );
		this->setFocusProxy( _container );

		// Connect picking.
		connect(
			_window,
			&Window::Renderer::clicked,
			[]( const Qt::MouseButton, const QPoint p_pos )
			{
				if ( not SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool() )
				{
					App::ACTION().execute<App::Action::Selection::Pick>(
						Vec2i( p_pos.x(), p_pos.y() ),
						static_cast<App::Action::Selection::E_GRANULARITY>(
							SETTINGS().value( SETTING_KEY_GRANULARITY, 0 ).toInt()
						),
						QGuiApplication::keyboardModifiers() & Qt::ControlModifier
					);
				}
			}
		);

		// Connect signals.
		App::HUB().connect<App::Events::PostRender, &OpenGLWidget::render>( this );

		// Setup resize timer.
		_resizeTimer.setSingleShot( true );
		connect( &_resizeTimer, &QTimer::timeout, this, &OpenGLWidget::onResizeFinished );
	}

	OpenGLWidget::~OpenGLWidget()
	{
		_context->doneCurrent();
		_container->removeEventFilter( this );
	}

	void OpenGLWidget::render()
	{
		_context->swapBuffers( _window );
		_context->makeCurrent( _window );
	}

	void OpenGLWidget::resizeEvent( QResizeEvent * p_event )
	{
		QWidget::resizeEvent( p_event );

		_container->setVisible( false );
		_resizeTimer.start( 40 );
	}

	void OpenGLWidget::onResizeFinished()
	{
		assert( _window );
		assert( _container );

		_container->setVisible( true );

		const QSize size = this->size();
		_window->resize( size );
		_container->resize( size );

		const QSize scaledSize = size * devicePixelRatioF();

		App::ACTION().execute<App::Action::Application::Resize>( scaledSize.width(), scaledSize.height() );
	}

	void OpenGLWidget::setVSync( const bool p_vsync )
	{
		assert( _context != nullptr );

		// Windows.
		auto wglSwapIntervalEXT
			= reinterpret_cast<void ( * )( int )>( _context->getProcAddress( "wglSwapIntervalEXT" ) );

		if ( wglSwapIntervalEXT )
		{
			wglSwapIntervalEXT( p_vsync ? 1 : 0 );
		}

		// Linux.
		auto glXSwapIntervalEXT
			= reinterpret_cast<void ( * )( int )>( _context->getProcAddress( "glXSwapIntervalEXT" ) );

		if ( glXSwapIntervalEXT )
		{
			glXSwapIntervalEXT( p_vsync ? 1 : 0 );
		}
	}

	bool OpenGLWidget::eventFilter( QObject * p_watched, QEvent * p_event )
	{
		if ( p_watched == _container )
		{
			auto * e = p_event->clone();
			if ( p_event->type() == QEvent::DragEnter )
			{
				QCoreApplication::sendEvent( &MAIN_WINDOW(), e );
				return true;
			}
			else if ( p_event->type() == QEvent::Drop )
			{
				QCoreApplication::sendEvent( &MAIN_WINDOW(), e );
				return true;
			}
		}
		return QWidget::eventFilter( p_watched, p_event );
	}

} // namespace VTX::UI::QT::Widget
