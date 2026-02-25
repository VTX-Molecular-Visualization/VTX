
#include "ui/qt/widget/tree/trajectory_player.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <app/action/action_manager.hpp>
#include <app/action/trajectory.hpp>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	TrajectoryPlayer::TrajectoryPlayer( App::ECS::Entity p_system, QWidget * p_parent ) :
		QWidget( p_parent ), _system( p_system )
	{
		// Main vertical layout
		auto * mainLayout = new QVBoxLayout( this );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		mainLayout->setSpacing( 0 );

		// Control bar (horizontal)
		auto * controlBar = new QHBoxLayout();
		controlBar->setContentsMargins( 4, 2, 4, 2 );
		controlBar->setSpacing( 4 );

		// Play/Pause button
		_btnPlayPause = new QPushButton( this );
		_btnPlayPause->setFixedSize( 24, 24 );
		_btnPlayPause->setToolTip( tr( "Play/Pause" ) );
		controlBar->addWidget( _btnPlayPause );

		// Stop button
		_btnStop = new QPushButton( this );
		_btnStop->setFixedSize( 24, 24 );
		_btnStop->setToolTip( tr( "Stop" ) );
		_btnStop->setIcon( _icons[ 2 ] );
		controlBar->addWidget( _btnStop );

		// Slider
		_slider = new TrajectorySlider( Qt::Horizontal, this );
		_slider->setMinimum( 0 );
		_slider->setMaximum( 100 );
		_slider->setValue( 0 );
		_slider->setToolTip( tr( "Seek" ) );
		controlBar->addWidget( _slider, 1 ); // Stretch factor 1

		// Frame label
		_frameLabel = new QLabel( "0/0", this );
		_frameLabel->setMinimumWidth( 60 );
		_frameLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		controlBar->addWidget( _frameLabel );

		// Settings Button
		_btnSettings = new QPushButton( this );
		_btnSettings->setIcon( _icons[ 3 ] );
		_btnSettings->setFixedSize( 24, 24 );
		_btnSettings->setCheckable( true );
		_btnSettings->setToolTip( tr( "Settings" ) );
		controlBar->addWidget( _btnSettings );

		mainLayout->addLayout( controlBar );

		// Settings panel (initially hidden)
		_settings = new TrajectorySettings( _system, this );
		_settings->setVisible( false );
		mainLayout->addWidget( _settings );

		// Connect signals
		connect( _btnPlayPause, &QPushButton::clicked, this, &TrajectoryPlayer::_onPlayPauseClicked );
		connect( _btnStop, &QPushButton::clicked, this, &TrajectoryPlayer::_onStopClicked );
		connect( _slider, &QSlider::valueChanged, this, &TrajectoryPlayer::_onSliderValueChanged );
		connect( _btnSettings, &QPushButton::clicked, this, &TrajectoryPlayer::_onSettingsClicked );

		// Connect to trajectory updates
		App::REG().on_update<App::System::TrajectoryFullBuffer>().connect<&TrajectoryPlayer::_onTrajectoryUpdated>(
			this
		);

		// Initial state
		_refresh();
	}

	std::array<QIcon, 4> TrajectoryPlayer::_getIcons()
	{
		return {
			STYLE().iconFromCodepoint( Style::Icons::PLAY_SHALLOW ),
			STYLE().iconFromCodepoint( Style::Icons::PAUSE_SHALLOW ),
			STYLE().iconFromCodepoint( Style::Icons::STOP_SHALLOW ),
			STYLE().iconFromCodepoint( Style::Icons::SETTINGS ),
		};
	}

	void TrajectoryPlayer::_onPlayPauseClicked()
	{
		App::ACTION().execute<App::Action::Trajectory::ToggleStartPause>( _system );
		_updatePlayPauseIcon();
	}

	void TrajectoryPlayer::_onStopClicked()
	{
		App::ACTION().execute<App::Action::Trajectory::Stop>( _system );
		_refresh();
	}

	void TrajectoryPlayer::_onSliderValueChanged( int p_value )
	{
		// Ignore changes triggered by _refresh() to prevent feedback loops
		if ( _isRefreshing )
		{
			return;
		}

		App::System::GenericTrajectory * trajPtr = nullptr;
		App::System::get( _system, trajPtr );

		if ( trajPtr && trajPtr->trajectorySize > 0 )
		{
			App::ACTION().execute<App::Action::Trajectory::JumpTo>( _system, uint( p_value ) );
		}
	}

	void TrajectoryPlayer::_onTrajectoryUpdated( App::ECS::Registry &, App::ECS::Entity p_entity )
	{
		// Only refresh if this is our system
		if ( p_entity == _system )
		{
			_refresh();
		}
	}

	void TrajectoryPlayer::_refresh()
	{
		App::System::GenericTrajectory * trajPtr = nullptr;
		App::System::get( _system, trajPtr );

		if ( trajPtr == nullptr )
		{
			_slider->setMaximum( 0 );
			_frameLabel->setText( "0/0" );
			return;
		}

		uint totalFrames  = trajPtr->trajectorySize;
		uint currentFrame = trajPtr->currentFrameIndex;

		// Handle max values
		if ( totalFrames == std::numeric_limits<uint>::max() )
		{
			totalFrames = 0;
		}
		if ( currentFrame == std::numeric_limits<uint>::max() )
		{
			currentFrame = 0;
		}

		// Guard to prevent valueChanged from triggering JumpTo during programmatic updates
		_isRefreshing = true;
		_slider->setMaximum( totalFrames > 0 ? int( totalFrames - 1 ) : 0 );
		_slider->setValue( int( currentFrame ) );
		_isRefreshing = false;

		// Update loaded range overlay
		App::System::AvailableFrames availableFrames;
		App::System::get( _system, availableFrames );
		_slider->setLoadedRange( int( availableFrames.lowerBoundIndex ), int( availableFrames.higherBoundIndex ) );

		// Update frame label
		_frameLabel->setText( QString( "%1/%2" ).arg( currentFrame ).arg( totalFrames > 0 ? totalFrames - 1 : 0 ) );

		// Update play/pause icon
		_updatePlayPauseIcon();
	}

	void TrajectoryPlayer::_updatePlayPauseIcon()
	{
		App::System::GenericTrajectory * trajPtr = nullptr;
		App::System::get( _system, trajPtr );

		bool isPlaying = trajPtr && !trajPtr->paused;

		// Use unicode symbols for now (can be replaced with icons)
		_btnPlayPause->setIcon( isPlaying ? _icons[ 1 ] : _icons[ 0 ] );
	}
	void TrajectoryPlayer::_onSettingsClicked() { _settings->setVisible( _btnSettings->isChecked() ); }

} // namespace VTX::UI::QT::Widget::Tree
