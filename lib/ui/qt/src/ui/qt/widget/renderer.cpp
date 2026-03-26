#include "ui/qt/widget/renderer.hpp"
#include "app/services.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include <QGuiApplication>
#include <QStyleHints>
#include <array>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/action/camera.hpp>
#include <qpa/qplatformnativeinterface.h>
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>

namespace
{
	using Action	   = VTX::App::Input::InputManager::Action;
	using BindingTable = std::array<std::optional<Action>, 256>;

	const BindingTable _QWERTY_BINDINGS = []()
	{
		BindingTable bindings;
		bindings[ Qt::Key_W ] = Action::MoveFront;
		bindings[ Qt::Key_S ] = Action::MoveBack;
		bindings[ Qt::Key_A ] = Action::MoveLeft;
		bindings[ Qt::Key_D ] = Action::MoveRight;
		bindings[ Qt::Key_R ] = Action::MoveUp;
		bindings[ Qt::Key_F ] = Action::MoveDown;
		bindings[ Qt::Key_Q ] = Action::RotateLeft;
		bindings[ Qt::Key_E ] = Action::RotateRight;
		return bindings;
	}();

	const BindingTable _AZERTY_BINDINGS = []()
	{
		BindingTable bindings;
		bindings[ Qt::Key_Z ] = Action::MoveFront;
		bindings[ Qt::Key_S ] = Action::MoveBack;
		bindings[ Qt::Key_Q ] = Action::MoveLeft;
		bindings[ Qt::Key_D ] = Action::MoveRight;
		bindings[ Qt::Key_R ] = Action::MoveUp;
		bindings[ Qt::Key_F ] = Action::MoveDown;
		bindings[ Qt::Key_A ] = Action::RotateLeft;
		bindings[ Qt::Key_E ] = Action::RotateRight;
		return bindings;
	}();
} // namespace

namespace VTX::UI::QT::Widget
{
	Renderer::Renderer( QWidget * p_parent ) : BaseWidget( p_parent ), _inputManager( App::INPUT() )
	{
		setFocusPolicy( Qt::StrongFocus );
		setAttribute( Qt::WA_NativeWindow, true );
		setAttribute( Qt::WA_NoSystemBackground, true );
		setAutoFillBackground( false );
		winId();

		App::HUB().connect<Events::KeyboardLayoutChanged, &Renderer::_onKBLayoutChange>( this );

		_resizeTimer.setSingleShot( true );
		connect( &_resizeTimer, &QTimer::timeout, this, &Renderer::onResizeFinished );
	}

	Renderer::~Renderer() = default;

	uintptr_t Renderer::getNativeSurface() const
	{
		const_cast<Renderer *>( this )->winId();

		if ( QGuiApplication::platformName() == "wayland" )
		{
			QPlatformNativeInterface * const nif = QGuiApplication::platformNativeInterface();
			if ( not nif )
			{
				throw std::runtime_error( "Qt: no platform native interface" );
			}

			QWindow * const handle = windowHandle();
			if ( handle == nullptr )
			{
				throw std::runtime_error( "Qt: widget has no window handle" );
			}

			void * const surface = nif->nativeResourceForWindow( "surface", handle );
			if ( not surface )
			{
				throw std::runtime_error( "Qt: native surface is null" );
			}
			VTX_TRACE(
				"Renderer::getNativeSurface platform={} widget_winid={} window_handle={} surface={}",
				QGuiApplication::platformName().toStdString(),
				static_cast<uintptr_t>( const_cast<Renderer *>( this )->winId() ),
				reinterpret_cast<uintptr_t>( handle ),
				reinterpret_cast<uintptr_t>( surface )
			);
			return reinterpret_cast<uintptr_t>( surface );
		}

		const uintptr_t surface = static_cast<uintptr_t>( const_cast<Renderer *>( this )->winId() );
		VTX_TRACE(
			"Renderer::getNativeSurface platform={} widget_winid={} surface={}",
			QGuiApplication::platformName().toStdString(),
			surface,
			surface
		);
		return surface;
	}

	uintptr_t Renderer::getNativeDisplay() const
	{
		QPlatformNativeInterface * const nif = QGuiApplication::platformNativeInterface();
		if ( nif == nullptr )
		{
			VTX_TRACE(
				"Renderer::getNativeDisplay platform={} native_interface=0 display=0",
				QGuiApplication::platformName().toStdString()
			);
			return 0;
		}

		if ( void * const display = nif->nativeResourceForIntegration( "display" ) )
		{
			VTX_TRACE(
				"Renderer::getNativeDisplay platform={} native_interface={} display={}",
				QGuiApplication::platformName().toStdString(),
				reinterpret_cast<uintptr_t>( nif ),
				reinterpret_cast<uintptr_t>( display )
			);
			return reinterpret_cast<uintptr_t>( display );
		}

		VTX_TRACE(
			"Renderer::getNativeDisplay platform={} native_interface={} display=0",
			QGuiApplication::platformName().toStdString(),
			reinterpret_cast<uintptr_t>( nif )
		);
		return 0;
	}

