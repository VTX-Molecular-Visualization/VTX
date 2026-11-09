#ifndef VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS_HPP
#define VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS_HPP

#include "ui/qt/dock_widget/base_dock_widget.hpp"

namespace VTX::UI::QT::DockWidget
{

	class Representations : public BaseDockWidget<Representations>
	{
	  public:
		Representations( QWidget * );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
