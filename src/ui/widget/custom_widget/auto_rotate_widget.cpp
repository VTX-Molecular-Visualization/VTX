#include "auto_rotate_widget.hpp"
#include "action/action_manager.hpp"
#include "action/transformable.hpp"
#include "setting.hpp"
#include "trajectory/trajectory_enum.hpp"
#include "ui/widget_factory.hpp"
#include "util/math.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace VTX::UI::Widget::CustomWidget
{
	AutoRotateWidget::AutoRotateWidget( QWidget * p_parent ) :
		BaseManualWidget<QWidget>( p_parent ), TMultiDataField<Generic::BaseAutoRotate>()
	{
	}

	void AutoRotateWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QGridLayout * const _mainLayout = new QGridLayout( this );

		QLabel * orientationLabel = new QLabel( this );
		orientationLabel->setText( "Orientation" );
		_orientationWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::Vector3Widget>( this, "trajectory_speed" );
		_orientationWidget->setMinMax( -Setting::AUTO_ROTATE_SPEED_MAX, Setting::AUTO_ROTATE_SPEED_MAX );
		_orientationWidget->setDragValueFactor( Setting::AUTO_ROTATE_SPEED_MAX / 100.f );

		QLabel * speedLabel = new QLabel( this );
		speedLabel->setText( "Speed" );
		_speedWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>( this, "trajectory_speed" );
		_speedWidget->setMinMax( Setting::AUTO_ROTATE_SPEED_MIN, Setting::AUTO_ROTATE_SPEED_MAX );

		_playButton = new QPushButton( this );
		_playButton->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );

		_mainLayout->addWidget( orientationLabel, 0, 0 );
		_mainLayout->addWidget( _orientationWidget, 0, 1 );
		_mainLayout->addWidget( speedLabel, 1, 0 );
		_mainLayout->addWidget( _speedWidget, 1, 1 );
		_mainLayout->addWidget( _playButton, 2, 0, 1, 2 );
	}
	void AutoRotateWidget::_setupSlots()
	{
		connect( _orientationWidget,
				 &CustomWidget::Vector3Widget::onValueChange,
				 this,
				 &AutoRotateWidget::_orientationChange );

		connect( _orientationWidget,
				 &CustomWidget::Vector3Widget::onValueDragged,
				 this,
				 &AutoRotateWidget::_orientationDragged );
		

		connect(
			_speedWidget, &CustomWidget::FloatFieldSliderWidget::onValueChange, this, &AutoRotateWidget::_speedChange );

		connect( _playButton, &QPushButton::clicked, this, &AutoRotateWidget::_togglePlayAction );
	}

	void AutoRotateWidget::resetState()
	{
		_targets.clear();

		_orientationWidget->resetState();
		_speedWidget->resetState();
		_playButton->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );
	}
	void AutoRotateWidget::updateWithNewValue( Generic::BaseAutoRotate & p_value )
	{
		_targets.emplace( &p_value );

		_orientationWidget->updateWithNewValue( p_value.getAutoRotationVector() );
		const float speed			  = p_value.getAutoRotationMagnitude();
		const bool	enableSpeedWidget = speed != 0.0f;
		_speedWidget->setEnabled( enableSpeedWidget );
		_speedWidget->updateWithNewValue( speed );

		if ( p_value.isAutoRotationPlaying() )
			_playButton->setIcon( QIcon( ":/sprite/trajectory_pause_icon.png" ) );
	}

	void AutoRotateWidget::_displayDifferentsDataFeedback() {}

	void AutoRotateWidget::localize() {}

	void AutoRotateWidget::_togglePlayAction()
	{
		// if one is playing => set to pause else play all
		bool setPlay = true;

		for ( const Generic::BaseAutoRotate * const target : _targets )
			setPlay = setPlay && !target->isAutoRotationPlaying();

		VTX_ACTION( new Action::Transformable::SetAutoRotationPlay( _targets, setPlay ) );
	}
	void AutoRotateWidget::_speedChange( const float p_speed ) const
	{
		bool dataChange = false;

		for ( const Generic::BaseAutoRotate * const target : _targets )
		{
			if ( target->getAutoRotationMagnitude() != p_speed )
			{
				dataChange = true;
				break;
			}
		}

		if ( dataChange )
			VTX_ACTION( new Action::Transformable::SetAutoRotationSpeed( _targets, p_speed ) );
	}

	void AutoRotateWidget::_orientationChange( const Vec3f & p_orientation ) const
	{
		bool dataChange = false;

		for ( const Generic::BaseAutoRotate * const target : _targets )
		{
			if ( target->getAutoRotationVector() != p_orientation )
			{
				dataChange = true;
				break;
			}
		}

		if ( dataChange )
			VTX_ACTION( new Action::Transformable::SetAutoRotationOrientation( _targets, p_orientation ) );
	}

	void AutoRotateWidget::_orientationDragged( const Vec3f & p_delta ) const
	{
		if ( p_delta == VEC3F_ZERO )
			return;
		
		VTX_ACTION( new Action::Transformable::AddToAutoRotationOrientation( _targets, p_delta ) );
	}

	

} // namespace VTX::UI::Widget::CustomWidget
