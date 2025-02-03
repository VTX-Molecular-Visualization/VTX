#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_BASE_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_BASE_PLAYER__

#include <QBoxLayout>
#include <QDockWidget>
#include <QGridLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <app/action/trajectory.hpp>

namespace VTX::UI::QT::Widget
{

	class TrajectoryBasePlayer : public QWidget
	{
	  public:
		TrajectoryBasePlayer( QWidget * p_parent, const App::Component::Chemistry::System * p_system ) :
			QWidget( p_parent ), _system( p_system )
		{
			setupLayout();

			setupPlayerElts();

			addPlayerToLayout();

			connectCallbacks();
		}

		virtual ~TrajectoryBasePlayer() {}

		const App::Component::Chemistry::System * getSystem( void ) { return _system; }

		QSlider * getProgressElt( void ) { return _progressElt; }

		QLineEdit * getFrameSelectorElt( void ) { return _frameSelectorElt; }

		QGridLayout * getLayout( void ) { return _layout; }

		void setStopPlayer( void )
		{
			_isPlaying = false;
			_playPauseElt->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );
		}

	  private:
		void setupLayout()
		{
			_layout = new QGridLayout( this );
			_layout->setContentsMargins( 0, 0, 0, 0 );

			_layout->setVerticalSpacing( 0 );

			_layout->setRowStretch( 0, 1 );
			_layout->setRowStretch( 1, 1 );
			_layout->setRowStretch( 2, 1 );

			_playButtonsLayout = new QHBoxLayout();
			_progressLayout	   = new QGridLayout();

			_progressLayout->setColumnStretch( 0, 3 );
			_progressLayout->setColumnStretch( 1, 1 );

			_layout->setSizeConstraint( QLayout::SetMaximumSize );
		}

		void setupPlayerElts()
		{
			setupStop();
			setupPlayPause();
			setupProgress();
			setupFrameRate();
			setupFrameSelector();
		}
		void setupStop()
		{
			_stopElt = new QPushButton( this );
			_stopElt->setIcon( QIcon( ":/sprite/background_white_icon.png" ) );
		}
		void setupPlayPause()
		{
			// FIXME default behavior in app is to NOT play trajectories when loaded
			// the UI state should maybe get this state instead of having this also hardcoded here
			_playPauseElt = new QPushButton( this );
			_playPauseElt->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );
		}
		void setupProgress()
		{
			_progressElt = new QSlider( Qt::Horizontal, this );

			_progressElt->setTickPosition( QSlider::TicksAbove );
		}
		void setupFrameRate()
		{
			_frameRateIncrElt = new QPushButton( this );
			_frameRateIncrElt->setIcon( QIcon( ":/sprite/trajectory_last_frame_icon.png" ) );
			_frameRateDecrElt = new QPushButton( this );
			_frameRateDecrElt->setIcon( QIcon( ":/sprite/trajectory_first_frame_icon.png" ) );
		}
		void setupFrameSelector()
		{
			_frameSelectorElt = new QLineEdit( this );
			//_frameSelectorElt->setInputMask( "9" );
			_frameSelectorElt->setValidator( new QIntValidator( 0, 2147483647, this ) );
		}

		void addPlayerToLayout()
		{
			// keep row idx 0 for player selector defined in children classes
			_layout->addLayout( _progressLayout, 1, 0 );
			_layout->addLayout( _playButtonsLayout, 2, 0 );

			_playButtonsLayout->addWidget( _playPauseElt );
			_playButtonsLayout->addWidget( _stopElt );
			_playButtonsLayout->addWidget( _frameRateDecrElt );
			_playButtonsLayout->addWidget( _frameRateIncrElt );

			_progressLayout->addWidget( _progressElt );
			_progressLayout->addWidget( _frameSelectorElt );
		}

		void connectCallbacks()
		{
			// connect callbacks
			connect(
				_playPauseElt,
				&QPushButton::clicked,
				this,
				[ & ]()
				{
					if ( _isPlaying )
					{
						setStopPlayer();

						App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetPauseTrajectory>( _system );
					}
					else
					{
						_isPlaying = true;
						_playPauseElt->setIcon( QIcon( ":/sprite/trajectory_pause_icon.png" ) );

						App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetPlayTrajectory>( _system );
					}
				}
			);

			connect(
				_stopElt,
				&QPushButton::clicked,
				this,
				[ & ]()
				{
					setStopPlayer();

					App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetStopTrajectory>( _system );
				}
			);

			connect(
				_frameRateDecrElt,
				&QPushButton::clicked,
				this,
				[ & ]()
				{
					if ( !_isPlaying )
						return;

					App::ACTION_SYSTEM().execute<App::Action::Trajectory::DecreaseFrameRate>( _system );
				}
			);

			connect(
				_frameRateIncrElt,
				&QPushButton::clicked,
				this,
				[ & ]()
				{
					if ( !_isPlaying )
						return;

					App::ACTION_SYSTEM().execute<App::Action::Trajectory::IncreaseFrameRate>( _system );
				}
			);

			connect(
				_progressElt,
				&QSlider::valueChanged,
				this,
				[ & ]( const int p_value )
				{
					App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetTrajectoryCurrentFrame>(
						_system, p_value
					);
				}
			);

			connect(
				_frameSelectorElt,
				&QLineEdit::returnPressed,
				this,
				[ & ]()
				{
					App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetTrajectoryCurrentFrame>(
						_system, _frameSelectorElt->text().toInt()
					);
				}
			);
		}

		std::atomic_bool _isPlaying; // FIXME dev purpose to stop threads needs improvement

		const App::Component::Chemistry::System * _system;
		QGridLayout *							  _layout;
		QHBoxLayout *							  _playButtonsLayout;
		QGridLayout *							  _progressLayout;
		QPushButton *							  _stopElt;
		QPushButton *							  _playPauseElt;
		QPushButton *							  _frameRateIncrElt;
		QPushButton *							  _frameRateDecrElt;
		QSlider *								  _progressElt;
		QLineEdit *								  _frameSelectorElt;
	};

} // namespace VTX::UI::QT::Widget

#endif
