#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_ONCE__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_ONCE__

#include "base_player.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class Once : public BasePlayer
	{
	  private:
		inline static const Registration<Once> reg = Registration<Once>();

	  public:
		inline static const std::string NAME = "Once";

	  public:
		Once()						  = default;
		Once( const Once & p_source ) = default;
		Once( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string & getName() override { return NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
