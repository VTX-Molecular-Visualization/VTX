#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_LOOP__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_LOOP__

#include "base_player.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class Loop : public BasePlayer
	{
	  private:
		inline static const Registration<Loop> reg = Registration<Loop>();

	  public:
		inline static const std::string NAME = "Loop";

	  public:
		Loop()						  = default;
		Loop( const Loop & p_source ) = default;
		Loop( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string & getName() override { return NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
