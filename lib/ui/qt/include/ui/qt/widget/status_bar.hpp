#ifndef __VTX_UI_QT_WIDGET_STATUS_BAR__
#define __VTX_UI_QT_WIDGET_STATUS_BAR__

#include <QLabel>
#include <QPointer>
#include <QStatusBar>
#include <QTimer>
#include <QToolButton>
#include <app/events.hpp>

namespace VTX::UI::QT::Widget
{

	class StatusBar : public QStatusBar
	{
		Q_OBJECT

	  public:
		StatusBar( QWidget * p_parent );

	  private:
		QPointer<QLabel> _fps;
		QPointer<QToolButton> _resolution;
		bool			 _rendering = false;
		QPointer<QLabel> _python;

		void _updateGPUState( const App::Events::PostRender & );
		void _setupResolutionMenu();
		void _setResolution( const size_t, const size_t );
		void _updateResolution( const App::Events::RendererResize & );
		void _setCurrentFPS();
		void _pythonInitialized( const App::Events::PythonInitialized & );
	};

} // namespace VTX::UI::QT::Widget

#endif
