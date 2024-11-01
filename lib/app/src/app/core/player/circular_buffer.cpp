#include <util/logger.hpp>
#include <app/core/player/circular_buffer.hpp>
#include <app/component/render/proxy_molecule.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <app/core/threading/threading_system.hpp>
#include <io/reader/molecule.hpp>

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
		BasePlayer::play();

		for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
			  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
			  ++iter )
		{
			auto & trajectory = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

			if (&(trajectory.getPlayer()) == this)
				trajectory.getMoleculePtr()->getTrajectory().Reset();
				
		}

		Core::Threading::ThreadingSystem & threader	 = App::THREADING_SYSTEM();
		auto							   funcWrite = std::function<uint( App::Core::Threading::BaseThread & )> {};
		funcWrite									 = [this]( App::Core::Threading::BaseThread & baseThread )
		{
			VTX_INFO( "writethread start" );
			
			IO::Reader::Molecule moleculeReader   = IO::Reader::Molecule();
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
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( entity );
					while ( trajectory.getPlayer().isPlaying() )
						moleculeReader.readFile( trajectory.getPath(), molecule.getMoleculeStruct() );
					// moleculeReader.readTrajectoryFile( moleculePath, molecule.getMoleculeStruct() );
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
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( entity );
					VTX::App::Component::Render::ProxyMolecule & proxy
						= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>( entity );
					VTX::Core::Struct::Frame					 currentFrame;
					while ( trajectory.getPlayer().isPlaying() )
					{
						molecule.getTrajectory().getCurrentFrame( currentFrame );
						trajectory.getPlayer().StackFrame( currentFrame );
					}
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
		BasePlayer::update( p_deltaTime );

		// devjla
		VTX::Core::Struct::Frame currentFrame;
		if ( !_tmpFrames.GetCopyFrame( currentFrame )  || !isPlaying())
			return;
		VTX::App::Component::Render::ProxyMolecule & proxy
			= ECS_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>( *(ECS_REGISTRY().findComponents<App::Component::Render::ProxyMolecule>().begin()));
		proxy._updateAtomsPositions( currentFrame );
	}

} // namespace VTX::App::Core::Player
