#ifndef __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__
#define __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QDockWidget>
#include <QGroupBox>

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
		QPointer<QGroupBox> _gbSphere;
		QPointer<QGroupBox> _gbCylinder;
		QPointer<QGroupBox> _gbRibbon;
		QPointer<QGroupBox> _gbSES;

		QGroupBox * const _createGroupBoxSphere( App::Component::Representation::Representation * const );
		QGroupBox * const _createGroupBoxCylinder( App::Component::Representation::Representation * const );
		QGroupBox * const _createGroupBoxRibbon( App::Component::Representation::Representation * const );
		QGroupBox * const _createGroupBoxSES( App::Component::Representation::Representation * const );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
