#ifndef __VTX_UI_QT_DOCK_WIDGET_COLORS__
#define __VTX_UI_QT_DOCK_WIDGET_COLORS__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Colors : public Core::BaseDockWidget<Colors>
	{
	  public:
		Colors( QWidget * p_parent );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
