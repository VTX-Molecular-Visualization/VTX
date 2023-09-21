#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_PING_PONG__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_PING_PONG__

#include "base_player.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class PingPong : public BasePlayer
	{
	  private:
		inline static const Registration<PingPong> reg = Registration<PingPong>();

	  public:
		inline static const std::string NAME = "Ping Pong";

	  public:
		PingPong()							  = default;
		PingPong( const PingPong & p_source ) = default;
		PingPong( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string & getName() override { return NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

	  private:
		bool _forward = true;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
