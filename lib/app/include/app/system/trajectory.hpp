#ifndef __VTX_APP_SYSTEM_TRAJECTORY__
#define __VTX_APP_SYSTEM_TRAJECTORY__

#include "app/threading/base_thread.hpp"
#include <atomic>
#include <util/players.hpp>
#include <util/types.hpp>
#include <utility>
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
		float		 playingSpeed		 = 35; // Time in milliseconds between each frame update
		float		 frameElapsedTime	 = 0;  // Unpaused time accumulated since the last frame update
		uint		 requestedFrameIndex = 0;  // Here lies the Frame index that is requested.
		uint		 currentFrameIndex	 = 0;  // Here is the actual index related to the positions.
		uint		 trajectorySize		 = 1;
	};

	/**
	 * @brief Single frame trajectory.
	 */
	struct TrajectorySingleFrame
	{
		std::vector<Vec3f> atomPositions;
	};

	/**
	 * @brief Multi-frame trajectory.
	 */
	struct TrajectoryFullBuffer
	{
		TrajectoryFullBuffer() = default;

		// Move needed because atomic.
		TrajectoryFullBuffer( TrajectoryFullBuffer && p_other ) noexcept :
			genericData( std::move( p_other.genericData ) ), frameCollection( std::move( p_other.frameCollection ) ),
			lastFrameAvailable( p_other.lastFrameAvailable.load() ), threadId( p_other.threadId )
		{
		}

		TrajectoryFullBuffer & operator=( TrajectoryFullBuffer && p_other ) noexcept
		{
			if ( this != &p_other )
			{
				genericData		   = std::move( p_other.genericData );
				frameCollection	   = std::move( p_other.frameCollection );
				lastFrameAvailable = p_other.lastFrameAvailable.load();
				threadId		   = p_other.threadId;
			}
			return *this;
		}

		GenericTrajectory				genericData;
		std::vector<std::vector<Vec3f>> frameCollection;
		std::atomic<uint>				lastFrameAvailable { 0 };
		Threading::BaseThread::ID		threadId;
	};

} // namespace VTX::App::System

#endif
