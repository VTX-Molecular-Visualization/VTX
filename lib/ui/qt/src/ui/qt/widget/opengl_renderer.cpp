#include "ui/qt/widget/opengl_renderer.hpp"
#include <QGuiApplication>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QStyleHints>
#include <QSurfaceFormat>
#include <app/action/camera.hpp>
#include <app/services.hpp>
#include <array>
#include <renderer/renderer.hpp>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
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
	OpenGLRenderer::OpenGLRenderer( QWidget * const p_parent ) :
		QOpenGLWidget( p_parent ), _inputManager( App::INPUT() )
	{
		_keyboardLayoutChangedConnection
			= App::HUB().connect<Events::KeyboardLayoutChanged, &OpenGLRenderer::_onKBLayoutChange>( this );

		setFormat( surfaceFormat() );
		setUpdateBehavior( QOpenGLWidget::PartialUpdate );

		connect(
			this,
			&QOpenGLWidget::frameSwapped,
			this,
			[ this ]()
			{
				if ( _glReady )
				{
					makeCurrent();
				}
			}
		);
	}

	OpenGLRenderer::~OpenGLRenderer() { destroyGLContext(); }

	QSurfaceFormat OpenGLRenderer::surfaceFormat()
	{
		QSurfaceFormat format;
		format.setRenderableType( QSurfaceFormat::OpenGL );
		format.setVersion(
			static_cast<int>( VTX::Renderer::Desc::OPENGL_MAJOR_VERSION ),
			static_cast<int>( VTX::Renderer::Desc::OPENGL_MINOR_VERSION )
		);
		format.setProfile( QSurfaceFormat::CoreProfile );
		format.setRedBufferSize( 8 );
		format.setGreenBufferSize( 8 );
		format.setBlueBufferSize( 8 );
		format.setAlphaBufferSize( 0 );
		format.setDepthBufferSize( 0 );
		format.setStencilBufferSize( 0 );
		format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
		format.setSwapInterval( 1 );
#ifdef _DEBUG
		format.setOption( QSurfaceFormat::DebugContext );
#endif

		return format;
	}

	bool OpenGLRenderer::isOpenGLSupported()
	{
		QOpenGLContext context;
		context.setFormat( surfaceFormat() );
		if ( not context.create() )
		{
			VTX_WARNING( "[Qt] Failed to create OpenGL context" );
			return false;
		}

		QOffscreenSurface surface;
		surface.setFormat( context.format() );
		surface.create();
		if ( not surface.isValid() || not context.makeCurrent( &surface ) )
		{
			VTX_WARNING( "[Qt] Failed to make OpenGL context current" );
			return false;
		}

		const QSurfaceFormat obtained = context.format();
		context.doneCurrent();

		const std::pair required { static_cast<int>( VTX::Renderer::Desc::OPENGL_MAJOR_VERSION ),
								   static_cast<int>( VTX::Renderer::Desc::OPENGL_MINOR_VERSION ) };
		const std::pair available { obtained.majorVersion(), obtained.minorVersion() };
		if ( available < required )
		{
			VTX_WARNING(
				"[Qt] OpenGL {}.{} required, {}.{} available",
				required.first,
				required.second,
				available.first,
				available.second
			);
			return false;
		}

		return true;
	}

	bool OpenGLRenderer::createGLContext()
	{
		if ( not isValid() )
		{
			VTX_WARNING( "[Qt] OpenGL context is not initialized" );
			return false;
		}

		makeCurrent();

		const QSurfaceFormat obtained = context()->format();
		VTX_INFO(
			"[Qt] OpenGL context {}.{} created (core={})",
			obtained.majorVersion(),
			obtained.minorVersion(),
			obtained.profile() == QSurfaceFormat::CoreProfile
		);

		_defaultFramebuffer = getGLDefaultFramebuffer();
		_glReady			= true;
		return true;
	}

	void OpenGLRenderer::destroyGLContext()
	{
		if ( not _glReady )
		{
			return;
		}

		_glReady = false;
		doneCurrent();
	}

	void OpenGLRenderer::swapGLBuffers()
	{
		if ( _glReady )
		{
			update();
		}
	}

	bool OpenGLRenderer::setGLSwapInterval( const int p_interval ) const
	{ return format().swapInterval() == p_interval; }

	void * OpenGLRenderer::getGLProcAddress( const char * const p_name ) const
	{
		if ( context() == nullptr )
		{
			return nullptr;
		}

		return reinterpret_cast<void *>( context()->getProcAddress( p_name ) );
	}

	uint32_t OpenGLRenderer::getGLDefaultFramebuffer() const
	{ return static_cast<uint32_t>( defaultFramebufferObject() ); }

	bool OpenGLRenderer::event( QEvent * const p_event )
	{
		const bool result = QOpenGLWidget::event( p_event );
		if ( p_event->type() == QEvent::DevicePixelRatioChange )
		{
			_syncDefaultFramebuffer();
		}
		return result;
	}

	void OpenGLRenderer::resizeGL( const int, const int )
	{
		_syncDefaultFramebuffer();
		App::RENDERER().setNeedUpdate();
	}

	void OpenGLRenderer::paintGL() { context()->functions()->glDisable( GL_BLEND ); }

	void OpenGLRenderer::keyPressEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, true ); }

	void OpenGLRenderer::keyReleaseEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, false ); }

	void OpenGLRenderer::focusInEvent( QFocusEvent * p_event )
	{
		QOpenGLWidget::focusInEvent( p_event );
		_handleModifiers();
	}

	void OpenGLRenderer::focusOutEvent( QFocusEvent * p_event )
	{
		QOpenGLWidget::focusOutEvent( p_event );

		_inputManager.clearActions();
		_pressedButton = Qt::NoButton;
		_dragging	   = false;
	}

	void OpenGLRenderer::mousePressEvent( QMouseEvent * p_event )
	{
		_pressedButton = p_event->button();
		_pressPos	   = p_event->position();
		_lastPos	   = _pressPos;
		_dragging	   = false;
	}

	void OpenGLRenderer::mouseMoveEvent( QMouseEvent * p_event )
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

	void OpenGLRenderer::mouseReleaseEvent( QMouseEvent * p_event )
	{
		if ( p_event->button() != _pressedButton )
		{
			return;
		}

		if ( not _dragging )
		{
			emit clicked( _pressedButton, _toDevicePixels( p_event->position() ) );
		}

		_pressedButton = Qt::NoButton;
		_dragging	   = false;
	}

	void OpenGLRenderer::mouseDoubleClickEvent( QMouseEvent * const )
	{
		_pressedButton = Qt::NoButton;
		_dragging	   = false;
		App::ACTION().execute<App::Action::Camera::Orient>();
	}

	void OpenGLRenderer::wheelEvent( QWheelEvent * const p_event )
	{ _inputManager.zoomBy( p_event->angleDelta().y() ); }

	void OpenGLRenderer::_handleKeyboard( QKeyEvent * const p_event, const bool p_enable )
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

	void OpenGLRenderer::_handleModifiers()
	{
		const Qt::KeyboardModifiers mods = QGuiApplication::queryKeyboardModifiers();
		_inputManager.setAction( App::Input::InputManager::Action::Accelerate, mods & Qt::ShiftModifier );
		_inputManager.setAction( App::Input::InputManager::Action::Decelerate, mods & Qt::AltModifier );
	}

	std::optional<App::Input::InputManager::Action> OpenGLRenderer::_getKeyboardAction( const int p_key ) const
	{
		if ( p_key < 0 || std::cmp_greater_equal( p_key, QWERTY_BINDINGS.size() ) )
		{
			return std::nullopt;
		}

		const BindingTable & bindings = _layout == KB_LAYOUT::AZERTY ? AZERTY_BINDINGS : QWERTY_BINDINGS;
		return bindings[ p_key ];
	}

	QPoint OpenGLRenderer::_toDevicePixels( const QPointF & p_logicalPos ) const
	{
		const qreal dpr = devicePixelRatio();
		return { static_cast<int>( p_logicalPos.x() * dpr ), static_cast<int>( p_logicalPos.y() * dpr ) };
	}

	void OpenGLRenderer::_syncDefaultFramebuffer()
	{
		const uint32_t framebuffer = getGLDefaultFramebuffer();
		if ( not _glReady || framebuffer == _defaultFramebuffer )
		{
			return;
		}

		_defaultFramebuffer = framebuffer;
		App::RENDERER().setDefaultFramebuffer( framebuffer );
	}

	void OpenGLRenderer::_onKBLayoutChange( const Events::KeyboardLayoutChanged & p_e )
	{ _layout = static_cast<KB_LAYOUT>( p_e.layout ); }

} // namespace VTX::UI::QT::Widget
