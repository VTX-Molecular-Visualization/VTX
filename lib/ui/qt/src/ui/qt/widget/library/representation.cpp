#include "ui/qt/widget/library/representation.hpp"
#include <QLabel>
#include <app/action/representation.hpp>

namespace VTX::UI::QT::Widget::Library
{

	Representation::Representation( QWidget * p_parent ) : BasePresetWidget( p_parent )
	{
		using namespace VTX::Core::Struct;
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
		_sliderSphereRadiusAdd->setMinimum( RADIUS_SPHERE_ADD_MIN );
		_sliderSphereRadiusAdd->setMaximum( RADIUS_SPHERE_ADD_MAX );
		_sliderSphereRadiusAdd->setSuffix( QStringLiteral( u"\u00C5" ) );

		_sliderSphereRadiusFixed = new EditableSlider( Qt::Orientation::Horizontal, _groupboxSphere );
		_groupboxSphere->addWidget( new QLabel( "Radius", _groupboxSphere ) );
		_groupboxSphere->addWidget( _sliderSphereRadiusFixed );
		_sliderSphereRadiusFixed->setMinimum( RADIUS_SPHERE_FIXED_MIN );
		_sliderSphereRadiusFixed->setMaximum( RADIUS_SPHERE_FIXED_MAX );
		_sliderSphereRadiusFixed->setSuffix( QStringLiteral( u"\u00C5" ) );

		// Cylinder.
		_groupboxCylinder = new HideableGroupBox( "Bonds", _groupboxPreset );
		_groupboxPreset->layout()->addWidget( _groupboxCylinder );

		_sliderCylinderRadius = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCylinder );
		_groupboxCylinder->addWidget( new QLabel( "Radius", _groupboxCylinder ) );
		_groupboxCylinder->addWidget( _sliderCylinderRadius );
		_sliderCylinderRadius->setMinimum( RADIUS_CYLINDER_MIN );
		_sliderCylinderRadius->setMaximum( RADIUS_CYLINDER_MAX );
		_sliderCylinderRadius->setSuffix( QStringLiteral( u"\u00C5" ) );

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

		_sliderSesProbeRadius = new EditableSlider( Qt::Orientation::Horizontal, _groupboxSes );
		_groupboxSes->addWidget( new QLabel( "Probe radius", _groupboxSes ) );
		_groupboxSes->addWidget( _sliderSesProbeRadius );
		_sliderSesProbeRadius->setMinimum( SES_PROBE_RADIUS_MIN );
		_sliderSesProbeRadius->setMaximum( SES_PROBE_RADIUS_MAX );
		_sliderSesProbeRadius->setSuffix( QStringLiteral( u"\u00C5" ) );

		// Connect widget callbacks.
		connect(
			_groupboxSphere,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::HAS_SPHERE, bool>>(
						_preset, p_checked
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
					.execute<
						App::Action::Representation::Change<E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED, bool>>(
						_preset, isFixed
					);
			}
		);

		connect(
			_sliderSphereRadiusAdd,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD, float>>(
						_preset, p_value
					);
			}
		);

		connect(
			_sliderSphereRadiusFixed,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED, float>>(
						_preset, p_value
					);
			}
		);

		connect(
			_groupboxCylinder,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::HAS_CYLINDER, bool>>(
						_preset, p_checked
					);
			}
		);

		connect(
			_sliderCylinderRadius,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::RADIUS_CYLINDER, float>>(
						_preset, p_value
					);
			}
		);

		connect(
			_checkBoxCylinderColorBlending,
			&QCheckBox::toggled,
			[ this ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::Representation::Change<E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING, bool>>(
						_preset, p_checked
					);
			}
		);

		connect(
			_groupboxRibbon,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::HAS_RIBBON, bool>>(
						_preset, p_checked
					);
			}
		);

		connect(
			_checkBoxRibbonColorBlending,
			&QCheckBox::toggled,
			[ this ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING, bool>>(
						_preset, p_checked
					);
			}
		);

		connect(
			_groupboxSes,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::HAS_SES, bool>>(
						_preset, p_checked
					);
			}
		);

		connect(
			_sliderSesProbeRadius,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::Change<E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float>>(
						_preset, p_value
					);
			}
		);
	}

	void Representation::_onPresetAdded( const std::string_view p_name )
	{
		using namespace VTX::Core::Struct;
		auto * const preset = _library->getPreset( p_name );

		// Connect app to widgets.
		preset->getCallback<E_REPRESENTATION_VALUES::HAS_SPHERE, bool>() += [ this, preset ]( const bool p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _groupboxSphere );
			_groupboxSphere->setChecked( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED, bool>() +=
			[ this, preset ]( const bool p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _comboBoxSphereRadiusType );
			_comboBoxSphereRadiusType->setCurrentIndex( int( p_value ) );
			_applyLogic();
		};
		preset->getCallback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD, float>() +=
			[ this, preset ]( const float p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _sliderSphereRadiusAdd );
			_sliderSphereRadiusAdd->setValue( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED, float>() +=
			[ this, preset ]( const float p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _sliderSphereRadiusFixed );
			_sliderSphereRadiusFixed->setValue( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::HAS_CYLINDER, bool>() += [ this, preset ]( const bool p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _groupboxCylinder );
			_groupboxCylinder->setChecked( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::RADIUS_CYLINDER, float>() +=
			[ this, preset ]( const float p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _sliderCylinderRadius );
			_sliderCylinderRadius->setValue( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING, bool>() +=
			[ this, preset ]( const bool p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _checkBoxCylinderColorBlending );
			_checkBoxCylinderColorBlending->setChecked( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::HAS_RIBBON, bool>() += [ this, preset ]( const bool p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _groupboxRibbon );
			_groupboxRibbon->setChecked( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING, bool>() +=
			[ this, preset ]( const bool p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _checkBoxRibbonColorBlending );
			_checkBoxRibbonColorBlending->setChecked( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::HAS_SES, bool>() += [ this, preset ]( const bool p_value )
		{
			if ( _preset != preset )
				return;
			const QSignalBlocker blocker( _groupboxSes );
			_groupboxSes->setChecked( p_value );
		};
		preset->getCallback<E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float>() +=
			[ this, preset ]( const float p_value )
		{
			if ( _preset != preset )
				return;

			_sliderSesProbeRadius->setValue( p_value );
		};
	}

	void Representation::_onPresetChanged()
	{
		assert( _preset != nullptr );

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
		const QSignalBlocker blocker10( _sliderSesProbeRadius );

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
		_sliderSesProbeRadius->setValue( _preset->getData().sesProbeRadius );

		_applyLogic();
	}

	void Representation::_applyLogic()
	{
		// Sphere.
		const bool isFixed = _comboBoxSphereRadiusType->currentIndex() == 1;
		_sliderSphereRadiusAdd->setVisible( not isFixed );
		_sliderSphereRadiusFixed->setVisible( isFixed );
	}

} // namespace VTX::UI::QT::Widget::Library
