#include "ui/qt/dock_widget/representations.hpp"
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <app/action/representation.hpp>
#include <app/application/scene.hpp>
#include <app/component/representation/representation.hpp>

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

		_createGroupBoxSphere( component );
		_createGroupBoxCylinder( component );
		_createGroupBoxRibbon( component );
		_createGroupBoxSES( component );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}

	void Representations::_createGroupBoxSphere( App::Component::Representation::Representation * const p_component )
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new QGroupBox( "Sphere" );
		auto * layout	= new QVBoxLayout( groupBox );
		_layout->addWidget( groupBox );

		// Active.
		groupBox->setCheckable( true );
		groupBox->setChecked( p_component->getRepresentation().hasSphere );

		connect(
			groupBox,
			&QGroupBox::toggled,
			[ p_component ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::Representation::ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>>(
						p_checked
					);
			}
		);

		// Callbacks.
		p_component->callback<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>() +=
			[ groupBox ]( const bool p_value ) { groupBox->setChecked( p_value ); };
	}

	void Representations::_createGroupBoxCylinder( App::Component::Representation::Representation * const p_component )
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new QGroupBox( "Cylinder" );
		auto * layout	= new QVBoxLayout( groupBox );
		_layout->addWidget( groupBox );

		// Active.
		groupBox->setCheckable( true );
		groupBox->setChecked( p_component->getRepresentation().hasCylinder );

		connect(
			groupBox,
			&QGroupBox::toggled,
			[ p_component ]( const bool p_checked )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::Representation::
								 ChangeRepresentation<E_REPRESENTATION_SETTINGS::HAS_CYLINDER, bool>>( p_checked );
			}
		);

		// Radius.
		auto * labelRadius = new QLabel( "Intensity", groupBox );
		auto * slider	   = new QSlider( Qt::Orientation::Horizontal, groupBox );
		layout->addWidget( labelRadius );
		layout->addWidget( slider );
		slider->setMinimum( 1 );
		slider->setMaximum( 100 );
		slider->setValue( p_component->getRepresentation().radiusCylinder * 100 );
		connect(
			slider,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
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
			[ p_component ]( const bool p_checked )
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
	}

	void Representations::_createGroupBoxRibbon( App::Component::Representation::Representation * const p_component )
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new QGroupBox( "Ribbon" );
		auto * layout	= new QVBoxLayout( groupBox );
		_layout->addWidget( groupBox );

		// Active.
		groupBox->setCheckable( true );
		groupBox->setChecked( p_component->getRepresentation().hasRibbon );

		connect(
			groupBox,
			&QGroupBox::toggled,
			[ p_component ]( const bool p_checked )
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
			[ p_component ]( const bool p_checked )
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
	}

	void Representations::_createGroupBoxSES( App::Component::Representation::Representation * const p_component )
	{
		using namespace Renderer::Proxy;

		auto * groupBox = new QGroupBox( "SES" );
		auto * layout	= new QVBoxLayout( groupBox );
		_layout->addWidget( groupBox );

		// Active.
		groupBox->setCheckable( true );
		groupBox->setChecked( p_component->getRepresentation().hasSes );

		connect(
			groupBox,
			&QGroupBox::toggled,
			[ p_component ]( const bool p_checked )
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
	}

} // namespace VTX::UI::QT::DockWidget
