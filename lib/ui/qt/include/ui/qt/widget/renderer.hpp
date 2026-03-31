#ifndef __VTX_UI_QT_WIDGET_RENDERER__
#define __VTX_UI_QT_WIDGET_RENDERER__

#include "ui/qt/events.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/window/renderer.hpp"
#include <QEvent>
#include <QHideEvent>
#include <QPointer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <vector>

namespace VTX::UI::QT::Widget
{
	using KB_LAYOUT = Window::KB_LAYOUT;

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
			toolBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
			for ( auto * button : toolBar->template findChildren<QToolButton *>() )
			{
				button->setToolButtonStyle( toolBar->toolButtonStyle() );
			}
			_addHUDWidget( toolBar, p_pos );
			return toolBar;
		}

		/**
		 * @brief Override resize.
		 */
		bool eventFilter( QObject *, QEvent * ) override;
		void showEvent( QShowEvent * ) override;
		void resizeEvent( QResizeEvent * ) override;

	  protected:
		/**
		 * @brief Debounce callback.
		 */
		void onResizeFinished();

	  private:
		struct HUDItem
		{
			QPointer<QWidget> widget;
			HUD_POSITION	  position;
		};

		/**
		 * @brief HUD widgets and their anchor positions.
		 */
		std::vector<HUDItem> _hudItems;

		/**
		 * @brief Debounce timer for resize events.
		 */
		QTimer _resizeTimer;

		/**
		 * @brief OpenGL rendering window.
		 */
		QPointer<Window::Renderer> _window;

		/**
		 * @brief Container widget for the OpenGL window.
		 */
		QPointer<QWidget> _container;

		/**
		 * @brief Add a widget to the overlay at the given position.
		 */
		void _addHUDWidget( QWidget * const, const HUD_POSITION );

		/**
		 * @brief Synchronize overlay geometry.
		 */
		void _syncOverlayGeometry();

		/**
		 * @brief Give focus back to the rendering surface.
		 */
		void _focusRenderer();
	};
} // namespace VTX::UI::QT::Widget

#endif
