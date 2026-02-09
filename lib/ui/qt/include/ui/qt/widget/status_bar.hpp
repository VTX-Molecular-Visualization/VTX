#ifndef __VTX_UI_QT_WIDGET_STATUS_BAR__
#define __VTX_UI_QT_WIDGET_STATUS_BAR__

#include <QLabel>
#include <QPointer>
#include <QStatusBar>
#include <QTimer>
#include <app/events.hpp>

namespace VTX::UI::QT::Widget
{

	class StatusBar : public QStatusBar
	{
		Q_OBJECT

	  public:
		StatusBar( QWidget * p_parent );

	  private:
		QPointer<QLabel> _label;
		bool			 _rendering = false;

		void _updateGPUState( const App::Events::PostRender & );
		void _setCurrentFPS();
	};

} // namespace VTX::UI::QT::Widget

#endif
