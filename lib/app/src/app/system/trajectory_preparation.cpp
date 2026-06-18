#include "app/system/trajectory_preparation.hpp"
#include "app/services.hpp"
#include "app/system/uid.hpp"
#include "app/threading/thread_manager.hpp"
#include "app/uid/uid_manager.hpp"
#include <io/reader.hpp>

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
			TrajectoryFullBufferReader( TrajectoryFullBuffer &, IO::SystemReader && reader );

			uint operator()( VTX::Util::StopToken, Threading::BaseThread & ) noexcept;

			struct _Data;

		  private:
			std::shared_ptr<_Data> _ptr
				= nullptr; // The shared ptr aims to allow the copy without actually copying the IO resource
		};

		struct TrajectoryFullBufferReader::_Data
		{
			TrajectoryFullBuffer * _dataPtr;
			IO::SystemReader	   reader;
		};

		struct Deleter
		{
			inline void operator()( TrajectoryFullBufferReader::_Data * p_ ) noexcept { delete p_; }
		};

		TrajectoryFullBufferReader::TrajectoryFullBufferReader(
			TrajectoryFullBuffer & p_traj,
			IO::SystemReader &&	   reader
		) : _ptr( std::shared_ptr<_Data>( new _Data { &p_traj, std::move( reader ) }, Deleter() ) )
		{
		}

		uint TrajectoryFullBufferReader::operator()(
			VTX::Util::StopToken	p_stopToken,
			Threading::BaseThread & p_thr
		) noexcept
		{
			p_thr.setProgressText( "Reading trajectory ..." );
			_ptr->reader.set( p_stopToken );
			const size_t frameCount = _ptr->reader.frameCount();

			for ( uint it_currentFrameIndex = 1; it_currentFrameIndex < frameCount; it_currentFrameIndex++ )
			{
				_ptr->_dataPtr->frameCollection.emplace_back();
				_ptr->reader.get( _ptr->_dataPtr->frameCollection.back(), it_currentFrameIndex );

				_ptr->_dataPtr->lastFrameAvailable = it_currentFrameIndex;
				if ( p_stopToken.stop_requested() )
				{
					break;
				}
			}
			return 0;
		}
	} // namespace

	void prepare( TrajectoryFullBuffer & p_trajectory, IO::SystemReader && p_loader ) noexcept
	{
		p_trajectory.genericData.trajectorySize = static_cast<uint>( p_loader.frameCount() );
		p_trajectory.frameCollection.resize( 1 );
		p_trajectory.frameCollection.reserve( p_trajectory.genericData.trajectorySize );
		p_loader.get( p_trajectory.frameCollection.back(), 0 );
		p_trajectory.genericData.playMode = TrajectoryPlayMode::pingpong;
		p_trajectory.genericData.player	  = Util::Players::PingPong( p_trajectory.genericData.trajectorySize - 1 );
		p_trajectory.genericData.currentFrameIndex = 0;
		p_trajectory.lastFrameAvailable			   = 0;
	}

	void prepare( TrajectorySingleFrame & p_trajectory, IO::SystemReader && p_loader ) noexcept
	{ p_loader.get( p_trajectory.atomPositions, 0 ); }

	void startAsyncTrajectoryWork( const Entity & p_entity, PendingSystem & p_pendingData ) noexcept
	{
		if ( auto traj = REG().try_get<TrajectoryFullBuffer>( p_entity ) )
		{
			traj->threadId
				= THREAD()
					  .createThread(
						  System::TrajectoryFullBufferReader( *traj, std::move( p_pendingData.reader.value() ) )
					  )
					  .getId();
		}
	}

} // namespace VTX::App::System
