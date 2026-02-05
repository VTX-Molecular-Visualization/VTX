#ifndef __VTX_APP_SYSTEM_TRAJECTORY__
#define __VTX_APP_SYSTEM_TRAJECTORY__

#include <app/ecs.hpp>
#include <functional>
#include <io/reader/system.hpp>
#include <memory>
#include <span>
#include <util/constants.hpp>
#include <util/players.hpp>
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
		none, // only one frame
		forward,
		forwardLoop,  // loop at the begining upon reaching the last
		backwardLoop, // loop at the end upon reaching the first
		backward,
		pingpong // forward then backward when end is reached
	};

	/**
	 * @brief Data shared by any type of trajectories
	 */
	struct GenericTrajectory
	{
		TrajectoryPlayMode playMode = TrajectoryPlayMode::none; // Help the trajectory reader to schedule frame reading.
		bool			   paused	= false;
		Util::Player	   player;
		float			   playingSpeed		   = 35; // Time in milliseconds between each frame update
		float			   lastFrameUpdateTime = 0;	 // last elapsed time where the trajectory has been changed
		uint			   requestedFrameIndex = 0;	 // Here lies the Frame index that is requested.
		uint			   currentFrameIndex   = TypeMax<uint>; // Here is the actual index related to the positions.
		uint			   trajectorySize	   = TypeMax<uint>;
	};
	void patchGenericTrajectories( ECS::Entity, std::function<void( GenericTrajectory & )> ) noexcept;

	struct TrajectorySingleFrame
	{
		std::vector<Vec3f> atomPositions;
	};

	struct TrajectoryFullBuffer
	{
		GenericTrajectory				genericData;
		std::vector<std::vector<Vec3f>> frameCollection;
		size_t lastFrameAvailable = TypeMax<size_t>; // Updated by the filling thread. Value is MAX
													 // when no frame are available.
	};

	std::span<const Vec3f> getCurrentAtomPositions( const ECS::Entity & ) noexcept;
	bool				   hasMultiFrameTrajectory( const ECS::Entity & ) noexcept;
	void				   get( const ECS::Entity &, GenericTrajectory *& ) noexcept;

	/**
	 * @brief Fill trajectory data structure up to the first frame, then start threaded frame loading.
	 * @param p_entity
	 * @param p_trajectory
	 * @param p_loader
	 */
	void prepare( ECS::Entity p_entity, TrajectoryFullBuffer & p_trajectory, IO::Reader::System && p_loader ) noexcept;

} // namespace VTX::App::System

#endif
