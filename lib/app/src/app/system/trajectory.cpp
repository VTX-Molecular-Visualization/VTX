#include "app/threading/base_thread.hpp"
#include <util/types.hpp>
//
#include "app/system/trajectory.hpp"

namespace VTX::App::System
{
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
		p_ptr( std::shared_ptr<_Data>( new _Data { std::move( entity ), std::move( loader ) }, Deleter() ) )
	{
	}
	uint TrajectoryFullBufferReader::operator()( VTX::Util::StopToken p_stopToken, Threading::BaseThread & ) noexcept {}

} // namespace VTX::App::System
