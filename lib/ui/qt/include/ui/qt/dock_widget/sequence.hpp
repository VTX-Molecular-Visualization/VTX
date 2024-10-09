#ifndef __VTX_UI_QT_DOCK_WIDGET_SEQUENCE__
#define __VTX_UI_QT_DOCK_WIDGET_SEQUENCE__

#include "ui/qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Sequence : public BaseWidget<Sequence, QDockWidget>
	{
	  public:
		Sequence( QWidget * p_parent ) : BaseWidget<Sequence, QDockWidget>( "Sequence", p_parent )
		{
			setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
		}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
