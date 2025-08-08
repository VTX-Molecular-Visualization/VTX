#include "ui/qt/dock_widget/representations.hpp"
#include <QCheckBox>
#include <QLabel>
#include <app/action/representation.hpp>
#include <app/settings.hpp>

namespace VTX::UI::QT::DockWidget
{

	Representations::Representations( QWidget * p_parent ) :
		BaseDockWidget<Representations>( "Representations", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		_presetSelector = new Widget::PresetSelector<App::Library::Preset::Representation>( this );
		_layout->addWidget( _presetSelector );
		connect(
			_presetSelector,
			&Widget::PresetSelector<App::Library::Preset::Representation>::presetChanged,
			this,
			[]( const QString & p_name ) { VTX_INFO( "Preset changed: {}", p_name.toStdString() ); }
		);

		auto * groupBox = new QGroupBox( "Edit representation", this );
		auto * layout	= new QVBoxLayout( groupBox );

		_gbSphere	= _createGroupBoxSphere();
		_gbCylinder = _createGroupBoxCylinder();
		_gbRibbon	= _createGroupBoxRibbon();
		_gbSES		= _createGroupBoxSES();

		layout->addWidget( _gbSphere );
		layout->addWidget( _gbCylinder );
		layout->addWidget( _gbRibbon );
		layout->addWidget( _gbSES );

		_layout->addWidget( groupBox );

		layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}

	Core::Widget::HideableGroupBox * const Representations::_createGroupBoxSphere()
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new Core::Widget::HideableGroupBox( "Atoms", this );

		// Active.
		// groupBox->setChecked( p_component->getRepresentation().hasSphere );

		connect(
			groupBox,
			&Core::Widget::HideableGroupBox::toggled,
			[ this ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::Representation::ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>>(
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
		sliderRadiusAdd->setMinimum( 0 );
		sliderRadiusAdd->setMaximum( 300 );
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
		sliderRadiusFixed->setMinimum( 1 );
		sliderRadiusFixed->setMaximum( 300 );
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
			*/

		return groupBox;
	}

	Core::Widget::HideableGroupBox * const Representations::_createGroupBoxCylinder()
	{
		using namespace Renderer::Proxy;

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
		slider->setMinimum( 1 );
		slider->setMaximum( 100 );
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

	Core::Widget::HideableGroupBox * const Representations::_createGroupBoxRibbon()
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new Core::Widget::HideableGroupBox( "Ribbons", this );

		// Active.
		// groupBox->setChecked( p_component->getRepresentation().hasRibbon );

		connect(
			groupBox,
			&Core::Widget::HideableGroupBox::toggled,
			[ this ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::Representation::ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_RIBBON, bool>>(
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

	Core::Widget::HideableGroupBox * const Representations::_createGroupBoxSES()
	{
		using namespace Renderer::Proxy;
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
					.execute<
						App::Action::Representation::ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SES, bool>>(
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

} // namespace VTX::UI::QT::DockWidget