	uint8_t Renderer::getNativePlatform() const
	{
		if ( QGuiApplication::platformName() == "wayland" )
		{
			VTX_TRACE(
				"Renderer::getNativePlatform platform={} code=3", QGuiApplication::platformName().toStdString()
			);
			return 3;
		}
		if ( QGuiApplication::platformName() == "xcb" )
		{
			VTX_TRACE(
				"Renderer::getNativePlatform platform={} code=2", QGuiApplication::platformName().toStdString()
			);
			return 2;
		}
#ifdef _WIN32
		VTX_TRACE( "Renderer::getNativePlatform platform={} code=1", QGuiApplication::platformName().toStdString() );
		return 1;
#else
		VTX_TRACE( "Renderer::getNativePlatform platform={} code=0", QGuiApplication::platformName().toStdString() );
		return 0;
#endif
	}

	bool Renderer::event( QEvent * p_event )
	{
		if ( p_event != nullptr )
		{
			VTX_DEBUG( "Renderer::event type={}", Util::Enum::enumName( p_event->type() ) );

			switch ( p_event->type() )
			{
			case QEvent::Show:
			case QEvent::ShowToParent:
			case QEvent::UpdateRequest:
			case QEvent::WindowActivate:
			case QEvent::ActivationChange:
			case QEvent::WindowStateChange:
			case QEvent::Expose:
			{
				if ( p_event->type() == QEvent::WindowActivate )
				{
					_handleModifiers();
				}

				QTimer::singleShot( 0, this, []() { App::RENDERER().setNeedUpdate( true ); } );
				break;
			}
			case QEvent::WindowDeactivate:
			{
				_resetKeyboardState();
				QTimer::singleShot( 0, this, []() { App::RENDERER().setNeedUpdate( true ); } );
				break;
			}
			default: break;
			}
		}

		return QWidget::event( p_event );
	}

	void Renderer::focusInEvent( QFocusEvent * p_event )
	{
		QWidget::focusInEvent( p_event );
		_handleModifiers();
	}

	void Renderer::focusOutEvent( QFocusEvent * p_event )
	{
		QWidget::focusOutEvent( p_event );
		_resetKeyboardState();
	}

	void Renderer::resizeEvent( QResizeEvent * p_event )
	{
		QWidget::resizeEvent( p_event );
		_syncOverlayGeometry();
		_resizeTimer.start( 40 );
	}

	void Renderer::showEvent( QShowEvent * p_event )
	{
		QWidget::showEvent( p_event );
		onResizeFinished();
	}

