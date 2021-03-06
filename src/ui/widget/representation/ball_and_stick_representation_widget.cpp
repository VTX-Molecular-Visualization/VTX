#include "ball_and_stick_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include <QGridLayout>

namespace VTX::UI::Widget::Representation
{
	void BallAndStickRepresentationWidget::localize() {};
	void BallAndStickRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_ballLabel = new QLabel( this );
		_ballLabel->setText( "Balls radius" );
		_stickLabel = new QLabel( this );
		_stickLabel->setText( "Sticks radius" );

		_ballRadius = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			this, "ball_radius" );
		_ballRadius->setMinMax( Setting::ATOMS_RADIUS_MIN, Setting::ATOMS_RADIUS_MAX );
		_stickRadius = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			this, "stick_radius" );
		_stickRadius->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		QGridLayout * layout = new QGridLayout( this );
		layout->addWidget( _ballLabel, 0, 0 );
		layout->addWidget( _ballRadius, 0, 1 );
		layout->addWidget( _stickLabel, 1, 0 );
		layout->addWidget( _stickRadius, 1, 1 );
	};
	void BallAndStickRepresentationWidget::_setupSlots()
	{
		connect( _ballRadius,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &BallAndStickRepresentationWidget::_onBallRadiusChange );
		connect( _stickRadius,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &BallAndStickRepresentationWidget::_onStickRadiusChange );
	};

	void BallAndStickRepresentationWidget::refresh()
	{
		_ballRadius->setValue( _instantiatedRepresentation->getSphereData()._radiusFixed );
		_stickRadius->setValue( _instantiatedRepresentation->getCylinderData()._radius );
	}

	void BallAndStickRepresentationWidget::_onBallRadiusChange( const float p_newRadius )
	{
		if ( p_newRadius < _stickRadius->getValue() )
			_stickRadius->setValue( p_newRadius );

		VTX_ACTION( new Action::ChangeRepresentationSphereRadius( _instantiatedRepresentation, p_newRadius ) );
		emit onDataChange();
	}
	void BallAndStickRepresentationWidget::_onStickRadiusChange( const float p_newRadius )
	{
		if ( p_newRadius > _ballRadius->getValue() )
			_ballRadius->setValue( p_newRadius );

		VTX_ACTION( new Action::ChangeRepresentationCylinderRadius( _instantiatedRepresentation, p_newRadius ) );
		emit onDataChange();
	}

} // namespace VTX::UI::Widget::Representation
