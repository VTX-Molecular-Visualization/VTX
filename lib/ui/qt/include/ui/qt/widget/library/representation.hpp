#ifndef __VTX_UI_QT_WIDGET_LIBRARY_REPRESENTATION__
#define __VTX_UI_QT_WIDGET_LIBRARY_REPRESENTATION__

#include "base_preset_widget.hpp"
#include "ui/qt/core/widget/editable_slider.hpp"
#include "ui/qt/core/widget/hideable_goupe_box.hpp"
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
			using namespace App::Library::Preset;
			using namespace Core::Widget;

			_groupboxPreset->setTitle( "Edit representation" );

			// Sphere.
			_groupboxSphere = new HideableGroupBox( "Atoms", _groupboxPreset );
			_groupboxPreset->layout()->addWidget( _groupboxSphere );

			_comboBoxSphereRadiusType = new QComboBox( _groupboxSphere );
			_groupboxSphere->addWidget( _comboBoxSphereRadiusType );
			_comboBoxSphereRadiusType->addItem( "Van der Waals radius" );
			_comboBoxSphereRadiusType->addItem( "Fixed radius" );

			_sliderSphereRadiusAdd = new EditableSlider( Qt::Orientation::Horizontal, _groupboxSphere );
			_groupboxSphere->addWidget( new QLabel( "Radius add", _groupboxSphere ) );
			_groupboxSphere->addWidget( _sliderSphereRadiusAdd );
			// TODO: move min and max to representation.
			// sliderRadiusAdd->setMinimum( -( VTX::Core::ChemDB::Atom::VDW_RADIUS_MIN ) * 100 + 1 );
			_sliderSphereRadiusAdd->setMinimum( RADIUS_SPHERE_ADD_MIN );
			_sliderSphereRadiusAdd->setMaximum( RADIUS_SPHERE_ADD_MAX );

			_sliderSphereRadiusFixed = new EditableSlider( Qt::Orientation::Horizontal, _groupboxSphere );
			_groupboxSphere->addWidget( new QLabel( "Radius", _groupboxSphere ) );
			_groupboxSphere->addWidget( _sliderSphereRadiusFixed );
			_sliderSphereRadiusFixed->setMinimum( RADIUS_SPHERE_FIXED_MIN );
			_sliderSphereRadiusFixed->setMaximum( RADIUS_SPHERE_FIXED_MAX );

			// Cylinder.
			_groupboxCylinder = new HideableGroupBox( "Bonds", _groupboxPreset );
			_groupboxPreset->layout()->addWidget( _groupboxCylinder );

			_sliderCylinderRadius = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCylinder );
			_groupboxCylinder->addWidget( new QLabel( "Radius", _groupboxCylinder ) );
			_groupboxCylinder->addWidget( _sliderCylinderRadius );
			_sliderCylinderRadius->setMinimum( RADIUS_CYLINDER_MIN );
			_sliderCylinderRadius->setMaximum( RADIUS_CYLINDER_MAX );

			_checkBoxCylinderColorBlending = new QCheckBox( "Blend colors", _groupboxCylinder );
			_groupboxCylinder->addWidget( _checkBoxCylinderColorBlending );

			// Ribbon.
			_groupboxRibbon = new HideableGroupBox( "Ribbons", _groupboxPreset );
			_groupboxPreset->layout()->addWidget( _groupboxRibbon );

			_checkBoxRibbonColorBlending = new QCheckBox( "Blend colors", _groupboxRibbon );
			_groupboxRibbon->addWidget( _checkBoxRibbonColorBlending );

			// SES.
			_groupboxSes = new HideableGroupBox( "SES", _groupboxPreset );
			_groupboxPreset->layout()->addWidget( _groupboxSes );

			// Connect callbacks.
			// TODO: this->_preset is a copy of pointer.
			connect(
				_groupboxSphere,
				&HideableGroupBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>>(
							this->_preset, p_checked
						);
				}
			);

			connect(
				_comboBoxSphereRadiusType,
				QOverload<int>::of( &QComboBox::currentIndexChanged ),
				[ this ]( const int p_index )
				{
					bool isFixed = bool( p_index );
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED, bool>>(
							this->_preset, isFixed
						);
				}
			);

			connect(
				_sliderSphereRadiusAdd,
				&EditableSlider::valueChanged,
				[ this ]( const float p_value )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, float>>(
							this->_preset, p_value
						);
				}
			);

			connect(
				_sliderSphereRadiusFixed,
				&EditableSlider::valueChanged,
				[ this ]( const float p_value )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, float>>(
							this->_preset, p_value
						);
				}
			);

			connect(
				_groupboxCylinder,
				&HideableGroupBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_CYLINDER, bool>>(
							this->_preset, p_checked
						);
				}
			);

			connect(
				_sliderCylinderRadius,
				&EditableSlider::valueChanged,
				[ this ]( const float p_value )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, float>>(
							this->_preset, p_value
						);
				}
			);

			connect(
				_checkBoxCylinderColorBlending,
				&QCheckBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, bool>>(
							this->_preset, p_checked
						);
				}
			);

			connect(
				_groupboxRibbon,
				&HideableGroupBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_RIBBON, bool>>(
							this->_preset, p_checked
						);
				}
			);

			connect(
				_checkBoxRibbonColorBlending,
				&QCheckBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, bool>>(
							this->_preset, p_checked
						);
				}
			);

			connect(
				_groupboxSes,
				&HideableGroupBox::toggled,
				[ this ]( const bool p_checked )
				{
					App::ACTION_SYSTEM()
						.execute<App::Action::Representation::
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SES, bool>>(
							this->_preset, p_checked
						);
				}
			);

			// Set default values.
			_onPresetChanged();
		}

	  protected:
		void _onPresetChanged() override
		{
			assert( this->_preset != nullptr );

			const QSignalBlocker blocker0( _groupboxSphere );
			const QSignalBlocker blocker1( _comboBoxSphereRadiusType );
			const QSignalBlocker blocker2( _sliderSphereRadiusAdd );
			const QSignalBlocker blocker3( _sliderSphereRadiusFixed );
			const QSignalBlocker blocker4( _groupboxCylinder );
			const QSignalBlocker blocker5( _sliderCylinderRadius );
			const QSignalBlocker blocker6( _checkBoxCylinderColorBlending );
			const QSignalBlocker blocker7( _groupboxRibbon );
			const QSignalBlocker blocker8( _checkBoxRibbonColorBlending );
			const QSignalBlocker blocker9( _groupboxSes );

			_groupboxSphere->setChecked( _preset->getData().hasSphere );
			_comboBoxSphereRadiusType->setCurrentIndex( _preset->getData().radiusFixed ? 1 : 0 );
			_sliderSphereRadiusAdd->setValue( _preset->getData().radiusSphereAdd );
			_sliderSphereRadiusFixed->setValue( _preset->getData().radiusSphereFixed );
			_groupboxCylinder->setChecked( _preset->getData().hasCylinder );
			_sliderCylinderRadius->setValue( _preset->getData().radiusCylinder );
			_checkBoxCylinderColorBlending->setChecked( _preset->getData().cylinderColorBlending );
			_groupboxRibbon->setChecked( _preset->getData().hasRibbon );
			_checkBoxRibbonColorBlending->setChecked( _preset->getData().ribbonColorBlending );
			_groupboxSes->setChecked( _preset->getData().hasSes );
		}

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

		/*
		Core::Widget::HideableGroupBox * const _createGroupBoxSphere()
		{
			p_component->callback<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED, bool>() +=
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
			p_component->callback<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, float>() +=
				[ sliderRadiusAdd ]( const float p_value ) { sliderRadiusAdd->setValue( p_value * 100 ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, float>() +=
				[ sliderRadiusFixed ]( const float p_value ) { sliderRadiusFixed->setValue( p_value * 100 ); };

			return groupBox;
		}


		Core::Widget::HideableGroupBox * const _createGroupBoxCylinder()
		{
			p_component->callback<E_REPRESENTATION_SETTINGS::HAS_CYLINDER, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, float>() +=
				[ slider ]( const float p_value ) { slider->setValue( p_value * 100 ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, bool>() +=
				[ checkBoxColorBlending ]( const bool p_value ) { checkBoxColorBlending->setChecked( p_value ); };

			return groupBox;
		}

		Core::Widget::HideableGroupBox * const _createGroupBoxRibbon()
		{
			p_component->callback<E_REPRESENTATION_SETTINGS::HAS_RIBBON, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, bool>() +=
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
									 ChangeRepresentation<E_REPRESENTATION_SETTINGS::SES_PROBE_RADIUS, float>>(
							this->_preset, static_cast<float>( p_value ) / 100.f
						);
				}
			);

			// Callbacks.
			p_component->callback<E_REPRESENTATION_SETTINGS::HAS_SES, bool>() +=
				[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
			p_component->callback<E_REPRESENTATION_SETTINGS::SES_PROBE_RADIUS, float>() +=
				[ slider ]( const float p_value ) { slider->setValue( p_value * 100 ); };
			return groupBox;

			return groupBox;
		}

		*/
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
