#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_LOOP__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_LOOP__

#include "base_player.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	class Loop : public BasePlayer
	{
	  public:
		Loop()						  = default;
		Loop( const Loop & p_source ) = default;
		Loop( VTX::Core::Struct::Trajectory * const p_trajectory );

		virtual void reset() override;
		virtual void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
