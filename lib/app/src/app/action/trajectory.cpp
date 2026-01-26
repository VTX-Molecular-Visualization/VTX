#include "app/action/trajectory.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"

namespace VTX::App::Action::Trajectory
{
	void ToggleStartPause::execute( ECS::Entity p_entity ) noexcept
	{
		System::GenericTrajectory * trajPtr = nullptr;
		System::get( p_entity, trajPtr );
		if ( trajPtr == nullptr )
			return;
		trajPtr->paused ^= 1;
	}

} // namespace VTX::App::Action::Trajectory
