#ifndef __VTX_UI_QT_WIDGET_LIBRARY_REPRESENTATION__
#define __VTX_UI_QT_WIDGET_LIBRARY_REPRESENTATION__

#include "base_preset_widget.hpp"
#include "ui/qt/core/widget/editable_slider.hpp"
#include "ui/qt/core/widget/hideable_goupe_box.hpp"
#include <QCheckBox>
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
		QPointer<Core::Widget::EditableSlider>	 _sliderSphereRadiusAdd;
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
			App::ACTION_SYSTEM().execute<App::Action::Representation::Change<S, T>>( _preset, p_value );
		}

		void _applyLogic();

		/*
		Core::Widget::HideableGroupBox * const _createGroupBoxSphere()
		{
			p_component->callback<E_REPRESENTATION_VALUES::HAS_SPHERE, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED, bool>() +=
				[ comboBox, labelRadiusAdd, sliderRadiusAdd, labelRadiusFixed, sliderRadiusFixed ]( const bool p_value )
			{
				comboBox->setCurrentIndex( int( p_value ) );
				switch ( p_value )
				{
				case false:
					labelRadiusAdd->setVisible( true );
					sliderRadiusAdd->setVisible( true );
					labelRadiusFixed->setVisible( false );
					sliderRadiusFixed->setVisible( false );
					break;
				case true:
					labelRadiusAdd->setVisible( false );
					sliderRadiusAdd->setVisible( false );
					labelRadiusFixed->setVisible( true );
					sliderRadiusFixed->setVisible( true );
					break;
				}
			};
			p_component->callback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD, float>() +=
				[ sliderRadiusAdd ]( const float p_value ) { sliderRadiusAdd->setValue( p_value * 100 ); };
			p_component->callback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED, float>() +=
				[ sliderRadiusFixed ]( const float p_value ) { sliderRadiusFixed->setValue( p_value * 100 ); };

			return groupBox;
		}


		Core::Widget::HideableGroupBox * const _createGroupBoxCylinder()
		{
			p_component->callback<E_REPRESENTATION_VALUES::HAS_CYLINDER, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_VALUES::RADIUS_CYLINDER, float>() +=
				[ slider ]( const float p_value ) { slider->setValue( p_value * 100 ); };
			p_component->callback<E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING, bool>() +=
				[ checkBoxColorBlending ]( const bool p_value ) { checkBoxColorBlending->setChecked( p_value ); };

			return groupBox;
		}

		Core::Widget::HideableGroupBox * const _createGroupBoxRibbon()
		{
			p_component->callback<E_REPRESENTATION_VALUES::HAS_RIBBON, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING, bool>() +=
				[ checkBoxColorBlending ]( const bool p_value ) { checkBoxColorBlending->setChecked( p_value ); };

			return groupBox;
		}

		Core::Widget::HideableGroupBox * const _createGroupBoxSES()
		{
			// Probe Radius.
			auto * labelRadius = new QLabel( "Probe radius", groupBox );
			auto * slider	   = new QSlider( Qt::Orientation::Horizontal, groupBox );
			groupBox->addWidget( labelRadius );
			groupBox->addWidget( slider );
			slider->setMinimum( SES_PROBE_RADIUS_MIN * 100 );
			slider->setMaximum( SES_PROBE_RADIUS_MAX * 100 );
			slider->setValue( p_component->getRepresentation().sesProbeRadius * 100 );
			connect(
				slider,
				&QSlider::valueChanged,
				[ this ]( const int p_value )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float>>(
							_preset, static_cast<float>( p_value ) / 100.f
						);
				}
			);

			// Callbacks.
			p_component->callback<E_REPRESENTATION_VALUES::HAS_SES, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float>() +=
				[ slider ]( const float p_value ) { slider->setValue( p_value * 100 ); };
			return groupBox;

			return groupBox;
		}

		*/
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
