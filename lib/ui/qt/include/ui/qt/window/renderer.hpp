#ifndef __VTX_UI_QT_WINDOW_RENDERER__
#define __VTX_UI_QT_WINDOW_RENDERER__

#include "ui/qt/events.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QWindow>
#include <app/input/input_manager.hpp>
#include <optional>

namespace VTX::UI::QT::Window
{
	// TODO: move to settings.
	enum struct KB_LAYOUT : uint8_t
	{
		QWERTY,
		AZERTY,
		COUNT
	};

	/**
	 * @brief Handle user input and forward it to the InputManager.
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
		 * @brief On click.
		 */
		void clicked( const Qt::MouseButton, const QPoint );

	  protected:
		/**
		 * @brief Override events.
		 */
		void keyPressEvent( QKeyEvent * const ) override;
		void keyReleaseEvent( QKeyEvent * const ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void mouseMoveEvent( QMouseEvent * ) override;
		void mouseReleaseEvent( QMouseEvent * ) override;
		void mouseDoubleClickEvent( QMouseEvent * const ) override;
		void wheelEvent( QWheelEvent * const ) override;

	  private:
		App::Input::InputManager & _inputManager;
		// TODO: move to settings.
		KB_LAYOUT		_layout		   = KB_LAYOUT::QWERTY;
		Qt::MouseButton _pressedButton = Qt::NoButton;
		QPointF			_pressPos	   = {};
		QPointF			_lastPos	   = {};
		bool			_dragging	   = false;

		void											_handleKeyboard( QKeyEvent * const, const bool p_enable );
		void											_handleModifiers();
		std::optional<App::Input::InputManager::Action> _getKeyboardAction( const int ) const;
		QPoint											_toDevicePixels( const QPointF & ) const;
		void											_onKBLayoutChange( const Events::KeyboardLayoutChanged & );
	};

} // namespace VTX::UI::QT::Window

#endif
