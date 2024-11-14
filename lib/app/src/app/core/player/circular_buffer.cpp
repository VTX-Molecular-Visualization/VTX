#include <util/logger.hpp>
#include <app/core/player/circular_buffer.hpp>
#include <app/component/render/proxy_system.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <app/core/threading/threading_system.hpp>
#include <io/reader/system.hpp>
// devjla needs a refacto - code from molecule reader in IO
#include <util/chrono.hpp>

namespace VTX::App::Core::Player
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

			if (&(trajectory.getPlayer()) == this)
			{
				trajectory.getSystemPtr()->getTrajectory().Reset();
				trajectory.getSystemPtr()->getTrajectory()._isOptimized = true;
			}				
		}

		Core::Threading::ThreadingSystem & threader	 = App::THREADING_SYSTEM();
		auto							   funcWrite = std::function<uint( App::Core::Threading::BaseThread & )> {};
		funcWrite									 = [this]( App::Core::Threading::BaseThread & baseThread )
		{
			VTX_INFO( "writethread start" );
			
			IO::Reader::System moleculeReader   = IO::Reader::System();
			/* const std::string moleculeName		= "2ama_1_npt";
			const std::string	 moleculePathname = moleculeName + ".trr";
			const VTX::FilePath	 moleculePath = VTX::Util::Filesystem::getExecutableDir() / "data\\" / moleculePathname;*/

			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
				  ++iter )
			{
				auto & trajectory = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

				if (&(trajectory.getPlayer()) == this)
				{
					auto entity = App::ECS_REGISTRY().getEntity( trajectory );
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( entity );

					////////////////////////////
					/* while ( trajectory.getPlayer().isPlaying() )
						moleculeReader.readFile( trajectory.getPath(), molecule.getMoleculeStruct() );
						*/
					////////////////////////////

					////////////////////////////
					// devjla needs a refacto - code from molecule reader in IO
					std::unique_ptr<IO::Reader::Chemfiles> chemfilesReader
						= IO::Reader::Chemfiles::readFile( trajectory.getPath() );
					IO::Reader::Chemfiles &_chemfileStruct = *chemfilesReader;
					size_t					_trajectoryFrameStart = 1;
					size_t validFrameCount = 0;

					for ( size_t frameIdx = 0; frameIdx < _chemfileStruct.getFrameCount() - _trajectoryFrameStart;
						  ++frameIdx )
					{
						_chemfileStruct.readNextFrame();
						const std::vector<Vec3f> atomPositions = _chemfileStruct.getCurrentFrameAtomPosition();

						if ( atomPositions.size() <= 0 )
							continue;

						Util::Chrono timeReadingFrames;
						timeReadingFrames.start();
						float elapsed = 0.f;
						float hardFrameRate = 1.f;
						for (;;)
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
											if (trajectory.getPlayer().isPlaying())
											{
												timeReadingFrames.stop();
												VTX_INFO(
													"writethread tick {}s {}s {}s {}s",
													timeReadingFrames.elapsedTime(),
													elapsed,
													timeReadingFrames.elapsedTime() + elapsed,
													hardFrameRate
												);
												if ( timeReadingFrames.elapsedTime() + elapsed >= hardFrameRate )
												{
													molecule.getSystemStruct().trajectory.FillFrame( 42, frame );
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
					////////////////////////////
				}
			}
			VTX_INFO( "writethread end" );
			return 1;

		};
		auto funcRead = std::function<uint( App::Core::Threading::BaseThread & )> {};
		funcRead	  = [this]( App::Core::Threading::BaseThread & baseThread )
		{
			VTX_INFO( "readthreadfromlocalmolecule start" );
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
				  ++iter )
			{
				auto & trajectory = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

				if ( &( trajectory.getPlayer() ) == this )
				{
					auto   entity	= App::ECS_REGISTRY().getEntity( trajectory );
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( entity );
					VTX::App::Component::Render::ProxySystem & proxy
						= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxySystem>( entity );

					while ( trajectory.getPlayer().isPlaying() )
						trajectory.getPlayer().StackFrame( molecule.getTrajectory()._framesCircBuff.ReadElement() );
				}
			}
			VTX_INFO( "readthreadfromlocalmolecule end" );
			return 1;
		};
		/* Core::Threading::ThreadingSystem & threader = App::THREADING_SYSTEM();
		auto							   funcWrite = std::function<uint( App::Core::Threading::BaseThread & )> {};
		funcWrite									 = []( App::Core::Threading::BaseThread & baseThread )
		{
			VTX_INFO( "writethread start" );
			auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>(
				*( App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Molecule>().begin() )
			);
			// if ( !molecule.hasTrajectory() )
			//	return;
			auto & trajectory = App::ECS_REGISTRY().getComponent<VTX::App::Component::Chemistry::Trajectory>(
				*( App::ECS_REGISTRY().findComponents<VTX::App::Component::Chemistry::Trajectory>().begin() )
			);

			IO::Reader::Molecule moleculeReader	  = IO::Reader::Molecule();
			const std::string	 moleculeName	  = "2ama_1_npt";
			const std::string	 moleculePathname = moleculeName + ".trr";
			const VTX::FilePath	 moleculePath = VTX::Util::Filesystem::getExecutableDir() / "data\\" / moleculePathname;
			
			while ( trajectory.getPlayer().isPlaying() )
				moleculeReader.readFile( moleculePath, molecule.getMoleculeStruct() );
				//moleculeReader.readTrajectoryFile( moleculePath, molecule.getMoleculeStruct() );
			VTX_INFO( "writethread end" );
			return 1;
		};
		auto funcRead = std::function<uint( App::Core::Threading::BaseThread & )> {};
		funcRead	  = []( App::Core::Threading::BaseThread & baseThread )
		{
			VTX_INFO( "readthreadfromlocalmolecule start" );
			auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>(
				*( App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Molecule>().begin() )
			);
			// if ( !molecule.hasTrajectory() )
			//	return; no trajectory as we just created an empty traj and moved it in the molecule
			// struct...

			auto & trajectory = App::ECS_REGISTRY().getComponent<VTX::App::Component::Chemistry::Trajectory>(
				*( App::ECS_REGISTRY().findComponents<VTX::App::Component::Chemistry::Trajectory>().begin() )
			);
			//trajectory.getPlayer().play();

			VTX::Core::Struct::Frame					 currentFrame;
			VTX::App::Component::Render::ProxyMolecule & proxy
				= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>(
					*( App::ECS_REGISTRY().findComponents<App::Component::Render::ProxyMolecule>().begin() )
				);

			// int idx( 0 );
			// while (idx < 98)
			while ( trajectory.getPlayer().isPlaying() )
			{
				molecule.getTrajectory().getCurrentFrame( currentFrame
				); // working with real molecule actually loaded in app
				// moleculeStruct.trajectory.getCurrentFrame( currentFrame ); // working with core empty
				// molecule created in this scope proxy._updateAtomsPositions( currentFrame ); // CANNOT
				// update renderer in this thread context
				trajectory.getPlayer().StackFrame( currentFrame );
				//++idx;
				// std::this_thread::sleep_for( std::chrono::milliseconds(1000));
			}
			VTX_INFO( "readthreadfromlocalmolecule end" );
			return 1;
		};*/
		_writeThread = &(threader.createThread( funcWrite ));
		_readThread = &(threader.createThread( funcRead ));

		BasePlayer::play();
	}
	void CircularBuffer::pause()
	{
		BasePlayer::pause();

		_writeThread->stop();
		_readThread->stop();
		_tmpFrames.Flush();
	}
	void CircularBuffer::stop()
	{
		BasePlayer::stop();

		_writeThread->stop();
		_readThread->stop();
		_tmpFrames.Flush();
	}
	
	void CircularBuffer::update( const float p_deltaTime )
	{
		//BasePlayer::update( p_deltaTime );
		if( !isPlaying() )
			return;

		// devjla
		// own legacy to be cleared
		/* VTX::App::Component::Render::ProxyMolecule & proxy
			= ECS_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>( *(ECS_REGISTRY().findComponents<App::Component::Render::ProxyMolecule>().begin()));
		proxy._updateAtomsPositions( currentFrame );*/

		//////////////////////////
		/*
		VTX::Core::Struct::Frame currentFrame;
		if ( getFPS() == 0u )
		{
			if ( _tmpFrames.GetCopyFrame( currentFrame ))
				onFrameChange( currentFrame );
		}
		else
		{
			const float frameRateMilliSec = ( 1.f / float( getFPS() ) ) * 1000.f;
			const float ellapsedTime	  = p_deltaTime - getTrajectoryTimer();
			if ( ellapsedTime >= frameRateMilliSec )
			{
				setTrajectoryTimer( p_deltaTime );
				if (_tmpFrames.GetCopyFrame(currentFrame))
					onFrameChange( currentFrame );
			}
		}
		*/
		//////////////////////////

		VTX::Core::Struct::Frame currentFrame;
		if ( _tmpFrames.GetCopyFrame( currentFrame ) )
			onFrameChange( currentFrame );
		//////////////////////////
	}

} // namespace VTX::App::Core::Player
