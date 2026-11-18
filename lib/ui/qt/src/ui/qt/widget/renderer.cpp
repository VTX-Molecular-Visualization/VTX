#include "ui/qt/widget/renderer.hpp"
#include "app/services.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/menu/selection.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QCursor>
#include <QGuiApplication>
#include <QPainter>
#include <QPalette>
#include <QScopedValueRollback>
#include <QStyleHints>
#include <algorithm>
#include <app/action/application.hpp>
#include <app/action/camera.hpp>
#include <array>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>
#include <utility>

namespace
{
	using Action	   = VTX::App::Input::InputManager::Action;
	using BindingTable = std::array<std::optional<Action>, 256>;

	constexpr BindingTable QWERTY_BINDINGS = []()
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

	constexpr BindingTable AZERTY_BINDINGS = []()
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
	Renderer::Renderer( QWidget * const p_parent ) : BaseWidget( p_parent ), _inputManager( App::INPUT() )
	{
		setAcceptDrops( true );
		_backgroundLogo = QPixmap( Resources::SPRITE_LOGO.toString() );
		setFocusPolicy( Qt::NoFocus );

		_resizeTimer.setSingleShot( true );
		connect( &_resizeTimer, &QTimer::timeout, this, &Renderer::onResizeFinished );

		_rendererResizeConnection
			= App::HUB().connect<App::Events::RendererResize, &Renderer::_onRendererResize>( this );
		_rendererReadyConnection = App::HUB().connect<App::Events::RendererReady, &Renderer::_onRendererReady>( this );
		_keyboardLayoutChangedConnection
			= App::HUB().connect<Events::KeyboardLayoutChanged, &Renderer::_onKBLayoutChange>( this );

		if ( App::ARGS().noGraphics || not OpenGLRenderer::isOpenGLSupported() )
		{
			return;
		}

		_openGLRenderer = Helper::makeQObject<OpenGLRenderer>( this );
		_openGLRenderer->setFixedSize( 1, 1 );
		_openGLRenderer->move( -1, -1 );
		setFocusPolicy( Qt::StrongFocus );
	}

	Renderer::~Renderer() = default;

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

