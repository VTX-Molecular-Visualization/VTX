#include "ui/qt/dock_widget/representations.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <app/action/representation.hpp>
#include <app/application/scene.hpp>
#include <app/component/representation/representation.hpp>
#include <core/chemdb/atom.hpp>

namespace VTX::UI::QT::DockWidget
{

	Representations::Representations( QWidget * p_parent ) :
		BaseDockWidget<Representations>( "Representations", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		const auto & scene	   = App::SCENE();
		auto *		 component = &App::ECS_REGISTRY().getComponent<App::Component::Representation::Representation>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );

		_gbCylinder = _createGroupBoxCylinder( component );
		_gbRibbon	= _createGroupBoxRibbon( component );
		_gbSphere	= _createGroupBoxSphere( component ); // Last because need others in the callback.

		_layout->addWidget( _gbSphere );
		_layout->addWidget( _gbCylinder );
		_layout->addWidget( _gbRibbon );

		// _gbSES = _createGroupBoxSES( component );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}

	QGroupBox * const Representations::_createGroupBoxSphere(
		App::Component::Representation::Representation * const p_component
	)
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new QGroupBox( "Atoms" );
		auto * layout	= new QVBoxLayout( groupBox );

		// Active.
		groupBox->setCheckable( true );
		groupBox->setChecked( p_component->getRepresentation().hasSphere );

