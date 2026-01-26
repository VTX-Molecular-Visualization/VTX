#include "app/threading/base_thread.hpp"
#include <util/types.hpp>
//
#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/uid/uid_manager.hpp"
#include <renderer/renderer.hpp>

namespace VTX::App::System
{

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
		RootUID		 systemUid	= REG().get<System::UID>( _ptr->entity ).system;
		_ptr->loader.readNextFrame(); // First frame has already been added

		for ( size_t it_currentFrameIndex = 1; it_currentFrameIndex < frameCount; it_currentFrameIndex++ )
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
