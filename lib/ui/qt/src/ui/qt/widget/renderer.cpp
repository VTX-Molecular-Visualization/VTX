#include "ui/qt/widget/renderer.hpp"
#include "app/services.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QCoreApplication>
#include <QGuiApplication>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <qpa/qplatformnativeinterface.h>
#include <renderer/renderer.hpp>

namespace VTX::UI::QT::Widget
{
	Renderer::Renderer( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setAcceptDrops( true );

		_window = new Window::Renderer();
		_window->setFlags( Qt::FramelessWindowHint );
		_window->create();

		_container = createWindowContainer( _window, this );
		_container->installEventFilter( this );
		_container->setFocusPolicy( Qt::StrongFocus );
		setFocusPolicy( Qt::NoFocus );
		setFocusProxy( _container );

		connect(
			_window,
			&Window::Renderer::clicked,
			this,
			[]( const Qt::MouseButton, const QPoint p_pos )
			{
				if ( not SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool() )
				{
					SELECTION().pick(
						Vec2i( p_pos.x(), p_pos.y() ), QGuiApplication::keyboardModifiers() & Qt::ControlModifier
					);
				}
			}
		);

		_resizeTimer.setSingleShot( true );
		connect( &_resizeTimer, &QTimer::timeout, this, &Renderer::onResizeFinished );
	}

	Renderer::~Renderer()
	{
		if ( _container != nullptr )
		{
			_container->removeEventFilter( this );
		}
	}

	uintptr_t Renderer::getNativeSurface() const
	{
		assert( _window != nullptr );

		if ( QGuiApplication::platformName() == "wayland" )
		{
			QPlatformNativeInterface * const nif = QGuiApplication::platformNativeInterface();
			if ( not nif )
			{
				throw std::runtime_error( "Qt: no platform native interface" );
			}

			void * const surface = nif->nativeResourceForWindow( "surface", _window );
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
		QPlatformNativeInterface * const nif = QGuiApplication::platformNativeInterface();
		if ( nif == nullptr )
		{
			return 0;
		}

		if ( void * const display = nif->nativeResourceForIntegration( "display" ) )
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

	bool Renderer::eventFilter( QObject * p_watched, QEvent * p_event )
	{
		if ( p_watched == _container && p_event != nullptr )
		{
			if ( p_event->type() == QEvent::DragEnter || p_event->type() == QEvent::Drop )
			{
				auto * const clonedEvent = p_event->clone();
				QCoreApplication::sendEvent( &MAIN_WINDOW(), clonedEvent );
				delete clonedEvent;
				return true;
			}
		}

		return QWidget::eventFilter( p_watched, p_event );
	}

	void Renderer::showEvent( QShowEvent * p_event )
	{
		QWidget::showEvent( p_event );
		_syncHUDWidgets( true );
		_focusRenderer();
	}

	void Renderer::resizeEvent( QResizeEvent * p_event )
	{
		QWidget::resizeEvent( p_event );

		_syncHUDWidgets();

		if ( _container != nullptr )
		{
			_container->setVisible( false );
		}

		_resizeTimer.start( 40 );
	}

	void Renderer::onResizeFinished()
	{
		assert( _window != nullptr );
		assert( _container != nullptr );

		_container->setVisible( true );
		_focusRenderer();

		const QSize size = this->size();
		_window->resize( size );
		_container->resize( size );
		_syncHUDWidgets();

		const QSize scaledSize = size * _window->devicePixelRatio();
		App::ACTION().execute<App::Action::Application::Resize>( scaledSize.width(), scaledSize.height() );
	}

	void Renderer::_focusRenderer()
	{
		if ( _container == nullptr || _window == nullptr || not isVisible() )
		{
			return;
		}

		//_container->setFocus( Qt::OtherFocusReason );
		//_window->requestActivate();
	}

	void Renderer::_addHUDWidget( QWidget * const p_widget, const HUD_POSITION p_pos )
	{
		p_widget->setParent( this );
		// Keep the HUD native so it stays above the embedded QWindow container.
		p_widget->setAttribute( Qt::WA_NativeWindow, true );
		p_widget->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
		_hudItems.push_back( { p_widget, p_pos } );
		_syncHUDWidgets( isVisible() );
	}

	void Renderer::_syncHUDWidgets( const bool p_showWidgets )
	{
		if ( not isVisible() )
		{
			return;
		}

		for ( const HUDItem & item : _hudItems )
		{
			QWidget * const widget = item.widget;
			if ( widget == nullptr )
			{
				continue;
			}

			widget->setGeometry( _getHUDGeometry( widget, item.position ) );
			if ( p_showWidgets )
			{
				widget->show();
			}
			widget->raise();
		}
	}

	QRect Renderer::_getHUDGeometry( QWidget * const p_widget, const HUD_POSITION p_pos ) const
	{
		assert( p_widget != nullptr );

		p_widget->adjustSize();

		const QRect availableRect = rect();
		const QSize size		  = p_widget->sizeHint().expandedTo( p_widget->minimumSizeHint() );
		int			x			  = availableRect.left();
		int			y			  = availableRect.top();

		switch ( p_pos )
		{
		case HUD_POSITION::TOP_LEFT: break;
		case HUD_POSITION::TOP_CENTER: x = availableRect.left() + ( availableRect.width() - size.width() ) / 2; break;
		case HUD_POSITION::TOP_RIGHT: x = availableRect.right() - size.width() + 1; break;
		case HUD_POSITION::CENTER_LEFT: y = availableRect.top() + ( availableRect.height() - size.height() ) / 2; break;
		case HUD_POSITION::CENTER_RIGHT:
			x = availableRect.right() - size.width() + 1;
			y = availableRect.top() + ( availableRect.height() - size.height() ) / 2;
			break;
		case HUD_POSITION::BOTTOM_LEFT: y = availableRect.bottom() - size.height() + 1; break;
		case HUD_POSITION::BOTTOM_CENTER:
			x = availableRect.left() + ( availableRect.width() - size.width() ) / 2;
			y = availableRect.bottom() - size.height() + 1;
			break;
		case HUD_POSITION::BOTTOM_RIGHT:
			x = availableRect.right() - size.width() + 1;
			y = availableRect.bottom() - size.height() + 1;
			break;
		default: assert( false && "Invalid HUD position" );
		}

		return { QPoint( x, y ), size };
	}

} // namespace VTX::UI::QT::Widget
