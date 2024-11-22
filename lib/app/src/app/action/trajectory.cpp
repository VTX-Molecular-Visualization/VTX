#include <app/core/ecs/registry.hpp>
#include "app/action/trajectory.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/render/proxy_system.hpp>
#include <app/core/player/players.hpp>
#include <app/core/player/circular_buffer.hpp>

namespace VTX::App::Action::Trajectory
{
	void SetPlayTrajectory::execute()
	{
		auto entity = getEntityFromUIDRange( _system );
		if ( !ECS_REGISTRY().isValid( entity ) )
			return;

		//VTX::Core::Struct::Molecule moleculeStruct = VTX::Core::Struct::Molecule();
		//auto & molecule = ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( entity );
		//molecule.setMoleculeStruct( moleculeStruct );

		auto & traj = ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( entity );
		traj.getPlayer().play();

		//////////////////////////////////////////////////
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
			// while (is_playing)
			// moleculeReader.readFile( moleculePath, moleculeStruct );
			while ( trajectory.getPlayer().isPlaying() )
				moleculeReader.readFile( moleculePath, molecule.getMoleculeStruct() );
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
			trajectory.getPlayer().play();

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
		};
		threader.createDetachedThread( funcWrite );
		threader.createDetachedThread( funcRead );
		*/
		//////////////////////////////////////////////////

		/*
		std::thread writethread(
			[]()
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
				const VTX::FilePath	 moleculePath
					= VTX::Util::Filesystem::getExecutableDir() / "data\\" / moleculePathname;
				// while (is_playing)
				// moleculeReader.readFile( moleculePath, moleculeStruct );
				while ( trajectory.getPlayer().isPlaying() )
					moleculeReader.readFile( moleculePath, molecule.getMoleculeStruct() );
				VTX_INFO( "writethread end" );
			}
		);

		std::thread readthreadfromlocalmolecule(
			[]()
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
				trajectory.getPlayer().play();

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
			}
		);
		VTX_INFO( "threads prejoin" );
		// writethread.join();
		// readthreadfromlocalmolecule.join();
		writethread.detach();
		readthreadfromlocalmolecule.detach();
		VTX_INFO( "threads joined" );
		*/
	}

	void SetPauseTrajectory::execute()
	{
		auto entity = getEntityFromUIDRange( _system );
		if ( !ECS_REGISTRY().isValid( entity ) )
			return;
		auto &		 traj = ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( entity );

		traj.getPlayer().pause();
	}

	void SetStopTrajectory::execute()
	{
		auto entity = getEntityFromUIDRange( _system );
		if ( !ECS_REGISTRY().isValid( entity ) )
			return;
		auto &		 traj = ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( entity );
	
		traj.getPlayer().stop();
	}

	void SetCircularPlayer::execute()
	{
		auto entity = getEntityFromUIDRange( _system );
		if ( !ECS_REGISTRY().isValid( entity ) )
			return;
		auto &		 traj = ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( entity );
		auto * const playMode
			= Util::Singleton<Core::Player::Players>::get().getOrCreate<Core::Player::CircularBuffer>();

		traj.setPlayer( playMode );
	}

	void DecreaseFrameRate::execute()
	{
		auto entity = getEntityFromUIDRange( _system );
		if ( !ECS_REGISTRY().isValid( entity ) )
			return;
		auto &		 traj = ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( entity );

		if ( !traj.getPlayer().getFPS() )
			traj.getPlayer().setFPS( 1 ); // FIXME raw fps in code = bad, find a way to define
		else
			traj.getPlayer().setFPS( traj.getPlayer().getFPS() / 2 );
	}

	void IncreaseFrameRate::execute()
	{
		auto entity = getEntityFromUIDRange( _system );
		if ( !ECS_REGISTRY().isValid( entity ) )
			return;
		auto &		 traj = ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( entity );

		if ( !traj.getPlayer().getFPS() )
			return; // nothing to do, already the fastest playing framerate
		else
			traj.getPlayer().setFPS( traj.getPlayer().getFPS() * 2 ); // FIXME at some point framerate = 0 and its the maximum speed
	}

	void SetTrajectoryCurrentFrame::execute()
	{
		auto entity = getEntityFromUIDRange( _system );
		if ( !ECS_REGISTRY().isValid( entity ) )
			return;
		auto & traj = ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( entity );

		traj.getPlayer().setCurrent( _value );
	}
} // namespace VTX::App::Action::Trajectory
