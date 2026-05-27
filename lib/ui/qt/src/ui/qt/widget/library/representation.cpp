#include "ui/qt/widget/library/representation.hpp"
#include <QLabel>

namespace VTX::UI::QT::Widget::Library
{

	Representation::Representation( QWidget * p_parent ) : BasePresetWidget( p_parent )
	{
		using namespace Renderer;

		setTitle( "Edit representation" );

		// Sphere.
		_groupboxSphere = new HideableGroupBox( "Atoms", presetGroupBox() );
		addWidget( _groupboxSphere );

		_comboBoxSphereRadiusType = new QComboBox( _groupboxSphere );
		_groupboxSphere->addWidget( _comboBoxSphereRadiusType );
		_comboBoxSphereRadiusType->addItem( "Van der Waals radius" );
		_comboBoxSphereRadiusType->addItem( "Fixed radius" );

		_sliderSphereRadiusAdd = new EditableSlider( Qt::Orientation::Horizontal, _groupboxSphere );
		_labelSphereRadiusAdd  = new QLabel( "Radius add", _groupboxSphere );
		_groupboxSphere->addWidget( _labelSphereRadiusAdd );
		_groupboxSphere->addWidget( _sliderSphereRadiusAdd );
		_sliderSphereRadiusAdd->setMinimum( RADIUS_SPHERE_ADD_MIN );
		_sliderSphereRadiusAdd->setMaximum( RADIUS_SPHERE_ADD_MAX );
		_sliderSphereRadiusAdd->setSuffix( QStringLiteral( u"\u00C5" ) );

		_sliderSphereRadiusFixed = new EditableSlider( Qt::Orientation::Horizontal, _groupboxSphere );
		_labelSphereRadiusFixed	 = new QLabel( "Radius", _groupboxSphere );
		_groupboxSphere->addWidget( _labelSphereRadiusFixed );
		_groupboxSphere->addWidget( _sliderSphereRadiusFixed );
		_sliderSphereRadiusFixed->setMinimum( RADIUS_SPHERE_FIXED_MIN );
		_sliderSphereRadiusFixed->setMaximum( RADIUS_SPHERE_FIXED_MAX );
		_sliderSphereRadiusFixed->setSuffix( QStringLiteral( u"\u00C5" ) );

		// Cylinder.
		_groupboxCylinder = new HideableGroupBox( "Bonds", presetGroupBox() );
		addWidget( _groupboxCylinder );

		_sliderCylinderRadius = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCylinder );
		_groupboxCylinder->addWidget( new QLabel( "Radius", _groupboxCylinder ) );
		_groupboxCylinder->addWidget( _sliderCylinderRadius );
		_sliderCylinderRadius->setMinimum( RADIUS_CYLINDER_MIN );
		_sliderCylinderRadius->setMaximum( RADIUS_CYLINDER_MAX );
		_sliderCylinderRadius->setSuffix( QStringLiteral( u"\u00C5" ) );

		_checkBoxCylinderColorBlending = new QCheckBox( "Blend colors", _groupboxCylinder );
		_groupboxCylinder->addWidget( _checkBoxCylinderColorBlending );

		// Ribbon.
		_groupboxRibbon = new HideableGroupBox( "Ribbons", presetGroupBox() );
		addWidget( _groupboxRibbon );

		_checkBoxRibbonColorBlending = new QCheckBox( "Blend colors", _groupboxRibbon );
		_groupboxRibbon->addWidget( _checkBoxRibbonColorBlending );

		// SES.
		_groupboxSes = new HideableGroupBox( "SES", presetGroupBox() );
		addWidget( _groupboxSes );

