#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_OPTIMIZED_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_OPTIMIZED_PLAYER__

#include <ui/qt/widget/trajectory/trajectory_base_player.hpp>
#include <ui/qt/base_widget.hpp>
#include <app/action/trajectory.hpp>
#include "app/application/system/ecs_system.hpp"
#include <app/component/chemistry/trajectory.hpp>
#include <app/core/player/circular_buffer.hpp>
#include <app/core/uid/uid.hpp>
#include <QDockWidget>
#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QLineEdit>

namespace VTX::UI::QT::Widget
{

	class TrajectoryOptimizedPlayer : public TrajectoryBasePlayer
	{
	  public:
		TrajectoryOptimizedPlayer( QWidget * p_parent, const App::Core::UID::UIDRange & p_systemUID ) :
			TrajectoryBasePlayer( p_parent , p_systemUID )
		{
			modifyProgressElt();
		}

		virtual ~TrajectoryOptimizedPlayer() {}
		
		void modifyProgressElt( void )
		{
			auto * progressElt = getProgressElt();

			// FIXME refafcto this code to get the trajectory
			auto & trajectory = App::ECS_REGISTRY().getComponent<VTX::App::Component::Chemistry::Trajectory>(
				*( App::ECS_REGISTRY().findComponents<VTX::App::Component::Chemistry::Trajectory>().begin() )
			);

			progressElt->setMinimum( 0 );
			progressElt->setMaximum( (int)trajectory.getFrameCount() );

			trajectory.getPlayer().onFrameChange += [ & ]( const VTX::Core::Struct::Frame p_frame )
			{
				VTX_INFO(
					"trajectory_player frame changed  = {}",
					trajectory.getSystemPtr()->getTrajectory().GetCurrentFrameIndex()
				);
				getProgressElt()->setValue(
					(int)dynamic_cast<VTX::App::Core::Player::CircularBuffer*>( &trajectory.getPlayer() )->getIndex());
			};

			progressElt->setEnabled( false );
		}

	  private:
	};

} // namespace VTX::UI::QT::Widget

#endif
