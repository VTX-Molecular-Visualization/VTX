#ifndef VTX_UI_QT_WIDGET_OPENGL_RENDERER_HPP
#define VTX_UI_QT_WIDGET_OPENGL_RENDERER_HPP

#include "ui/qt/events.hpp"
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QWheelEvent>
#include <app/input/input_manager.hpp>
#include <cstdint>
#include <optional>
#include <util/event_hub.hpp>

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
	 * @brief Handle user input and forward it to the InputManager.
	 */
	class OpenGLRenderer : public QOpenGLWidget
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		explicit OpenGLRenderer( QWidget * p_parent );
		~OpenGLRenderer() override;

		static QSurfaceFormat surfaceFormat();
		static bool			  isOpenGLSupported();

		bool	 createGLContext();
		void	 destroyGLContext();
		void	 swapGLBuffers();
		bool	 setGLSwapInterval( int ) const;
		void *	 getGLProcAddress( const char * ) const;
		uint32_t getGLDefaultFramebuffer() const;

	  signals:
		/**
		 * @brief On click.
		 */
		void clicked( Qt::MouseButton, QPoint );

	  protected:
		/**
		 * @brief Override events.
		 */
		bool event( QEvent * ) override;
		void resizeGL( int, int ) override;
		void paintGL() override;
		void keyPressEvent( QKeyEvent * ) override;
		void keyReleaseEvent( QKeyEvent * ) override;
		void focusInEvent( QFocusEvent * ) override;
		void focusOutEvent( QFocusEvent * ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void mouseMoveEvent( QMouseEvent * ) override;
		void mouseReleaseEvent( QMouseEvent * ) override;
		void mouseDoubleClickEvent( QMouseEvent * ) override;
		void wheelEvent( QWheelEvent * ) override;

	  private:
		App::Input::InputManager & _inputManager;
		// TODO: move to settings.
		KB_LAYOUT		_layout		   = KB_LAYOUT::QWERTY;
		Qt::MouseButton _pressedButton = Qt::NoButton;
		QPointF			_pressPos	   = {};
		QPointF			_lastPos	   = {};
		bool			_dragging	   = false;

		void											_handleKeyboard( QKeyEvent *, bool p_enable );
		void											_handleModifiers();
		std::optional<App::Input::InputManager::Action> _getKeyboardAction( int ) const;
		QPoint											_toDevicePixels( const QPointF & ) const;
		void											_onKBLayoutChange( const Events::KeyboardLayoutChanged & );

		VTX::Util::EventHub::ScopedConnection _keyboardLayoutChangedConnection;

		bool	 _glReady			 = false;
		uint32_t _defaultFramebuffer = 0;

		void _syncDefaultFramebuffer();
	};

} // namespace VTX::UI::QT::Widget

#endif
