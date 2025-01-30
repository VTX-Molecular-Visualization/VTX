#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_MANAGER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_MANAGER__

#include "app/component/scene/uid_component.hpp"
#include <QGroupBox>
#include <QVBoxLayout>
#include <app/application/scene.hpp>
#include <ui/qt/widget/trajectory/trajectory_base_player.hpp>
#include <ui/qt/widget/trajectory/trajectory_legacy_player.hpp>
#include <ui/qt/widget/trajectory/trajectory_optimized_player.hpp>

namespace VTX::UI::QT::Widget
{
	class TrajectoryManager : public QWidget
	{
	  public:
		TrajectoryManager( QWidget * p_parent ) : QWidget( p_parent ), _parent( p_parent )
		{
			setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Maximum );

			setupLayout();

			addPlayersRoutine();
		}

		virtual ~TrajectoryManager() {}

	  private:
		void setupLayout()
		{
			_layout = new QVBoxLayout( this );
			_layout->setContentsMargins( 0, 0, 0, 0 );
		}

		void addPlayersRoutine()
		{
			using namespace App::Component::Scene;
			App::SCENE().onSceneItemAdded += [ this ]( const SceneItemComponent & p_sceneItem )
			{
				if ( App::ECS_REGISTRY().hasComponent<App::Component::Chemistry::System>( p_sceneItem ) )
				{
					auto & system = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( p_sceneItem );
					auto & uid = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( p_sceneItem );

					if ( system.hasTrajectory() )
					{
						if ( system.getTrajectory().isOptimized() )
						{
							auto *						playerZone = new QGroupBox( system.getName().c_str() );
							TrajectoryOptimizedPlayer * player	   = new TrajectoryOptimizedPlayer( this, &system );
							playerZone->setLayout( player->layout() );
							_layout->addWidget( playerZone );
						}
						else
						{
							auto *					 playerZone = new QGroupBox( system.getName().c_str() );
							TrajectoryLegacyPlayer * player		= new TrajectoryLegacyPlayer( this, &system );
							playerZone->setLayout( player->layout() );
							_layout->addWidget( playerZone );
						}
					}
					system.onTrajectoryAdded += [ & ]()
					{
						if ( system.getTrajectory().isOptimized() )
						{
							auto *						playerZone = new QGroupBox( system.getName().c_str() );
							TrajectoryOptimizedPlayer * player	   = new TrajectoryOptimizedPlayer( this, &system );
							playerZone->setLayout( player->layout() );
							_layout->addWidget( playerZone );
						}
						else
						{
							auto *					 playerZone = new QGroupBox( system.getName().c_str() );
							TrajectoryLegacyPlayer * player		= new TrajectoryLegacyPlayer( this, &system );
							playerZone->setLayout( player->layout() );
							_layout->addWidget( playerZone );
						}
					};
				}
			};
		}

		QWidget *	 _parent;
		QBoxLayout * _layout;
	};

} // namespace VTX::UI::QT::Widget

#endif
