#include "ui/qt/widget/renderer.hpp"
#include "app/services.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include <QGuiApplication>
#include <QRegion>
#include <QStyleHints>
#include <QWindow>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/action/camera.hpp>
#include <qpa/qplatformnativeinterface.h>
#include <util/event_hub.hpp>
#include <util/type_traits.hpp>

namespace
{
	using namespace VTX::UI::QT::Widget;

	enum ACTION
	{
		MOVE_FRONT,
		MOVE_BACK,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN,
		ROTATE_LEFT,
		ROTATE_RIGHT
	};

	template<ACTION A, KB_LAYOUT = KB_LAYOUT::QWERTY>
	constexpr Qt::Key _key()
	{
		if constexpr ( A == MOVE_FRONT )
		{
			return Qt::Key::Key_W;
		}
		else if constexpr ( A == MOVE_BACK )
		{
			return Qt::Key::Key_S;
		}
		else if constexpr ( A == MOVE_LEFT )
		{
			return Qt::Key::Key_A;
		}
		else if constexpr ( A == MOVE_RIGHT )
		{
			return Qt::Key::Key_D;
		}
		else if constexpr ( A == MOVE_UP )
		{
			return Qt::Key::Key_R;
		}
		else if constexpr ( A == MOVE_DOWN )
		{
			return Qt::Key::Key_F;
		}
		else if constexpr ( A == ROTATE_LEFT )
		{
			return Qt::Key::Key_Q;
		}
		else if constexpr ( A == ROTATE_RIGHT )
		{
			return Qt::Key::Key_E;
		}
		else
		{
			static_assert( VTX::always_false_v<A>, "Invalid action" );
			return Qt::Key::Key_unknown;
		}
	}

	template<>
	constexpr Qt::Key _key<MOVE_FRONT, KB_LAYOUT::AZERTY>()
	{
		return Qt::Key::Key_Z;
	}

	template<>
	constexpr Qt::Key _key<MOVE_LEFT, KB_LAYOUT::AZERTY>()
	{
		return Qt::Key::Key_Q;
	}

	template<>
	constexpr Qt::Key _key<ROTATE_LEFT, KB_LAYOUT::AZERTY>()
	{
		return Qt::Key::Key_A;
	}
} // namespace

namespace VTX::UI::QT::Widget
{
	Renderer::Renderer( QWidget * p_parent ) : BaseWidget( p_parent ), _inputManager( App::INPUT() )
	{
		constexpr int overlayBorderSize = 64;

		setFocusPolicy( Qt::StrongFocus );
		setAttribute( Qt::WA_NativeWindow, true );
		setAttribute( Qt::WA_PaintOnScreen, true );
		setAttribute( Qt::WA_OpaquePaintEvent, true );
		setAttribute( Qt::WA_NoSystemBackground, true );
		setAutoFillBackground( false );
		winId();

		_overlay = new QWidget( this, Qt::FramelessWindowHint );
		_overlay->setAttribute( Qt::WA_TranslucentBackground, true );
		_overlay->setAttribute( Qt::WA_NoSystemBackground, true );
		_overlay->setAttribute( Qt::WA_NativeWindow, true );
		_overlay->setAutoFillBackground( false );
		_overlay->setFocusPolicy( Qt::NoFocus );
		_overlay->winId();
		_overlayLayout = new QGridLayout( _overlay );
		_overlayLayout->setContentsMargins(
			overlayBorderSize, overlayBorderSize, overlayBorderSize, overlayBorderSize
		);
		_overlayLayout->setSpacing( 0 );
		_overlayLayout->setRowStretch( 0, 0 );
		_overlayLayout->setRowStretch( 1, 1 );
		_overlayLayout->setRowStretch( 2, 0 );
		_overlayLayout->setColumnStretch( 0, 0 );
		_overlayLayout->setColumnStretch( 1, 1 );
		_overlayLayout->setColumnStretch( 2, 0 );
		_overlay->show();

		App::HUB().connect<Events::KeyboardLayoutChanged, &Renderer::_onKBLayoutChange>( this );

		_resizeTimer.setSingleShot( true );
		connect( &_resizeTimer, &QTimer::timeout, this, &Renderer::onResizeFinished );
	}

	Renderer::~Renderer()
	{
		if ( _overlay )
		{
			delete _overlay;
		}
	}

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

	void Renderer::resizeEvent( QResizeEvent * p_event )
	{
		QWidget::resizeEvent( p_event );
		_syncOverlayGeometry();
		_resizeTimer.start( 40 );
	}

	void Renderer::showEvent( QShowEvent * p_event )
	{
		QWidget::showEvent( p_event );
		winId();
		if ( _overlay )
		{
			_overlay->show();
			_overlay->raise();
		}
		onResizeFinished();
	}

	void Renderer::hideEvent( QHideEvent * p_event )
	{
		QWidget::hideEvent( p_event );
		if ( _overlay )
		{
			_overlay->hide();
		}
	}

