#ifndef __VTX_UI_QT_WIDGET_LIBRARY_REPRESENTATION__
#define __VTX_UI_QT_WIDGET_LIBRARY_REPRESENTATION__

#include "base_preset_widget.hpp"
#include "ui/qt/core/widget/editable_slider.hpp"
#include "ui/qt/core/widget/hideable_goupe_box.hpp"
#include <QCheckBox>
#include <QLabel>
#include <app/action/representation.hpp>
#include <app/library/preset/representation.hpp>

namespace VTX::UI::QT::Widget::Library
{

	class Representation final : public BasePresetWidget<App::Library::Preset::Representation>
	{
	  public:
		Representation( QWidget * p_parent );

	  protected:
		void _onPresetAdded( const std::string_view ) override;
		void _onPresetChanged() override;

	  private:
		// Sphere.
		QPointer<Core::Widget::HideableGroupBox> _groupboxSphere;
		QPointer<QComboBox>						 _comboBoxSphereRadiusType;
		QPointer<QLabel>						 _labelSphereRadiusAdd;
		QPointer<Core::Widget::EditableSlider>	 _sliderSphereRadiusAdd;
		QPointer<QLabel>						 _labelSphereRadiusFixed;
		QPointer<Core::Widget::EditableSlider>	 _sliderSphereRadiusFixed;

		// Cylinder.
		QPointer<Core::Widget::HideableGroupBox> _groupboxCylinder;
		QPointer<Core::Widget::EditableSlider>	 _sliderCylinderRadius;
		QPointer<QCheckBox>						 _checkBoxCylinderColorBlending;

		// Ribbon.
		QPointer<Core::Widget::HideableGroupBox> _groupboxRibbon;
		QPointer<QCheckBox>						 _checkBoxRibbonColorBlending;

		// SES.
		QPointer<Core::Widget::HideableGroupBox> _groupboxSes;
		QPointer<Core::Widget::EditableSlider>	 _sliderSesProbeRadius;

		template<VTX::Core::Struct::E_REPRESENTATION_VALUES S, typename T>
		void _changeValue( const T p_value )
		{
			App::ACTION().execute<App::Action::Representation::Change<S, T>>( _preset, p_value );
		}

		void _applyLogic();
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
