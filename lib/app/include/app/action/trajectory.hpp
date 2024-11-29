#ifndef __VTX_UI_ACTION_TRAJECTORY__
#define __VTX_UI_ACTION_TRAJECTORY__

#include "app/core/action/base_action.hpp"
#include "app/core/player/concepts.hpp"
#include <app/component/chemistry/system.hpp>
#include <app/component/scene/uid_component.hpp>
#include <app/core/ecs/registry.hpp>
#include <app/entity/system.hpp>

namespace VTX::App::Action::Trajectory
{
	class SetPlayTrajectory final : public App::Core::Action::BaseAction
	{
	  public:
		SetPlayTrajectory( const App::Component::Chemistry::System * p_system ) : _system( p_system ) {}
		void execute() override;

	  private:
		const App::Component::Chemistry::System * _system;
	};

	class SetPauseTrajectory final : public App::Core::Action::BaseAction
	{
	  public:
		SetPauseTrajectory( const App::Component::Chemistry::System * p_system ) : _system( p_system ) {}
		void execute() override;

	  private:
		const App::Component::Chemistry::System * _system;
	};

	class SetStopTrajectory final : public App::Core::Action::BaseAction
	{
	  public:
		SetStopTrajectory( const App::Component::Chemistry::System * p_system ) : _system( p_system ) {}
		void execute() override;

	  private:
		const App::Component::Chemistry::System * _system;
	};

	class DecreaseFrameRate final : public App::Core::Action::BaseAction
	{
	  public:
		DecreaseFrameRate( const App::Component::Chemistry::System * p_system ) : _system( p_system ) {}
		void execute() override;

	  private:
		const App::Component::Chemistry::System * _system;
	};

	class IncreaseFrameRate final : public App::Core::Action::BaseAction
	{
	  public:
		IncreaseFrameRate( const App::Component::Chemistry::System * p_system ) : _system( p_system ) {}
		void execute() override;

	  private:
		const App::Component::Chemistry::System * _system;
	};

	class SetTrajectoryCurrentFrame final : public App::Core::Action::BaseAction
	{
	  public:
		SetTrajectoryCurrentFrame( const App::Component::Chemistry::System * p_system, const int p_value ) :
			_system( p_system ), _value( p_value )
		{
		}
		void execute() override;

	  private:
		const App::Component::Chemistry::System * _system;
		const int								  _value;
	};

	template<Core::Player::ConceptPlayer P>
	class SetLegacyPlayerType final : public App::Core::Action::BaseAction
	{
	  public:
		SetLegacyPlayerType( const App::Component::Chemistry::System * p_system ) : _system( p_system ) {}

		void execute() override
		{
			auto & traj = ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *_system );

			bool previousState = traj.getPlayer().isPlaying();

			traj.getPlayer().stop();
			traj.template setPlayer<P>();
			traj.getPlayer().reset();

			if ( previousState )
			{
				traj.getPlayer().play();
			}
		}

	  private:
		const App::Component::Chemistry::System * _system;
	};

} // namespace VTX::App::Action::Trajectory
#endif
