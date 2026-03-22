#ifndef __VTX_UI_QT_WINDOW_RENDERER__
#define __VTX_UI_QT_WINDOW_RENDERER__

#include "ui/qt/events.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QWindow>
#include <app/input/input_manager.hpp>

namespace VTX::UI::QT::Window
{

	/**
	 * @brief Layouts.
	 */
	enum struct KB_LAYOUT : uint8_t
	{
		QWERTY,
		AZERTY,
		COUNT
	};

	/**
	 * @brief A window that catches events and forwards them to the application's input manager.
	 */
	class Renderer : public QWindow
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		Renderer();

	  signals:
		/**
		 * @brief Emitted when a mouse button is clicked (pressed and released without dragging).
		 */
		void clicked( const Qt::MouseButton, const QPoint );
		// void dragged( const Qt::MouseButton );

	  protected:
		/**
		 * @brief Override event handlers.
		 */
		void keyPressEvent( QKeyEvent * const ) override;
		void keyReleaseEvent( QKeyEvent * const ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void mouseMoveEvent( QMouseEvent * ) override;
		void mouseReleaseEvent( QMouseEvent * ) override;
		void mouseDoubleClickEvent( QMouseEvent * const ) override;
		void wheelEvent( QWheelEvent * const ) override;

	  private:
		/**
		 * @brief Reference to the application's input manager.
		 */
		App::Input::InputManager & _inputManager;

		/**
		 * @brief Current keyboard layout.
		 */
		KB_LAYOUT _layout = KB_LAYOUT::QWERTY;

		/**
		 * @brief State for mouse dragging.
		 */
		Qt::MouseButton _pressedButton = Qt::NoButton;
		QPointF			_pressPos	   = {};
		QPointF			_lastPos	   = {};
		bool			_dragging	   = false;

		/**
		 * @brief Handle keyboard events and forward them to the input manager.
		 */
		void _handleKeyboard( QKeyEvent * const, const bool p_enable );

		/**
		 * @brief Handle modifier keys (Shift, Ctrl, Alt).
		 */
		void _handleModifiers();

		/**
		 * @brief Convert a point from logical pixels to device pixels.
		 */
		QPoint _toDevicePixels( const QPointF & ) const;

		/**
		 * @brief Set the current keyboard.
		 */
		void _onKBLayoutChange( const Events::KeyboardLayoutChanged & );
	};

} // namespace VTX::UI::QT::Window

#endif
