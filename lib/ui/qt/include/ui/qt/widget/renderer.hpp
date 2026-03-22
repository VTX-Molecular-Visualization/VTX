#ifndef __VTX_UI_QT_WIDGET_RENDERER__
#define __VTX_UI_QT_WIDGET_RENDERER__

#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/window/renderer.hpp"
#include <QPointer>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget to present an OpenGL rendering context.
	 * No more Qt OpenGL implementation used.
	 */
	class Renderer : public BaseWidget<Renderer, QWidget>
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		Renderer( QWidget * );
		~Renderer();

		uintptr_t getNativeSurface() const;
		uintptr_t getNativeDisplay() const;
		uint8_t	  getNativePlatform() const;

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
