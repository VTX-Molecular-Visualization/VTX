#ifndef __VTX_UI_QT_DOCK_WIDGET_COLORS__
#define __VTX_UI_QT_DOCK_WIDGET_COLORS__

#include "ui/qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Colors : public BaseWidget<Colors, QDockWidget>
	{
	  public:
		Colors( QWidget * p_parent );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
