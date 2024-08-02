#include "ui/qt/gateways/inspector_gw.hpp"
// read trajectory
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/render/proxy_molecule.hpp"
// write trajectory
#include <io/reader/molecule.hpp>

namespace VTX::UI::QT::Gateways
{
	InspectorGw::InspectorGw() : is_playing(false) {}
	void InspectorGw::registerResource( QObject * inspectorWidget )
	{
		_resource = inspectorWidget;
	}

	App::Component::Chemistry::Molecule & InspectorGw::getMoleculeData(
		const VTX::App::Component::Scene::SceneItemComponent & p_item
	)
	{
		//assert( App::MAIN_REGISTRY().hasComponent<App::Component::Chemistry::Molecule>( p_item ) );

		//return App::MAIN_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( p_item );
		return App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
	}

	VTX::App::Component::Chemistry::Trajectory & InspectorGw::getTrajectoryData()
	{
		return App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
	}

	void InspectorGw::setPlayTrajectoryPlayer( const App::Component::Scene::SceneItemComponent & p_item )
	{
		auto & molecule = getMoleculeData( p_item );
		if ( !molecule.hasTrajectory() )
			return;

		auto & trajectory = getTrajectoryData();
		//trajectory.getPlayer().play();

		is_playing							  = true;
		Core::Struct::Molecule moleculeStruct = Core::Struct::Molecule();

		std::thread writethread(
			[ & ]()
			{
				VTX_INFO( "writethread start" );
				IO::Reader::Molecule				 moleculeReader	  = IO::Reader::Molecule();
				const std::string					 moleculeName	  = "2ama_1_npt";
				const std::string					 moleculePathname = moleculeName + ".trr";
				const FilePath moleculePath = Util::Filesystem::getExecutableDir() / "data\\" / moleculePathname;
				//while (is_playing)
					moleculeReader.readFile( moleculePath, moleculeStruct );
				VTX_INFO( "writethread end" );
			}
		);

		/* std::thread readthreadfromlocalmolecule(
			[ & ]()
			{
				VTX_INFO( "readthreadfromlocalmolecule start" );
				VTX::Core::Struct::Frame currentFrame;
				VTX::App::Component::Render::ProxyMolecule & proxy
					= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyMolecule>();
				while ( moleculeStruct.trajectory.getCurrentFrame( currentFrame ) )
				{
					proxy._updateAtomsPositions( currentFrame );
					//std::this_thread::sleep_for( std::chrono::milliseconds(1000));
				}
				VTX_INFO( "readthreadfromlocalmolecule end" );
			}
		); */
		VTX_INFO( "readthreadfromlocalmolecule start" );
		VTX::Core::Struct::Frame					 currentFrame;
		VTX::App::Component::Render::ProxyMolecule & proxy
			= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyMolecule>();
		int idx(0);
		// FIXME this counter is just here to ensure we dont deadlock by reading frames for test purpose
		while ( idx < 98 )
		{
			moleculeStruct.trajectory.getCurrentFrame( currentFrame );
			proxy._updateAtomsPositions( currentFrame );
			++idx;
			// std::this_thread::sleep_for( std::chrono::milliseconds(1000));
		}
		VTX_INFO( "readthreadfromlocalmolecule end" );
		// this thread reads traj from the molecule trajectory data written when VTX is launched
		// currently problematic because no code is written to update a trajectory directly in the molecule : 
		// we are supposed to create a full Core::Struct::Molecule, write its content then std::move it into the existing molecule
		/* std::thread readthreadfromtraj(
			[ & ]()
			{
				VTX_INFO( "readthreadfromtraj start" );
				VTX::Core::Struct::Frame currentFrame;
				// VTX::App::Component::Render::ProxyMolecule &proxy =
				// App::MAIN_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>(*this);
				VTX::App::Component::Render::ProxyMolecule & proxy
					= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyMolecule>();
				while ( molecule.getTrajectory().getCurrentFrame( currentFrame ) )
				{
					// does it matter? no.. do I still write it? yes..
					size_t currentFrameIdx = trajectory.getCurrentFrame();
					proxy._updateAtomsPositions( currentFrame );
					// std::this_thread::sleep_for( std::chrono::milliseconds(1000));
				}
				VTX_INFO( "readthreadfromtraj end" );
			}
		); */
		VTX_INFO( "threads prejoin");
		writethread.join();
		//readthreadfromlocalmolecule.join();
		VTX_INFO( "threads joined");
	}
	void InspectorGw::setPauseTrajectoryPlayer( const App::Component::Scene::SceneItemComponent & p_item )
	{
		is_playing = false;

		auto & molecule = getMoleculeData( p_item );
		if ( !molecule.hasTrajectory() )
			return;

		auto & trajectory = getTrajectoryData();
		trajectory.getPlayer().pause();
	}
	void InspectorGw::setStopTrajectoryPlayer( const App::Component::Scene::SceneItemComponent & p_item )
	{
		is_playing = false;

		auto & molecule = getMoleculeData( p_item );
		if ( !molecule.hasTrajectory() )
			return;

		auto & trajectory = getTrajectoryData();
		trajectory.getPlayer().stop();
	}
} // namespace VTX::UI::QT::Gateways