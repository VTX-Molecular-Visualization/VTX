#include "app.hpp"
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <memory>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <string>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::App::Test::Util
{
	void App::initApp()
	{
		static bool isInit;
		if ( !isInit )
		{
			const FilePath path = VTX::Util::Filesystem::getExecutableDir() / "logs";
			std::filesystem::create_directory( path );
			LOGGER().init( path );

			PythonBinding::INTERPRETOR().addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();

			APP().start( {} );
			isInit = true;
		}

		SCENE().reset();
		resetInterpretor();
	}

	void App::resetInterpretor()
	{
		PythonBinding::INTERPRETOR().clearBinders();
		PythonBinding::INTERPRETOR().addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();
	}

	void App::loadMolecule( const std::string & p_moleculePath )
	{
		// Create MoleculeEntity
		const FilePath moleculePath = IO::Internal::Filesystem::getInternalDataDir() / p_moleculePath;
		VTX::App::Action::Scene::LoadMolecule openAction = VTX::App::Action::Scene::LoadMolecule( moleculePath );
		openAction.execute();
	}

	void App::loadTestMolecule() { loadMolecule( MOLECULE_TEST_NAME_EXT ); }

} // namespace VTX::App::Test::Util
