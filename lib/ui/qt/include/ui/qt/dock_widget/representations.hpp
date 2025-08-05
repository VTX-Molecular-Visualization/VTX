#ifndef __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__
#define __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__

#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/core/widget/hideable_goupe_box.hpp"
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
		QPointer<QGroupBox>						 _gbPreset;
		QPointer<Core::Widget::HideableGroupBox> _gbSphere;
		QPointer<Core::Widget::HideableGroupBox> _gbCylinder;
		QPointer<Core::Widget::HideableGroupBox> _gbRibbon;
		QPointer<Core::Widget::HideableGroupBox> _gbSES;

		QGroupBox * const					   _createGroupBoxPreset();
		Core::Widget::HideableGroupBox * const _createGroupBoxSphere(
			App::Component::Representation::Representation * const
		);
		Core::Widget::HideableGroupBox * const _createGroupBoxCylinder(
			App::Component::Representation::Representation * const
		);
		Core::Widget::HideableGroupBox * const _createGroupBoxRibbon(
			App::Component::Representation::Representation * const
		);
		Core::Widget::HideableGroupBox * const _createGroupBoxSES(
			App::Component::Representation::Representation * const
		);
	};

} // namespace VTX::UI::QT::DockWidget

#endif
