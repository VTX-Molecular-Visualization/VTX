#include "ui/qt/widget/renderer.hpp"
#include "app/services.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QGridLayout>
#include <QGuiApplication>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/action/selection.hpp>
#include <app/events.hpp>
#include <qpa/qplatformnativeinterface.h>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{

	Renderer::Renderer( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		constexpr int overlayBorderSize = 64;

		setAcceptDrops( true );

		// Create window.
		_window = new Window::Renderer();
		_window->setFlags( Qt::FramelessWindowHint );
		_window->installEventFilter( this );
		_window->create();

		// Use a widget container to embed the window.
		_container = createWindowContainer( _window, this );
		_container->installEventFilter( this );

		// Create transparent overlay for hud toolbars.
		_overlay = new QWidget( this );
		_overlay->setAttribute( Qt::WA_TranslucentBackground, true );
		_overlay->setAttribute( Qt::WA_TransparentForMouseEvents, true );
		_overlayLayout = new QGridLayout( _overlay );
		_overlayLayout->setContentsMargins( 0, 0, 0, 0 );
		_overlayLayout->setSpacing( 0 );
		_overlayLayout->setRowMinimumHeight( 0, overlayBorderSize );
		_overlayLayout->setRowMinimumHeight( 2, overlayBorderSize );
		_overlayLayout->setColumnMinimumWidth( 0, overlayBorderSize );
		_overlayLayout->setColumnMinimumWidth( 2, overlayBorderSize );
		_overlayLayout->setRowStretch( 0, 0 );
		_overlayLayout->setRowStretch( 1, 1 );
		_overlayLayout->setRowStretch( 2, 0 );
		_overlayLayout->setColumnStretch( 0, 0 );
		_overlayLayout->setColumnStretch( 1, 1 );
		_overlayLayout->setColumnStretch( 2, 0 );

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
		connect( &_resizeTimer, &QTimer::timeout, this, &Renderer::onResizeFinished );
	}

	Renderer::~Renderer()
	{
		_container->removeEventFilter( this );
		_window->removeEventFilter( this );
	}

	uintptr_t Renderer::getNativeSurface() const
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

	uintptr_t Renderer::getNativeDisplay() const
	{
		QPlatformNativeInterface * nif = QGuiApplication::platformNativeInterface();
		if ( nif == nullptr )
		{
			return 0;
		}

		if ( void * display = nif->nativeResourceForIntegration( "display" ) )
		{
			return reinterpret_cast<uintptr_t>( display );
		}

		return 0;
	}

	uint8_t Renderer::getNativePlatform() const
	{
		if ( QGuiApplication::platformName() == "wayland" )
		{
			return 3;
		}
		if ( QGuiApplication::platformName() == "xcb" )
		{
			return 2;
		}
#ifdef _WIN32
		return 1;
#else
		return 0;
#endif
	}

	void Renderer::resizeEvent( QResizeEvent * p_event )
	{
		QWidget::resizeEvent( p_event );

		_container->setVisible( false );
		_resizeTimer.start( 40 );
	}

	void Renderer::onResizeFinished()
	{
		assert( _window );
		assert( _container );

		_container->setVisible( true );

		const QSize size = this->size();
		_window->resize( size );
		_container->resize( size );
		_overlay->resize( size );
		_overlay->raise();

		const QSize scaledSize = size * _window->devicePixelRatio();

		App::ACTION().execute<App::Action::Application::Resize>( scaledSize.width(), scaledSize.height() );
	}

	bool Renderer::eventFilter( QObject * p_watched, QEvent * p_event )
	{
		if ( p_watched == _container )
		{
			if ( p_event->type() == QEvent::DragEnter )
			{
				auto * e = p_event->clone();
				QCoreApplication::sendEvent( &MAIN_WINDOW(), e );
				delete e;
				return true;
			}
			else if ( p_event->type() == QEvent::Drop )
			{
				auto * e = p_event->clone();
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

	void Renderer::_addHUDWidget( QWidget * const p_widget, const HUD_POSITION p_pos )
	{
		int row = 0;
		int col = 0;
		switch ( p_pos )
		{
		case HUD_POSITION::TOP_LEFT:
			row = 0;
			col = 0;
			break;
		case HUD_POSITION::TOP_CENTER:
			row = 0;
			col = 1;
			break;
		case HUD_POSITION::TOP_RIGHT:
			row = 0;
			col = 2;
			break;
		case HUD_POSITION::CENTER_LEFT:
			row = 1;
			col = 0;
			break;
		case HUD_POSITION::CENTER_RIGHT:
			row = 1;
			col = 2;
			break;
		case HUD_POSITION::BOTTOM_LEFT:
			row = 2;
			col = 0;
			break;
		case HUD_POSITION::BOTTOM_CENTER:
			row = 2;
			col = 1;
			break;
		case HUD_POSITION::BOTTOM_RIGHT:
			row = 2;
			col = 2;
			break;
		default: assert( false && "Invalid HUD position" );
		}
		p_widget->setParent( _overlay );
		// p_widget->setAttribute( Qt::WA_TranslucentBackground, true );
		p_widget->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
		_overlayLayout->addWidget( p_widget, row, col, Qt::AlignCenter );
	}

} // namespace VTX::UI::QT::Widget
