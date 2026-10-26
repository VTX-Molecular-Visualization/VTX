#ifndef VTX_UI_QT_DOCK_WIDGET_COLOR_LAYOUTS_HPP
#define VTX_UI_QT_DOCK_WIDGET_COLOR_LAYOUTS_HPP

#include "ui/qt/dock_widget/base_dock_widget.hpp"

namespace VTX::UI::QT::DockWidget
{

	class ColorLayouts : public BaseDockWidget<ColorLayouts>
	{
	  public:
		ColorLayouts( QWidget * );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
