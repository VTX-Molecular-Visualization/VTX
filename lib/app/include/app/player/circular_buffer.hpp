#ifndef __VTX_APP_PLAYER_CIRCULAR_BUFFER__
#define __VTX_APP_PLAYER_CIRCULAR_BUFFER__

#include <app/threading/base_thread.hpp>
#include <app/player/base_player.hpp>
#include <core/struct/frames_to_render.hpp>

namespace VTX::App::Player
{
	class CircularBuffer : public BasePlayer
	{
	  public:
		inline static const std::string DISPLAYED_NAME = "CircularBuffer";

	  public:
		CircularBuffer()								  = default;
		CircularBuffer( const CircularBuffer & p_source ) = default;
		virtual ~CircularBuffer()						  = default;

		const std::string & getDisplayName() const override { return DISPLAYED_NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		void update( const float, const float );

		/**
		 * @brief Creates two threads managed by the ThreadManager.
		 * First thread reads trajectory file and writes circular buffer data.
		 * Second thread reads circular buffer data and writes Frames to be displayed in a FramesToRender object (queue
		 * container).
		 */
		void play() override;
		void pause() override;
		void stop() override;

		/**
		 * @brief Copy a frame to be displayed into a FramesToRender object (queue container).
		 * FIXME can be optimized by not taking a copy?
		 */
		void stackFrame( VTX::Core::Struct::Frame elem ) { _displayFrames.addElement( elem ); }
		/**
		 * @brief Get a copy of the next frame to be displayed to the user.
		 * Using a copy to ensure we do not erase the frame in the lifo queue before it is processed by the engine.
		 * FIXME can be optimized by not taking a copy?
		 */
		bool getCopyFrame( VTX::Core::Struct::Frame & frame ) { return _displayFrames.getCopyFrame( frame ); }

		/**
		 * @brief Index to update progress slider in UI.
		 */
		const size_t getIndex( void ) const { return _displayFrames.getindex(); }

	  private:
		VTX::Core::Struct::FramesToRender  _displayFrames;
		App::Threading::BaseThread * _readThread	= nullptr;
		App::Threading::BaseThread * _writeThread = nullptr;
	};
#include <app/component/chemistry/trajectory.hpp>
#include <app/component/render/proxy_system.hpp>
#include <app/core/threading/threading_system.hpp>
#include <app/player/circular_buffer.hpp>
#include <io/reader/system.hpp>
#include <util/logger.hpp>

	namespace VTX::App::Player
	{
		void CircularBuffer::reset() { setCurrent( 0 ); }
		void CircularBuffer::nextFrame( const size_t p_count )
		{
			VTX_INFO( "Player CircularBuffer count {}", p_count );
			const size_t newFrameIndex = ( getCurrent() + p_count ) % getCount();
			setCurrent( newFrameIndex );
		}

		void CircularBuffer::play()
		{
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
				  ++iter )
			{
				auto & trajectory = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

				if ( &( trajectory.getPlayer() ) == this )
				{
					trajectory.getSystemPtr()->getTrajectory().reset();
					trajectory.getSystemPtr()->getTrajectory().setOptimized();
					_displayFrames.setMaxIndex( trajectory.getSystemPtr()->getTrajectory().getFrameCount() );
				}
			}

