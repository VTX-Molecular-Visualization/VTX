#ifndef __VTX_UI_QT_WIDGET_OPENGL_WIDGET__
#define __VTX_UI_QT_WIDGET_OPENGL_WIDGET__

#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/window/renderer.hpp"
#include <QPointer>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget to present an OpenGL rendering context.
	 * No more Qt OpenGL implementation used.
	 */
	class OpenGLWidget : public BaseWidget<OpenGLWidget, QWidget>
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		OpenGLWidget( QWidget * );
		~OpenGLWidget();

		/**
		 * @brief Get the native window handle (X11 Window or wl_surface*).
		 */
		uintptr_t getNativeSurface() const;

		/**
		 * @brief Get the native display handle when needed by the platform backend.
		 */
		uintptr_t getNativeDisplay() const;

		/**
		 * @brief Override resize.
		 */
		void resizeEvent( QResizeEvent * ) override;

	  protected:
		/**
		 * @brief Override event filter to handle events from the window and container.
		 */
		bool eventFilter( QObject *, QEvent * ) override;

		/**
		 * @brief Debounce callback.
		 */
		void onResizeFinished();

	  private:
		/**
		 * @brief OpenGL rendering window.
		 */
		QPointer<Window::Renderer> _window;

		/**
		 * @brief Container widget for the OpenGL window.
		 */
		QPointer<QWidget> _container;

		/**
		 * @brief Debounce timer for resize events.
		 */
		QTimer _resizeTimer;
	};
} // namespace VTX::UI::QT::Widget

#endif
