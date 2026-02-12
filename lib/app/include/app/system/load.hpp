#ifndef __VTX_APP_SYSTEM_LOAD__
#define __VTX_APP_SYSTEM_LOAD__

#include "app/system/trajectory.hpp"
#include "app/threading/base_thread.hpp"
#include <atomic>
#include <core/struct/system.hpp>
#include <functional>
#include <io/reader/system.hpp>
#include <latch>
#include <optional>
#include <util/thread.hpp>
#include <variant>

namespace VTX::App::System
{
	/**
	 * @brief Datastruct that will hold data while the system is being filled
	 */
	struct PendingSystem
	{
		bool					   onlyTrajectory = false; // Set to true when the goal is only to set a new trajectory
		FilePath				   path;
		std::optional<std::string> buffer;
		std::optional<IO::Reader::System> loader;
		Core::Struct::System			  system;
		std::string						  pdbIdCode;
		// Two pass on the system : when the topology is ready, the decision of what kind of trajectory to have in made
		// on the main loop. Then, the trajectory is red asynchronously. Then, when trajectory is ready, the main loop
		// proceed with the system creation.
		std::atomic_bool readyToDeliver { false };

		std::variant<System::TrajectorySingleFrame, System::TrajectoryFullBuffer> trajectoryData;
	};

	/**
	 * @brief Returns a callable that will read the file and fill the pendingSystem data.
	 */
	std::function<uint( Util::StopToken, Threading::BaseThread & )> fillerCallable(
		const ECS::Entity &,
		PendingSystem &
	) noexcept;

	/**
	 * @brief If PendingSystem::onlyTrajectory is false, actually create a system, appending all necessary component to
	 * the input entity from scratch, using data from the input PendingSystem. Else, only append the trajectory
	 */
	void deliver( const ECS::Entity &, PendingSystem & ) noexcept;
} // namespace VTX::App::System
#endif
