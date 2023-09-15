#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_REVERT_LOOP__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_REVERT_LOOP__

#include "base_player.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class RevertLoop : public BasePlayer
	{
	  private:
		inline static const Registration<RevertLoop> reg = Registration<RevertLoop>();

	  public:
		inline static const std::string NAME = "Revert Loop";

	  public:
		RevertLoop()							  = default;
		RevertLoop( const RevertLoop & p_source ) = default;
		RevertLoop( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string & getName() override { return NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
