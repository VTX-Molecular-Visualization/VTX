#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Inspector : public BaseWidget<Inspector, QDockWidget>
	{
	  public:
		Inspector( QWidget * p_parent ) : BaseWidget<Inspector, QDockWidget>( "Inspector", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~Inspector() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
