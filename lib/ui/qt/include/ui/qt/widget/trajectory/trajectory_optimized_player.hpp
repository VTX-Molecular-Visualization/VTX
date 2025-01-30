#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_OPTIMIZED_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_OPTIMIZED_PLAYER__

#include <QLabel>
#include <app/action/trajectory.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <app/core/player/circular_buffer.hpp>
#include <ui/qt/widget/trajectory/trajectory_base_player.hpp>

namespace VTX::UI::QT::Widget
{

	class TrajectoryOptimizedPlayer : public TrajectoryBasePlayer
	{
	  public:
		TrajectoryOptimizedPlayer( QWidget * p_parent, const App::Component::Chemistry::System * p_system ) :
			TrajectoryBasePlayer( p_parent, p_system )
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
			auto * progressElt		= getProgressElt();
			auto * frameSelectorElt = getFrameSelectorElt();

			auto & traj = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *getSystem() );

			// define min and max of slider from traj info
			progressElt->setMinimum( 0 );
			progressElt->setMaximum( (int)traj.getFrameCount() - 1 );

			// display current frame index in selector lineedit
			frameSelectorElt->setText( QLocale().toString(
				(int)dynamic_cast<VTX::App::Core::Player::CircularBuffer *>( &traj.getPlayer() )->getIndex()
			) );

			// update both slider and lineedit zone with current frame
			traj.getPlayer().onFrameChange += [ & ]( const VTX::Core::Struct::Frame p_frame )
			{
				VTX_INFO(
					"trajectory_player frame changed  = {}", traj.getSystemPtr()->getTrajectory().getCurrentFrameIndex()
				);
				int currentFrameIdx
					= (int)dynamic_cast<VTX::App::Core::Player::CircularBuffer *>( &traj.getPlayer() )->getIndex();
				getProgressElt()->setValue( currentFrameIdx );
				getFrameSelectorElt()->setText( QLocale().toString( currentFrameIdx ) );
			};

			progressElt->setEnabled( false );
			frameSelectorElt->setEnabled( false );
		}

		QLabel * _circbuffElt;
	};

} // namespace VTX::UI::QT::Widget

#endif
