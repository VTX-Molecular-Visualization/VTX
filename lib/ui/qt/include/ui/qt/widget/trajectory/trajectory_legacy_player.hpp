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

			// FIXME refacto this code to get trajectory from UID? also used in trajectory actions
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto & component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if (component.contains(getSystemUID()))
				{
					auto & traj = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>(App::ECS_REGISTRY().getEntity( component ));

					progressElt->setMinimum( 0 );
					progressElt->setMaximum( (int)traj.getFrameCount() );

					traj.getPlayer().onFrameChange += [ & ]( const VTX::Core::Struct::Frame p_frame )
					{
						VTX_INFO(
							"trajectory_player frame changed  = {}",
							traj.getSystemPtr()->getTrajectory().GetCurrentFrameIndex()
						);
						getProgressElt()->setValue( (int)traj.getSystemPtr()->getTrajectory().GetCurrentFrameIndex());
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
				[ & ]() { 
					//App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetCircularPlayer>( getSystemUID() );
				}
			);
		}

		QComboBox *_playerSelector;
	};

} // namespace VTX::UI::QT::Widget

#endif
