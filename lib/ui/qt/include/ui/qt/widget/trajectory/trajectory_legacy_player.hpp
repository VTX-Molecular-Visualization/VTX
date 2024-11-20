#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__

#include <ui/qt/widget/trajectory/trajectory_base_player.hpp>
#include "app/application/system/ecs_system.hpp"
//#include <functional>
#include <ui/qt/base_widget.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <app/core/uid/uid.hpp>
#include <app/action/trajectory.hpp>
#include <QDockWidget>
#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QLineEdit>

namespace VTX::UI::QT::Widget
{

	class TrajectoryLegacyPlayer : public TrajectoryBasePlayer
	{
	  public:
		TrajectoryLegacyPlayer( QWidget * p_parent, const App::Core::UID::UIDRange & p_systemUID ) :
			TrajectoryBasePlayer( p_parent , p_systemUID )
		{
			setupAdditionalLayout();

			setupAdditionalElts();

			addAdditionalToLayout();

			modifyProgressElt();

			connectAdditionalCallbacks();
		}

		virtual ~TrajectoryLegacyPlayer() {}
		
	  private:
		void setupAdditionalLayout()
		{
			_additionalLayout = new QVBoxLayout( this );
			_additionalLayout->setContentsMargins( 0, 0, 0, 0 );
		}

		void setupAdditionalElts()
		{
			setupTmpPlayerSelector();
		}

		void setupTmpPlayerSelector()
		{ 
			_tmpPlayerSelector = new QPushButton( this );
		}

		void addAdditionalToLayout()
		{
			_additionalLayout->addWidget( _tmpPlayerSelector );
		}

		void modifyProgressElt(void)
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
				getProgressElt()->setValue( (int)trajectory.getSystemPtr()->getTrajectory().GetCurrentFrameIndex() );
			};
		}

		void connectAdditionalCallbacks()
		{
			connect(
				_tmpPlayerSelector,
				&QPushButton::clicked,
				this,
				[ & ]() { 
					App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetCircularPlayer>( getSystemUID() );
				}
			);
		}

		QVBoxLayout	  *_additionalLayout;
		QPushButton *  _tmpPlayerSelector;
	};

} // namespace VTX::UI::QT::Widget

#endif
