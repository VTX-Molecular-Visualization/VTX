#ifndef __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__
#define __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__

#include "ui/qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Representations : public BaseWidget<Representations, QDockWidget>
	{
	  public:
		Representations( QWidget * p_parent ) : BaseWidget<Representations, QDockWidget>( "Representations", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
