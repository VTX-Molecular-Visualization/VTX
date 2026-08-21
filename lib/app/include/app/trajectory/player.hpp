#ifndef __VTX_APP_TRAJECTORY_PLAYER__
#define __VTX_APP_TRAJECTORY_PLAYER__

#include <util/players.hpp>
#include <util/types.hpp>

namespace VTX::App::Trajectory
{
	/**
	 * @brief Player modes.
	 */
	enum struct PLAY_MODE : uint8_t
	{
		NONE,
		FORWARD,
		FORWARD_LOOP,
		BACKWARD_LOOP,
		BACKWARD,
		PING_PONG
	};

	/**
	 * @brief Multi-frame trajectory playback data.
	 */
	struct Player
	{
		PLAY_MODE	 playMode			 = PLAY_MODE::PING_PONG;
		bool		 paused				 = false;
		Util::Player player				 = Util::Players::PingPong( 1 );
		float		 playingSpeed		 = 35;
		float		 frameElapsedTime	 = 0;
		uint		 requestedFrameIndex = 0;
		uint		 currentFrameIndex	 = 0;
	};
} // namespace VTX::App::Trajectory

#endif
