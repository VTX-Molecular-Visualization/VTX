#ifndef __VTX_APP_TEST_UTIL_APP__
#define __VTX_APP_TEST_UTIL_APP__

#include <app/application/scene.hpp>
#include <app/internal/action/ecs.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <memory>
#include <python_binding/interpretor.hpp>
#include <string>

namespace VTX::App::Test::Util
{
	class App
	{
	  public:
		inline static const std::string MOLECULE_TEST_NAME	   = "1AGA";
		inline static const std::string MOLECULE_TEST_NAME_EXT = MOLECULE_TEST_NAME + ".mmtf";

		static void										   initApp();
		static std::unique_ptr<PythonBinding::Interpretor> createInterpretor();

		static void loadMolecule( const std::string & p_moleculePath );
		static void loadTestMolecule();
	};
} // namespace VTX::App::Test::Util
#endif
