#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "ui/qt/core/base_dock_widget.hpp"
#include <ui/qt/widget/trajectory/trajectory_manager.hpp>

namespace VTX::UI::QT::DockWidget
{

	class Inspector : public Core::BaseDockWidget<Inspector>
	{
	  public:
		Inspector( QWidget * p_parent ) : BaseDockWidget<Inspector>( "Selection", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
			
			auto * trajManager = new Widget::TrajectoryManager( this );
			setWidget( trajManager );
		}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
