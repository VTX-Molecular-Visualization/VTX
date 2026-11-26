#pragma once

#include "ui/qt/dock_widget/base_dock_widget.hpp"

namespace VTX::UI::QT::DockWidget
{

	class Representations : public BaseDockWidget<Representations>
	{
	  public:
		Representations( QWidget * );
	};

} // namespace VTX::UI::QT::DockWidget
