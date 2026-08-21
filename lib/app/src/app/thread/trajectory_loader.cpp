#include "app/thread/trajectory_loader.hpp"
#include <chrono>
#include <fmt/format.h>
#include <util/logger.hpp>
#include <utility>

namespace VTX::App::Thread
{
	TrajectoryLoader::TrajectoryLoader(
		Util::Thread::ThreadManager &			 p_manager,
		IO::SystemReader &&						 p_reader,
		const Trajectory::TRAJECTORY_BUFFER_MODE p_mode
	) : BaseThread( p_manager ), _reader( std::move( p_reader ) ), _mode( p_mode )
	{
	}

	void TrajectoryLoader::start()
	{
		BaseThread::_start( [ this ]( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & p_thread )
							{ return _run( std::move( p_stopToken ), p_thread ); } );
	}

	void TrajectoryLoader::stop()
	{
		BaseThread::stop();
		_frameRequestCondition.notify_all();
	}

	void TrajectoryLoader::requestFrameWindow(
		const Trajectory::FrameRange				p_frames,
		const Trajectory::TRAJECTORY_READ_DIRECTION p_direction,
		const Trajectory::FrameRange				p_availableFrames
	)
	{
		if ( p_frames.isEmpty() )
		{
			return;
		}

		{
			const std::scoped_lock lock( _frameMutex );
			if ( p_availableFrames.contains( p_frames )
				 || ( _requestedFrames == p_frames && _requestedDirection == p_direction
					  && _requestedAvailableFrames == p_availableFrames ) )
			{
				return;
			}

			_requestedFrames		  = p_frames;
			_requestedDirection		  = p_direction;
			_requestedAvailableFrames = p_availableFrames;
			_requestVersion++;
		}
		_frameRequestCondition.notify_one();
	}

	void TrajectoryLoader::cancelFrameRequest()
	{
		const std::scoped_lock lock( _frameMutex );
		if ( not _requestedFrames && not _loadedFrames )
		{
			return;
		}

		_requestedFrames.reset();
		_loadedFrames.reset();
		_requestVersion++;
	}

	std::optional<Trajectory::LoadedFrameBatch> TrajectoryLoader::takeLoadedFrames()
	{
		const std::scoped_lock lock( _frameMutex );
		if ( not _loadedFrames )
		{
			return std::nullopt;
		}

		if ( _mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR && _loadedFrames->requestVersion != _requestVersion )
		{
			_loadedFrames.reset();
			return std::nullopt;
		}

		std::optional<Trajectory::LoadedFrameBatch> result = std::move( _loadedFrames );
		_loadedFrames.reset();

		return result;
	}

	uint64_t TrajectoryLoader::getRequestVersion() const
	{
		const std::scoped_lock lock( _frameMutex );

		return _requestVersion;
	}

	Core::Struct::Frame TrajectoryLoader::readFrame( const uint p_frame )
	{
		Core::Struct::Frame	   frame;
		const std::scoped_lock lock( _readerMutex );
		_reader.get( frame, p_frame );

		return frame;
	}

	uint TrajectoryLoader::_run( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & p_thread )
	{
		_reader.set( p_stopToken );

		return _mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR
				   ? _loadCircularBuffer( std::move( p_stopToken ), p_thread )
				   : _loadFullBuffer( std::move( p_stopToken ), p_thread );
	}

