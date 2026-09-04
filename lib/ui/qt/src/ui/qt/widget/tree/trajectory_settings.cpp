#include "ui/qt/widget/tree/trajectory_settings.hpp"
#include <QEvent>
#include <QFormLayout>
#include <QHBoxLayout>
#include <app/action/action_manager.hpp>
#include <app/action/trajectory.hpp>
#include <app/services.hpp>
#include <app/trajectory/player.hpp>
#include <core/struct/trajectory.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	TrajectorySettings::TrajectorySettings( Entity p_system, QWidget * p_parent ) :
		QWidget( p_parent ), _system( p_system )
	{
		auto * formLayout = new QFormLayout( this );
		formLayout->setContentsMargins( 8, 4, 8, 4 );
		formLayout->setSpacing( 4 );

		// Player mode combobox
		_playerModeCombo = new QComboBox( this );
		_playerModeCombo->addItem( tr( "Forward" ), int( App::Trajectory::PLAY_MODE::FORWARD ) );
		_playerModeCombo->addItem( tr( "Forward Loop" ), int( App::Trajectory::PLAY_MODE::FORWARD_LOOP ) );
		_playerModeCombo->addItem( tr( "Backward" ), int( App::Trajectory::PLAY_MODE::BACKWARD ) );
		_playerModeCombo->addItem( tr( "Backward Loop" ), int( App::Trajectory::PLAY_MODE::BACKWARD_LOOP ) );
		_playerModeCombo->addItem( tr( "Ping Pong" ), int( App::Trajectory::PLAY_MODE::PING_PONG ) );
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
		App::REG().on_update<App::Trajectory::Player>().connect<&TrajectorySettings::_onTrajectoryUpdated>( this );

		_refresh();
	}

	TrajectorySettings::~TrajectorySettings()
	{ App::REG().on_update<App::Trajectory::Player>().disconnect<&TrajectorySettings::_onTrajectoryUpdated>( this ); }

	void TrajectorySettings::_onPlayerModeChanged( int p_index )
	{
		if ( _isRefreshing )
		{
			return;
		}

		auto mode = static_cast<App::Trajectory::PLAY_MODE>( _playerModeCombo->itemData( p_index ).toInt() );
		App::ACTION().execute<App::Action::Trajectory::ChangePlayer>( _system, mode );
	}

	void TrajectorySettings::_onSpeedSliderChanged( int p_value )
	{
		if ( _isRefreshing )
		{
			return;
		}

		_isRefreshing = true;
		_speedSpinBox->setValue( double( p_value ) );
		_isRefreshing = false;

		App::ACTION().execute<App::Action::Trajectory::ChangeSpeed>( _system, float( p_value ) );
	}

	void TrajectorySettings::_onSpeedSpinBoxChanged( double p_value )
	{
		if ( _isRefreshing )
		{
			return;
		}

		_isRefreshing = true;
		_speedSlider->setValue( int( p_value ) );
		_isRefreshing = false;

		App::ACTION().execute<App::Action::Trajectory::ChangeSpeed>( _system, float( p_value ) );
	}

	void TrajectorySettings::_onFrameSpinBoxChanged( int p_value )
	{
		if ( _isRefreshing )
		{
			return;
		}

		App::ACTION().execute<App::Action::Trajectory::JumpTo>( _system, uint( p_value ) );
	}

	void TrajectorySettings::_onTrajectoryUpdated( Registry &, Entity p_entity )
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
		const App::Trajectory::Player & player = App::REG().get<App::Trajectory::Player>( _system );

		if ( not player.paused )
		{
			App::ACTION().execute<App::Action::Trajectory::ToggleStartPause>( _system );
		}
	}

	void TrajectorySettings::_refresh()
	{
		const App::Trajectory::Player & player = App::REG().get<App::Trajectory::Player>( _system );

		_isRefreshing = true;

		const uint totalFrames	= static_cast<uint>( App::REG().get<Core::Struct::Trajectory>( _system ).frameCount );
		const uint currentFrame = player.currentFrameIndex;

		_playerModeCombo->setCurrentIndex( _playerModeCombo->findData( int( player.playMode ) ) );

		// Speed
		_speedSlider->setValue( int( player.playingSpeed ) );
		_speedSpinBox->setValue( double( player.playingSpeed ) );

		// Frame
		_frameSpinBox->setMaximum( int( totalFrames - 1 ) );

		// Only update frame value if the spinbox doesn't have focus (user might be typing)
		if ( not _frameSpinBox->hasFocus() )
		{
			_frameSpinBox->setValue( int( currentFrame ) );
		}

		_isRefreshing = false;
	}

} // namespace VTX::UI::QT::Widget::Tree
