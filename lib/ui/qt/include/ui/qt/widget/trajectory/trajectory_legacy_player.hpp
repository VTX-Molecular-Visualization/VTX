#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__

#include <ui/qt/widget/trajectory/trajectory_base_player.hpp>
#include "app/application/system/ecs_system.hpp"
#include <ui/qt/base_widget.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <app/core/uid/uid.hpp>
#include <app/action/trajectory.hpp>
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
		TrajectoryLegacyPlayer( QWidget * p_parent, const App::Core::UID::UIDRange & p_systemUID ) :
			TrajectoryBasePlayer( p_parent , p_systemUID )
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

			//_playerSelector->setCurrentIndex( 0 ); // FIXME
		}

		// player selector combobox goes on top
		void addAdditionalToLayout()
		{
			getLayout()->addWidget( _playerSelector, 0, 0 );
		}

		void modifyProgressElt(void)
		{
			auto * progressElt = getProgressElt();
			auto * frameSelectorElt = getFrameSelectorElt();

			// FIXME refacto this code to get trajectory from UID? also used in trajectory actions
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto & component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if (component.contains(getSystemUID()))
				{
					auto & traj = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>(App::ECS_REGISTRY().getEntity( component ));

					// define min and max of slider from traj info
					progressElt->setMinimum( 0 );
					progressElt->setMaximum( (int)traj.getFrameCount() );

					// display current frame index in selector lineedit
					frameSelectorElt->setText( QLocale().toString((int)traj.getSystemPtr()->getTrajectory().GetCurrentFrameIndex()) );

					// update both slider and lineedit zone with current frame
					traj.getPlayer().onFrameChange += [ & ]( const VTX::Core::Struct::Frame p_frame )
					{
						VTX_INFO(
							"trajectory_player frame changed  = {}",
							traj.getSystemPtr()->getTrajectory().GetCurrentFrameIndex()
						);
						int currentFrameIdx = (int)traj.getSystemPtr()->getTrajectory().GetCurrentFrameIndex();
						getProgressElt()->setValue( currentFrameIdx );
						getFrameSelectorElt()->setText( QLocale().toString( currentFrameIdx ) );
					};
				}
			}
		}

		void connectAdditionalCallbacks()
		{
			connect(
				_playerSelector,
				&QComboBox::currentIndexChanged,
				this,
				[ & ](const int p_index) { 
					App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetLegacyPlayerType>( getSystemUID(), _playerSelector->itemText(p_index).toStdString() );
				}
			);
		}

		QComboBox *_playerSelector;
	};

} // namespace VTX::UI::QT::Widget

#endif
