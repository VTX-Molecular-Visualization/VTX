#pragma once

#include "ui/qt/dock_widget/base_dock_widget.hpp"

namespace VTX::UI::QT::DockWidget
{

	class ColorLayouts : public BaseDockWidget<ColorLayouts>
	{
	  public:
		ColorLayouts( QWidget * );
	};

} // namespace VTX::UI::QT::DockWidget