	uint TrajectoryLoader::_loadFullBuffer( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & p_thread )
	{
		p_thread.setProgressText( "Reading trajectory ..." );
		const size_t frameCount = _reader.frameCount();
		p_thread.setProgress( 1.f / static_cast<float>( frameCount ) );

		for ( uint frameIndex = 1; frameIndex < frameCount; frameIndex++ )
		{
			Core::Struct::Frame frame;
			{
				const std::scoped_lock lock( _readerMutex );
				_reader.get( frame, frameIndex );
			}
			if ( p_stopToken.stop_requested() )
			{
				break;
			}

			{
				const std::scoped_lock lock( _frameMutex );
				if ( not _loadedFrames )
				{
					_loadedFrames.emplace();
				}
				_loadedFrames->frames.emplace_back( frameIndex, std::move( frame ) );
				_loadedFrames->availableFrames = Trajectory::FrameRange::fromFirstCount( 0, frameIndex + 1 );
			}

			VTX_DEBUG( "Trajectory frame {} loaded", frameIndex );
			p_thread.setProgress( static_cast<float>( frameIndex + 1 ) / static_cast<float>( frameCount ) );
		}
		return 0;
	}

	uint TrajectoryLoader::_loadCircularBuffer(
		Util::Thread::StopToken	   p_stopToken,
		Util::Thread::BaseThread & p_thread
	)
	{
		p_thread.setProgressText( "Reading trajectory ..." );

		while ( not p_stopToken.stop_requested() )
		{
			Trajectory::FrameRange				  requestedFrameWindow;
			Trajectory::FrameRange				  availableFrames;
			Trajectory::TRAJECTORY_READ_DIRECTION direction;
			uint64_t							  currentRequestVersion = 0;
			{
				std::unique_lock lock( _frameMutex );
				_frameRequestCondition.wait_for(
					lock,
					std::chrono::milliseconds( 50 ),
					[ & ] { return p_stopToken.stop_requested() || _requestedFrames.has_value(); }
				);
				if ( p_stopToken.stop_requested() )
				{
					break;
				}
				if ( not _requestedFrames )
				{
					continue;
				}

				requestedFrameWindow  = *_requestedFrames;
				direction			  = _requestedDirection;
				currentRequestVersion = _requestVersion;
				availableFrames		  = _requestedAvailableFrames;
			}

			p_thread.setProgressText(
				fmt::format(
					"Reading trajectory frames [{}-{}] ...",
					requestedFrameWindow.getFirst(),
					requestedFrameWindow.getLast() - 1
				)
			);
			p_thread.setProgress( 0.f );

			Trajectory::LoadedFrameBatch loadedBatch;
			loadedBatch.availableFrames = requestedFrameWindow;
			loadedBatch.requestVersion	= currentRequestVersion;
			loadedBatch.frames.reserve( requestedFrameWindow.getCount() );
			uint processedFrameCount = 0;
			for ( uint frameOffset = 0; frameOffset < requestedFrameWindow.getCount(); frameOffset++ )
			{
				const uint frameIndex = direction == Trajectory::TRAJECTORY_READ_DIRECTION::FORWARD
											? requestedFrameWindow.getFirst() + frameOffset
											: requestedFrameWindow.getLast() - 1 - frameOffset;
				if ( availableFrames.contains( frameIndex ) )
				{
					processedFrameCount++;
					p_thread.setProgress(
						static_cast<float>( processedFrameCount )
						/ static_cast<float>( requestedFrameWindow.getCount() )
					);
					continue;
				}

				Core::Struct::Frame frame;
				{
					const std::scoped_lock lock( _readerMutex );
					_reader.get( frame, frameIndex );
				}
				if ( p_stopToken.stop_requested() )
				{
					return 0;
				}

				{
					const std::scoped_lock lock( _frameMutex );
					if ( currentRequestVersion != _requestVersion )
					{
						break;
					}
				}
				loadedBatch.frames.emplace_back( frameIndex, std::move( frame ) );
				processedFrameCount++;
				p_thread.setProgress(
					static_cast<float>( processedFrameCount ) / static_cast<float>( requestedFrameWindow.getCount() )
				);
			}

			{
				const std::scoped_lock lock( _frameMutex );
				if ( currentRequestVersion != _requestVersion )
				{
					continue;
				}

				_loadedFrames = std::move( loadedBatch );
				_requestedFrames.reset();
			}
		}

		return 0;
	}
} // namespace VTX::App::Thread
