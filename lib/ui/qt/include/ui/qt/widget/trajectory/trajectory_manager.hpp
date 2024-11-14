#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_MANAGER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_MANAGER__

#include <QBoxLayout>
#include <app/application/scene.hpp>
#include "app/component/scene/uid_component.hpp"
#include <ui/qt/base_widget.hpp>
#include <ui/qt/widget/trajectory/trajectory_player.hpp>

namespace VTX::UI::QT::Widget
{
	class TrajectoryManager : public QWidget
	{
	  public:
		TrajectoryManager( QWidget * p_parent ) :
			QWidget( p_parent ), _parent( p_parent )
		{
			setupLayout();

			addPlayersRoutine();
		}

		virtual ~TrajectoryManager() {}
		
	  private:
		void setupLayout()
		{
			_layout = new QBoxLayout( QBoxLayout::LeftToRight, this );
			_layout->setContentsMargins( 0, 0, 0, 0 );
		}

		void addPlayersRoutine()
		{
			using namespace App::Component::Scene;
			App::SCENE().onSceneItemAdded += [ this ]( const SceneItemComponent & p_sceneItem )
			{
				if ( App::ECS_REGISTRY().hasComponent<App::Component::Chemistry::System>( p_sceneItem ) )
				{
					auto & system
						= App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( p_sceneItem );
					auto & uid = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( p_sceneItem );

					if ( system.hasTrajectory() )
					{
						TrajectoryPlayer * player = new TrajectoryPlayer( this, system.getAtomUIDs() );
						_layout->addWidget( player );
					}
					system.onTrajectoryAdded += [ & ]()
					{
						TrajectoryPlayer * player = new TrajectoryPlayer( this, system.getAtomUIDs() );
						_layout->addWidget( player );
					};
				}
			};
		}

		QWidget		  *_parent;
		QBoxLayout * _layout;
	};

} // namespace VTX::UI::QT::Widget

#endif
