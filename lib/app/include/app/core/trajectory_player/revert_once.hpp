#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_REVERT_ONCE__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_REVERT_ONCE__

#include "base_player.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class RevertOnce : public BasePlayer
	{
	  private:
		inline static const Registration<RevertOnce> reg = Registration<RevertOnce>();

	  public:
		inline static const std::string NAME = "Revert Once";

	  public:
		RevertOnce()							  = default;
		RevertOnce( const RevertOnce & p_source ) = default;
		RevertOnce( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string & getName() override { return NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
