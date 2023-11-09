#include "app.hpp"
#include <app/application/scene.hpp>
#include <app/application/system.hpp>
#include <app/internal/action/ecs.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <memory>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <string>
#include <util/logger.hpp>

namespace VTX::App::Test::Util
{
	void App::initApp()
	{
		static bool isInit;
		if ( !isInit )
		{
			VTX::Util::Logger::get().init();

			VTXApp::get().start( {} );
			isInit = true;
		}

		VTXApp::get().getScene().reset();
	}

	std::unique_ptr<PythonBinding::Interpretor> App::createInterpretor()
	{
		std::unique_ptr<PythonBinding::Interpretor> interpretorPtr = std::make_unique<PythonBinding::Interpretor>();

		if ( !VTX::App::VTXApp::get().getSystem().exists( PythonBinding::Interpretor::SYSTEM_KEY ) )
			VTX::App::VTXApp::get().getSystem().referenceSystem(
				PythonBinding::Interpretor ::SYSTEM_KEY, interpretorPtr.get()
			);

		interpretorPtr->addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();

		return std::move( interpretorPtr );
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
