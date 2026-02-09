#include "app/system/trajectory_preparation.hpp"
#include "app/services.hpp"
#include "app/system/uid.hpp"
#include "app/threading/thread_manager.hpp"
#include "app/uid/uid_manager.hpp"

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
			TrajectoryFullBufferReader( TrajectoryFullBuffer &, IO::Reader::System && loader );

			uint operator()( VTX::Util::StopToken, Threading::BaseThread & ) noexcept;

			struct _Data;

		  private:
			std::shared_ptr<_Data> _ptr
				= nullptr; // The shared ptr aims to allow the copy without actually copying the IO resource
		};
		struct TrajectoryFullBufferReader::_Data
		{
			TrajectoryFullBuffer * _dataPtr;
			IO::Reader::System	   loader;
		};

		struct Deleter
		{
			inline void operator()( TrajectoryFullBufferReader::_Data * p_ ) noexcept { delete p_; }
		};

		TrajectoryFullBufferReader::TrajectoryFullBufferReader(
			TrajectoryFullBuffer & p_traj,
			IO::Reader::System &&  loader
		) : _ptr( std::shared_ptr<_Data>( new _Data { &p_traj, std::move( loader ) }, Deleter() ) )
		{
		}
		uint TrajectoryFullBufferReader::operator()(
			VTX::Util::StopToken p_stopToken,
			Threading::BaseThread &
		) noexcept
		{
			auto &		 reader		= _ptr->loader.getChemfilesReader();
			const size_t frameCount = reader.getFrameCount();
			_ptr->loader.readNextFrame(); // First frame has already been added

			for ( size_t it_currentFrameIndex = 1; it_currentFrameIndex < frameCount - 1; it_currentFrameIndex++ )
			{
				std::vector<Vec3f> new_frame = reader.getCurrentFrameAtomPosition();
				_ptr->loader.readNextFrame();
				_ptr->_dataPtr->frameCollection.emplace_back( std::move( new_frame ) );
				_ptr->_dataPtr->lastFrameAvailable = it_currentFrameIndex;
				if ( p_stopToken.stop_requested() )
					break;
			}
			return 0;
		}
	} // namespace

	void prepare( TrajectoryFullBuffer & p_trajectory, IO::Reader::System && p_loader ) noexcept
	{
		p_trajectory.genericData.trajectorySize = p_loader.getChemfilesReader().getFrameCount();
		p_trajectory.frameCollection.reserve( p_loader.getChemfilesReader().getFrameCount() );
		p_trajectory.frameCollection.emplace_back( p_loader.getChemfilesReader().getCurrentFrameAtomPosition() );
		p_trajectory.genericData.playMode = TrajectoryPlayMode::pingpong;
		p_trajectory.genericData.player	  = Util::Players::PingPong( p_trajectory.genericData.trajectorySize - 1 );
		p_trajectory.genericData.currentFrameIndex = 0;
		p_trajectory.lastFrameAvailable			   = 0;

		THREAD().createThread( System::TrajectoryFullBufferReader( p_trajectory, std::move( p_loader ) ) );
	}
	void prepare( TrajectorySingleFrame & p_trajectory, IO::Reader::System && p_loader ) noexcept
	{
		p_trajectory.atomPositions = p_loader.getChemfilesReader().getCurrentFrameAtomPosition();
	}

} // namespace VTX::App::System
