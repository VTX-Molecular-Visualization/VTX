#ifndef __VTX_APP_SYSTEM_TRAJECTORY__
#define __VTX_APP_SYSTEM_TRAJECTORY__

#include <app/ecs.hpp>
#include <io/reader/system.hpp>
#include <memory>
#include <span>
#include <util/thread.hpp>
#include <util/types.hpp>

namespace VTX::App::Threading
{
	class BaseThread;
}

namespace VTX::App::System
{
	/**
	 * @brief Enumerates playing mode for a trajectory
	 */
	enum class TrajectoryPlayMode : uint8_t
	{
		none,	  // only one frame
		forward,  // loop at the begining upon reaching the last
		backward, // loop at the end upon reaching the first
		pingpong  // forward then backward when end is reached
	};

	/**
	 * @brief Data shared by any type of trajectories
	 */
	struct GenericTrajectory
	{
		TrajectoryPlayMode playMode = TrajectoryPlayMode::none; // Help the trajectory reader to schedule frame reading.
		uint			   requestedFrameIndex = 0;				// Here lies the Frame index that is requested.
		uint currentFrameIndex = std::numeric_limits<uint>::max(); // Here is the actual index related to the positions.
		size_t trajectorySize  = std::numeric_limits<size_t>::max();
	};

	struct TrajectorySingleFrame
	{
		std::vector<Vec3f> atomPositions;
	};

	struct TrajectoryFullBuffer
	{
		GenericTrajectory				genericData;
		std::vector<std::vector<Vec3f>> frameCollection;
		size_t lastFrameAvailable = std::numeric_limits<size_t>::max(); // Updated by the filling thread. Value is MAX
																		// when no frame are available.
	};

	/**
	 * @brief Meant to be executed as another stoppable thread to fill a trajectory its frame asynchronously.
	 */
	class TrajectoryFullBufferReader
	{
	  public:
		TrajectoryFullBufferReader( ECS::Entity entity, IO::Reader::System && loader );

		uint operator()( VTX::Util::StopToken, Threading::BaseThread & ) noexcept;

		struct _Data;

	  private:
		std::shared_ptr<_Data> _ptr
			= nullptr; // The shared ptr aims to allow the copy without actually copying the IO resource
	};

} // namespace VTX::App::System

#endif
