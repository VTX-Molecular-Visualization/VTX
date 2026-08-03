#ifndef __VTX_APP_ACTION_TRAJECTORY__
#define __VTX_APP_ACTION_TRAJECTORY__

#include "app/ecs.hpp"
#include "app/system/trajectory.hpp"

namespace VTX::App::Action::Trajectory
{
	struct ToggleStartPause
	{
	  public:
		void execute( Entity ) noexcept;
	};

	struct Stop
	{
	  public:
		void execute( Entity ) noexcept;
	};

	struct JumpTo
	{
	  public:
		void execute( Entity, uint ) noexcept;
	};

	struct ChangePlayer
	{
	  public:
		void execute( Entity, System::TRAJECTORY_PLAY_MODE ) noexcept;
	};

	struct ChangeSpeed
	{
	  public:
		void execute( Entity, float ) noexcept;
	};
} // namespace VTX::App::Action::Trajectory

#endif
