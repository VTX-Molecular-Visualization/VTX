#include "ui/qt/widget/controller.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/expandable_tool_bar.hpp"
#include <QLabel>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <app/action/action_manager.hpp>
#include <app/action/controller.hpp>
#include <app/services.hpp>
#include <app/setting/controller.hpp>

namespace VTX::UI::QT::Widget
{
	Controller::Controller( const Entity p_entity, QWidget * const p_parent ) :
		QGroupBox( "Controller", p_parent ), _entity( p_entity )
	{
		auto * layout = Helper::makeQObject<QVBoxLayout>( this );

		auto * labelMode   = Helper::makeQObject<QLabel>( "Mode", this );
		auto * toolbarMode = Helper::makeQObject<ExpandableToolBar>( this );
		toolbarMode->addAction( Action::Camera::TRACKBALL );
		toolbarMode->addAction( Action::Camera::FREEFLY );
		layout->addWidget( labelMode );
		layout->addWidget( toolbarMode );

		_checkBoxInvertY = Helper::makeQObject<QCheckBox>( "Invert mouse Y", this );
		layout->addWidget( _checkBoxInvertY );

		auto * labelTranslationSpeed = Helper::makeQObject<QLabel>( "Translation speed", this );
		_sliderTranslationSpeed		 = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		_sliderTranslationSpeed->setMinimum( App::Setting::TRANSLATION_SPEED_MIN );
		_sliderTranslationSpeed->setMaximum( App::Setting::TRANSLATION_SPEED_MAX );
		_sliderTranslationSpeed->setStep( 1.f );
		_sliderTranslationSpeed->setDecimals( 0 );
		layout->addWidget( labelTranslationSpeed );
		layout->addWidget( _sliderTranslationSpeed );

		auto * labelRotationSpeed = Helper::makeQObject<QLabel>( "Rotation speed", this );
		_sliderRotationSpeed	  = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		_sliderRotationSpeed->setMinimum( App::Setting::ROTATION_SPEED_MIN );
		_sliderRotationSpeed->setMaximum( App::Setting::ROTATION_SPEED_MAX );
		_sliderRotationSpeed->setStep( 0.1f );
		_sliderRotationSpeed->setDecimals( 1 );
		layout->addWidget( labelRotationSpeed );
		layout->addWidget( _sliderRotationSpeed );

		auto * labelAccelerationFactor = Helper::makeQObject<QLabel>( "Acceleration factor", this );
		_sliderAccelerationFactor	   = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		_sliderAccelerationFactor->setMinimum( App::Setting::ACCELERATION_FACTOR_MIN );
		_sliderAccelerationFactor->setMaximum( App::Setting::ACCELERATION_FACTOR_MAX );
		_sliderAccelerationFactor->setStep( 0.1f );
		_sliderAccelerationFactor->setDecimals( 1 );
		layout->addWidget( labelAccelerationFactor );
		layout->addWidget( _sliderAccelerationFactor );

		auto * labelDecelerationFactor = Helper::makeQObject<QLabel>( "Deceleration factor", this );
		_sliderDecelerationFactor	   = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		_sliderDecelerationFactor->setMinimum( App::Setting::DECELERATION_FACTOR_MIN );
		_sliderDecelerationFactor->setMaximum( App::Setting::DECELERATION_FACTOR_MAX );
		_sliderDecelerationFactor->setStep( 0.1f );
		_sliderDecelerationFactor->setDecimals( 1 );
		layout->addWidget( labelDecelerationFactor );
		layout->addWidget( _sliderDecelerationFactor );

		_checkBoxElasticityActive = Helper::makeQObject<QCheckBox>( "Elasticity", this );
		layout->addWidget( _checkBoxElasticityActive );

		_labelElasticityFactor	= Helper::makeQObject<QLabel>( "Elasticity factor", this );
		_sliderElasticityFactor = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		_sliderElasticityFactor->setMinimum( App::Setting::ELASTICITY_FACTOR_MIN );
		_sliderElasticityFactor->setMaximum( App::Setting::ELASTICITY_FACTOR_MAX );
		_sliderElasticityFactor->setStep( 0.1f );
		_sliderElasticityFactor->setDecimals( 1 );
		layout->addWidget( _labelElasticityFactor );
		layout->addWidget( _sliderElasticityFactor );

		connect( _checkBoxInvertY,
				 &QCheckBox::toggled,
				 []( const bool p_value ) { App::ACTION().execute<App::Action::Controller::SetInvertY>( p_value ); } );
		connect( _sliderTranslationSpeed,
				 &EditableSlider::valueChanged,
				 []( const float p_value )
				 { App::ACTION().execute<App::Action::Controller::SetTranslationSpeed>( p_value ); } );
		connect( _sliderRotationSpeed,
				 &EditableSlider::valueChanged,
				 []( const float p_value )
				 { App::ACTION().execute<App::Action::Controller::SetRotationSpeed>( p_value ); } );
		connect( _sliderAccelerationFactor,
				 &EditableSlider::valueChanged,
				 []( const float p_value )
				 { App::ACTION().execute<App::Action::Controller::SetAccelerationFactor>( p_value ); } );
		connect( _sliderDecelerationFactor,
				 &EditableSlider::valueChanged,
				 []( const float p_value )
				 { App::ACTION().execute<App::Action::Controller::SetDecelerationFactor>( p_value ); } );
		connect( _checkBoxElasticityActive,
				 &QCheckBox::toggled,
				 []( const bool p_value )
				 { App::ACTION().execute<App::Action::Controller::SetElasticityActive>( p_value ); } );
		connect( _sliderElasticityFactor,
				 &EditableSlider::valueChanged,
				 []( const float p_value )
				 { App::ACTION().execute<App::Action::Controller::SetElasticityFactor>( p_value ); } );

		_controllerUpdatedConnection
			= App::REG().on_update<App::Setting::Controller>().connect<&Controller::_setController>( this );

		_setController();
	}

	void Controller::_setController()
	{
		const QSignalBlocker blockerInvertY( _checkBoxInvertY );
		const QSignalBlocker blockerElasticityActive( _checkBoxElasticityActive );
		const QSignalBlocker blockerTranslationSpeed( _sliderTranslationSpeed );
		const QSignalBlocker blockerRotationSpeed( _sliderRotationSpeed );
		const QSignalBlocker blockerAccelerationFactor( _sliderAccelerationFactor );
		const QSignalBlocker blockerDecelerationFactor( _sliderDecelerationFactor );
		const QSignalBlocker blockerElasticityFactor( _sliderElasticityFactor );

		const auto & controller = App::REG().get<App::Setting::Controller>( _entity );
		_checkBoxInvertY->setChecked( controller.invertY );
		_checkBoxElasticityActive->setChecked( controller.elasticityActive );
		_sliderTranslationSpeed->setValue( controller.translationSpeed );
		_sliderRotationSpeed->setValue( controller.rotationSpeed );
		_sliderAccelerationFactor->setValue( controller.accelerationFactor );
		_sliderDecelerationFactor->setValue( controller.decelerationFactor );
		_sliderElasticityFactor->setValue( controller.elasticityFactor );

		const bool displayElasticity = controller.current == App::Setting::E_CONTROLLER::TRACKBALL;
		_checkBoxElasticityActive->setVisible( displayElasticity );
		_labelElasticityFactor->setVisible( displayElasticity );
		_sliderElasticityFactor->setVisible( displayElasticity );
	}
} // namespace VTX::UI::QT::Widget
