#ifndef __VTX_UI_QT_DOCK_WIDGET_OPTIONS__
#define __VTX_UI_QT_DOCK_WIDGET_OPTIONS__

#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Options : public QDockWidget
	{
	  public:
		Options( QWidget * p_parent ) : QDockWidget( "Options", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~Options() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
