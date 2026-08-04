#ifndef __VTX_APP_ACTION_TRAJECTORY__
#define __VTX_APP_ACTION_TRAJECTORY__

#include "app/ecs.hpp"
#include "app/system/trajectory.hpp"

namespace VTX::App::Action::Trajectory
{
	struct ToggleStartPause
	{
	  public:
		void execute( Entity );
	};

	struct SetPaused
	{
	  public:
		void execute( Entity, const bool );
	};

	struct Stop
	{
	  public:
		void execute( Entity );
	};

	struct JumpTo
	{
	  public:
		void execute( Entity, uint );
	};

	struct ChangePlayer
	{
	  public:
		void execute( Entity, System::TRAJECTORY_PLAY_MODE );
	};

	struct ChangeSpeed
	{
	  public:
		void execute( Entity, float );
	};
} // namespace VTX::App::Action::Trajectory

#endif
