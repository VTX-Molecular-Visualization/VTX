#include "ui/qt/widget/trajectory_player.hpp"
#include <QHBoxLayout>
#include <app/action/action_manager.hpp>
#include <app/action/trajectory.hpp>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>

namespace VTX::UI::QT::Widget
{

	TrajectoryPlayer::TrajectoryPlayer( App::ECS::Entity p_system, QWidget * p_parent ) :
		QWidget( p_parent ), _system( p_system )
	{
		// Create layout
		auto * layout = new QHBoxLayout( this );
		layout->setContentsMargins( 4, 2, 4, 2 );
		layout->setSpacing( 4 );

		// Play/Pause button
		_btnPlayPause = new QPushButton( this );
		_btnPlayPause->setFixedSize( 24, 24 );
		_btnPlayPause->setToolTip( tr( "Play/Pause" ) );
		layout->addWidget( _btnPlayPause );

		// Stop button
		_btnStop = new QPushButton( this );
		_btnStop->setFixedSize( 24, 24 );
		_btnStop->setToolTip( tr( "Stop" ) );
		layout->addWidget( _btnStop );

		// Slider
		_slider = new QSlider( Qt::Horizontal, this );
		_slider->setMinimum( 0 );
		_slider->setMaximum( 100 );
		_slider->setValue( 0 );
		_slider->setToolTip( tr( "Seek" ) );
		layout->addWidget( _slider, 1 ); // Stretch factor 1

		// Frame label
		_frameLabel = new QLabel( "0/0", this );
		_frameLabel->setMinimumWidth( 60 );
		_frameLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout->addWidget( _frameLabel );

		// Connect signals
		connect( _btnPlayPause, &QPushButton::clicked, this, &TrajectoryPlayer::_onPlayPauseClicked );
		connect( _btnStop, &QPushButton::clicked, this, &TrajectoryPlayer::_onStopClicked );
		connect( _slider, &QSlider::sliderPressed, this, &TrajectoryPlayer::_onSliderPressed );
		connect( _slider, &QSlider::sliderReleased, this, &TrajectoryPlayer::_onSliderReleased );
		connect( _slider, &QSlider::sliderMoved, this, &TrajectoryPlayer::_onSliderMoved );

		// Connect to trajectory updates
		// App::REG().on_update<App::System::TrajectoryFullBuffer>().connect<&TrajectoryPlayer::_onTrajectoryUpdated>(
		//	this
		//);

		// Initial state
		_refresh();
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

	void TrajectoryPlayer::_onSliderPressed() { _isUpdatingSlider = true; }

	void TrajectoryPlayer::_onSliderReleased()
	{
		App::System::GenericTrajectory * trajPtr = nullptr;
		App::System::get( _system, trajPtr );

		if ( trajPtr && trajPtr->trajectorySize > 0 )
		{
			uint frame = uint( _slider->value() );
			App::ACTION().execute<App::Action::Trajectory::JumpTo>( _system, frame );
		}
		_refresh();
		_isUpdatingSlider = false;
	}

	void TrajectoryPlayer::_onSliderMoved( int p_value )
	{
		App::System::GenericTrajectory * trajPtr = nullptr;
		App::System::get( _system, trajPtr );

		if ( trajPtr && trajPtr->trajectorySize > 0 )
		{
			_frameLabel->setText( QString( "%1/%2" ).arg( p_value ).arg( trajPtr->trajectorySize - 1 ) );
			App::ACTION().execute<App::Action::Trajectory::JumpTo>( _system, p_value );
		}
		_refresh();
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

		// Update slider (only if not being dragged)
		if ( !_isUpdatingSlider )
		{
			_slider->setMaximum( totalFrames > 0 ? int( totalFrames - 1 ) : 0 );
			_slider->setValue( int( currentFrame ) );
		}

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
		_btnPlayPause->setText( isPlaying ? QString::fromUtf8( "\u23F8" ) : QString::fromUtf8( "\u25B6" ) );
		_btnStop->setText( QString::fromUtf8( "\u23F9" ) );
	}

} // namespace VTX::UI::QT::Widget
