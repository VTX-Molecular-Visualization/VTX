#include "app.hpp"
#ifdef JEVEUPAS

#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/filesystem.hpp>
#include <app/vtx_app.hpp>
#endif // JEVEUPAS
#include <memory>
#include <python_binding/binding/vtx_api.hpp>
#include <python_binding/binding/vtx_module.hpp>
#include <python_binding/interpretor.hpp>
#include <string>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::App::Test::Util
{
	PythonFixture::PythonFixture()
	{
		// INTERPRETOR().addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();

		const FilePath path = VTX::Util::Filesystem::getExecutableDir() / "logs";
		std::filesystem::create_directory( path );
		LOGGER::init( path );

		try
		{
			// We intend to load API into the test executable interpretor. This has to be done once per process, so if
			// multiple UT are running from the same process, the frist will be able to appl the binding, and the
			// following will throw harmlessly
			pybind11::module_ * modul = nullptr;
			INTERPRETOR().getPythonModule( &modul );

			VTX::PythonBinding::Binding::applyVtxBinding( *modul );
		}
		catch ( ... )
		{
		}

		// SCENE().reset();
	}
	PythonFixture::~PythonFixture()
	{
		resetInterpretor();
		//  SCENE().reset();
	}
	void PythonFixture::resetInterpretor()
	{
		INTERPRETOR().clearBinders();
		// INTERPRETOR().addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();
	}

	void PythonFixture::loadSystem( const std::string & p_systemPath )
	{
		// Create SystemEntity
		// const FilePath						systemPath = VTX::App::Filesystem::getInternalDataDir() / p_systemPath;
		// VTX::App::Action::Scene::LoadSystem openAction = VTX::App::Action::Scene::LoadSystem( systemPath );
		// openAction.execute();
	}

	void PythonFixture::loadTestSystem() { loadSystem( MOLECULE_TEST_NAME_EXT ); }

} // namespace VTX::App::Test::Util
