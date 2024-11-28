#include "app.hpp"
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/filesystem.hpp>
#include <app/vtx_app.hpp>
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
			LOGGER::init( path );

			INTERPRETOR().addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();

			APP().start( { 0, nullptr } );
			isInit = true;
		}

		SCENE().reset();
		resetInterpretor();
	}

	void App::resetInterpretor()
	{
		INTERPRETOR().clearBinders();
		INTERPRETOR().addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();
	}

	void App::loadSystem( const std::string & p_moleculePath )
	{
		// Create SystemEntity
		const FilePath						moleculePath = VTX::App::Filesystem::getInternalDataDir() / p_moleculePath;
		VTX::App::Action::Scene::LoadSystem openAction	 = VTX::App::Action::Scene::LoadSystem( moleculePath );
		openAction.execute();
	}

	void App::loadTestSystem() { loadSystem( MOLECULE_TEST_NAME_EXT ); }

} // namespace VTX::App::Test::Util
