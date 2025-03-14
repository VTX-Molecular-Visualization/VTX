#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "ui/qt/core/base_dock_widget.hpp"

namespace VTX::UI::QT::DockWidget
{

	class Inspector : public Core::BaseDockWidget<Inspector>
	{
	  public:
		Inspector( QWidget * p_parent ) : BaseDockWidget<Inspector>( "Selection", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
