#ifndef __VTX_UI_QT_DOCK_WIDGET_SEQUENCE__
#define __VTX_UI_QT_DOCK_WIDGET_SEQUENCE__

#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Sequence : public QDockWidget
	{
	  public:
		Sequence( QWidget * p_parent ) : QDockWidget( "Sequence", p_parent )
		{
			setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
		}

		virtual ~Sequence() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
