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

} // namespace VTX::App::Test::Util