	void Renderer::keyPressEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, true ); }

	void Renderer::keyReleaseEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, false ); }

	void Renderer::mousePressEvent( QMouseEvent * p_event )
	{
		_pressPos = p_event->position();
		_lastPos  = _pressPos;
		_dragging = false;
	}

	void Renderer::mouseMoveEvent( QMouseEvent * p_event )
	{
		if ( p_event->buttons() == Qt::NoButton )
		{
			return;
		}

		const int threshold = QGuiApplication::styleHints()->startDragDistance();
		if ( not _dragging && ( p_event->position() - _pressPos ).manhattanLength() >= threshold )
		{
			_dragging = true;
			_lastPos  = p_event->position();
			return;
		}

		if ( _dragging )
		{
			const QPoint delta = _toDevicePixels( p_event->position() - _lastPos );
			const Vec2i	 deltaVec( delta.x(), delta.y() );

			if ( p_event->buttons() & Qt::LeftButton )
			{
				_inputManager.rotateBy( deltaVec );
			}
			else if ( p_event->buttons() & Qt::MiddleButton )
			{
				_inputManager.panBy( deltaVec );
			}
			else if ( p_event->buttons() & Qt::RightButton )
			{
				_inputManager.rotateAltBy( deltaVec );
			}

			_lastPos = p_event->position();
		}
	}

	void Renderer::mouseReleaseEvent( QMouseEvent * p_event )
	{
		if ( not _dragging && not SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool() )
		{
			const QPoint pos = _toDevicePixels( p_event->position() );
			SELECTION().pick( Vec2i( pos.x(), pos.y() ), QGuiApplication::keyboardModifiers() & Qt::ControlModifier );
		}

		_dragging = false;
	}

	void Renderer::mouseDoubleClickEvent( QMouseEvent * const )
	{
		_dragging = false;
		App::ACTION().execute<App::Action::Camera::Orient>();
	}

	void Renderer::wheelEvent( QWheelEvent * const p_event ) { _inputManager.zoomBy( p_event->angleDelta().y() ); }

	void Renderer::onResizeFinished()
	{
		_syncOverlayGeometry();

		const QSizeF scaledSize = QSizeF( size() ) * devicePixelRatioF();
		App::ACTION().execute<App::Action::Application::Resize>(
			int( scaledSize.width() ), int( scaledSize.height() )
		);
	}

	void Renderer::_addHUDWidget( QWidget * const p_widget, const HUD_POSITION p_pos )
	{
		p_widget->setParent( this );

		// QPainterPath path;
		// path.addRoundedRect( p_widget->rect(), 2, 2 );
		//  p_widget->setMask( QRegion( path.toFillPolygon().toPolygon() ) );

		// QRegion region;

		// for ( QWidget * const child : findChildren<QWidget *>( QString(), Qt::FindDirectChildrenOnly ) )
		//{
		//	if ( child->isVisible() )
		//	{
		//		region |= child->geometry();
		//	}
		// }

		// p_widget->setMask( region );

		p_widget->setAttribute( Qt::WA_NativeWindow, true );
		p_widget->setAttribute( Qt::WA_TranslucentBackground );
		p_widget->setAttribute( Qt::WA_NoSystemBackground );
		p_widget->setAutoFillBackground( false );
		p_widget->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
		p_widget->move( 0, 0 );
		p_widget->show();
		_hudItems.push_back( { p_widget, p_pos } );
		_syncOverlayGeometry();
	}

	void Renderer::_syncOverlayGeometry()
	{
		if ( not isVisible() )
		{
			return;
		}

		const QRect availableRect = rect();
		for ( const HUDItem & item : _hudItems )
		{
			QWidget * const widget = item.widget;
			if ( widget == nullptr || not widget->isVisible() )
			{
				continue;
			}

			widget->adjustSize();

			const QSize size = widget->sizeHint().expandedTo( widget->minimumSizeHint() );
			int			x	 = availableRect.left();
			int			y	 = availableRect.top();

			switch ( item.position )
			{
			case HUD_POSITION::TOP_LEFT: break;
			case HUD_POSITION::TOP_CENTER:
				x = availableRect.left() + ( availableRect.width() - size.width() ) / 2;
				break;
			case HUD_POSITION::TOP_RIGHT: x = availableRect.right() - size.width() + 1; break;
			case HUD_POSITION::CENTER_LEFT:
				y = availableRect.top() + ( availableRect.height() - size.height() ) / 2;
				break;
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

			const QRect geometry( QPoint( x, y ), size );
			widget->setGeometry( geometry );
			widget->raise();
		}
	}

	void Renderer::_handleKeyboard( QKeyEvent * const p_event, const bool p_enable )
	{
		if ( p_event->isAutoRepeat() )
		{
			return;
		}

		if ( const auto action = _getKeyboardAction( p_event->key() ); action.has_value() )
		{
			_inputManager.setAction( *action, p_enable );
		}

		_handleModifiers();
	}

	void Renderer::_handleModifiers()
	{
		const Qt::KeyboardModifiers mods = QGuiApplication::queryKeyboardModifiers();

		_inputManager.setAction( App::Input::InputManager::Action::Accelerate, mods & Qt::ShiftModifier );
		_inputManager.setAction( App::Input::InputManager::Action::Decelerate, mods & Qt::ControlModifier );
	}

	std::optional<App::Input::InputManager::Action> Renderer::_getKeyboardAction( const int p_key ) const
	{
		if ( p_key < 0 || p_key >= int( _QWERTY_BINDINGS.size() ) )
		{
			return std::nullopt;
		}

		const BindingTable & bindings = _layout == KB_LAYOUT::AZERTY ? _AZERTY_BINDINGS : _QWERTY_BINDINGS;
		return bindings[ p_key ];
	}

	void Renderer::_resetKeyboardState()
	{
		_inputManager.clearActions();
		_dragging = false;
	}

	QPoint Renderer::_toDevicePixels( const QPointF & p_logicalPos ) const
	{
		const qreal dpr = devicePixelRatioF();
		return { int( p_logicalPos.x() * dpr ), int( p_logicalPos.y() * dpr ) };
	}

	void Renderer::_onKBLayoutChange( const Events::KeyboardLayoutChanged & p_e )
	{
		_layout = static_cast<KB_LAYOUT>( p_e.layout );
	}
} // namespace VTX::UI::QT::Widget