	void Renderer::keyPressEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, true ); }

	void Renderer::keyReleaseEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, false ); }

	void Renderer::mousePressEvent( QMouseEvent * p_event )
	{
		setFocus( Qt::MouseFocusReason );

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
		int			  row		= 0;
		int			  col		= 0;
		Qt::Alignment alignment = Qt::AlignCenter;
		switch ( p_pos )
		{
		case HUD_POSITION::TOP_LEFT:
			row		  = 0;
			col		  = 0;
			alignment = Qt::AlignTop | Qt::AlignLeft;
			break;
		case HUD_POSITION::TOP_CENTER:
			row		  = 0;
			col		  = 1;
			alignment = Qt::AlignTop | Qt::AlignHCenter;
			break;
		case HUD_POSITION::TOP_RIGHT:
			row		  = 0;
			col		  = 2;
			alignment = Qt::AlignTop | Qt::AlignRight;
			break;
		case HUD_POSITION::CENTER_LEFT:
			row		  = 1;
			col		  = 0;
			alignment = Qt::AlignVCenter | Qt::AlignLeft;
			break;
		case HUD_POSITION::CENTER_RIGHT:
			row		  = 1;
			col		  = 2;
			alignment = Qt::AlignVCenter | Qt::AlignRight;
			break;
		case HUD_POSITION::BOTTOM_LEFT:
			row		  = 2;
			col		  = 0;
			alignment = Qt::AlignBottom | Qt::AlignLeft;
			break;
		case HUD_POSITION::BOTTOM_CENTER:
			row		  = 2;
			col		  = 1;
			alignment = Qt::AlignBottom | Qt::AlignHCenter;
			break;
		case HUD_POSITION::BOTTOM_RIGHT:
			row		  = 2;
			col		  = 2;
			alignment = Qt::AlignBottom | Qt::AlignRight;
			break;
		default: assert( false && "Invalid HUD position" );
		}

		p_widget->setParent( _overlay );
		p_widget->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
		_overlayLayout->addWidget( p_widget, row, col, alignment );
		_syncOverlayGeometry();
		_overlay->show();
		_overlay->raise();
	}

	void Renderer::_syncOverlayGeometry()
	{
		if ( _overlay == nullptr || _overlayLayout == nullptr || not isVisible() )
		{
			return;
		}

		_overlay->setGeometry( rect() );
		_overlayLayout->activate();

		QRegion mask;
		for ( int i = 0; i < _overlayLayout->count(); ++i )
		{
			QLayoutItem * const item   = _overlayLayout->itemAt( i );
			QWidget * const		widget = item ? item->widget() : nullptr;
			if ( widget == nullptr || not widget->isVisible() )
			{
				continue;
			}

			mask += QRegion( widget->geometry() );
		}

		if ( mask.isEmpty() )
		{
			_overlay->clearMask();
		}
		else
		{
			_overlay->setMask( mask );
		}

		_overlay->raise();
	}

	void Renderer::_handleKeyboard( QKeyEvent * const p_event, const bool p_enable )
	{
		if ( p_event->isAutoRepeat() )
		{
			return;
		}

		if ( _layout == KB_LAYOUT::QWERTY )
		{
			switch ( p_event->key() )
			{
			case _key<MOVE_FRONT>(): _inputManager.setMoveFront( p_enable ); break;
			case _key<MOVE_BACK>(): _inputManager.setMoveBack( p_enable ); break;
			case _key<MOVE_LEFT>(): _inputManager.setMoveLeft( p_enable ); break;
			case _key<MOVE_RIGHT>(): _inputManager.setMoveRight( p_enable ); break;
			case _key<MOVE_UP>(): _inputManager.setMoveUp( p_enable ); break;
			case _key<MOVE_DOWN>(): _inputManager.setMoveDown( p_enable ); break;
			case _key<ROTATE_LEFT>(): _inputManager.setRotateLeft( p_enable ); break;
			case _key<ROTATE_RIGHT>(): _inputManager.setRotateRight( p_enable ); break;
			default: break;
			}
		}
		else if ( _layout == KB_LAYOUT::AZERTY )
		{
			switch ( p_event->key() )
			{
			case _key<MOVE_FRONT, KB_LAYOUT::AZERTY>(): _inputManager.setMoveFront( p_enable ); break;
			case _key<MOVE_BACK, KB_LAYOUT::AZERTY>(): _inputManager.setMoveBack( p_enable ); break;
			case _key<MOVE_LEFT, KB_LAYOUT::AZERTY>(): _inputManager.setMoveLeft( p_enable ); break;
			case _key<MOVE_RIGHT, KB_LAYOUT::AZERTY>(): _inputManager.setMoveRight( p_enable ); break;
			case _key<MOVE_UP, KB_LAYOUT::AZERTY>(): _inputManager.setMoveUp( p_enable ); break;
			case _key<MOVE_DOWN, KB_LAYOUT::AZERTY>(): _inputManager.setMoveDown( p_enable ); break;
			case _key<ROTATE_LEFT, KB_LAYOUT::AZERTY>(): _inputManager.setRotateLeft( p_enable ); break;
			case _key<ROTATE_RIGHT, KB_LAYOUT::AZERTY>(): _inputManager.setRotateRight( p_enable ); break;
			default: break;
			}
		}

		_handleModifiers();
	}

	void Renderer::_handleModifiers()
	{
		const Qt::KeyboardModifiers mods = QGuiApplication::queryKeyboardModifiers();

		_inputManager.setAccelerate( mods & Qt::ShiftModifier );
		_inputManager.setDecelerate( mods & Qt::ControlModifier );
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
