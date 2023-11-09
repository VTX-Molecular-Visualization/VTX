#include "app.hpp"
#include <app/application/scene.hpp>
#include <app/internal/action/ecs.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <string>

namespace VTX::App::Test::Util
{
	void App::initApp()
	{
		static bool isInit;
		if ( !isInit )
		{
			VTXApp::get().start( {} );
			isInit = true;
		}

		VTXApp::get().getScene().reset();
	}

	void App::loadMolecule( const std::string & p_moleculePath )
	{
		Application::Scene & scene = VTXApp::get().getScene();

		// Create MoleculeEntity
		const FilePath				moleculePath = IO::Internal::Filesystem::getInternalDataDir() / p_moleculePath;
		Internal::Action::ECS::Open openAction	 = Internal::Action::ECS::Open( moleculePath );
		openAction.execute();
	}

	void App::loadTestMolecule() { loadMolecule( MOLECULE_TEST_NAME_EXT ); }
	void App::loadTestTrajectoryMolecule() { loadMolecule( MOLECULE_TRAJECTORY_TEST_NAME_EXT ); }

	bool App::checkItemOrder(
		const VTX::App::Application::Scene &	   p_scene,
		const std::vector<Core::ECS::BaseEntity> & p_entities
	)
	{
		if ( p_entities.size() >= 2 )
		{
			size_t prevIndex = p_scene.getItemIndex( p_entities[ 0 ] );
			for ( size_t i = 1; i < p_entities.size(); i++ )
			{
				const size_t index = p_scene.getItemIndex( p_entities[ i ] );

				if ( index <= prevIndex )
				{
					return false;
				}
			}
		}
		return true;
	}

} // namespace VTX::App::Test::Util