		_focusRenderer();
	}

	void Renderer::resizeEvent( QResizeEvent * p_event )
	{
		QWidget::resizeEvent( p_event );

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

		_resizeTimer.start( 40 );
	}

	void Renderer::onResizeFinished()
	{
		if ( QGuiApplication::mouseButtons() != Qt::NoButton )
		{
			_resizeTimer.start( 40 );
			return;
		}
		if ( _ignoreResizeEvents )
		{
			return;
		}
		if ( not _rendererReady )
		{
			return;
		}

		assert( _openGLRenderer != nullptr );

		const QSize size = this->size();
		_openGLRenderer->setMinimumSize( 0, 0 );
		_openGLRenderer->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
		_openGLRenderer->setGeometry( QRect( QPoint( 0, 0 ), size ) );

		const QSize scaledSize = size * _openGLRenderer->devicePixelRatio();
		App::ACTION().execute<App::Action::Application::Resize>( scaledSize.width(), scaledSize.height(), false );
		_focusRenderer();
	}

	void Renderer::keyPressEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, true ); }

	void Renderer::keyReleaseEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, false ); }

	void Renderer::focusInEvent( QFocusEvent * const p_event )
	{
		QWidget::focusInEvent( p_event );
		_handleModifiers();
	}

	void Renderer::focusOutEvent( QFocusEvent * const p_event )
	{
		QWidget::focusOutEvent( p_event );

		_inputManager.clearActions();
		_pressedButton = Qt::NoButton;
		_dragging	   = false;
	}

	void Renderer::mousePressEvent( QMouseEvent * const p_event )
	{
		if ( not _rendererReady )
		{
			return;
		}

		_pressedButton = p_event->button();
		_pressPos	   = p_event->position();
		_lastPos	   = _pressPos;
		_dragging	   = false;
	}

	void Renderer::mouseMoveEvent( QMouseEvent * const p_event )
	{
		if ( _pressedButton == Qt::NoButton )
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

			switch ( _pressedButton )
			{
			case Qt::LeftButton: _inputManager.rotateBy( deltaVec ); break;
			case Qt::MiddleButton: _inputManager.panBy( deltaVec ); break;
			case Qt::RightButton: _inputManager.rotateAltBy( deltaVec ); break;
			default: break;
			}

			_lastPos = p_event->position();
		}
	}

	void Renderer::mouseReleaseEvent( QMouseEvent * const p_event )
	{
		if ( p_event->button() != _pressedButton )
		{
			return;
		}

		if ( not _dragging )
		{
			_onClick( _pressedButton, _toDevicePixels( p_event->position() ) );
		}

		_pressedButton = Qt::NoButton;
		_dragging	   = false;
	}

	void Renderer::mouseDoubleClickEvent( QMouseEvent * const )
	{
		_pressedButton = Qt::NoButton;
		_dragging	   = false;

		if ( not _rendererReady )
		{
			return;
		}

		App::ACTION().execute<App::Action::Camera::Orient>();
	}

	void Renderer::wheelEvent( QWheelEvent * const p_event )
	{
		if ( not _rendererReady )
		{
			return;
		}

		_inputManager.zoomBy( p_event->angleDelta().y() );
	}

	void Renderer::_focusRenderer()
	{
		if ( _openGLRenderer == nullptr || not isVisible() )
		{
			return;
		}

		setFocus( Qt::OtherFocusReason );
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
		_inputManager.setAction( App::Input::InputManager::Action::Decelerate, mods & Qt::AltModifier );
	}

	std::optional<App::Input::InputManager::Action> Renderer::_getKeyboardAction( const int p_key ) const
	{
		if ( p_key < 0 || std::cmp_greater_equal( p_key, QWERTY_BINDINGS.size() ) )
		{
			return std::nullopt;
		}

		const BindingTable & bindings = _layout == KB_LAYOUT::AZERTY ? AZERTY_BINDINGS : QWERTY_BINDINGS;
		return bindings[ p_key ];
	}

	QPoint Renderer::_toDevicePixels( const QPointF & p_logicalPos ) const
	{
		const qreal dpr = devicePixelRatio();
		return { static_cast<int>( p_logicalPos.x() * dpr ), static_cast<int>( p_logicalPos.y() * dpr ) };
	}

	void Renderer::_onClick( const Qt::MouseButton p_button, const QPoint p_pos )
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

	void Renderer::_onKBLayoutChange( const Events::KeyboardLayoutChanged & p_e )
	{ _layout = static_cast<KB_LAYOUT>( p_e.layout ); }

	void Renderer::_onRendererReady( const App::Events::RendererReady & )
	{
		_rendererReady = true;
		App::ACTION().execute<App::Action::Application::SetVSync>(
			SETTINGS().value( SETTING_KEY_VSYNC, VSYNC_DEFAULT ).toBool()
		);
		QTimer::singleShot( 0, this, &Renderer::onResizeFinished );
	}

	void Renderer::_onRendererResize( const App::Events::RendererResize & p_e )
	{
		if ( _openGLRenderer == nullptr )
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
		qreal dpr = _openGLRenderer->devicePixelRatio();
		if ( dpr <= 0 )
		{
			dpr = 1;
		}

		const QSize size(
			qRound( static_cast<qreal>( p_e.width ) / dpr ), qRound( static_cast<qreal>( p_e.height ) / dpr )
		);

		const QSize currentSize		  = this->size();
		const bool	widgetSizeChanged = currentSize != size;
		const bool	windowSizeChanged = _openGLRenderer->size() != size;
		if ( not widgetSizeChanged && not windowSizeChanged )
		{
			return;
		}

		if ( widgetSizeChanged )
		{
			_pendingLayoutReboundSize = this->size();
		}

		_resizeTimer.stop();
		const QScopedValueRollback ignoreResizeEvents( _ignoreResizeEvents, true );

		if ( widgetSizeChanged )
		{
			const QSize delta = size - currentSize;
			MAIN_WINDOW().resize( MAIN_WINDOW().size() + delta );
		}

		const QSize appliedSize = this->size();
		if ( _openGLRenderer->size() != appliedSize )
		{
			_openGLRenderer->resize( appliedSize );
		}
		QTimer::singleShot(
			0,
			this,
			[ this ]()
			{
				if ( _openGLRenderer == nullptr )
				{
					return;
				}

				_resizeTimer.stop();
				const QScopedValueRollback ignoreResizeEvents( _ignoreResizeEvents, true );

				const QSize size = this->size();
				if ( _openGLRenderer->size() != size )
				{
					_openGLRenderer->resize( size );
				}
			}
		);
	}

} // namespace VTX::UI::QT::Widget
