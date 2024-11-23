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
			setupAdditionalElts();

			addAdditionalToLayout();

			modifyProgressElt();
		}

		virtual ~TrajectoryOptimizedPlayer() {}
		
	  private:
		// adds circular buffer label
		void setupAdditionalElts()
		{
			_circbuffElt = new QLabel( App::Core::Player::CircularBuffer::DISPLAYED_NAME.c_str() );
			_circbuffElt->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Maximum );
		}

		// circular buffer label goes on top
		void addAdditionalToLayout() { getLayout()->addWidget( _circbuffElt, 0, 0 ); }

		void modifyProgressElt( void )
		{
			auto * progressElt = getProgressElt();
			auto * frameSelectorElt = getFrameSelectorElt();

			// FIXME refacto this code to get trajectory from UID? also used in trajectory actions
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto & component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if ( component.contains( getSystemUID() ) )
				{
					auto & traj = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>(
						App::ECS_REGISTRY().getEntity( component )
					);

					// define min and max of slider from traj info
					progressElt->setMinimum( 0 );
					progressElt->setMaximum( (int)traj.getFrameCount() );

					// display current frame index in selector lineedit
					frameSelectorElt->setText( QLocale().toString((int)dynamic_cast<VTX::App::Core::Player::CircularBuffer *>( &traj.getPlayer() )->getIndex()) );

					// update both slider and lineedit zone with current frame
					traj.getPlayer().onFrameChange += [ & ]( const VTX::Core::Struct::Frame p_frame )
					{
						VTX_INFO(
							"trajectory_player frame changed  = {}",
							traj.getSystemPtr()->getTrajectory().GetCurrentFrameIndex()
						);
						int currentFrameIdx = (int)dynamic_cast<VTX::App::Core::Player::CircularBuffer *>( &traj.getPlayer() )->getIndex();
						getProgressElt()->setValue( currentFrameIdx );
						getFrameSelectorElt()->setText( QLocale().toString(currentFrameIdx) );
					};
				}
			}

			progressElt->setEnabled( false );
		}

		QLabel * _circbuffElt;
	};

} // namespace VTX::UI::QT::Widget

#endif
