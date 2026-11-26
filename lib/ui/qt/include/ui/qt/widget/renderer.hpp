#pragma once

#include "ui/qt/events.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/opengl_renderer.hpp"
#include <QEvent>
#include <QFocusEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QPointer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QWheelEvent>
#include <app/events.hpp>
#include <app/input/input_manager.hpp>
#include <cassert>
#include <cstdint>
#include <optional>
#include <util/event_hub.hpp>
#include <vector>

namespace VTX::UI::QT::Widget
{
	// TODO: move to settings.
	enum struct KB_LAYOUT : uint8_t
	{
		QWERTY,
		AZERTY,
		COUNT
	};

	/**
	 * @brief Widget to present an OpenGL rendering context.
	 * No more Qt OpenGL implementation used.
	 */
	class Renderer : public BaseWidget<Renderer, QWidget>
	{
		Q_OBJECT

	  public:
		static constexpr bool VSYNC_DEFAULT = true;

		/**
		 * @brief Constructors.
		 */
		Renderer( QWidget * );
		~Renderer() override;

		/**
		 * @brief Get OpenGLRenderer widget.
		 */
		OpenGLRenderer * getOpenGLRenderer() const { return _openGLRenderer; }

	  protected:
		/**
		 * @brief Events.
		 */
		void paintEvent( QPaintEvent * ) override;
		void showEvent( QShowEvent * ) override;
		void resizeEvent( QResizeEvent * ) override;
		void keyPressEvent( QKeyEvent * ) override;
		void keyReleaseEvent( QKeyEvent * ) override;
		void focusInEvent( QFocusEvent * ) override;
		void focusOutEvent( QFocusEvent * ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void mouseMoveEvent( QMouseEvent * ) override;
		void mouseReleaseEvent( QMouseEvent * ) override;
		void mouseDoubleClickEvent( QMouseEvent * ) override;
		void wheelEvent( QWheelEvent * ) override;

		/**
		 * @brief Debounce callback.
		 */
		void onResizeFinished();

	  private:
		/**
		 * @brief Debounce timer for resize events.
		 */
		QTimer _resizeTimer;

		/**
		 * @brief OpenGL rendering window.
		 */
		QPointer<OpenGLRenderer> _openGLRenderer;

		/**
		 * @brief Input manager.
		 */
		App::Input::InputManager & _inputManager;

		/**
		 * @brief Logo painted behind the native renderer surface during startup and resize.
		 */
		QPixmap _backgroundLogo;

		/**
		 * @brief True while applying a resize requested by App, to avoid sending it back to App.
		 */
		bool _ignoreResizeEvents = false;

		/**
		 * @brief True once the native renderer is initialized and can be shown.
		 */
		bool _rendererReady = false;

		/**
		 * @brief Size to ignore if Qt relayouts back after an App-requested resize.
		 */
		std::optional<QSize> _pendingLayoutReboundSize;

		/**
		 * @brief Current keyboard layout.
		 * TODO: move to settings.
		 */
		KB_LAYOUT _layout = KB_LAYOUT::QWERTY;

		/**
		 * @brief Handle input events.
		 */
		Qt::MouseButton _pressedButton = Qt::NoButton;
		QPointF			_pressPos	   = {};
		QPointF			_lastPos	   = {};
		bool			_dragging	   = false;

		/**
		 * @brief Give focus back to the rendering surface.
		 */
		void _focusRenderer();

		/**
		 * @brief Synchronize widget size after an App renderer resize.
		 */
		void _onRendererResize( const App::Events::RendererResize & );

		/**
		 * @brief On ready event from the native renderer.
		 */
		void _onRendererReady( const App::Events::RendererReady & );

		/**
		 * @brief Handle keyboard events and forward them to the InputManager.
		 */
		void											_handleKeyboard( QKeyEvent *, bool p_enable );
		void											_handleModifiers();
		std::optional<App::Input::InputManager::Action> _getKeyboardAction( int ) const;
		QPoint											_toDevicePixels( const QPointF & ) const;
		void											_onClick( Qt::MouseButton, QPoint );
		void											_onKBLayoutChange( const Events::KeyboardLayoutChanged & );

		/**
		 * @brief Connection.
		 */
		VTX::Util::EventHub::ScopedConnection _rendererResizeConnection;
		VTX::Util::EventHub::ScopedConnection _rendererReadyConnection;
		VTX::Util::EventHub::ScopedConnection _keyboardLayoutChangedConnection;
	};
} // namespace VTX::UI::QT::Widget
