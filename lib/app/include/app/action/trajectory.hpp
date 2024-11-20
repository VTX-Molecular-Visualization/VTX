#ifndef __VTX_UI_ACTION_TRAJECTORY__
#define __VTX_UI_ACTION_TRAJECTORY__

#include "app/core/action/base_action.hpp"
#include <app/component/chemistry/system.hpp>
#include <app/component/scene/uid_component.hpp>
#include <app/entity/system.hpp>
#include <app/core/ecs/registry.hpp>

namespace VTX::App::Action::Trajectory
{
	class SetPlayTrajectory final : public App::Core::Action::BaseAction
	{
	  public:
		SetPlayTrajectory( const App::Core::UID::UIDRange & p_molecule ) : _molecule( p_molecule ) {}
		void execute() override;

	  private:
		VTX::App::Core::ECS::BaseEntity getEntityFromUIDRange(
			const App::Component::Scene::UIDComponent & p_UIDComponent
		) const
		{
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto &component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if ( component.contains( _molecule ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}

		const App::Core::UID::UIDRange & _molecule;
	};

	class SetPauseTrajectory final : public App::Core::Action::BaseAction
	{
	  public:
		SetPauseTrajectory( const App::Core::UID::UIDRange & p_molecule ) : _molecule( p_molecule ) {}
		void execute() override;

	  private:
		VTX::App::Core::ECS::BaseEntity getEntityFromUIDRange(
			const App::Component::Scene::UIDComponent & p_UIDComponent
		) const
		{
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto & component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if ( component.contains( _molecule ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}

		const App::Core::UID::UIDRange & _molecule;
	};

	class SetStopTrajectory final : public App::Core::Action::BaseAction
	{
	  public:
		SetStopTrajectory( const App::Core::UID::UIDRange & p_molecule ) : _molecule( p_molecule ) {}
		void execute() override;

	  private:
		VTX::App::Core::ECS::BaseEntity getEntityFromUIDRange(
			const App::Component::Scene::UIDComponent & p_UIDComponent
		) const
		{
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto & component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if ( component.contains( _molecule ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}

		const App::Core::UID::UIDRange & _molecule;
	};

	class SetCircularPlayer final : public App::Core::Action::BaseAction
	{
	  public:
		SetCircularPlayer( const App::Core::UID::UIDRange & p_molecule ) : _molecule( p_molecule ) {}
		void execute() override;

	  private:
		VTX::App::Core::ECS::BaseEntity getEntityFromUIDRange( const App::Component::Scene::UIDComponent & p_UIDComponent
		) const
		{
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto &component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if ( component.contains( _molecule ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _molecule;
	};

	class DecreaseFrameRate final : public App::Core::Action::BaseAction
	{
	  public:
		DecreaseFrameRate( const App::Core::UID::UIDRange & p_molecule ) : _molecule( p_molecule ) {}
		void execute() override;

	  private:
		VTX::App::Core::ECS::BaseEntity getEntityFromUIDRange(
			const App::Component::Scene::UIDComponent & p_UIDComponent
		) const
		{
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto & component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if ( component.contains( _molecule ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _molecule;
	};

	class IncreaseFrameRate final : public App::Core::Action::BaseAction
	{
	  public:
		IncreaseFrameRate( const App::Core::UID::UIDRange & p_molecule ) : _molecule( p_molecule ) {}
		void execute() override;

	  private:
		VTX::App::Core::ECS::BaseEntity getEntityFromUIDRange(
			const App::Component::Scene::UIDComponent & p_UIDComponent
		) const
		{
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto & component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if ( component.contains( _molecule ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _molecule;
	};

	class SetTrajectoryCurrentFrame final : public App::Core::Action::BaseAction
	{
	  public:
		SetTrajectoryCurrentFrame( const App::Core::UID::UIDRange & p_molecule, const int p_value ) : _molecule( p_molecule ), _value( p_value ) {}
		void execute() override;

	  private:
		VTX::App::Core::ECS::BaseEntity getEntityFromUIDRange(
			const App::Component::Scene::UIDComponent & p_UIDComponent
		) const
		{
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Scene::UIDComponent>().end();
				  ++iter )
			{
				auto & component = App::ECS_REGISTRY().getComponent<App::Component::Scene::UIDComponent>( *iter );

				if ( component.contains( _molecule ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _molecule;
		const int						 _value;
	};

} // namespace VTX::App::Action::Trajectory
#endif
