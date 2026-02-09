#ifndef __VTX_APP_SYSTEM_LOAD__
#define __VTX_APP_SYSTEM_LOAD__

#include "app/system/trajectory.hpp"
#include <atomic>
#include <core/struct/system.hpp>
#include <io/reader/system.hpp>
#include <latch>
#include <optional>
#include <variant>

namespace VTX::App::System
{

	struct PendingSystem
	{
		FilePath						  path;
		std::optional<IO::Reader::System> loader;
		Core::Struct::System			  system;
		std::string						  pdbIdCode;
		// Two pass on the system : when the topology is ready, the decision of what kind of trajectory to have in made
		// on the main loop. Then, the trajectory is red asynchronously. Then, when trajectory is ready, the main loop
		// proceed with the system creation.
		std::atomic_bool topologyReady { false };
		std::atomic_bool decisionMade { false };
		std::latch		 trajectoryDecision { 1 };

		std::variant<System::TrajectorySingleFrame, System::TrajectoryFullBuffer> trajectoryData;

		std::atomic_bool trajectoryReady { false };
	};
	void create( const ECS::Entity &, PendingSystem & ) noexcept;
} // namespace VTX::App::System
#endif
