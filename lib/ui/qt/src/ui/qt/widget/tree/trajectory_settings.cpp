#include "ui/qt/widget/tree/trajectory_settings.hpp"
#include <QEvent>
#include <QFormLayout>
#include <QHBoxLayout>
#include <app/action/action_manager.hpp>
#include <app/action/trajectory.hpp>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	TrajectorySettings::TrajectorySettings( App::ECS::Entity p_system, QWidget * p_parent ) :
		QWidget( p_parent ), _system( p_system )
	{
		auto * formLayout = new QFormLayout( this );
		formLayout->setContentsMargins( 8, 4, 8, 4 );
		formLayout->setSpacing( 4 );

		// Player mode combobox
		_playerModeCombo = new QComboBox( this );
		_playerModeCombo->addItem( tr( "Forward" ), int( App::System::TrajectoryPlayMode::forward ) );
		_playerModeCombo->addItem( tr( "Forward Loop" ), int( App::System::TrajectoryPlayMode::forwardLoop ) );
		_playerModeCombo->addItem( tr( "Backward" ), int( App::System::TrajectoryPlayMode::backward ) );
		_playerModeCombo->addItem( tr( "Backward Loop" ), int( App::System::TrajectoryPlayMode::backwardLoop ) );
		_playerModeCombo->addItem( tr( "Ping Pong" ), int( App::System::TrajectoryPlayMode::pingpong ) );
		formLayout->addRow( tr( "Mode" ), _playerModeCombo );

		// Speed control: slider + spinbox in a horizontal layout
		auto * speedLayout = new QHBoxLayout();
		speedLayout->setSpacing( 4 );

		_speedSlider = new QSlider( Qt::Horizontal, this );
		_speedSlider->setMinimum( 1 );
		_speedSlider->setMaximum( 200 );
		_speedSlider->setToolTip( tr( "Playback speed (ms between frames)" ) );
		speedLayout->addWidget( _speedSlider, 1 );

		_speedSpinBox = new QDoubleSpinBox( this );
		_speedSpinBox->setMinimum( 1.0 );
		_speedSpinBox->setMaximum( 200.0 );
		_speedSpinBox->setSingleStep( 1.0 );
		_speedSpinBox->setSuffix( " ms" );
		_speedSpinBox->setFixedWidth( 80 );
		speedLayout->addWidget( _speedSpinBox );

		formLayout->addRow( tr( "Speed" ), speedLayout );

		// Frame spinbox
		_frameSpinBox = new QSpinBox( this );
		_frameSpinBox->setMinimum( 0 );
		_frameSpinBox->setMaximum( 0 );
		_frameSpinBox->setValue( 0 );
		formLayout->addRow( tr( "Frame" ), _frameSpinBox );

		// Connect signals
		connect( _playerModeCombo, &QComboBox::currentIndexChanged, this, &TrajectorySettings::_onPlayerModeChanged );
		connect( _speedSlider, &QSlider::valueChanged, this, &TrajectorySettings::_onSpeedSliderChanged );
		connect( _speedSpinBox, &QDoubleSpinBox::valueChanged, this, &TrajectorySettings::_onSpeedSpinBoxChanged );
		connect( _frameSpinBox, &QSpinBox::valueChanged, this, &TrajectorySettings::_onFrameSpinBoxChanged );

		// Install event filter to detect focus on frame spinbox (focusing pauses the player)
		_frameSpinBox->installEventFilter( this );

		// Connect to trajectory updates
		App::REG().on_update<App::System::TrajectoryFullBuffer>().connect<&TrajectorySettings::_onTrajectoryUpdated>(
			this
		);

		_refresh();
	}

	void TrajectorySettings::_onPlayerModeChanged( int p_index )
	{
		if ( _isRefreshing )
			return;

		auto mode = static_cast<App::System::TrajectoryPlayMode>( _playerModeCombo->itemData( p_index ).toInt() );
		App::ACTION().execute<App::Action::Trajectory::ChangePlayer>( _system, mode );
	}

	void TrajectorySettings::_onSpeedSliderChanged( int p_value )
	{
		if ( _isRefreshing )
			return;

		_isRefreshing = true;
		_speedSpinBox->setValue( double( p_value ) );
		_isRefreshing = false;

		App::ACTION().execute<App::Action::Trajectory::ChangeSpeed>( _system, float( p_value ) );
	}

	void TrajectorySettings::_onSpeedSpinBoxChanged( double p_value )
	{
		if ( _isRefreshing )
			return;

		_isRefreshing = true;
		_speedSlider->setValue( int( p_value ) );
		_isRefreshing = false;

		App::ACTION().execute<App::Action::Trajectory::ChangeSpeed>( _system, float( p_value ) );
	}

	void TrajectorySettings::_onFrameSpinBoxChanged( int p_value )
	{
		if ( _isRefreshing )
			return;

		App::ACTION().execute<App::Action::Trajectory::JumpTo>( _system, uint( p_value ) );
	}

	void TrajectorySettings::_onTrajectoryUpdated( App::ECS::Registry &, App::ECS::Entity p_entity )
	{
		if ( p_entity == _system )
		{
			_refresh();
		}
	}

	bool TrajectorySettings::eventFilter( QObject * p_watched, QEvent * p_event )
	{
		if ( p_watched == _frameSpinBox && p_event->type() == QEvent::FocusIn )
		{
			_onFrameSpinBoxFocused();
		}
		return QWidget::eventFilter( p_watched, p_event );
	}

	void TrajectorySettings::_onFrameSpinBoxFocused()
	{
		App::System::GenericTrajectory * trajPtr = nullptr;
		App::System::get( _system, trajPtr );

		if ( trajPtr && !trajPtr->paused )
		{
			App::ACTION().execute<App::Action::Trajectory::ToggleStartPause>( _system );
		}
	}

	void TrajectorySettings::_refresh()
	{
		App::System::GenericTrajectory * trajPtr = nullptr;
		App::System::get( _system, trajPtr );

		if ( trajPtr == nullptr )
			return;

		_isRefreshing = true;

		uint totalFrames  = trajPtr->trajectorySize;
		uint currentFrame = trajPtr->currentFrameIndex;

		if ( totalFrames == std::numeric_limits<uint>::max() )
			totalFrames = 0;
		if ( currentFrame == std::numeric_limits<uint>::max() )
			currentFrame = 0;

		_playerModeCombo->setCurrentIndex( _playerModeCombo->findData( int( trajPtr->playMode ) ) );

		// Speed
		_speedSlider->setValue( int( trajPtr->playingSpeed ) );
		_speedSpinBox->setValue( double( trajPtr->playingSpeed ) );

		// Frame
		_frameSpinBox->setMaximum( totalFrames > 0 ? int( totalFrames - 1 ) : 0 );

		// Only update frame value if the spinbox doesn't have focus (user might be typing)
		if ( !_frameSpinBox->hasFocus() )
		{
			_frameSpinBox->setValue( int( currentFrame ) );
		}

		_isRefreshing = false;
	}

} // namespace VTX::UI::QT::Widget::Tree
