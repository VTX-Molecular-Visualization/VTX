module;

#include <app/application/scene.hpp>
#include <app/internal/action/ecs.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <string>

export module Test.Utils;

// std::string MOLECULE_TEST_NAME = "1AGA";
export const std::string MOLECULE_TEST_NAME		= "8OIT";
export const std::string MOLECULE_TEST_NAME_EXT = MOLECULE_TEST_NAME + ".mmtf";

export const std::string MOLECULE_TRAJECTORY_TEST_NAME	   = "1NIM";
export const std::string MOLECULE_TRAJECTORY_TEST_NAME_EXT = MOLECULE_TRAJECTORY_TEST_NAME + ".mmtf";

// std::string MOLECULE_TEST_NAME = "7Y7A";

export void initApp()
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

export void loadTestMolecule()
{
	using namespace VTX;
	using namespace VTX::App;

	Application::Scene & scene = VTXApp::get().getScene();

	// Create MoleculeEntity
	const FilePath				moleculePath = IO::Internal::Filesystem::getInternalDataDir() / MOLECULE_TEST_NAME_EXT;
	Internal::Action::ECS::Open openAction	 = Internal::Action::ECS::Open( moleculePath );
	openAction.execute();
}

export void loadTestTrajectoryMolecule()
{
	using namespace VTX;
	using namespace VTX::App;

	Application::Scene & scene = VTXApp::get().getScene();

	// Create MoleculeEntity
	const FilePath moleculePath = IO::Internal::Filesystem::getInternalDataDir() / MOLECULE_TRAJECTORY_TEST_NAME_EXT;
	Internal::Action::ECS::Open openAction = Internal::Action::ECS::Open( moleculePath );
	openAction.execute();
}
