#include "app/pass/trajectory_updater.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"
namespace VTX::App::Pass
{
	TrajectoryUpdater::TrajectoryUpdater()
	{
		auto & reg = REG();

		reg.on_update<System::TrajectoryFullBuffer>().connect<&TrajectoryUpdater::_onUpdateFullBuffer>( this );
	}

	void TrajectoryUpdater::_onUpdateFullBuffer( ECS::Registry & p_reg, ECS::Entity _entity )
	{
		// Probably nothing to do here as we need to update the frame on the mainloop during the update call
	}

} // namespace VTX::App::Pass
