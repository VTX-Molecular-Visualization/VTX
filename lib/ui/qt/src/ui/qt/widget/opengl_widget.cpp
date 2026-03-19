#include "ui/qt/widget/opengl_widget.hpp"
#include "app/services.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QGuiApplication>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/action/selection.hpp>
#include <app/events.hpp>
#include <qpa/qplatformnativeinterface.h>
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{

	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setAcceptDrops( true );

		// Create window.
		_window = new Window::Renderer();
		_window->setFlags( Qt::FramelessWindowHint );
		_window->installEventFilter( this );
		_window->create();

		// Use a widget container to embed the window.
		_container = createWindowContainer( _window, this );
		_container->installEventFilter( this );

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
					SELECTION().clearBut( E_SELECTION_GROUP::SYSTEM );

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

		// Setup resize timer.
		_resizeTimer.setSingleShot( true );
		connect( &_resizeTimer, &QTimer::timeout, this, &OpenGLWidget::onResizeFinished );
	}

	OpenGLWidget::~OpenGLWidget()
	{
		_container->removeEventFilter( this );
		_window->removeEventFilter( this );
	}

	uintptr_t OpenGLWidget::getNativeSurface() const
	{
		assert( _window );
		if ( QGuiApplication::platformName() == "wayland" )
		{
			QPlatformNativeInterface * nif = QGuiApplication::platformNativeInterface();
			if ( not nif )
			{
				throw std::runtime_error( "Qt: no platform native interface" );
			}
			void * surface = nif->nativeResourceForWindow( "surface", _window );
			if ( not surface )
			{
				throw std::runtime_error( "Qt: native surface is null" );
			}
			return reinterpret_cast<uintptr_t>( surface );
		}
		return static_cast<uintptr_t>( _window->winId() );
	}

	uintptr_t OpenGLWidget::getNativeDisplay() const
	{
		QPlatformNativeInterface * nif = QGuiApplication::platformNativeInterface();
		if ( not nif )
		{
			return 0;
		}

		if ( QGuiApplication::platformName() == "wayland" )
		{
			if ( void * display = nif->nativeResourceForIntegration( "display" ) )
			{
				return reinterpret_cast<uintptr_t>( display );
			}
		}
		else if ( QGuiApplication::platformName() == "xcb" )
		{
			if ( void * display = nif->nativeResourceForIntegration( "display" ) )
			{
				return reinterpret_cast<uintptr_t>( display );
			}
		}

		return 0;
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

		const QSize scaledSize = size * _window->devicePixelRatio();

		App::ACTION().execute<App::Action::Application::Resize>( scaledSize.width(), scaledSize.height() );
	}

	bool OpenGLWidget::eventFilter( QObject * p_watched, QEvent * p_event )
	{
		if ( p_watched == _container )
		{
			auto * e = p_event->clone();
			if ( p_event->type() == QEvent::DragEnter )
			{
				QCoreApplication::sendEvent( &MAIN_WINDOW(), e );
				delete e;
				return true;
			}
			else if ( p_event->type() == QEvent::Drop )
			{
				QCoreApplication::sendEvent( &MAIN_WINDOW(), e );
				delete e;
				return true;
			}
		}
		else if ( p_watched == _window )
		{
			if ( p_event->type() == QEvent::Expose )
			{
				onResizeFinished();
			}
			else if ( p_event->type() == QEvent::WindowActivate || p_event->type() == QEvent::Show )
			{
				_container->setFocus( Qt::ActiveWindowFocusReason );
			}
		}

		return QWidget::eventFilter( p_watched, p_event );
	}

} // namespace VTX::UI::QT::Widget
