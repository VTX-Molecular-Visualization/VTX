#ifndef __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__
#define __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__

#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Representations : public QDockWidget
	{
	  public:
		Representations( QWidget * p_parent ) : QDockWidget( "Representations", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~Representations() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
