#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__

#include "app/application/system/ecs_system.hpp"
#include <app/action/trajectory.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <ui/qt/base_widget.hpp>
#include <ui/qt/widget/trajectory/trajectory_base_player.hpp>
////////
#include <app/core/player/loop.hpp>
#include <app/core/player/once.hpp>
#include <app/core/player/ping_pong.hpp>
#include <app/core/player/revert_loop.hpp>
#include <app/core/player/revert_once.hpp>
#include <app/core/player/stop.hpp> // UH?
////////
#include <QComboBox>

namespace VTX::UI::QT::Widget
{

	class TrajectoryLegacyPlayer : public TrajectoryBasePlayer
	{
	  public:
		TrajectoryLegacyPlayer( QWidget * p_parent, const App::Component::Chemistry::System * p_system ) :
			TrajectoryBasePlayer( p_parent, p_system )
		{
			setupAdditionalElts();

			addAdditionalToLayout();

			modifyProgressElt();

			connectAdditionalCallbacks();
		}

		virtual ~TrajectoryLegacyPlayer() {}

	  private:
		// adds player selector combobox
		void setupAdditionalElts()
		{
			_playerSelector = new QComboBox( this );
			_playerSelector->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

			_playerSelector->addItem( App::Core::Player::Loop::DISPLAYED_NAME.c_str() );
			_playerSelector->addItem( App::Core::Player::Once::DISPLAYED_NAME.c_str() );
			_playerSelector->addItem( App::Core::Player::PingPong::DISPLAYED_NAME.c_str() );
			_playerSelector->addItem( App::Core::Player::RevertOnce::DISPLAYED_NAME.c_str() );
			_playerSelector->addItem( App::Core::Player::RevertLoop::DISPLAYED_NAME.c_str() );
			_playerSelector->addItem( App::Core::Player::Stop::DISPLAYED_NAME.c_str() );

			auto & traj	  = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *getSystem() );
			auto   player = traj.getPlayer().getDisplayName();

			_playerSelector->setCurrentText( QString( player.c_str() ) );
		}

		// player selector combobox goes on top
		void addAdditionalToLayout() { getLayout()->addWidget( _playerSelector, 0, 0 ); }

		void modifyProgressElt( void )
		{
			auto * progressElt		= getProgressElt();
			auto * frameSelectorElt = getFrameSelectorElt();

			auto & traj = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *getSystem() );

			// define min and max of slider from traj info
			progressElt->setMinimum( 0 );
			progressElt->setMaximum( (int)traj.getFrameCount() - 1 );

			// set cursor at the current frame index
			// progressElt->setValue( (int)traj.getCurrentFrame() );
			progressElt->setValue( (int)traj.getPlayer().getCurrent() );

			// display current frame index in selector lineedit
			// frameSelectorElt->setText(
			// QLocale().toString((int)traj.getSystemPtr()->getTrajectory().GetCurrentFrameIndex()) );
			frameSelectorElt->setText( QLocale().toString( (int)traj.getPlayer().getCurrent() ) );

			// update both slider and lineedit zone with current frame
			traj.getPlayer().onFrameChange += [ & ]( const VTX::Core::Struct::Frame p_frame )
			{
				VTX_INFO(
					"trajectory_player frame changed  = {}", traj.getSystemPtr()->getTrajectory().getCurrentFrameIndex()
				);
				int currentFrameIdx = (int)traj.getSystemPtr()->getTrajectory().getCurrentFrameIndex();
				getProgressElt()->setValue( currentFrameIdx );
				getFrameSelectorElt()->setText( QLocale().toString( currentFrameIdx ) );

				if ( !traj.getPlayer().isPlaying() )
					setStopPlayer();
			};
		}

		void updateOnFrameChangeCallback( App::Component::Chemistry::Trajectory & p_traj ) {}

		void connectAdditionalCallbacks()
		{
			connect(
				_playerSelector,
				&QComboBox::currentIndexChanged,
				this,
				[ & ]( const int p_index )
				{
					using namespace App::Core::Player;
					using namespace App::Action::Trajectory;

					switch ( p_index )
					{
					case 0: App::ACTION_SYSTEM().execute<SetLegacyPlayerType<Loop>>( getSystem() ); break;
					case 1: App::ACTION_SYSTEM().execute<SetLegacyPlayerType<Once>>( getSystem() ); break;
					case 2: App::ACTION_SYSTEM().execute<SetLegacyPlayerType<PingPong>>( getSystem() ); break;
					case 3: App::ACTION_SYSTEM().execute<SetLegacyPlayerType<RevertOnce>>( getSystem() ); break;
					case 4: App::ACTION_SYSTEM().execute<SetLegacyPlayerType<RevertLoop>>( getSystem() ); break;
					case 5: App::ACTION_SYSTEM().execute<SetLegacyPlayerType<Stop>>( getSystem() ); break;
					default: break;
					}

					// setStopPlayer();
					modifyProgressElt();
				}
			);
		}

		QComboBox * _playerSelector;
	};

} // namespace VTX::UI::QT::Widget

#endif
