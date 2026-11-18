#ifndef VTX_UI_QT_WIDGET_RENDERER_HPP
#define VTX_UI_QT_WIDGET_RENDERER_HPP

#include "ui/qt/events.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/opengl_renderer.hpp"
#include <QEvent>
#include <QHideEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QPointer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <app/events.hpp>
#include <cassert>
#include <optional>
#include <util/event_hub.hpp>
#include <vector>

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
		 * @brief Give focus back to the rendering surface.
		 */
		void _focusRenderer();

		/**
		 * @brief Synchronize widget size after an App renderer resize.
		 */
		void _onRendererResize( const App::Events::RendererResize & );
		void _onRendererReady( const App::Events::RendererReady & );

		/**
		 * @brief Connection.
		 */
		VTX::Util::EventHub::ScopedConnection _rendererResizeConnection;
		VTX::Util::EventHub::ScopedConnection _rendererReadyConnection;
	};
} // namespace VTX::UI::QT::Widget

#endif
