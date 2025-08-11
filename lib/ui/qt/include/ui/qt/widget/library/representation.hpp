#ifndef __VTX_UI_QT_WIDGET_LIBRARY_REPRESENTATION__
#define __VTX_UI_QT_WIDGET_LIBRARY_REPRESENTATION__

#include "base_preset_widget.hpp"
#include <QCheckbox>
#include <QLabel>
#include <app/action/representation.hpp>
#include <app/library/preset/representation.hpp>

namespace VTX::UI::QT::Widget::Library
{

	class Representation final : public BasePresetWidget<App::Library::Preset::Representation>
	{
	  public:
		Representation( QWidget * p_parent ) : BasePresetWidget( p_parent )
		{
			_groupboxPreset->setTitle( "Edit representation" );
		}

	  protected:
		void _onPresetChanged() override
		{
			// TODO: update all widgets and connect app callbacks.
		}

	  private:
		Core::Widget::HideableGroupBox * const _createGroupBoxSphere()
		{
			using namespace App::Library::Preset;

			auto * groupBox = new Core::Widget::HideableGroupBox( "Atoms", this );

			// Active.
			// groupBox->setChecked( p_component->getRepresentation().hasSphere );

			connect(
				groupBox,
				&Core::Widget::HideableGroupBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>>(
							_presetSelector->getCurrentPreset(), p_checked
						);
				}
			);

			// Fixed.
			auto * comboBox = new QComboBox( groupBox );

			groupBox->addWidget( comboBox );
			comboBox->addItem( "Van der Waals radius" );
			comboBox->addItem( "Fixed radius" );
			// comboBox->setCurrentIndex( int( p_component->getRepresentation().radiusFixed ) );

			connect(
				comboBox,
				QOverload<int>::of( &QComboBox::currentIndexChanged ),
				[ this ]( const int p_index )
				{
					bool isFixed = bool( p_index );
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED, bool>>(
							_presetSelector->getCurrentPreset(), isFixed
						);
				}
			);

			// Radius add (VdW only).
			auto * labelRadiusAdd  = new QLabel( "Radius add", groupBox );
			auto * sliderRadiusAdd = new QSlider( Qt::Orientation::Horizontal, groupBox );
			groupBox->addWidget( labelRadiusAdd );
			groupBox->addWidget( sliderRadiusAdd );
			// TODO: move min and max to representation.
			// sliderRadiusAdd->setMinimum( -( VTX::Core::ChemDB::Atom::VDW_RADIUS_MIN ) * 100 + 1 );
			sliderRadiusAdd->setMinimum( RADIUS_SPHERE_ADD_MIN );
			sliderRadiusAdd->setMaximum( RADIUS_SPHERE_ADD_MAX );
			// sliderRadiusAdd->setValue( p_component->getRepresentation().radiusSphereAdd * 100 );
			connect(
				sliderRadiusAdd,
				&QSlider::valueChanged,
				[ this ]( const int p_value )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, float>>(
							_presetSelector->getCurrentPreset(), float( p_value ) / 100.f
						);
				}
			);

