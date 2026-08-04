#ifndef __VTX_APP_SYSTEM_TRAJECTORY__
#define __VTX_APP_SYSTEM_TRAJECTORY__

#include "app/threading/base_thread.hpp"
#include <util/constants.hpp>
#include <util/players.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::System
{
	/**
	 * @brief Enumerates playing mode for a trajectory
	 */
	enum struct TRAJECTORY_PLAY_MODE : uint8_t
	{
		NONE, // only one frame
		FORWARD,
		FORWARD_LOOP,  // loop at the begining upon reaching the last
		BACKWARD_LOOP, // loop at the end upon reaching the first
		BACKWARD,
		PING_PONG // forward then backward when end is reached
	};

	/**
	 * @brief Data shared by any type of trajectories
	 */
	struct GenericTrajectory
	{
		TRAJECTORY_PLAY_MODE playMode
			= TRAJECTORY_PLAY_MODE::NONE; // Help the trajectory reader to schedule frame reading.
		bool		 paused = false;
		Util::Player player;
		float		 playingSpeed		 = 35;			  // Time in milliseconds between each frame update
		float		 frameElapsedTime	 = 0;			  // Unpaused time accumulated since the last frame update
		uint		 requestedFrameIndex = 0;			  // Here lies the Frame index that is requested.
		uint		 currentFrameIndex	 = TypeMax<uint>; // Here is the actual index related to the positions.
		uint		 trajectorySize		 = TypeMax<uint>;
	};

	struct TrajectorySingleFrame
	{
		std::vector<Vec3f> atomPositions;
	};

	struct TrajectoryFullBuffer
	{
		GenericTrajectory				genericData;
		std::vector<std::vector<Vec3f>> frameCollection;
		uint lastFrameAvailable { TypeMax<uint> }; // Updated by the filling thread. Value is MAX
												   // when no frame are available.
		Threading::BaseThread::ID threadId;
	};

} // namespace VTX::App::System

#endif
