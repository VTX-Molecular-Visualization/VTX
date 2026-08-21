#include "app/helper/trajectory.hpp"
#include "app/services.hpp"
#include "app/trajectory/loader.hpp"
#include <algorithm>
#include <io/writer/system.hpp>
#include <stdexcept>
#include <util/thread/thread_manager.hpp>

namespace VTX::App::Helper::Trajectory
{
	App::Trajectory::FrameRange getFrameWindow(
		const uint										 p_frame,
		const uint										 p_frameCount,
		const size_t									 p_windowFrameCount,
		const App::Trajectory::TRAJECTORY_READ_DIRECTION p_direction
	) noexcept
	{
		if ( p_frameCount == 0 || p_frame >= p_frameCount )
		{
			return {};
		}

		const uint windowFrameCount = static_cast<uint>(
			std::min( static_cast<size_t>( p_frameCount ), std::max( size_t( 1 ), p_windowFrameCount ) )
		);
		if ( p_direction == App::Trajectory::TRAJECTORY_READ_DIRECTION::FORWARD )
		{
			const uint firstFrame = std::min( p_frame, p_frameCount - windowFrameCount );
			return App::Trajectory::FrameRange::fromFirstCount( firstFrame, windowFrameCount );
		}

		const uint lastFrame = std::max( p_frame + 1, windowFrameCount );
		return App::Trajectory::FrameRange( lastFrame - windowFrameCount, lastFrame );
	}

	bool isFrameAvailable( const Entity p_entity, const uint p_frame )
	{
		if ( const auto * const loader = REG().try_get<App::Trajectory::Loader>( p_entity ) )
		{
			return loader->availableFrames.contains( p_frame );
		}

		const auto * const trajectory = REG().try_get<Core::Struct::Trajectory>( p_entity );
		if ( trajectory == nullptr || trajectory->frames.empty() )
		{
			return false;
		}
		return p_frame == 0;
	}

	bool visitFrame( const Entity p_entity, const uint p_frame, const FrameVisitor & p_visitor )
	{
		const auto * const trajectory = REG().try_get<Core::Struct::Trajectory>( p_entity );
		if ( trajectory == nullptr )
		{
			return false;
		}

		if ( const auto * const loader = REG().try_get<App::Trajectory::Loader>( p_entity ) )
		{
			const std::optional<size_t> storageFrameIndex = App::Trajectory::resolveStorageFrameIndex(
				p_frame,
				trajectory->frames.size(),
				loader->mode,
				loader->availableFrames.getFirst(),
				static_cast<uint>( loader->availableFrames.getCount() )
			);
			if ( not storageFrameIndex )
			{
				return false;
			}

			p_visitor( trajectory->frames[ *storageFrameIndex ] );
			return true;
		}

		if ( p_frame != 0 || trajectory->frames.empty() )
		{
			return false;
		}

		p_visitor( trajectory->frames[ 0 ] );
		return true;
	}

	bool visitCurrentFrame( const Entity p_entity, const FrameVisitor & p_visitor )
	{
		const auto * const player = REG().try_get<App::Trajectory::Player>( p_entity );
		return visitFrame( p_entity, player ? player->currentFrameIndex : 0, p_visitor );
	}

	Core::Struct::Frame getFrame( const Entity p_entity, const uint p_frame )
	{
		return THREAD().synchronize(
			[ p_entity, p_frame ]
			{
				if ( not REG().valid( p_entity ) )
				{
					throw std::invalid_argument( "Invalid trajectory entity." );
				}

				const auto * const trajectory = REG().try_get<Core::Struct::Trajectory>( p_entity );
				if ( trajectory == nullptr || p_frame >= trajectory->frameCount )
				{
					throw std::out_of_range( "Trajectory frame index out of range." );
				}

				Core::Struct::Frame frame;
				if ( visitFrame(
						 p_entity,
						 p_frame,
						 [ &frame ]( const Core::Struct::FrameView p_positions )
						 { frame.assign( p_positions.begin(), p_positions.end() ); }
					 ) )
				{
					return frame;
				}

				const auto * const loader = REG().try_get<App::Trajectory::Loader>( p_entity );
				if ( loader == nullptr )
				{
					throw std::runtime_error( "Trajectory frame is unavailable." );
				}

				return loader->thread->readFrame( p_frame );
			}
		);
	}

	bool hasMultiFrameTrajectory( const Entity p_entity ) { return REG().any_of<App::Trajectory::Player>( p_entity ); }

	const App::Trajectory::Player * getPlayer( const Entity p_entity )
	{ return REG().try_get<App::Trajectory::Player>( p_entity ); }

	App::Trajectory::FrameRange getAvailableFrames( const Entity p_entity )
	{
		if ( const auto * const loader = REG().try_get<App::Trajectory::Loader>( p_entity ) )
		{
			return loader->availableFrames;
		}

		const auto * const trajectory = REG().try_get<Core::Struct::Trajectory>( p_entity );
		return trajectory != nullptr && not trajectory->frames.empty()
				   ? App::Trajectory::FrameRange::fromFirstCount( 0, 1 )
				   : App::Trajectory::FrameRange {};
	}

	class FrameGetter
	{
	  public:
		FrameGetter( const Entity p_entity ) : _entity( p_entity ), _frames( getAvailableFrames( p_entity ) ) {}

		inline uint frameCount() const { return static_cast<uint>( _frames.getCount() ); }

		inline bool visitAtomPositions(
			const uint													 p_index,
			const VTX::IO::Writer::TrajectoryFrameGetter::FrameVisitor & p_visitor
		) const
		{
			return p_index < _frames.getCount()
				   && visitFrame( _entity, static_cast<uint>( _frames.getFirst() + p_index ), p_visitor );
		}

	  private:
		Entity						_entity;
		App::Trajectory::FrameRange _frames;
	};

	void get( const Entity p_entity, VTX::IO::Writer::TrajectoryFrameGetter & p_trajectory )
	{ p_trajectory = FrameGetter( p_entity ); }
} // namespace VTX::App::Helper::Trajectory
