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
		SetPlayTrajectory( const App::Core::UID::UIDRange & p_system ) : _system( p_system ) {}
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

				if ( component.contains( _system ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}

		const App::Core::UID::UIDRange & _system;
	};

	class SetPauseTrajectory final : public App::Core::Action::BaseAction
	{
	  public:
		SetPauseTrajectory( const App::Core::UID::UIDRange & p_system ) : _system( p_system ) {}
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

				if ( component.contains( _system ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}

		const App::Core::UID::UIDRange & _system;
	};

	class SetStopTrajectory final : public App::Core::Action::BaseAction
	{
	  public:
		SetStopTrajectory( const App::Core::UID::UIDRange & p_system ) : _system( p_system ) {}
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

				if ( component.contains( _system ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}

		const App::Core::UID::UIDRange & _system;
	};

	class SetCircularPlayer final : public App::Core::Action::BaseAction
	{
	  public:
		SetCircularPlayer( const App::Core::UID::UIDRange & p_system ) : _system( p_system ) {}
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

				if ( component.contains( _system ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _system;
	};

	class DecreaseFrameRate final : public App::Core::Action::BaseAction
	{
	  public:
		DecreaseFrameRate( const App::Core::UID::UIDRange & p_system ) : _system( p_system ) {}
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

				if ( component.contains( _system ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _system;
	};

	class IncreaseFrameRate final : public App::Core::Action::BaseAction
	{
	  public:
		IncreaseFrameRate( const App::Core::UID::UIDRange & p_system ) : _system( p_system ) {}
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

				if ( component.contains( _system ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _system;
	};

	class SetTrajectoryCurrentFrame final : public App::Core::Action::BaseAction
	{
	  public:
		SetTrajectoryCurrentFrame( const App::Core::UID::UIDRange & p_system, const int p_value ) : _system( p_system ), _value( p_value ) {}
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

				if ( component.contains( _system ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _system;
		const int						 _value;
	};

	class SetLegacyPlayerType final : public App::Core::Action::BaseAction
	{
	  public:
		SetLegacyPlayerType( const App::Core::UID::UIDRange & p_system, const std::string p_name ) :
			_system( p_system ), _name( p_name )
		{
		}
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

				if ( component.contains( _system ) )
					return App::ECS_REGISTRY().getEntity( component );
			}

			return VTX::App::Core::ECS::INVALID_ENTITY;
		}
		const App::Core::UID::UIDRange & _system;
		const std::string				 _name;
	};

} // namespace VTX::App::Action::Trajectory
#endif
