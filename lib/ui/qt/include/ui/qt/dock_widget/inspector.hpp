#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "ui/qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Inspector : public BaseWidget<Inspector, QDockWidget>
	{
	  public:
		Inspector( QWidget * p_parent ) : BaseWidget<Inspector, QDockWidget>( "Selection", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
