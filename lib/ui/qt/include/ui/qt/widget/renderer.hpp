#ifndef __VTX_UI_QT_WIDGET_RENDERER__
#define __VTX_UI_QT_WIDGET_RENDERER__

#include "ui/qt/events.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/window/renderer.hpp"
#include <QEvent>
#include <QHideEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QPointer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QSize>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <app/events.hpp>
#include <optional>
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
		/*
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
		*/

		/**
		 * @brief Override resize.
		 */
		bool eventFilter( QObject *, QEvent * ) override;
		void paintEvent( QPaintEvent * ) override;
		void showEvent( QShowEvent * ) override;
		void resizeEvent( QResizeEvent * ) override;

	  protected:
		/**
		 * @brief Debounce callback.
		 */
		void onResizeFinished();

	  private:
		/*
		struct HUDItem
		{
			QPointer<QWidget> widget;
			HUD_POSITION	  position;
		};
		*/

		/**
		 * @brief HUD widgets and their anchor positions.
		 */
		// std::vector<HUDItem> _hudItems;

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

		bool  _pendingRendererReveal = false;
		QSize _pendingRendererRevealSize;

		/**
		 * @brief Add a widget to the overlay at the given position.
		 */
		// void _addHUDWidget( QWidget * const, const HUD_POSITION );

		/**
		 * @brief Synchronize HUD geometry and optionally show items.
		 */
		// void _syncHUDWidgets( const bool p_showWidgets = false );

		/**
		 * @brief Compute the geometry of a HUD widget for the current renderer rect.
		 */
		// QRect _getHUDGeometry( QWidget * const, const HUD_POSITION ) const;

		/**
		 * @brief Give focus back to the rendering surface.
		 */
		void _focusRenderer();

		bool _shouldDelayRendererReveal() const;

		void _collapseRenderer();

		void _revealRenderer( const QSize & );

		void _onPostRender( const App::Events::PostRender & );

		/**
		 * @brief Synchronize widget size after an App renderer resize.
		 */
		void _onRendererResize( const App::Events::RendererResize & );
	};
} // namespace VTX::UI::QT::Widget

#endif
