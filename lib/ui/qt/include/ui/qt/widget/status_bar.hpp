#ifndef __VTX_UI_QT_WIDGET_STATUS_BAR__
#define __VTX_UI_QT_WIDGET_STATUS_BAR__

#include "ui/qt/tool_button/resolution_selector.hpp"
#include <QLabel>
#include <QPointer>
#include <QStatusBar>
#include <QTimer>
#include <QToolButton>
#include <app/events.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget to display application status, such as FPS, resolution and Python state.
	 */
	class StatusBar : public QStatusBar
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		StatusBar( QWidget * p_parent );

	  private:
		/**
		 * @brief FPS counter.
		 */
		QPointer<QLabel> _fps;

		/**
		 * @brief Resolution widget to display current/change from menu.
		 */
		QPointer<ToolButton::ResolutionSelector> _resolution;

		/**
		 * @brief Idle state.
		 */
		bool _rendering = false;

		/**
		 * @brief Python version.
		 */
		QPointer<QLabel> _python;

		/**
		 * @brief Update functions.
		 */
		void _updateGPUState( const App::Events::PostRender & );
		void _updateResolution( const App::Events::RendererResize & p_e );
		void _setCurrentFPS();
		void _pythonInitialized( const App::Events::PythonInitialized & );
	};

} // namespace VTX::UI::QT::Widget

#endif
