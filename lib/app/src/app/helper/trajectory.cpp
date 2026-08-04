#include "app/helper/trajectory.hpp"
#include "app/services.hpp"
#include <io/writer/system.hpp>

namespace VTX::App::Helper::Trajectory
{
	FrameView getCurrentAtomPositions( const Entity p_entity )
	{
		VTX::IO::Writer::TrajectoryFrameGetter trajectory;
		get( p_entity, trajectory );
		return trajectory.getCurrentAtomPositions();
	}

	FrameView getAtomPositions( const Entity p_entity, const uint p_frame )
	{
		VTX::IO::Writer::TrajectoryFrameGetter trajectory;
		get( p_entity, trajectory );
		return p_frame < trajectory.frameCount() ? trajectory.getAtomPositions( p_frame ) : FrameView();
	}

	uint getFrameCount( const Entity p_entity ) noexcept
	{
		const VTX::App::System::GenericTrajectory * const trajectory = getGeneric( p_entity );
		if ( trajectory )
		{
			return trajectory->trajectorySize == TypeMax<uint> ? 0 : trajectory->trajectorySize;
		}

		return hasTrajectory( p_entity ) ? 1 : 0;
	}

	uint getLoadedFrameCount( const Entity p_entity )
	{
		VTX::IO::Writer::TrajectoryFrameGetter trajectory;
		get( p_entity, trajectory );
		return trajectory.frameCount();
	}

	uint getCurrentFrameIndex( const Entity p_entity ) noexcept
	{
		const VTX::App::System::GenericTrajectory * const trajectory = getGeneric( p_entity );
		return trajectory ? trajectory->currentFrameIndex : hasTrajectory( p_entity ) ? 0 : TypeMax<uint>;
	}

	uint getRequestedFrameIndex( const Entity p_entity ) noexcept
	{
		const VTX::App::System::GenericTrajectory * const trajectory = getGeneric( p_entity );
		return trajectory ? trajectory->requestedFrameIndex : hasTrajectory( p_entity ) ? 0 : TypeMax<uint>;
	}

	VTX::App::System::TRAJECTORY_PLAY_MODE getPlayMode( const Entity p_entity ) noexcept
	{
		const VTX::App::System::GenericTrajectory * const trajectory = getGeneric( p_entity );
		return trajectory ? trajectory->playMode : VTX::App::System::TRAJECTORY_PLAY_MODE::NONE;
	}

	bool isPaused( const Entity p_entity ) noexcept
	{
		const VTX::App::System::GenericTrajectory * const trajectory = getGeneric( p_entity );
		return trajectory ? trajectory->paused : true;
	}

	float getPlayingSpeed( const Entity p_entity ) noexcept
	{
		const VTX::App::System::GenericTrajectory * const trajectory = getGeneric( p_entity );
		return trajectory ? trajectory->playingSpeed : 0.f;
	}

	bool isFrameAvailable( const Entity p_entity, const uint p_frame )
	{ return p_frame < getLoadedFrameCount( p_entity ); }

	bool hasTrajectory( const Entity p_entity ) noexcept
	{
		return REG().any_of<VTX::App::System::TrajectorySingleFrame, VTX::App::System::TrajectoryFullBuffer>(
			p_entity
		);
	}

	bool hasMultiFrameTrajectory( const Entity p_entity ) noexcept
	{ return REG().any_of<VTX::App::System::TrajectoryFullBuffer>( p_entity ); }

	FrameRange getAvailableFrames( const Entity p_entity )
	{ return FrameRange::fromFirstCount( 0, static_cast<FrameRange::Count>( getLoadedFrameCount( p_entity ) ) ); }

	const VTX::App::System::GenericTrajectory * getGeneric( const Entity p_entity ) noexcept
	{
		if ( const auto * const trajectory = REG().try_get<VTX::App::System::TrajectoryFullBuffer>( p_entity ) )
		{
			return &trajectory->genericData;
		}

		return nullptr;
	}

	class SingleFrameGetter
	{
	  public:
		SingleFrameGetter( const VTX::App::System::TrajectorySingleFrame & p_trajectory ) : _trajectory( p_trajectory )
		{
		}

		inline uint frameCount() const { return 1u; }

		inline FrameView getAtomPositions( const uint & ) const { return _trajectory.get().atomPositions; }

		inline FrameView getCurrentAtomPositions() const { return _trajectory.get().atomPositions; }

	  private:
		std::reference_wrapper<const VTX::App::System::TrajectorySingleFrame> _trajectory;
	};

	class FullBufferGetter
	{
	  public:
		FullBufferGetter( const VTX::App::System::TrajectoryFullBuffer & p_trajectory ) : _trajectory( p_trajectory ) {}

		inline uint frameCount() const
		{
			const uint lastFrameAvailable = _trajectory.get().lastFrameAvailable;
			return lastFrameAvailable == TypeMax<uint> ? 0 : lastFrameAvailable + 1;
		}

		inline FrameView getAtomPositions( const uint & p_index ) const
		{ return _trajectory.get().frameCollection[ p_index ]; }

		inline FrameView getCurrentAtomPositions() const
		{ return _trajectory.get().frameCollection[ _trajectory.get().genericData.currentFrameIndex ]; }

	  private:
		std::reference_wrapper<const VTX::App::System::TrajectoryFullBuffer> _trajectory;
	};

	void get( const Entity p_entity, VTX::IO::Writer::TrajectoryFrameGetter & p_trajectory )
	{
		if ( const auto * const trajectory = REG().try_get<VTX::App::System::TrajectorySingleFrame>( p_entity ) )
		{
			p_trajectory = SingleFrameGetter( *trajectory );
		}
		if ( const auto * const trajectory = REG().try_get<VTX::App::System::TrajectoryFullBuffer>( p_entity ) )
		{
			p_trajectory = FullBufferGetter( *trajectory );
		}
	}
} // namespace VTX::App::Helper::Trajectory
