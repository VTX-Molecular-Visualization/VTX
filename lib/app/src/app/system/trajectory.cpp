#include "app/threading/base_thread.hpp"
#include <util/types.hpp>
//
#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"
#include <renderer/renderer.hpp>

namespace VTX::App::System
{
	std::span<const Vec3f> getCurrentAtomPositions( const ECS::Entity & p_entity ) noexcept
	{
		if ( auto traj = REG().try_get<TrajectorySingleFrame>( p_entity ) )
		{
			return traj->atomPositions;
		}
		if ( auto traj = REG().try_get<TrajectoryFullBuffer>( p_entity ) )
		{
			return traj->frameCollection[ traj->genericData.currentFrameIndex ];
		}
		return {};
	}

	bool hasMultiFrameTrajectory( const ECS::Entity & p_entity ) noexcept
	{
		return REG().any_of<TrajectoryFullBuffer>( p_entity );
	}
	void get( const ECS::Entity & p_entity, GenericTrajectory *& p_trajPtr ) noexcept
	{
		p_trajPtr = nullptr;
		if ( REG().all_of<TrajectoryFullBuffer>( p_entity ) )
			p_trajPtr = &REG().get<TrajectoryFullBuffer>( p_entity ).genericData;
	}
	void get( const ECS::Entity & p_entity, AvailableFrames & p_out ) noexcept
	{
		if ( auto traj = REG().try_get<TrajectoryFullBuffer>( p_entity ) )
		{
			p_out.lowerBoundIndex  = 0;
			p_out.higherBoundIndex = traj->lastFrameAvailable;
			return;
		}
	}

	void eraseTrajectory( const ECS::Entity & p_entity ) noexcept
	{
		REG().remove<TrajectoryFullBuffer>( p_entity );
		REG().remove<TrajectorySingleFrame>( p_entity );
	}

	void patchGenericTrajectories( ECS::Entity p_entity, std::function<void( GenericTrajectory & )> p_lambda ) noexcept
	{
		if ( REG().all_of<TrajectoryFullBuffer>( p_entity ) )
		{
			REG().patch<TrajectoryFullBuffer>(
				p_entity, [ &p_lambda ]( TrajectoryFullBuffer & p_ ) { p_lambda( p_.genericData ); }
			);
		}
	}

} // namespace VTX::App::System