			// Radius fixed.
			auto * labelRadiusFixed	 = new QLabel( "Radius", groupBox );
			auto * sliderRadiusFixed = new QSlider( Qt::Orientation::Horizontal, groupBox );
			groupBox->addWidget( labelRadiusFixed );
			groupBox->addWidget( sliderRadiusFixed );
			sliderRadiusFixed->setMinimum( RADIUS_SPHERE_FIXED_MIN );
			sliderRadiusFixed->setMaximum( RADIUS_SPHERE_FIXED_MAX );
			// sliderRadiusFixed->setValue( p_component->getRepresentation().radiusSphereFixed * 100 );
			connect(
				sliderRadiusFixed,
				&QSlider::valueChanged,
				[ this ]( const int p_value )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, float>>(
							_presetSelector->getCurrentPreset(), float( p_value ) / 100.f
						);
				}
			);

			// Callbacks.
			/*
			p_component->callback<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED, bool>() +=
				[ comboBox, labelRadiusAdd, sliderRadiusAdd, labelRadiusFixed, sliderRadiusFixed ]( const bool p_value
			)
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
			p_component->callback<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, float>() +=
				[ sliderRadiusAdd ]( const float p_value ) { sliderRadiusAdd->setValue( p_value * 100 ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, float>() +=
				[ sliderRadiusFixed ]( const float p_value ) { sliderRadiusFixed->setValue( p_value * 100 ); };
				*/

			return groupBox;
		}

		Core::Widget::HideableGroupBox * const _createGroupBoxCylinder()
		{
			using namespace App::Library::Preset;

			auto * groupBox = new Core::Widget::HideableGroupBox( "Bonds", this );

			// Active.
			// groupBox->setChecked( p_component->getRepresentation().hasCylinder );

			connect(
				groupBox,
				&Core::Widget::HideableGroupBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_CYLINDER, bool>>(
							_presetSelector->getCurrentPreset(), p_checked
						);
				}
			);

			// Radius.
			auto * labelRadius = new QLabel( "Radius", groupBox );
			auto * slider	   = new QSlider( Qt::Orientation::Horizontal, groupBox );
			groupBox->addWidget( labelRadius );
			groupBox->addWidget( slider );
			slider->setMinimum( RADIUS_CYLINDER_MIN );
			slider->setMaximum( RADIUS_CYLINDER_MAX );
			// slider->setValue( p_component->getRepresentation().radiusCylinder * 100 );
			connect(
				slider,
				&QSlider::valueChanged,
				[ this ]( const int p_value )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, float>>(
							_presetSelector->getCurrentPreset(), static_cast<float>( p_value ) / 100.f
						);
				}
			);

			// Color blending.
			auto * checkBoxColorBlending = new QCheckBox( "Blend colors", groupBox );
			groupBox->addWidget( checkBoxColorBlending );
			// checkBoxColorBlending->setChecked( p_component->getRepresentation().cylinderColorBlending );
			connect(
				checkBoxColorBlending,
				&QCheckBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, bool>>(
							_presetSelector->getCurrentPreset(), p_checked
						);
				}
			);

			// Callbacks.
			/*
			p_component->callback<E_REPRESENTATION_SETTINGS::HAS_CYLINDER, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, float>() +=
				[ slider ]( const float p_value ) { slider->setValue( p_value * 100 ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, bool>() +=
				[ checkBoxColorBlending ]( const bool p_value ) { checkBoxColorBlending->setChecked( p_value ); };
	*/

			return groupBox;
		}

		Core::Widget::HideableGroupBox * const _createGroupBoxRibbon()
		{
			using namespace App::Library::Preset;

			auto * groupBox = new Core::Widget::HideableGroupBox( "Ribbons", this );

			// Active.
			// groupBox->setChecked( p_component->getRepresentation().hasRibbon );

			connect(
				groupBox,
				&Core::Widget::HideableGroupBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_RIBBON, bool>>(
							_presetSelector->getCurrentPreset(), p_checked
						);
				}
			);

			// Color blending.
			auto * checkBoxColorBlending = new QCheckBox( "Blend colors", groupBox );
			groupBox->addWidget( checkBoxColorBlending );
			// checkBoxColorBlending->setChecked( p_component->getRepresentation().ribbonColorBlending );
			connect(
				checkBoxColorBlending,
				&QCheckBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, bool>>(
							_presetSelector->getCurrentPreset(), p_checked
						);
				}
			);

			// Callbacks.
			/*
			p_component->callback<E_REPRESENTATION_SETTINGS::HAS_RIBBON, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, bool>() +=
				[ checkBoxColorBlending ]( const bool p_value ) { checkBoxColorBlending->setChecked( p_value ); };
				*/

			return groupBox;
		}

		Core::Widget::HideableGroupBox * const _createGroupBoxSES()
		{
			using namespace App::Library::Preset;

			auto * groupBox = new Core::Widget::HideableGroupBox( "SES", this );

			// Active.
			// groupBox->setChecked( p_component->getRepresentation().hasSes );

			connect(
				groupBox,
				&Core::Widget::HideableGroupBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SES, bool>>(
							_presetSelector->getCurrentPreset(), p_checked
						);
				}
			);

			// Probe Radius.
			auto * labelRadius = new QLabel( "Probe radius", groupBox );
			auto * slider	   = new QSlider( Qt::Orientation::Horizontal, groupBox );
			groupBox->addWidget( labelRadius );
			groupBox->addWidget( slider );
			slider->setMinimum( SES_PROBE_RADIUS_MIN * 100 );
			slider->setMaximum( SES_PROBE_RADIUS_MAX * 100 );
			// slider->setValue( p_component->getRepresentation().sesProbeRadius * 100 );
			connect(
				slider,
				&QSlider::valueChanged,
				[ this ]( const int p_value )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::SES_PROBE_RADIUS, float>>(
							_presetSelector->getCurrentPreset(), static_cast<float>( p_value ) / 100.f
						);
				}
			);

			// Callbacks.
			/*
			p_component->callback<E_REPRESENTATION_SETTINGS::HAS_SES, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::SES_PROBE_RADIUS, float>() +=
				[ slider ]( const float p_value ) { slider->setValue( p_value * 100 ); };
			return groupBox;
				*/

			return groupBox;
		}
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
