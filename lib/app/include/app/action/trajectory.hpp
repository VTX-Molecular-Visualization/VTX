#ifndef __VTX_APP_ACTION_TRAJECTORY__
#define __VTX_APP_ACTION_TRAJECTORY__

#include "app/ecs.hpp"
#include "app/system/trajectory_player.hpp"

namespace VTX::App::Action::Trajectory
{
	struct ToggleStartPause
	{
	  public:
		void execute( const Entity );
	};

	struct SetPaused
	{
	  public:
		void execute( const Entity, const bool );
	};

	struct Stop
	{
	  public:
		void execute( const Entity );
	};

	struct JumpTo
	{
	  public:
		void execute( const Entity, const uint );
	};

	struct ChangePlayer
	{
	  public:
		void execute( const Entity, const System::TRAJECTORY_PLAY_MODE );
	};

	struct ChangeSpeed
	{
	  public:
		void execute( const Entity, const float );
	};
} // namespace VTX::App::Action::Trajectory

#endif
