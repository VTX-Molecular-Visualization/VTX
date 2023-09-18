#include "test_utils.hpp"
#include <app/application/scene.hpp>
#include <app/internal/action/ecs.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <string>

// std::string MOLECULE_TEST_NAME = "1AGA";
// const std::string MOLECULE_TEST_NAME	 = "8OIT";
// const std::string MOLECULE_TEST_NAME_EXT = MOLECULE_TEST_NAME + ".mmtf";
//
// const std::string MOLECULE_TRAJECTORY_TEST_NAME		= "1NIM";
// const std::string MOLECULE_TRAJECTORY_TEST_NAME_EXT = MOLECULE_TRAJECTORY_TEST_NAME + ".mmtf";

// std::string MOLECULE_TEST_NAME = "7Y7A";

namespace VTX::App::Test
{
	void Util::initApp()
	{
		using namespace VTX::App;

		static bool isInit;
		if ( !isInit )
		{
			VTXApp::get().start( {} );
			isInit = true;
		}

		VTXApp::get().getScene().reset();
	}

	void Util::loadTestMolecule()
	{
		using namespace VTX;
		using namespace VTX::App;

		Application::Scene & scene = VTXApp::get().getScene();

		// Create MoleculeEntity
		const FilePath moleculePath = IO::Internal::Filesystem::getInternalDataDir() / MOLECULE_TEST_NAME_EXT;
		Internal::Action::ECS::Open openAction = Internal::Action::ECS::Open( moleculePath );
		openAction.execute();
	}

	void Util::loadTestTrajectoryMolecule()
	{
		using namespace VTX;
		using namespace VTX::App;

		Application::Scene & scene = VTXApp::get().getScene();

		// Create MoleculeEntity
		const FilePath moleculePath
			= IO::Internal::Filesystem::getInternalDataDir() / MOLECULE_TRAJECTORY_TEST_NAME_EXT;
		Internal::Action::ECS::Open openAction = Internal::Action::ECS::Open( moleculePath );
		openAction.execute();
	}
} // namespace VTX::App::Test
