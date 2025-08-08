#ifndef __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__
#define __VTX_UI_QT_DOCK_WIDGET_REPRESENTATIONS__

#include "app/library/preset/representation.hpp"
#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/core/widget/hideable_goupe_box.hpp"
#include "ui/qt/widget/preset_selector.hpp"

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
		QPointer<Widget::PresetSelector<App::Library::Preset::Representation>> _presetSelector;

		QPointer<Core::Widget::HideableGroupBox> _gbSphere;
		QPointer<Core::Widget::HideableGroupBox> _gbCylinder;
		QPointer<Core::Widget::HideableGroupBox> _gbRibbon;
		QPointer<Core::Widget::HideableGroupBox> _gbSES;

		Core::Widget::HideableGroupBox * const _createGroupBoxSphere();
		Core::Widget::HideableGroupBox * const _createGroupBoxCylinder();
		Core::Widget::HideableGroupBox * const _createGroupBoxRibbon();
		Core::Widget::HideableGroupBox * const _createGroupBoxSES();

		void setPreset() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