		_comboBoxSesComputeMode = new QComboBox( _groupboxSes );
		_groupboxSes->addWidget( new QLabel( "Compute mode", _groupboxSes ) );
		_groupboxSes->addWidget( _comboBoxSesComputeMode );
		_comboBoxSesComputeMode->addItem( "System" );
		_comboBoxSesComputeMode->addItem( "Category" );

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
			[ this ]( const bool p_checked ) { _changeValue<E_REPRESENTATION_VALUES::HAS_SPHERE, bool>( p_checked ); }
		);

		connect(
			_comboBoxSphereRadiusType,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED, bool>( bool( p_index ) ); }
		);

		connect(
			_sliderSphereRadiusAdd,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD, float>( p_value ); }
		);

		connect(
			_sliderSphereRadiusFixed,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED, float>( p_value ); }
		);

		connect(
			_groupboxCylinder,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_checked ) { _changeValue<E_REPRESENTATION_VALUES::HAS_CYLINDER, bool>( p_checked ); }
		);

		connect(
			_sliderCylinderRadius,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_REPRESENTATION_VALUES::RADIUS_CYLINDER, float>( p_value ); }
		);

		connect(
			_checkBoxCylinderColorBlending,
			&QCheckBox::toggled,
			[ this ]( const bool p_checked )
			{ _changeValue<E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING, bool>( p_checked ); }
		);

		connect(
			_groupboxRibbon,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_checked ) { _changeValue<E_REPRESENTATION_VALUES::HAS_RIBBON, bool>( p_checked ); }
		);

		connect(
			_checkBoxRibbonColorBlending,
			&QCheckBox::toggled,
			[ this ]( const bool p_checked )
			{ _changeValue<E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING, bool>( p_checked ); }
		);

		connect(
			_groupboxSes,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_checked ) { _changeValue<E_REPRESENTATION_VALUES::HAS_SES, bool>( p_checked ); }
		);

		connect(
			_comboBoxSesComputeMode,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{
				_changeValue<E_REPRESENTATION_VALUES::SES_COMPUTE_MODE, E_SES_COMPUTE_MODE>(
					p_index == 0 ? E_SES_COMPUTE_MODE::SYSTEM : E_SES_COMPUTE_MODE::CATEGORY
				);
			}
		);

		connect(
			_sliderSesProbeRadius,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float>( p_value ); }
		);
	}

	void Representation::_update( Entity p_e )
	{
		auto & preset = App::REG().get<Renderer::Representation>( p_e );

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
		const QSignalBlocker blocker10( _comboBoxSesComputeMode );
		const QSignalBlocker blocker11( _sliderSesProbeRadius );

		_groupboxSphere->setChecked( preset.hasSphere );
		_comboBoxSphereRadiusType->setCurrentIndex( preset.isRadiusSphereFixed ? 1 : 0 );
		_sliderSphereRadiusAdd->setValue( preset.radiusSphereAdd );
		_sliderSphereRadiusFixed->setValue( preset.radiusSphereFixed );
		_groupboxCylinder->setChecked( preset.hasCylinder );
		_sliderCylinderRadius->setValue( preset.radiusCylinder );
		_checkBoxCylinderColorBlending->setChecked( preset.cylinderColorBlending );
		_groupboxRibbon->setChecked( preset.hasRibbon );
		_checkBoxRibbonColorBlending->setChecked( preset.ribbonColorBlending );
		_groupboxSes->setChecked( preset.hasSes );
		_comboBoxSesComputeMode->setCurrentIndex(
			preset.sesComputeMode == Renderer::E_SES_COMPUTE_MODE::SYSTEM ? 0 : 1
		);
		_sliderSesProbeRadius->setValue( preset.sesProbeRadius );

		_applyLogic( preset );
	}

	void Representation::_applyLogic( const Renderer::Representation & p_preset )
	{
		// Sphere.
		const bool isFixed = p_preset.isRadiusSphereFixed;
		_labelSphereRadiusAdd->setVisible( not isFixed );
		_sliderSphereRadiusAdd->setVisible( not isFixed );
		_labelSphereRadiusFixed->setVisible( isFixed );
		_sliderSphereRadiusFixed->setVisible( isFixed );
	}

} // namespace VTX::UI::QT::Widget::Library
