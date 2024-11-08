#ifndef __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__
#define __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QDockWidget>

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

	  private:
		void _createGroupBoxSphere( App::Component::Representation::Representation * const );
		void _createGroupBoxCylinder( App::Component::Representation::Representation * const );
		void _createGroupBoxRibbon( App::Component::Representation::Representation * const );
		void _createGroupBoxSES( App::Component::Representation::Representation * const );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
