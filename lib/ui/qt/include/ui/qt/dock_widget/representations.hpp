#ifndef __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__
#define __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__

#include "app/library/preset/representation.hpp"
#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/core/widget/hideable_goupe_box.hpp"

namespace VTX::App::Component::Representation
{
	class Representation;
}

namespace VTX::UI::QT::DockWidget
{

	class Representations : public Core::BaseDockWidget<Representations>
	{
	  public:
		Representations( QWidget * );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
