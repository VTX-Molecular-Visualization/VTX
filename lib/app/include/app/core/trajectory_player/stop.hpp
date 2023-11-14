#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_STOP__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_STOP__

#include "base_player.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class Stop : public BasePlayer
	{
	  private:
		inline static const Registration<Stop> reg = Registration<Stop>();

	  public:
		inline static const std::string NAME = "Stop";

	  public:
		Stop();
		Stop( const Stop & p_source );
		Stop( VTX::Core::Struct::Trajectory * const p_trajectory );

		void play() override;

		const std::string & getName() override { return NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