		connect(
			groupBox,
			&QGroupBox::toggled,
			[]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::Representation::ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>>(
						p_checked
					);
			}
		);

		// Fixed.
		auto * comboBox = new QComboBox( groupBox );

		layout->addWidget( comboBox );
		comboBox->addItem( "Van der Waals radius" );
		comboBox->addItem( "Fixed radius" );
		comboBox->setCurrentIndex( int( p_component->getRepresentation().radiusFixed ) );

		connect(
			comboBox,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{
				bool isFixed = bool( p_index );
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::
								 ChangeRepresentation<E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED, bool>>( isFixed
					);

				_gbCylinder->setVisible( isFixed );
				_gbRibbon->setVisible( isFixed );
			}
		);

		// Radius add (VdW only).
		auto * labelRadiusAdd  = new QLabel( "Radius add", groupBox );
		auto * sliderRadiusAdd = new QSlider( Qt::Orientation::Horizontal, groupBox );
		layout->addWidget( labelRadiusAdd );
		layout->addWidget( sliderRadiusAdd );
		// TODO: move min and max to representation.
		// sliderRadiusAdd->setMinimum( -( VTX::Core::ChemDB::Atom::VDW_RADIUS_MIN ) * 100 + 1 );
		sliderRadiusAdd->setMinimum( 0 );
		sliderRadiusAdd->setMaximum( 300 );
		sliderRadiusAdd->setValue( p_component->getRepresentation().radiusSphereAdd * 100 );
		connect(
			sliderRadiusAdd,
			&QSlider::valueChanged,
			[]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::
								 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, float>>(
						float( p_value ) / 100.f
					);
			}
		);

		// Radius fixed.
		auto * labelRadiusFixed	 = new QLabel( "Radius", groupBox );
		auto * sliderRadiusFixed = new QSlider( Qt::Orientation::Horizontal, groupBox );
		layout->addWidget( labelRadiusFixed );
		layout->addWidget( sliderRadiusFixed );
		sliderRadiusFixed->setMinimum( 1 );
		sliderRadiusFixed->setMaximum( 300 );
		sliderRadiusFixed->setValue( p_component->getRepresentation().radiusSphereFixed * 100 );
		connect(
			sliderRadiusFixed,
			&QSlider::valueChanged,
			[]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::
								 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, float>>(
						float( p_value ) / 100.f
					);
			}
		);

		// Callbacks.
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

		// Emit init.
		emit comboBox->currentIndexChanged( comboBox->currentIndex() );

		return groupBox;
	}

	QGroupBox * const Representations::_createGroupBoxCylinder(
		App::Component::Representation::Representation * const p_component
	)
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new QGroupBox( "Bonds" );
		auto * layout	= new QVBoxLayout( groupBox );

		// Active.
		groupBox->setCheckable( true );
		groupBox->setChecked( p_component->getRepresentation().hasCylinder );

		connect(
			groupBox,
			&QGroupBox::toggled,
			[]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::
								 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_CYLINDER, bool>>( p_checked );
			}
		);

		// Radius.
		auto * labelRadius = new QLabel( "Radius", groupBox );
		auto * slider	   = new QSlider( Qt::Orientation::Horizontal, groupBox );
		layout->addWidget( labelRadius );
		layout->addWidget( slider );
		slider->setMinimum( 1 );
		slider->setMaximum( 100 );
		slider->setValue( p_component->getRepresentation().radiusCylinder * 100 );
		connect(
			slider,
			&QSlider::valueChanged,
			[]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::
								 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Color blending.
		auto * checkBoxColorBlending = new QCheckBox( "Blend colors", groupBox );
		layout->addWidget( checkBoxColorBlending );
		checkBoxColorBlending->setChecked( p_component->getRepresentation().cylinderColorBlending );
		connect(
			checkBoxColorBlending,
			&QCheckBox::toggled,
			[]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::
								 ChangeRepresentation<E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, bool>>(
						p_checked
					);
			}
		);

		// Callbacks.
		p_component->callback<E_REPRESENTATION_SETTINGS::HAS_CYLINDER, bool>() +=
			[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
		p_component->callback<E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, float>() +=
			[ slider ]( const float p_value ) { slider->setValue( p_value * 100 ); };
		p_component->callback<E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, bool>() +=
			[ checkBoxColorBlending ]( const bool p_value ) { checkBoxColorBlending->setChecked( p_value ); };

		// Emit init.
		emit groupBox->toggled( groupBox->isChecked() );

		return groupBox;
	}

	QGroupBox * const Representations::_createGroupBoxRibbon(
		App::Component::Representation::Representation * const p_component
	)
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new QGroupBox( "Ribbons" );
		auto * layout	= new QVBoxLayout( groupBox );

		// Active.
		groupBox->setCheckable( true );
		groupBox->setChecked( p_component->getRepresentation().hasRibbon );

		connect(
			groupBox,
			&QGroupBox::toggled,
			[]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::Representation::ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_RIBBON, bool>>(
						p_checked
					);
			}
		);

		// Color blending.
		auto * checkBoxColorBlending = new QCheckBox( "Blend colors", groupBox );
		layout->addWidget( checkBoxColorBlending );
		checkBoxColorBlending->setChecked( p_component->getRepresentation().ribbonColorBlending );
		connect(
			checkBoxColorBlending,
			&QCheckBox::toggled,
			[]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::
								 ChangeRepresentation<E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, bool>>(
						p_checked
					);
			}
		);

		// Callbacks.
		p_component->callback<E_REPRESENTATION_SETTINGS::HAS_RIBBON, bool>() +=
			[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
		p_component->callback<E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, bool>() +=
			[ checkBoxColorBlending ]( const bool p_value ) { checkBoxColorBlending->setChecked( p_value ); };

		return groupBox;
	}

	QGroupBox * const Representations::_createGroupBoxSES(
		App::Component::Representation::Representation * const p_component
	)
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new QGroupBox( "SES" );
		auto * layout	= new QVBoxLayout( groupBox );

		// Active.
		groupBox->setCheckable( true );
		groupBox->setChecked( p_component->getRepresentation().hasSes );

		connect(
			groupBox,
			&QGroupBox::toggled,
			[]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::Representation::ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SES, bool>>(
						p_checked
					);
			}
		);

		// Callbacks.
		p_component->callback<E_REPRESENTATION_SETTINGS::HAS_SES, bool>() +=
			[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };

		return groupBox;
	}

} // namespace VTX::UI::QT::DockWidget
