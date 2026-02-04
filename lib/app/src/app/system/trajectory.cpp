#include "app/threading/base_thread.hpp"
#include <util/types.hpp>
//
#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/threading/thread_manager.hpp"
#include "app/uid/uid_manager.hpp"
#include <renderer/renderer.hpp>

namespace VTX::App::System
{
	namespace
	{

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
	} // namespace

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

	void patchGenericTrajectories( ECS::Entity p_entity, std::function<void( GenericTrajectory & )> p_lambda ) noexcept
	{
		if ( REG().all_of<TrajectoryFullBuffer>( p_entity ) )
		{
			REG().patch<TrajectoryFullBuffer>(
				p_entity, [ &p_lambda ]( TrajectoryFullBuffer & p_ ) { p_lambda( p_.genericData ); }
			);
		}
	}

	void prepare( ECS::Entity p_entity, TrajectoryFullBuffer & p_trajectory, IO::Reader::System && p_loader ) noexcept
	{
		p_trajectory.genericData.trajectorySize = p_loader.getChemfilesReader().getFrameCount();
		p_trajectory.frameCollection.reserve( p_loader.getChemfilesReader().getFrameCount() );
		p_trajectory.frameCollection.emplace_back( p_loader.getChemfilesReader().getCurrentFrameAtomPosition() );
		p_trajectory.genericData.playMode = TrajectoryPlayMode::pingpong;
		p_trajectory.genericData.player	  = Util::Players::PingPong( p_trajectory.genericData.trajectorySize - 1 );
		p_trajectory.genericData.currentFrameIndex = 0;
		p_trajectory.lastFrameAvailable			   = 0;

		THREAD().createThread( System::TrajectoryFullBufferReader( p_entity, std::move( p_loader ) ) );
	}
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

	struct TrajectoryFullBufferReader::_Data
	{
		ECS::Entity		   entity;
		IO::Reader::System loader;
	};

	struct Deleter
	{
		inline void operator()( TrajectoryFullBufferReader::_Data * p_ ) noexcept { delete p_; }
	};

	TrajectoryFullBufferReader::TrajectoryFullBufferReader( ECS::Entity entity, IO::Reader::System && loader ) :
		_ptr( std::shared_ptr<_Data>( new _Data { std::move( entity ), std::move( loader ) }, Deleter() ) )
	{
	}
	uint TrajectoryFullBufferReader::operator()( VTX::Util::StopToken p_stopToken, Threading::BaseThread & ) noexcept
	{
		auto &		 reader		= _ptr->loader.getChemfilesReader();
		const size_t frameCount = reader.getFrameCount();
		SystemUID		 systemUid	= REG().get<System::UID>( _ptr->entity ).system;
		_ptr->loader.readNextFrame(); // First frame has already been added

		for ( size_t it_currentFrameIndex = 1; it_currentFrameIndex < frameCount - 1; it_currentFrameIndex++ )
		{
			std::vector<Vec3f> new_frame = reader.getCurrentFrameAtomPosition();
			_ptr->loader.readNextFrame();
			REG().patch<TrajectoryFullBuffer>(
				_ptr->entity,
				[ &new_frame, &systemUid, &it_currentFrameIndex ]( TrajectoryFullBuffer & data )
				{
					data.frameCollection.emplace_back( std::move( new_frame ) );
					data.lastFrameAvailable = it_currentFrameIndex;
				}
			);
			if ( p_stopToken.stop_requested() )
				break;
		}
		return 0;
	}

} // namespace VTX::App::System
