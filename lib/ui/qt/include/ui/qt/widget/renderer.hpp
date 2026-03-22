#ifndef __VTX_UI_QT_WIDGET_RENDERER__
#define __VTX_UI_QT_WIDGET_RENDERER__

#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/window/renderer.hpp"
#include <QGridLayout>
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
		 * @brief Positions for HUD elements.
		 */
		enum struct HUD_POSITION
		{
			TOP_LEFT,
			TOP_CENTER,
			TOP_RIGHT,
			CENTER_LEFT,
			CENTER_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_CENTER,
			BOTTOM_RIGHT
		};

		/**
		 * @brief Constructor.
		 */
		Renderer( QWidget * );
		~Renderer();

		/**
		 * @brief Get native surface, display and platform for graphic context creation.
		 */
		uintptr_t getNativeSurface() const;
		uintptr_t getNativeDisplay() const;
		uint8_t	  getNativePlatform() const;

		/**
		 * @brief Create a toolbar from type.
		 */
		template<typename TB>
		TB * const createToolBar( const HUD_POSITION p_pos )
		{
			TB * const toolBar = new TB( this );
			_addHUDWidget( toolBar, p_pos );
			return toolBar;
		}

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
		 * @brief Transparent overlay widget above the rendering surface.
		 */
		QPointer<QWidget> _overlay;

		/**
		 * @brief Grid layout used to place overlay widgets.
		 */
		QPointer<QGridLayout> _overlayLayout;

		/**
		 * @brief Debounce timer for resize events.
		 */
		QTimer _resizeTimer;

		/**
		 * @brief Add a widget to the overlay at the given position.
		 */
		void _addHUDWidget( QWidget * const, const HUD_POSITION );
	};
} // namespace VTX::UI::QT::Widget

#endif