			Core::Threading::ThreadingSystem & threader	 = App::THREADING_SYSTEM();
			auto							   funcWrite = std::function<uint( App::Core::Threading::BaseThread & )> {};
			funcWrite									 = [ this ]( App::Core::Threading::BaseThread & baseThread )
			{
				VTX_INFO( "writethread start" );

				IO::Reader::System moleculeReader = IO::Reader::System();

				for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
					  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
					  ++iter )
				{
					auto & trajectory
						= App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

					if ( &( trajectory.getPlayer() ) == this )
					{
						auto   entity	= App::ECS_REGISTRY().getEntity( trajectory );
						auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( entity );

						////////////////////////////
						// flaw : cannot stop reading the player until a file is completely read
						while ( trajectory.getPlayer().isPlaying() )
						{
							moleculeReader.readFile( trajectory.getPath(), molecule.getSystemStruct() );
							std::this_thread::sleep_for( std::chrono::milliseconds( 20000 ) );
						}
						////////////////////////////

						////////////////////////////
						// code from molecule reader in IO to replicate the reading process
						// be able to be more atomic : stop player even if the file is not completely read
						/*
						std::unique_ptr<IO::Reader::Chemfiles> chemfilesReader
							= IO::Reader::Chemfiles::readFile( trajectory.getPath() );
						IO::Reader::Chemfiles & _chemfileStruct		  = *chemfilesReader;
						size_t					_trajectoryFrameStart = 1;
						size_t					validFrameCount		  = 0;

						for ( size_t frameIdx = 0; frameIdx < _chemfileStruct.getFrameCount() - _trajectoryFrameStart;
							  ++frameIdx )
						{
							_chemfileStruct.readNextFrame();
							const std::vector<Vec3f> atomPositions = _chemfileStruct.getCurrentFrameAtomPosition();

							if ( atomPositions.size() <= 0 )
								continue;

							Util::Chrono timeReadingFrames;
							timeReadingFrames.start();
							float elapsed		= 0.f;
							float hardFrameRate = 1000.f;
							for ( ;; )
							{
								if ( trajectory.getPlayer().isPlaying() )
								{
									for ( float x = -100.f; x <= 100.f; x += 50.f )
									{
										if ( trajectory.getPlayer().isPlaying() )
										{
											for ( float y = -100.f; y <= 100.f; y += 50.f )
											{
												std::vector<Vec3f>		 frame;
												const std::vector<Vec3f> atomPositionsDebug { { x, y, 0 } };
												frame.resize( atomPositionsDebug.size() );
												std::copy(
													atomPositionsDebug.begin(), atomPositionsDebug.end(), frame.begin()
												);
												if ( trajectory.getPlayer().isPlaying() )
												{
													timeReadingFrames.stop();
													// VTX_INFO(
													//	"writethread tick {}s {}s {}s {}s",
													//	timeReadingFrames.elapsedTime(),
													//	elapsed,
													//	timeReadingFrames.elapsedTime() + elapsed,
													//	hardFrameRate
													//);
													if ( timeReadingFrames.elapsedTime() + elapsed >= hardFrameRate )
													{
														molecule.getSystemStruct().trajectory.fillFrame( 42, frame );
														elapsed = 0.f;
													}
													else
														elapsed += timeReadingFrames.elapsedTime();
													timeReadingFrames.start();
												}
												else
													break;
											}
										}
										else
											break;
									}
								}
								else
									break;
							}
							timeReadingFrames.stop();
						}
						*/
						////////////////////////////
					}
				}
				VTX_INFO( "writethread end" );
				return 1;
			};
			auto funcRead = std::function<uint( App::Core::Threading::BaseThread & )> {};
			funcRead	  = [ this ]( App::Core::Threading::BaseThread & baseThread )
			{
				VTX_INFO( "readthread start" );
				for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
					  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
					  ++iter )
				{
					auto & trajectory
						= App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

					if ( &( trajectory.getPlayer() ) == this )
					{
						auto   entity	= App::ECS_REGISTRY().getEntity( trajectory );
						auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( entity );
						VTX::App::Component::Render::ProxySystem & proxy
							= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxySystem>( entity );

						while ( trajectory.getPlayer().isPlaying() )
							trajectory.getPlayer().stackFrame( molecule.getTrajectory().readOptimizedElement() );
					}
				}
				VTX_INFO( "readthread end" );
				return 1;
			};

			_writeThread = &( threader.createThread( funcWrite ) );
			_readThread	 = &( threader.createThread( funcRead ) );

			BasePlayer::play();
		}
		void CircularBuffer::pause()
		{
			_writeThread->stop();
			_readThread->stop();
			_displayFrames.flush();

			BasePlayer::pause();
		}
		void CircularBuffer::stop()
		{
			_writeThread->stop();
			_readThread->stop();
			_displayFrames.flush();

			BasePlayer::stop();
		}

		void CircularBuffer::update( const float p_deltaTime, const float p_elapsedTime )
		{
			if ( !isPlaying() )
				return;

			//////////////////////////
			VTX::Core::Struct::Frame currentFrame;
			if ( getFPS() == 0u )
			{
				if ( _displayFrames.getCopyFrame( currentFrame ) )
					onFrameChange( currentFrame );
			}
			else
			{
				const float frameRateMilliSec = ( 1.f / float( getFPS() ) ) * 1000.f;
				const float ellapsedTime	  = p_elapsedTime - getTrajectoryTimer();
				if ( ellapsedTime >= frameRateMilliSec )
				{
					setTrajectoryTimer( p_elapsedTime );
					if ( _displayFrames.getCopyFrame( currentFrame ) )
						onFrameChange( currentFrame );
				}
			}
			//////////////////////////

			//////////////////////////
			/*
			VTX::Core::Struct::Frame currentFrame;
			if ( _displayFrames.getCopyFrame( currentFrame ) )
				onFrameChange( currentFrame );
			*/
			//////////////////////////
		}

	} // namespace VTX::App::Player

} // namespace VTX::App::Player
#endif
