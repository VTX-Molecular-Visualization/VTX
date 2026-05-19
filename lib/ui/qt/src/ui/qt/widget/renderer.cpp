#include "ui/qt/widget/renderer.hpp"
#include "app/services.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/menu/selection.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QCoreApplication>
#include <QCursor>
#include <QGuiApplication>
#include <QPainter>
#include <QPalette>
#include <QScopedValueRollback>
#include <algorithm>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <qpa/qplatformnativeinterface.h>
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{
	Renderer::Renderer( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setAcceptDrops( true );
		_backgroundLogo = QPixmap( Resources::SPRITE_LOGO.data() );

		_window = new Window::Renderer();
		_window->setFlags( Qt::FramelessWindowHint );
		_window->create();

		_container = createWindowContainer( _window, this );
		_container->installEventFilter( this );
		_container->setFocusPolicy( Qt::StrongFocus );
		setFocusPolicy( Qt::NoFocus );
		setFocusProxy( _container );
		_container->setFixedSize( 0, 0 );

		connect(
			_window,
			&Window::Renderer::clicked,
			this,
			[ this ]( const Qt::MouseButton p_button, const QPoint p_pos )
			{
				const bool preserveSelection
					= p_button == Qt::RightButton && ( QGuiApplication::keyboardModifiers() & Qt::ControlModifier );
				if ( not preserveSelection && not UI_ACTIONS().isChecked( Action::Selection::LOCK ) )
				{
					SELECTION().pick(
						Vec2i( p_pos.x(), p_pos.y() ), QGuiApplication::keyboardModifiers() & Qt::ControlModifier
					);
				}

				if ( p_button == Qt::RightButton )
				{
					Menu::Selection menu( this );
					menu.exec( QCursor::pos() );
				}
			}
		);

		_resizeTimer.setSingleShot( true );
		connect( &_resizeTimer, &QTimer::timeout, this, &Renderer::onResizeFinished );

		App::HUB().connect<App::Events::RendererResize, &Renderer::_onRendererResize>( this );
		App::RENDERER().onReady += [ this ]()
		{
			_rendererReady = true;
			App::ACTION().execute<App::Action::Application::SetVSync>(
				SETTINGS().value( SETTING_KEY_VSYNC, VSYNC_DEFAULT ).toBool()
			);
			QTimer::singleShot( 0, this, &Renderer::onResizeFinished );
		};
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

	void Renderer::paintEvent( QPaintEvent * p_event )
	{
		QWidget::paintEvent( p_event );

		QPainter painter( this );
		painter.fillRect( rect(), palette().color( QPalette::Base ) );

		if ( _backgroundLogo.isNull() )
		{
			return;
		}

		const int	maxSide	 = std::max( 1, std::min( std::min( width(), height() ) / 3, 180 ) );
		const QSize logoSize = _backgroundLogo.size().scaled( maxSide, maxSide, Qt::KeepAspectRatio );
		const QRect logoRect(
			( width() - logoSize.width() ) / 2,
			( height() - logoSize.height() ) / 2,
			logoSize.width(),
			logoSize.height()
		);

		painter.setOpacity( 0.5 );
		painter.drawPixmap( logoRect, _backgroundLogo );
	}

	void Renderer::showEvent( QShowEvent * p_event )
	{
		QWidget::showEvent( p_event );
		//_syncHUDWidgets( true );
		_focusRenderer();
	}

	void Renderer::resizeEvent( QResizeEvent * p_event )
	{
		QWidget::resizeEvent( p_event );

		//_syncHUDWidgets();

		if ( not _rendererReady )
		{
			_resizeTimer.stop();
			return;
		}

		if ( _ignoreResizeEvents )
		{
			_resizeTimer.stop();
			return;
		}
		if ( _pendingLayoutReboundSize && p_event->size() == *_pendingLayoutReboundSize )
		{
			_resizeTimer.stop();
			_pendingLayoutReboundSize.reset();
			return;
		}

		if ( _window != nullptr && _container != nullptr )
		{
#ifdef _WIN32
			_container->setVisible( false );
			update();
#endif
		}
		_resizeTimer.start( 40 );
	}

	void Renderer::onResizeFinished()
	{
		if ( _ignoreResizeEvents )
		{
			return;
		}
		if ( not _rendererReady )
		{
			return;
		}

		assert( _window != nullptr );
		assert( _container != nullptr );

		const QSize size = this->size();
		_container->setMinimumSize( 0, 0 );
		_container->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
		_window->resize( size );
		_container->resize( size );
#ifdef _WIN32
		_container->setVisible( true );
#endif
		//_syncHUDWidgets();

		const QSize scaledSize = size * _window->devicePixelRatio();
		App::ACTION().execute<App::Action::Application::Resize>( scaledSize.width(), scaledSize.height(), false );
		_focusRenderer();
	}

	void Renderer::_focusRenderer()
	{
		if ( _container == nullptr || _window == nullptr || not isVisible() )
		{
			return;
		}

		_container->setFocus( Qt::OtherFocusReason );
		_window->requestActivate();
	}

	void Renderer::_onRendererResize( const App::Events::RendererResize & p_e )
	{
		if ( _window == nullptr || _container == nullptr )
		{
			return;
		}
		if ( not isVisible() )
		{
			return;
		}
		if ( not _rendererReady )
		{
			return;
		}
		if ( not p_e.resizeMainWindow )
		{
			return;
		}
		qreal dpr = _window->devicePixelRatio();
		if ( dpr <= 0 )
		{
			dpr = 1;
		}

		const QSize size(
			qRound( static_cast<qreal>( p_e.width ) / dpr ), qRound( static_cast<qreal>( p_e.height ) / dpr )
		);

		const QSize currentSize			 = this->size();
		const bool	widgetSizeChanged	 = currentSize != size;
		const bool	windowSizeChanged	 = _window->size() != size;
		const bool	containerSizeChanged = _container->size() != size;
		if ( not widgetSizeChanged && not windowSizeChanged && not containerSizeChanged )
		{
			return;
		}

		if ( widgetSizeChanged )
		{
			_pendingLayoutReboundSize = this->size();
		}

		_resizeTimer.stop();
		QScopedValueRollback ignoreResizeEvents( _ignoreResizeEvents, true );

		if ( widgetSizeChanged )
		{
			const QSize delta = size - currentSize;
			MAIN_WINDOW().resize( MAIN_WINDOW().size() + delta );
		}

		const QSize appliedSize = this->size();
		if ( _window->size() != appliedSize )
		{
			_window->resize( appliedSize );
		}
		if ( _container->size() != appliedSize )
		{
			_container->resize( appliedSize );
		}
#ifdef _WIN32
		_container->setVisible( true );
#endif

		QTimer::singleShot(
			0,
			this,
			[ this ]()
			{
				if ( _window == nullptr || _container == nullptr )
				{
					return;
				}

				_resizeTimer.stop();
				QScopedValueRollback ignoreResizeEvents( _ignoreResizeEvents, true );

				const QSize size = this->size();
				if ( _window->size() != size )
				{
					_window->resize( size );
				}
				if ( _container->size() != size )
				{
					_container->resize( size );
				}
#ifdef _WIN32
				_container->setVisible( true );
#endif
			}
		);
	}

	/*
	void Renderer::_addHUDWidget( QWidget * const p_widget, const HUD_POSITION p_pos )
	{
		p_widget->setParent( this );
		// Keep the HUD native so it stays above the embedded QWindow container.
		p_widget->setAttribute( Qt::WA_NativeWindow, true );
		p_widget->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
		_hudItems.push_back( { p_widget, p_pos } );
		_syncHUDWidgets( isVisible() );
	}
	*/

	/*
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
	*/

	/*
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
	*/

} // namespace VTX::UI::QT::Widget
